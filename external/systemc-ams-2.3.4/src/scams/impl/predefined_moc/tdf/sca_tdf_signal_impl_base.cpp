/*****************************************************************************

    Copyright 2010-2014
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

  sca_tdf_signal_impl_base.cpp - description

  Original Author: Karsten Einwich COSEDA Technologies GmbH

  Created on: 06.08.2009

   SVN Version       :  $Revision: 2320 $
   SVN last checkin  :  $Date: 2023-06-08 12:47:38 +0000 (Thu, 08 Jun 2023) $
   SVN checkin by    :  $Author: pehrlich $
   SVN Id            :  $Id: sca_tdf_signal_impl_base.cpp 2320 2023-06-08 12:47:38Z pehrlich $

 *****************************************************************************/

/*****************************************************************************/

#define SC_INCLUDE_DYNAMIC_PROCESSES

#include<systemc-ams>
#include "scams/impl/predefined_moc/tdf/sca_tdf_signal_impl_base.h"
#include "scams/impl/synchronization/sca_synchronization_obj_if.h"
#include "scams/impl/core/sca_solver_base.h"
#include "scams/impl/util/tracing/sca_trace_file_base.h"
#include "scams/impl/analysis/ac/sca_ac_domain_globals.h"




namespace sca_tdf
{
namespace sca_implementation
{


void sca_tdf_signal_impl_base::resize_port_data(unsigned long port_id)
{
	unsigned long size=port_id+1;

	if(timestep_calculated.size()<size)
	{
		timestep_calculated.resize(size,NULL);
		port_timestep_change_id.resize(size,NULL);
		rates.resize(size,NULL);
		rates_old.resize(size,NULL);
		delays.resize(size,NULL);
		delays_old.resize(size,NULL);
	}
}



void sca_tdf_signal_impl_base::resize_buffer()
{
}





/** Gets the absolute number of samples (from simulation start) samples
    of the current call are not included */
unsigned long sca_tdf_signal_impl_base::get_sample_cnt(unsigned long port)
{
    return *(call_counter_refs[port]) * *rates[port];
}

/** Gets the set sampling rate of the port */
unsigned long& sca_tdf_signal_impl_base::get_rate(unsigned long port)
{
    return *rates[port];
}




/** Gets the setted sampling delay of the port */
unsigned long& sca_tdf_signal_impl_base::get_delay(unsigned long port)
{
    return *delays[port];
}


sca_core::sca_time& sca_tdf_signal_impl_base::get_timestep_calculated_ref(unsigned long port) const
{
	if(csync_data!=NULL)
	{
		//due dtdf timestep may be changed
		(*timestep_calculated[port]) =
			csync_data->cluster_period/(*rates[driver_port] * *(calls_per_period[driver_port]));
	}

	return const_cast<sca_core::sca_time&>(*timestep_calculated[port]);
}



sc_dt::int64& sca_tdf_signal_impl_base::get_timestep_change_id(unsigned long port)
{
    return *port_timestep_change_id[port];
}




//prepare channel datastructures
void sca_tdf_signal_impl_base::end_of_elaboration()
{
    unsigned long max_in_delay = 0;

    if(get_samples_per_period()<0)
        return; //not used


    long out_delay = 0;


    unsigned long    nports = get_number_of_connected_ports();
    sca_core::sca_implementation::sca_port_base**  plist  = get_connected_port_list();

    nr_in_delay_buffer_resize(nports);
    //in_delay_buffer.resize(nports,NULL);

    is_inport = new bool[nports];


    buffer_offsets        = new long[nports];
    call_counter_refs     = new const sc_dt::int64* [nports];
    calls_per_period      = new       long*[nports];
    last_calls_per_period = new       long*[nports];

    sca_core::sca_implementation::sca_port_base* cp;
    for(unsigned long i=0; i<nports; ++i)
    {
        cp = plist[i];
        cp->set_if_id(i); //set port identification

        //sdf ports can be used in all domains due they are sync ports
        call_counter_refs[i]=&(cp->get_sync_domain()->get_call_counter());

        calls_per_period[i]=&(cp->get_sync_domain()->
        		get_synchronization_object_data()->calls_per_period);

        last_calls_per_period[i]=&(cp->get_sync_domain()->
        		get_synchronization_object_data()->last_calls_per_period);

        buffer_offsets[i] = 0;

        if(cp->get_port_type()==sca_core::sca_implementation::sca_port_base::SCA_IN_PORT)
        {
            is_inport[i]=true;
            buffer_offsets[i] = -(*delays[i]);

            if(*delays[i]>0)
            {
            	in_delay_buffer_create(i,*delays[i]);
                //in_delay_buffer[i]=new T[delays[i]];

                if(max_in_delay<*delays[i])
                    max_in_delay = *delays[i];
            }

        }
        else if((cp->get_port_type()==sca_core::sca_implementation::sca_port_base::SCA_OUT_PORT) ||
        		(cp->get_port_type()==sca_core::sca_implementation::sca_port_base::SCA_DECL_OUT_PORT))
        {
            is_inport[i]=false;
            out_delay = *delays[i];
            buffer_offsets[i] = *delays[i];
            if(driver_port>=0)
            {
                std::ostringstream str;
                str << "Multiple driver for a sca_tdf_channel found." << std::endl
                << "Channel driven from module: "
                << plist[driver_port]->get_parent_module()->name()
                << " port #" << plist[driver_port]->get_port_number()
                << " and "
                << plist[i]->get_parent_module()->name()
                << " port #" << plist[i]->get_port_number() << "." << std::endl;
                SC_REPORT_ERROR("SystemC-AMS",str.str().c_str());
            }
            driver_port=i;
        }
    }


    indelay_start_sample=0;

    csync_data=get_synchronization_if()->get_cluster_synchronization_data();
    cluster_period=&(csync_data->cluster_period);
    last_cluster_period=&(csync_data->last_cluster_period);
    current_cluster_timestep=&(csync_data->current_cluster_timestep);

    buffer_size = get_samples_per_period()+out_delay+max_in_delay;

    attribute_change_id=&(csync_data->attribute_change_id);
    attributes_changed=&(csync_data->attributes_changed);

    create_buffer(buffer_size);
    //buffer = new T[buffer_size];
    //for(unsigned long i=0; i<buffer_size; ++i)	buffer[i] = T();

    if(this->max_buffer_usage==1)
    {
        if(!this->signal_traced)
        {
        	this->quick_access_possible=true;

        	if(out_delay+max_in_delay ==0)
        	{
        		this->quick_access_enabled=true;
        	}
        }
    }
}

long sca_tdf_signal_impl_base::get_driver_port_id()
{
    return driver_port;
}

const char* sca_tdf_signal_impl_base::kind() const
{
    return "sca_sdf_signal";
}



void sca_tdf_signal_impl_base::construct()
{
    driver_port=-1;
    buffer_offsets=NULL;
    buffer_size=0;
    call_counter_refs=NULL;
    calls_per_period=NULL;
    last_calls_per_period=NULL;
    cluster_period=NULL;
    last_cluster_period=NULL;
    current_cluster_timestep=NULL;
    is_inport=NULL;

    attribute_change_id=NULL;
    attributes_changed=NULL;

    csync_data=NULL;

    trace_callback_cnt=-1;

    trace_cb_ptr=NULL;
    trace_cb_arg=NULL;

    force_value_flag=false;
    scheduled_force_value_flag=false;

    callback_registered=false;

    quick_access_enabled=false;
    quick_access_possible=false;
    signal_traced=false;
}


sca_tdf_signal_impl_base::sca_tdf_signal_impl_base()
{
    construct();
}


sca_tdf_signal_impl_base::sca_tdf_signal_impl_base(const char* name_):
	sca_core::sca_prim_channel(name_)
{
    construct();
}

sca_tdf_signal_impl_base::~sca_tdf_signal_impl_base()
{
    buffer_size = 0;
    delete[] buffer_offsets;
    delete[] call_counter_refs;
    delete[] calls_per_period;
    delete[] is_inport;
}

const std::string& sca_tdf_signal_impl_base::get_current_trace_value(unsigned long sample) const
{
	return empty_string;
}

sc_dt::int64 sca_tdf_signal_impl_base::get_number_of_trace_value_sample() const
{
	//disable quick access due all sample should be stored in buffer
	//if interactive tracing is used
	this->quick_access_enabled=false;
	this->quick_access_possible=false;


	//not yet elaborated
	if(csync_data==NULL)
	{

		std::ostringstream str;

		str << "Signal: " << this->name() << " cannot be accessed by get_trace_value()";
		str << " before end of elaboration";
		SC_REPORT_WARNING("SystemC-AMS",str.str().c_str());

		return -1;
	}

	const sca_core::sca_time& ctime=csync_data->cluster_start_time;
	const sca_core::sca_time& cperiod=csync_data->cluster_period;


	// SystemC time
	sc_core::sc_time sctime=sc_core::sc_time_stamp();

	if(sctime<ctime)
	{
		std::ostringstream str;
		str << "Internal not possible error for interactive tracing (SystemC time < SystemC-AMS time)";
		str << " SystemC time: " << sctime << " sca time: " << ctime << " for signal: " << this->name();
		SC_REPORT_ERROR("SystemC-AMS",str.str().c_str());
	}

	const long&   driver_calls_per_period  = *this->calls_per_period[this->driver_port];

	//calculate time reached (or yet not reached)
	sca_core::sca_time next_sca_time;
	if(csync_data->cluster_executes)
	{
		const sc_dt::int64&  driver_call_cnt   = *this->call_counter_refs[this->driver_port];
		const long&   driver_calls_per_period  = *this->calls_per_period[this->driver_port];
		//unsigned long driver_rate       = *this->rates[this->driver_port];
		//sca_core::sca_time driver_timestep = *this->timestep_calculated[this->driver_port];



		// cluster start time + cluster period * call of current period/number of calls per period
		next_sca_time=ctime + (cperiod*(((driver_call_cnt-1)%driver_calls_per_period)+1))/driver_calls_per_period;
		//next_sca_time=ctime +
		//		(driver_call_cnt%driver_calls_per_period) * driver_rate * driver_timestep;

/*
		std::cout << "between: " << sctime << "   " << ctime
				<< " driver_call_cnt: "<< driver_call_cnt
				<< " driver_calls_per_period: "<< driver_calls_per_period
				<< " cperiod: " << cperiod
				<< " next_sca_time: " << next_sca_time << std::endl;
*/
	}
	else
	{
		//long   driver_calls_per_period     = *this->calls_per_period[this->driver_port];
		//unsigned long driver_rate          = *this->rates[this->driver_port];
		//sca_core::sca_time driver_timestep = *this->timestep_calculated[this->driver_port];

		next_sca_time=ctime + cperiod;
		//next_sca_time=ctime + driver_calls_per_period*driver_rate*driver_timestep;

		//std::cout << "full: " << sctime << "  " << ctime << "  " << next_sca_time << std::endl;

	}

	unsigned long driver_rate          = *this->rates[this->driver_port];

	if(next_sca_time<sctime) //value not yet available
	{
		std::ostringstream str;

		str << "Signal: " << this->name() << " cannot be accessed by get_trace_value()";
		str << " due the signal belongs to a non causal TDF cluster: ";
		str << " available TDF sample: " << next_sca_time;
		str << " current SystemC time: " << sctime;
		SC_REPORT_WARNING("SystemC-AMS",str.str().c_str());

		return -1;
	}


	//the request sample has the timestamp of the next sample to calculate
	//in this case we must use the old sample
	if(next_sca_time==sctime) //in this case we use the old sample
	{

		if(sctime!=sc_core::SC_ZERO_TIME)
			sctime-=sc_core::sc_get_time_resolution();
	}

	//sca_core::sca_time time_period=get_timestep_calculated_ref(driver_port);

	//calculate sample in buffer from start time
	sc_dt::uint64 nsample = (((sctime-ctime).value()*driver_rate*driver_calls_per_period)/cperiod.value());

	//sample number of cluster start
	sc_dt::int64 sample_cnt_start;
	sample_cnt_start=(*(call_counter_refs[driver_port]) -
	                *(calls_per_period[driver_port]) ) * *rates[driver_port];

	//std::cout << "nsample: " << nsample << " bsize: " << buffer_size << std::endl;

	return (nsample+sample_cnt_start)%buffer_size;

}

///////////////////////////////////////////////////////////////////////////////


const std::string& sca_tdf_signal_impl_base::get_trace_value() const
{
	sc_dt::int64 nsample=this->get_number_of_trace_value_sample();

	if(nsample<0) return empty_string;


    return this->get_current_trace_value(nsample);
}

///////////////////////////////////////////////////////////////////////////////


bool sca_tdf_signal_impl_base::force_value(const std::string& str)
{
	this->set_force_value(str);

	this->force_forced_value();

	return true;
}


void sca_tdf_signal_impl_base::force_trigger_process()
{
	this->force_value_flag=this->scheduled_force_value_flag;

	if((this->quick_access_possible)&&(!this->force_value_flag)&&
			(!this->signal_traced) )
	{
		this->quick_access_enabled=true;
	}
	else
	{
		this->quick_access_enabled=false;
	}

	force_proc_handle.disable();
}

///////////////////////////////////////////////////////////////////////////////


void sca_tdf_signal_impl_base::force_forced_value()
{
	if(csync_data==NULL)
	{
		this->force_value_flag=true;
		this->quick_access_enabled=false;
		return;
	}

	if(!csync_data->cluster_executes)
	{
		this->force_value_flag=true;
		this->quick_access_enabled=false;
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
							sc_bind(&sca_tdf_signal_impl_base::force_trigger_process,this),
							sc_core::sc_gen_unique_name("force_trigger_process"),&opt);
		}
		else
		{
			this->force_proc_handle.enable();
		}
	}
}



