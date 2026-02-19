/*****************************************************************************

 Copyright 2010-2013
 Fraunhofer-Gesellschaft zur Foerderung der angewandten Forschung e.V.

 Copyright 2015-2022
 COSEDA Technologies GmbH

 Licensed under the Apache License, Version 2.0 (the "License");
 you may not use this file except in compliance with the License.
 You may obtain a copy of the License at

 http://www.apache.org/licenses/LICENSE-2.0

 Unless required by applicable law or agreed to in writing, software
 distributed under the License is distributed on an "AS IS" BASIS,
 WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 See the License for the specific language governing permissions and
 limitations under the License.

 *****************************************************************************/

/*****************************************************************************

 sca_trace_file_base.cpp - description

 Original Author: Karsten Einwich Fraunhofer / COSEDA Technologies

 Created on: 13.11.2009

 SVN Version       :  $Revision: 2320 $
 SVN last checkin  :  $Date: 2023-06-08 12:47:38 +0000 (Thu, 08 Jun 2023) $
 SVN checkin by    :  $Author: pehrlich $
 SVN Id            :  $Id: sca_trace_file_base.cpp 2320 2023-06-08 12:47:38Z pehrlich $

 *****************************************************************************/

/*****************************************************************************/

#include <systemc-ams>
#include "scams/impl/util/tracing/sca_trace_file_base.h"
#include "scams/impl/util/tracing/sca_trace_buffer.h"
#include "scams/impl/core/sca_simcontext.h"
#include "scams/impl/core/sca_object_manager.h"

#ifndef DISABLE_PARALLEL_TRACING

#include <vector>
#include <thread>
#include <mutex>
#include <condition_variable>

#endif

namespace sca_util
{

namespace sca_implementation
{

static const unsigned long MAX_THREAD_TRACE_BUFFER_SIZE=10000;

void sca_disable_all_traces()
{
	auto tr_listp=sca_core::sca_implementation::sca_get_curr_simcontext()->get_trace_list();
	if(tr_listp==NULL) return;

	for(auto trace : *tr_listp)
	{
		trace->disable_tmp();
	}
}

void sca_restore_enable_all_traces()
{
	auto tr_listp=sca_core::sca_implementation::sca_get_curr_simcontext()->get_trace_list();
	if(tr_listp==NULL) return;

	for(auto trace : *tr_listp)
	{
		trace->enable_restore();
	}
}


struct sca_trace_file_base::states
{
#ifndef DISABLE_PARALLEL_TRACING

	std::vector<sca_trace_value_handler_base*>  stamp_buffer;
	std::vector<sca_trace_value_handler_base*>  stamp_pre_buffer;


	 std::mutex              mutex;
	 std::condition_variable condition_buffer_not_empty;
	 std::condition_variable condition_sync_done;

	 std::thread store_timestamp_thread;

	 bool wait_for_read=false;
	 bool sync_done=false;


	 states()
	 {
		 stamp_buffer.reserve(MAX_THREAD_TRACE_BUFFER_SIZE);
		 stamp_pre_buffer.reserve(MAX_THREAD_TRACE_BUFFER_SIZE/10);
	 }

#endif

