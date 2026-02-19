/*****************************************************************************

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

  sca_con_interactive_trace_data.cpp - description

  Original Author: Karsten Einwich COSEDA Technologies GmbH Dresden

  Created on: Aug 31, 2015

   SVN Version       :  $Revision$
   SVN last checkin  :  $Date$
   SVN checkin by    :  $Author$
   SVN Id            :  $Id$

 *****************************************************************************/

#define SC_INCLUDE_DYNAMIC_PROCESSES


#include <systemc-ams>
#include "scams/impl/core/sca_solver_base.h"

#include "sca_con_interactive_trace_data.h"

namespace sca_core
{

namespace sca_implementation
{

static const double ZERO_VALUE=0.0;

void sca_con_interactive_trace_data::register_trace_callback(sca_util::sca_traceable_object::sca_trace_callback cb,void* cb_arg)
{
	this->trace_cb_ptr=cb;
	this->trace_cb_arg=cb_arg;

	if(!this->callback_registered)
	{
		this->callback_registered=true;
		sc_core::sc_spawn_options opt;
		opt.spawn_method();


		sc_core::sc_spawn(sc_bind(&sca_con_interactive_trace_data::trace_callback,this),
			sc_core::sc_gen_unique_name("trace_callback"),&opt);
	}
}



void sca_con_interactive_trace_data::register_trace_callback(sca_util::sca_traceable_object::callback_functor_base& func)
{
	this->callbacks.push_back(&func);


	if(!this->callback_registered)
	{
		this->callback_registered=true;

		sc_core::sc_spawn_options opt;
		opt.spawn_method();

		sc_core::sc_spawn(sc_bind(&sca_con_interactive_trace_data::trace_callback,this),
			sc_core::sc_gen_unique_name("trace_callback"),&opt);
	}
}

bool sca_con_interactive_trace_data::remove_trace_callback(sca_util::sca_traceable_object::callback_functor_base& func)
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



//////////////////////////////////////////////////////////////////////////////

void sca_con_interactive_trace_data::trace_callback()
{
	if(module==NULL)
	{
		module=node->get_connected_module();

		if(module==NULL) //if not yet initialized we try the next delta
		{
			if(initialize_delta_waited) return;

			initialize_delta_waited=true;

			if(sc_core::sc_delta_count()<2)
			{
				next_trigger(sc_core::SC_ZERO_TIME);
			}

			return;
		}

		if(module->get_sync_domain()->get_calls_per_period()>1)
		{
			this->enable_multirate();
		}
	}

	if(node==NULL) this->enable_multirate();


	if(trace_callback_cnt<0)
	{
		trace_callback_cnt=0;
		sca_core::sca_time dtime=module->get_sync_domain()->get_cluster_start_time()
				- sc_core::sc_time_stamp();

		if(dtime>sc_core::SC_ZERO_TIME)
		{
			next_trigger(dtime);
			return;
		}
	}


	sc_dt::int64 available_sample=module->get_call_counter();

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

		//we cannot use get_timestep, due the call is maybe in an inconsistent state
		next_trigger(module->get_sync_domain()->get_cluster_period() /
				module->get_sync_domain()->get_calls_per_period());
	}
	else //if not wait on new cluster start
	{
		next_trigger(module->get_sync_domain()->get_cluster_synchronization_data()->cluster_start_event);
	}

}


////////////////////////////////////////////////////////////////////////////////

void sca_con_interactive_trace_data::construct()
{
	initialize_delta_waited=false;

	trace_callback_cnt=-1;
	last_pos=-1;

	trace_cb_ptr=NULL;
	trace_cb_arg=NULL;

	multirate=false;

	this->callback_registered=false;

	if(module==NULL) return;


	sca_core::sca_implementation::sca_solver_base* solv=module->get_sync_domain();

	if(solv==NULL) return;

	long calls_per_period=solv->get_calls_per_period();

	if(calls_per_period>1) //single rate cluster or module at lowest rate
	{
		enable_multirate();
	}
}



sca_con_interactive_trace_data::sca_con_interactive_trace_data(const sca_core::sca_implementation::sca_conservative_module* mod)
{
	node=NULL;
	module=const_cast<sca_core::sca_implementation::sca_conservative_module*>(mod);

	construct();

	//use always trace callback to get the value
	enable_multirate();
}

sca_con_interactive_trace_data::sca_con_interactive_trace_data(const sca_core::sca_implementation::sca_conservative_signal* cnode)
{
	node=const_cast<sca_core::sca_implementation::sca_conservative_signal*>(cnode);

	module=node->get_connected_module();

	construct();
}


//////////////////////////////////////////////////////////////////////////////

//stores values in the case of multirate
void sca_con_interactive_trace_data::store_value(double val)
{
	//may calls per period has been changed
	unsigned long calls_per_period=module->get_calls_per_period();
	buffer.resize(calls_per_period,0.0);

	last_pos=module->get_sync_domain()->get_call_number_in_current_period();

	buffer[last_pos]=val;
}

///////////////////////////////////////////////////////////////////////////////

void sca_con_interactive_trace_data::enable_multirate()
{
	if(multirate) return;

	long calls_per_period=module->get_calls_per_period();
	if(calls_per_period<0) return; //not yet initialized
	buffer.resize(calls_per_period,0.0);


	//add solver callback after each calculation

	if(node!=NULL)
	{
		data.trace_object=node;
	}
	else
	{
		data.trace_object=dynamic_cast<sca_util::sca_traceable_object*>(module);
	}

	if(data.trace_object!=NULL)
	{
		sca_core::sca_implementation::sca_solver_base* solv=module->get_sync_domain();
		if(solv==NULL) return;
		if(!module->get_sync_domain()->add_solver_trace(data)) return;
	}

	multirate=true;
}

//////////////////////////////////////////////////////////////////////////////

const double& sca_con_interactive_trace_data::get_value()
{
	if(module==NULL)
	{
		module=node->get_connected_module();
		if(module==NULL) return ZERO_VALUE;
	}


	//may calls per period has been changed
	unsigned long calls_per_period=module->get_calls_per_period();

	if((!multirate)||(last_pos<0))
	{
		if((calls_per_period>1) || (node==NULL))
		{
			enable_multirate();
		}

		double*res=NULL;

		if(node!=NULL)
		{
			res=node->get_result_ref();
		}

		if(res==NULL) return ZERO_VALUE;
		else          return *res;
	}


	buffer.resize(calls_per_period,0.0);


	//cluster start time
	sca_core::sca_time cstime=module->get_sync_domain()->get_cluster_start_time();

	//we cannot use get_timestep, due it refers may to inconsistent data
	sca_core::sca_time timestep=module->get_sync_domain()->get_cluster_period()/calls_per_period;

	sc_core::sc_time ctime=sc_core::sc_time_stamp();

	long sample=long((ctime-cstime).value()/timestep.value());

	//if SystemC before available sample time return always last available sample
	if(sample>last_pos)
	{
		return buffer[last_pos];
	}

	return buffer[sample];
}

///////////////////////////////////////////////////////////////////////////////


}
}