///////////////////////////////////////////////////////////////////////////////


void sca_tdf_signal_impl_base::release_value()
{
	if(csync_data==NULL)
	{
		this->force_value_flag=false;
		if(this->quick_access_possible)
		{
			if(!this->signal_traced) this->quick_access_enabled=true;
		}

		return;
	}

	if(!csync_data->cluster_executes)
	{
		this->force_value_flag=false;
		if(this->quick_access_possible)
		{
			if(!this->signal_traced) this->quick_access_enabled=true;
		}
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
							sc_bind(&sca_tdf_signal_impl_base::force_trigger_process,this),
							sc_core::sc_gen_unique_name("force_trigger_process"),&opt);
		}
		else
		{
			this->force_proc_handle.enable();
		}
	}
}


///////////////////////////////////////////////////////////////////////////////

void sca_tdf_signal_impl_base::trace_callback()
{
	if(csync_data==NULL) //we try it after the next delta again
	{
		if(proc_try_cnt<2) //we try it after the next delta again
		{                               //due it is may not elaborated
			next_trigger(sc_core::SC_ZERO_TIME);
			proc_try_cnt++;
		}

		//otherwise the signal may not belongs to any cluster
		return;
	}

	if(trace_callback_cnt<0)
	{
		trace_callback_cnt=0;
		sca_core::sca_time dtime=csync_data->cluster_start_time - sc_core::sc_time_stamp();

		if(dtime>sc_core::SC_ZERO_TIME)
		{
			next_trigger(dtime);
			return;
		}
	}

	sc_dt::int64  driver_call_cnt   = *this->call_counter_refs[this->driver_port];
	unsigned long driver_rate       = *this->rates[this->driver_port];

	sc_dt::int64 available_sample=driver_call_cnt*driver_rate;

	//if sample available call callback
	if(available_sample > this->trace_callback_cnt)
	{
		//call callback if available
		if(this->trace_cb_ptr!=NULL)
		{
			this->trace_cb_ptr(this->trace_cb_arg);
		}

		for(unsigned long i=0;i<this->callbacks.size();++i)
		{
			(*(this->callbacks[i]))();
		}

		this->trace_callback_cnt++;
	}


	//if still sample available call callback after timestep
	if(available_sample > this->trace_callback_cnt)
	{
		next_trigger(*this->timestep_calculated[this->driver_port]);
	}
	else //if not wait on new cluster start
	{
		next_trigger(csync_data->cluster_start_event);
	}

}