	 bool ac_disabled=false;
	 bool time_domain_disabled=false;
};

///////////////////////////////////////////////////////////////////////////////

void sca_trace_file_base::store_abstract_time_stamp_to_buffer(sca_trace_value_handler_base* value)
{
#ifndef DISABLE_PARALLEL_TRACING

	if(this->parallel_tracing_disabled)
	{
		this->store_abstract_time_stamp(*value);
		return;
	}

	s.stamp_pre_buffer.push_back(value);

	if( (value->id<0) || (s.stamp_pre_buffer.size()>=MAX_THREAD_TRACE_BUFFER_SIZE/10))
	{
		std::unique_lock<std::mutex> mlock(s.mutex);

		s.stamp_buffer.insert(s.stamp_buffer.end(),s.stamp_pre_buffer.begin(),s.stamp_pre_buffer.end());
		s.stamp_pre_buffer.clear();


		if(s.stamp_buffer.size()>MAX_THREAD_TRACE_BUFFER_SIZE) //back pressure
		{
			s.wait_for_read=true;
			while(s.wait_for_read)
			{
				s.condition_sync_done.wait(mlock);
			}
		}


		s.condition_buffer_not_empty.notify_one();
	}

#else
    if(value!=NULL) this->store_abstract_time_stamp(*value);
#endif
}


void sca_trace_file_base::store_abstract_time_stamp_thread()
{
#ifndef DISABLE_PARALLEL_TRACING

	std::vector<sca_trace_value_handler_base*>  stamp_post_buffer;
	stamp_post_buffer.reserve(MAX_THREAD_TRACE_BUFFER_SIZE);

	std::unique_lock<std::mutex> mlock(s.mutex,std::defer_lock);

	while(true)
	{
		mlock.lock();  //start protected/serial section

		while(s.stamp_buffer.empty())
		{
			s.condition_buffer_not_empty.wait(mlock);
		}

		stamp_post_buffer.swap(s.stamp_buffer);


		if(s.wait_for_read)
		{
			if(s.stamp_buffer.size()<MAX_THREAD_TRACE_BUFFER_SIZE/2)
			{
				s.wait_for_read=false;
				s.condition_sync_done.notify_one();
			}
		}

		mlock.unlock();

		//start parallel section

		for(auto& item : stamp_post_buffer)
		{

			if(item->id==-2) //all previous stamps consumed
			{
				mlock.lock();

				s.sync_done=true;
				s.condition_sync_done.notify_one();

				mlock.unlock();

				item->free();

				continue;
			}

			if(item->id<0)
			{
				item->free();
				return; //finish thread
			}

			store_abstract_time_stamp(*item);
		}

		stamp_post_buffer.clear();

	} //while(true)

#endif
}


void sca_trace_file_base::sync_with_thread()
{
#ifndef DISABLE_PARALLEL_TRACING

	if( (!this->parallel_tracing_disabled) &&(this->init_done))
	{
		sca_trace_value_handler<bool>* vh= this->mm_manager.get_new<bool>(sc_core::sc_time_stamp(),-2,false);
		this->store_abstract_time_stamp_to_buffer(vh); //send sync command

		std::unique_lock<std::mutex> mlock(s.mutex);

		while(!s.sync_done)
		{
			s.condition_sync_done.wait(mlock);
		}

		s.sync_done=false;  //we reset in a locked area
	}

#endif
}


void sca_trace_file_base::synchronize()
{
	this->sync_with_thread();
	if(outstr!=NULL) outstr->flush();
}



sca_trace_file_base::sca_trace_file_base() : sp(new states), s(*sp)
{
    nwords=0;
    outstr=NULL;

	init_done = false;
	trace_is_disabled = false;
	trace_is_disabled_restore=false;
	header_written = false;
	sca_core::sca_implementation::sca_get_curr_simcontext()->get_trace_list()->push_back(
			this);
	written_flags = NULL;
	disabled_traces = NULL;

	//we allow trace file generation after simulation start -> this flag is used
	//for fast detection of a new added trace file to perform initialization in for the
	//next cluster activation
	sca_core::sca_implementation::sca_get_curr_simcontext()->all_traces_initialized()
			= false;

	buffer = new sca_trace_buffer;

	ac_active = false;

	closed = false;

#ifdef DISABLE_PARALLEL_TRACING
	parallel_tracing_disabled=true;
#endif
}

///////////////////////////////////////////////////////////////////////////////

sca_trace_file_base::~sca_trace_file_base()
{
}

///////////////////////////////////////////////////////////////////////////////

void sca_trace_file_base::add(sca_traceable_object* obj,
		const std::string& name, bool dont_interpolate)
{
	if (closed)
	{
		SC_REPORT_WARNING("SytemC-AMS","Can't add trace to closed sca_trace_file");
		return;
	}

	if (init_done)
	{
		std::ostringstream str;
		str << "A trace can only be added before the next delta cycle after "
			"trace file generation (LRM clause 6.1) for: " << name;
		SC_REPORT_ERROR("SystemC-AMS",str.str().c_str());
	}

	//generate id and resize the trace buffer
	long id = buffer->register_trace();

	//store trace infos
	traces.resize(id + 1);


	traces[id].trace_object = obj;
	traces[id].name = name;
	traces[id].original_name = name;
	traces[id].id = id;
	traces[id].dont_interpolate = dont_interpolate;
	traces[id].trace_base=this;

	sca_core::sca_physical_domain_interface* phd=dynamic_cast<sca_core::sca_physical_domain_interface*>(obj);
	if(phd!=NULL)
	{
		traces[id].unit=phd->get_unit();
		traces[id].type=phd->get_domain();
	}

}

///////////////////////////////////////////////////////////////////////////////

void sca_trace_file_base::initialize()
{

	if (!this->is_time_domain_enabled() || init_done) return;

#ifndef DISABLE_PARALLEL_TRACING
	if(!this->parallel_tracing_disabled)
	{
		std::thread store_thread([&](){this->store_abstract_time_stamp_thread();});
		s.store_timestamp_thread.swap(store_thread);
	}
#else
	parallel_tracing_disabled=true;
#endif

	init_done = true;
	ac_active = false;

	const std::size_t trsize=traces.size();

	//for detection whether a time stamp was written
	//(for waiting until all written) , a char has always 8Bit
	nwords = (long)(trsize) / (8 * sizeof(unsigned long)) + 1;
	written_flags = new unsigned long[nwords];
	sc_assert(written_flags!=NULL);

	//traces may disabled if the signal/node is nowhere connected
	disabled_traces = new unsigned long[nwords];
	sc_assert(disabled_traces!=NULL);

	for (std::size_t i = 0; i < std::size_t(nwords); i++)
	{
		disabled_traces[i] = ~((unsigned long) 0);
	}

	buffer->no_interpolation = new bool[trsize];
	sc_assert(buffer->no_interpolation!=NULL);

	for (std::size_t i = 0; i < trsize; i++)
	{
		traces[i].trace_init();
		buffer->no_interpolation[i] = traces[i].dont_interpolate;
	}

	//initialize written_flags
	for (int i = 0; i < nwords; i++)
		written_flags[i] = disabled_traces[i];

	if(!this->parallel_tracing_disabled)
	{
		this->sync_with_thread();
	}
}

///////////////////////////////////////////////////////////////////////////////
void sca_trace_file_base::write_to_file()
{
	if (closed)
	{
		return;
	}

	ac_active = false;

	if (!header_written)
	{
		header_written = true;
		write_header();
	}

	if (this->trace_disabled()) return;

	//if no waves available return (may closed without initialization)
	if(nwords<=0) return;

	//reset written flags
	for (int i = 0; i < nwords; i++)
	{
		written_flags[i] = disabled_traces[i];
	}

	//call virtual method for writing the waves
	write_waves();
}

///////////////////////////////////////////////////////////////////////////////

void sca_trace_file_base::enable_not_init()
{
	trace_is_disabled = false;
	buffer->enable();
}

void sca_trace_file_base::enable()
{
	trace_is_disabled = false;
	buffer->enable();

	if (!init_done)
	{
		//only allowed if systemc-ams has initialized
		if ((sca_core::sca_implementation::sca_get_curr_simcontext()->initialized()))
			initialize();
	}

	for (int i = 0; i < nwords; i++)
		written_flags[i] = disabled_traces[i];

}

void sca_trace_file_base::enable_restore()
{
	if(trace_is_disabled_restore) this->enable();
	trace_is_disabled_restore=false;
}

///////////////////////////////////////////////////////////////////////////////

bool sca_trace_file_base::trace_disabled()
{
	return trace_is_disabled;
}


void sca_trace_file_base::disable_ac()
{
	s.ac_disabled=true;
}

void sca_trace_file_base::enable_ac()
{
	s.ac_disabled=false;
}

bool sca_trace_file_base::is_ac_enabled()
{
	return !s.ac_disabled | !this->trace_disabled();
}


void sca_trace_file_base::disable_time_domain()
{
	s.time_domain_disabled=true;
}
void sca_trace_file_base::enable_time_domain()
{
	s.time_domain_disabled=false;
}

bool sca_trace_file_base::is_time_domain_enabled()
{
	return !s.time_domain_disabled & !this->trace_disabled();
}

///////////////////////////////////////////////////////////////////////////////

void sca_trace_file_base::disable()
{
	this->sync_with_thread();

	buffer->disable();
	trace_is_disabled = true;
}




void sca_trace_file_base::disable_tmp()
{
	if(this->trace_disabled())
	{
		trace_is_disabled_restore=false;
		return;
	}

	trace_is_disabled_restore=true;
	this->disable();
}

///////////////////////////////////////////////////////////////////////////////

void sca_trace_file_base::create_trace_file(const std::string& name,
		std::ios_base::openmode m)
{
	header_written = false;

	fout.open(name.c_str(), m);
	if (!fout)
	{
		std::string str = "Cannot open trace file: " + name;
		SC_REPORT_ERROR("SystemC-AMS", str.c_str());
	}

	fname=name;
	outstr = &fout;
}

///////////////////////////////////////////////////////////////////////////////

void sca_trace_file_base::create_trace_file(std::ostream& str)
{
	if (fout)
		fout.close();

	header_written = false;

	outstr = &str;
}

void sca_trace_file_base::set_mode(const sca_util::sca_trace_mode_base& mode)
{
}

void sca_trace_file_base::set_mode_impl(const sca_util::sca_trace_mode_base& mode)
{
	set_mode(mode);
}

///////////////////////////////////////////////////////////////////////////////


void sca_trace_file_base::reopen(const std::string& name,
		std::ios_base::openmode m)
{
	if (closed)
	{
		SC_REPORT_WARNING("SytemC-AMS","Can't reopen closed sca_trace_file");
		return;
	}

	this->sync_with_thread();

	if (fout)
	{
		fout.flush();
		fout.close();
	}

	if (outstr)
	{
		outstr->flush();
		outstr = NULL;
	}


	if ((m & std::ios::app) != (std::ios::app))
	{
		header_written = false;
	}
	else
	{
		header_written = true;
	}

	fout.open(name.c_str(), m);
	if (!fout)
	{
		std::string str = "Cannot open trace file: " + name;
		SC_REPORT_ERROR("SystemC-AMS", str.c_str());
	}

	fname=name;
	outstr = &fout;
}

void sca_trace_file_base::reopen_impl(const std::string& name,
		std::ios_base::openmode m)
{
	reopen(name,m);
}

///////////////////////////////////////////////////////////////////////////////

void sca_trace_file_base::reopen(std::ostream& str)
{
	if (closed)
	{
		SC_REPORT_WARNING("SytemC-AMS","Can't reopen closed sca_trace_file");
		return;
	}

	this->sync_with_thread();

	if (fout)
	{
		fout.flush();
		fout.close();
	}

	if (outstr)
	{
		outstr->flush();
		outstr = NULL;
	}


	header_written = false;

	fname.clear();
	outstr = &str;
}

void sca_trace_file_base::finish_writing()
{
}

void sca_trace_file_base::write_ac_domain_init()
{
	if (closed)
	{
		return;
	}

	this->sync_with_thread();

	write_header();
	ac_active = true;
}

void sca_trace_file_base::write_ac_noise_domain_init(sca_util::sca_vector<
		std::string>& names)
{
	SC_REPORT_WARNING("SystemC-AMS","AC tracing not available for this format");
}

void sca_trace_file_base::write_ac_noise_domain_stamp(double w,
		sca_util::sca_matrix<sca_util::sca_complex>& tr_matrix)
{
}

void sca_trace_file_base::write_ac_domain_stamp(double w, std::vector<sca_util::sca_complex>& tr_vec)
{
}

void sca_trace_file_base::close_trace()
{
	if (closed)
	{
		SC_REPORT_WARNING("SytemC-AMS","Can't close yet closed sca_trace_file");
		return;
	}

#ifndef DISABLE_PARALLEL_TRACING

	if(!this->parallel_tracing_disabled)
	{
		sca_trace_value_handler<bool>* vh= this->mm_manager.get_new<bool>(sc_core::sc_time_stamp(),-1,false);
		this->store_abstract_time_stamp_to_buffer(vh);

		if(s.store_timestamp_thread.joinable())
		{
			s.store_timestamp_thread.join();
		}
	}

#endif

	if(sca_core::sca_implementation::sca_get_curr_simcontext()==NULL)
	{
		std::ostringstream str;
		str << "SystemC AMS simcontext deleted before closing the trace file ";
		if(fname.size()>0)
		{
			str << "  " << fname << "  ";
		}
		str << "(module or signal was deleted)";

		SC_REPORT_WARNING("SytemC-AMS",str.str().c_str());
		return;
	}

	if(sca_core::sca_implementation::sca_get_curr_simcontext()->get_sca_object_manager()==NULL)
	{
		std::ostringstream str;
		str << "SystemC AMS object manager deleted before closing the trace file ";
		if(fname.size()>0)
		{
			str << "  " << fname << "  ";
		}
		str << "(module or signal was deleted)";


		SC_REPORT_WARNING("SytemC-AMS",str.str().c_str());
		return;
	}


	if(sca_core::sca_implementation::sca_get_curr_simcontext()->get_sca_object_manager()->is_object_deleted())
	{
		std::ostringstream str;
		str << "SystemC AMS module or signal was deleted before closing the trace file ";
		if(fname.size()>0)
		{
			str << "  " << fname;
		}

		SC_REPORT_WARNING("SytemC-AMS",str.str().c_str());
		return;
	}

	//after an error the datastructures may not consistent - we dont perform finish writing
	//only if time domain header was written to the current file datastructures may valid
	//if ac - simulation was executed only -> no header was written
	if ( (sc_core::sc_report_handler::get_count(sc_core::SC_ERROR) <= 0) && header_written)
	{

		for (unsigned long i = 0; i < traces.size(); i++)
		{
			//delete sc traces
			sca_util::sca_implementation::sca_sc_trace_base* obj;
			obj
					= dynamic_cast<sca_util::sca_implementation::sca_sc_trace_base*> (traces[i].trace_object);
			if (obj != NULL)
			{
				traces[i].trace();
				delete obj;
				traces[i].trace_object = NULL;
			}
		}


		if(this->buffer!=NULL)
		{
			//permit writing of timestamps of current time
			this->buffer->finish = true;
		}


		if (!ac_active)
		{
			write_to_file();
			finish_writing();
		}

	}

	init_done = false;
	trace_is_disabled = true;

	if(written_flags!=NULL)   delete[] written_flags;
	if(disabled_traces!=NULL) delete[] disabled_traces;
	if(buffer!=NULL) delete buffer;

	written_flags = NULL;
	disabled_traces = NULL;
	buffer = NULL;

	closed = true;
}

void sca_trace_file_base::close_file()
{
	if (fout.is_open())
	{
		fout.flush();
		fout.close();
	}

	if (outstr != NULL)
	{
		outstr->flush();
		outstr = NULL;
	}
}

void sca_trace_file_base::close()
{
	close_trace();
	close_file();
}

void sca_trace_file_base::send_to_file()
{

	if(this->closed) return;

    //fast trace activation check
    bool ready_flag=true;
    for(int i=0;i<nwords;i++)
    {
        if(this->written_flags[i]!=0)
        {
            ready_flag=false;
            break;
        }
    }

    if(ready_flag)
    {
        this->write_to_file();
    }
}

void sca_trace_file_base::disable_parallel_tracing()
{
	this->parallel_tracing_disabled=true;
}

void sca_trace_file_base::enable_parallel_tracing()
{
	this->parallel_tracing_disabled=false;
}



void sca_trace_file_base::store_abstract_time_stamp(sca_trace_value_handler_base& value)
{
	this->buffer->store_abstract_time_stamp(value);

	//set bit to sign trace as written
	this->written_flags[traces[value.id].word]&=traces[value.id].bit_mask;

	this->send_to_file();
}

} // namespace sca_implementation
} // namespace sca_util

