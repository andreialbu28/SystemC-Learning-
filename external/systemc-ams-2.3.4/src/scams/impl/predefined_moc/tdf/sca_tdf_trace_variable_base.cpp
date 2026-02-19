/*****************************************************************************

    Copyright 2010-2013
    Fraunhofer-Gesellschaft zur Foerderung der angewandten Forschung e.V.

    Copyright 2015-2016
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

  sca_tdf_trace_variable_base.cpp - description

  Original Author: Karsten Einwich Fraunhofer IIS/EAS Dresden

  Created on: 12.12.2009

   SVN Version       :  $Revision: 2320 $
   SVN last checkin  :  $Date: 2023-06-08 12:47:38 +0000 (Thu, 08 Jun 2023) $
   SVN checkin by    :  $Author: pehrlich $
   SVN Id            :  $Id: sca_tdf_trace_variable_base.cpp 2320 2023-06-08 12:47:38Z pehrlich $

 *****************************************************************************/

/*****************************************************************************/

#define SC_INCLUDE_DYNAMIC_PROCESSES

#include "systemc-ams"
#include "scams/impl/predefined_moc/tdf/sca_tdf_trace_variable_base.h"
#include "scams/impl/core/sca_simcontext.h"
#include "scams/impl/core/sca_solver_base.h"