bool sca_tdf_signal_impl_base::register_trace_callback(sca_util::sca_traceable_object::sca_trace_callback cb,void* cb_arg)
{
	this->trace_cb_ptr=cb;
	this->trace_cb_arg=cb_arg;


	//disable quick access due all sample should be stored in buffer
	this->quick_access_enabled=false;
	this->quick_access_possible=false;

	if(!this->callback_registered)
	{
		sc_core::sc_spawn_options opt;
		opt.spawn_method();



		sc_core::sc_spawn(sc_bind(&sca_tdf_signal_impl_base::trace_callback,this),
				sc_core::sc_gen_unique_name("trace_callback"),&opt);
		this->callback_registered=true;
	}

	return true;

}


bool sca_tdf_signal_impl_base::register_trace_callback(sca_util::sca_traceable_object::callback_functor_base& func)
{
	//disable quick access due all sample should be stored in buffer
	this->quick_access_enabled=false;
	this->quick_access_possible=false;

	this->callbacks.push_back(&func);

	if(!this->callback_registered)
	{
		sc_core::sc_spawn_options opt;
		opt.spawn_method();


		sc_core::sc_spawn(sc_bind(&sca_tdf_signal_impl_base::trace_callback,this),
				sc_core::sc_gen_unique_name("trace_callback"),&opt);

		this->callback_registered=true;
	}

	return true;

}

bool sca_tdf_signal_impl_base::remove_trace_callback(sca_util::sca_traceable_object::callback_functor_base& func)
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

bool sca_tdf_signal_impl_base::trace_init(sca_util::sca_implementation::sca_trace_object_data& data)
{
    //trace will be activated after every complete cluster calculation
    //by the synchronization layer
    if(get_synchronization_if()==NULL)
        return false;

    set_type_info(data);

    get_synchronization_if()->add_cluster_trace(data);

    signal_traced=true;
    quick_access_enabled=false;

    return true;
}

//is called after cluster was calculated
void sca_tdf_signal_impl_base::trace(long id,sca_util::sca_implementation::sca_trace_file_base& tf)
{
    sca_core::sca_time ctime;

    ctime=csync_data->cluster_start_time;
    sca_core::sca_time time_period=get_timestep_calculated_ref(driver_port);


    unsigned long sample_per_period=*(calls_per_period[driver_port])* *rates[driver_port];

    //call_counter_refs[driver_port] starts at now at 1  !!!!
    // (start at 0 and cnt after run thus for trace it starts with 1)
    sc_dt::int64 sample_cnt;
    sample_cnt=(*(call_counter_refs[driver_port]) -
                *(calls_per_period[driver_port]) ) * *rates[driver_port];

    for(unsigned long i=0; i<sample_per_period; ++i, ++sample_cnt)
    {
        store_trace_buffer_time_stamp(tf,id,ctime,sample_cnt%buffer_size);
        ctime+=time_period;
    }
}

void sca_tdf_signal_impl_base::trace_interactive()
{

}

} //namespace sca_implementation
} //namespace sca_tdf