namespace sca_tdf
{
namespace sca_implementation
{

sca_trace_variable_base::sca_trace_variable_base(const char* nm) :
	sc_core::sc_object(nm)
{
	rate = 1;
	timeoffset = sc_core::SC_ZERO_TIME;

	parent_module = dynamic_cast<sca_tdf::sca_module*>(this->get_parent_object());

	if (!parent_module)
	{
		std::ostringstream str;
		str << "A sca_tdf::sca_trace_variable"
				<< " can be used only in the context of a "
				<< "sca_tdf::sca_module (SCA_TDF_MODULE) for vatiable: "<< name()<< std::endl;
		SC_REPORT_ERROR("SystemC-AMS", str.str().c_str());
	}

	initialized=false;

	call_counter_ref=NULL;
	buffer_size=-1;
	calls_per_period=0;

	trace_cb_ptr=NULL;
    trace_cb_arg=NULL;
    trace_callback_pos=-1;

    scheduled_force_value_flag=false;
    force_value_flag=false;

    callback_registered=false;
}

bool sca_trace_variable_base::initialize()
{
	if (parent_module->get_sync_domain()==NULL)
	{
		return false;
	}

	calls_per_period = parent_module->get_sync_domain()->get_calls_per_period();
	call_counter_ref = &(parent_module->get_sync_domain()->get_call_counter());

	resize_buffer(rate*calls_per_period);
	write_flags.resize(rate*calls_per_period, false);

	initialized=true;

	return true;
}

////////////////////////////////////////////////////////////////////////////
bool sca_trace_variable_base::trace_init(sca_util::sca_implementation::sca_trace_object_data& data)
{
	//trace will be activated after every complete cluster calculation
	//by the synchronization layer

	set_type_info(data);


	if(initialize())
	{
		parent_module->get_sync_domain()->add_cluster_trace(data);
		return true;
	}

	return false;
}



sca_core::sca_time sca_trace_variable_base::get_trace_time(int sample_id)
{
	if(rate==1)
	{
		return parent_module->sca_get_time() + timeoffset;
	}
	else
	{   //time progresses from last time to current time
		sca_core::sca_time ttime;
		ttime= parent_module->sca_get_time()+timeoffset+(sample_id+1)*((sca_core::sca_module*)parent_module)->get_timestep()/rate;

		if(ttime<((sca_core::sca_module*)parent_module)->get_timestep()) return sc_core::SC_ZERO_TIME;
		else return ttime-((sca_core::sca_module*)parent_module)->get_timestep();
	}
}



//type independend part of write
long sca_trace_variable_base::get_write_index(unsigned long sample_id)
{
	if (!initialized)
		if (!initialize()) return -1;

	if (sample_id>=rate)
	{
		std::ostringstream str;
		str << "sample_id must be smaller than rate in sca_tdf::sca_trace_variable: "
		    << name();
		SC_REPORT_ERROR("SystemC-AMS",str.str().c_str());
	}


	long index=((*call_counter_ref)%calls_per_period)*rate + sample_id;

	write_flags[index] = true;
	time_buffer[index]=get_trace_time(sample_id);

	if(this->callback_registered)
	{
		sc_core::sc_time ctime=sc_core::sc_time_stamp();
		if(ctime<=time_buffer[index])
		{
			this->trace_callback_write_event.notify(time_buffer[index]-ctime);
		}
		else
		{
			//execute callback immedately (nevertheless it will be to late)
			this->trace_callback_write_event.notify();
		}
	}

	return index;
}

////////////////////////////////////////////////////////////////////////////

void sca_trace_variable_base::trace(long id, sca_util::sca_implementation::sca_trace_file_base& tf)
{
	for (int i=0; i<buffer_size; i++)
	{
		if (write_flags[i])
		{
			store_to_last(i);;
			write_flags[i]=false;
			trace_last(id, time_buffer[i], tf);
		}
		else
		{
			//ensure time progress - hold last_value
			trace_last(id, parent_module->sca_get_time()+timeoffset, tf);
		}
	}

}


void sca_trace_variable_base::trace_interactive()
{
}

////////////////////////////////////////////////////////////////////////////


void sca_trace_variable_base::trace( sc_core::sc_trace_file* tf ) const
{
	std::ostringstream str;
	str << "sc_trace of sca_trace_variable not supported for variable: " << this->name();
	SC_REPORT_WARNING("SystemC-AMS",str.str().c_str());
}

///////////////////////////////////////////////////////////////////////////////

sca_trace_variable_base::~sca_trace_variable_base()
{
}


///////////////////////////////////////////////////////////////////////////////

const std::string& sca_trace_variable_base::get_current_trace_value(unsigned long sample) const
{
	return empty_string;
}

long sca_trace_variable_base::get_trace_value_index() const
{
	sc_core::sc_time ctime=sc_core::sc_time_stamp();

	long def_idx=0;
	sc_core::sc_time htime;

	for(long i=0;i<long(time_buffer.size());i++)
	{
		//use the value before or at the current time
		if(time_buffer[i]>=ctime)
		{
			if(time_buffer[i]==ctime) return i;
			if(i>long(1))             return i-1;
			return    0;
		}

		//if no stamp found use largest
		if(htime<time_buffer[i])
		{
			htime=time_buffer[i];
			def_idx=i;
		}
	}

	//otherwise use the first available value
    return def_idx;
}

///////////////////////////////////////////////////////////////////////////////


const std::string& sca_trace_variable_base::get_trace_value() const
{
	long idx=this->get_trace_value_index();

	//otherwise use the first available value
    return this->get_current_trace_value(idx);
}


//////////////////////////////////////////////////////////////////////////////


void sca_trace_variable_base::trace_callback()
{
	//initialization
	if(trace_callback_pos<0)
	{
		trace_callback_pos=0;
		next_trigger(this->trace_callback_write_event);
		return;
	}


	sc_core::sc_time ctime=sc_core::sc_time_stamp();

	//call callback if available
	if(this->trace_cb_ptr!=NULL)
	{
		this->trace_cb_ptr(this->trace_cb_arg);
	}


	for(unsigned long i=0;i<this->callbacks.size();++i)
	{
		(*(this->callbacks[i]))();
	}


	this->trace_callback_pos++;
	if(trace_callback_pos>=long(time_buffer.size()))
	{
		trace_callback_pos=0;
	}


	if(time_buffer[trace_callback_pos]>ctime)
	{
		next_trigger(time_buffer[trace_callback_pos]-ctime);
	}
	else
	{
		next_trigger(this->trace_callback_write_event);
	}
}


///////////////////////////////////////////////////////////////////////////////

bool sca_trace_variable_base::register_trace_callback(sca_util::sca_traceable_object::sca_trace_callback cb,void* cb_arg)
{
	this->trace_cb_ptr=cb;
	this->trace_cb_arg=cb_arg;

	if(!this->callback_registered)
	{
		sc_core::sc_spawn_options opt;
		opt.spawn_method();

		sc_core::sc_spawn(sc_bind(&sca_trace_variable_base::trace_callback,this),
				sc_core::sc_gen_unique_name("trace_callback"),&opt);


		this->callback_registered=true;
	}

	return true;

}


///////////////////////////////////////////////////////////////////////////////

bool sca_trace_variable_base::register_trace_callback(sca_util::sca_traceable_object::callback_functor_base& func)
{
	this->callbacks.push_back(&func);

	if(!this->callback_registered)
	{
		sc_core::sc_spawn_options opt;
		opt.spawn_method();

		sc_core::sc_spawn(sc_bind(&sca_trace_variable_base::trace_callback,this),
				sc_core::sc_gen_unique_name("trace_callback"),&opt);

		this->callback_registered=true;
	}

	return true;
}

///////////////////////////////////////////////////////////////////////////////

bool sca_trace_variable_base::remove_trace_callback(sca_util::sca_traceable_object::callback_functor_base& func)
{
	for(std::vector<sca_util::sca_traceable_object::callback_functor_base*>::iterator
			it=this->callbacks.begin();it!=this->callbacks.end();++it)
	{
		if(*it == &func)
		{
			this->callbacks.erase(it);
			return true;
		}
	}

	return false;
}

///////////////////////////////////////////////////////////////////////////////


bool sca_trace_variable_base::force_value(const std::string& str)
{
	this->set_force_value(str);

	this->force_forced_value();

	return true;
}

///////////////////////////////////////////////////////////////////////////////


void sca_trace_variable_base::force_trigger_process()
{
	this->force_value_flag=this->scheduled_force_value_flag;
	force_proc_handle.disable();
}

///////////////////////////////////////////////////////////////////////////////


void sca_trace_variable_base::force_forced_value()
{
	if(parent_module->get_sync_domain()==NULL)
	{
		this->force_value_flag=true;
		return;
	}

	sca_core::sca_implementation::sca_cluster_synchronization_data* csync_data;
	csync_data=parent_module->get_sync_domain()->get_cluster_synchronization_data();

	if(!csync_data->cluster_executes)
	{
		this->force_value_flag=true;
	}
	else //wait until cluster finished to guarantee causality
	{
		this->scheduled_force_value_flag=true;

		if(!force_proc_handle.valid())
		{
			sc_core::sc_spawn_options opt;
			opt.spawn_method();
			opt.dont_initialize();
			opt.set_sensitivity(&csync_data->cluster_end_event);
			this->force_proc_handle=
					sc_core::sc_spawn(
							sc_bind(&sca_trace_variable_base::force_trigger_process,this),
							sc_core::sc_gen_unique_name("force_trigger_process"),&opt);
		}
		else
		{
			this->force_proc_handle.enable();
		}
	}
}



///////////////////////////////////////////////////////////////////////////////


void sca_trace_variable_base::release_value()
{
	if(parent_module->get_sync_domain()==NULL)
	{
		this->force_value_flag=false;
		return;
	}

	sca_core::sca_implementation::sca_cluster_synchronization_data* csync_data;
	csync_data=parent_module->get_sync_domain()->get_cluster_synchronization_data();


	if(!csync_data->cluster_executes)
	{
		this->force_value_flag=false;
	}
	else //wait until cluster finished to guarantee causality
	{
		this->scheduled_force_value_flag=false;

		if(!force_proc_handle.valid())
		{
			sc_core::sc_spawn_options opt;
			opt.spawn_method();
			opt.dont_initialize();
			opt.set_sensitivity(&csync_data->cluster_end_event);
			this->force_proc_handle=
					sc_core::sc_spawn(
							sc_bind(&sca_trace_variable_base::force_trigger_process,this),
							sc_core::sc_gen_unique_name("force_trigger_process"),&opt);
		}
		else
		{
			this->force_proc_handle.enable();
		}
	}
}



void sca_trace_variable_base::set_unit(const std::string& unit_)
{
	unit=unit_;
}

const std::string& sca_trace_variable_base::get_unit() const
{
	return unit;
}

void sca_trace_variable_base::set_unit_prefix(const std::string& prefix_)
{
	unit_prefix=prefix_;
}

const std::string& sca_trace_variable_base::get_unit_prefix() const
{
	return unit_prefix;
}

void sca_trace_variable_base::set_domain(const std::string& domain_)
{
	domain=domain_;
}

const std::string& sca_trace_variable_base::get_domain() const
{
	return domain;
}


} //namespace sca_implementation
} //namespace sca_tdf
