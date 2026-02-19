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

 sca_synchronization_layer_process.cpp - description

 Original Author: Karsten Einwich Fraunhofer IIS Dresden / COSEDA Technologies

 Created on: 26.08.2009

 SVN Version       :  $Revision: 2320 $
 SVN last checkin  :  $Date: 2023-06-08 12:47:38 +0000 (Thu, 08 Jun 2023) $
 SVN checkin by    :  $Author: pehrlich $
 SVN Id            :  $Id: sca_synchronization_layer_process.cpp 2320 2023-06-08 12:47:38Z pehrlich $

 *****************************************************************************/

/*****************************************************************************/

#define SC_INCLUDE_DYNAMIC_PROCESSES
#include"systemc-ams"

#include "scams/impl/synchronization/sca_synchronization_layer_process.h"
#include "scams/impl/util/tracing/sca_trace_file_base.h"
#include "scams/impl/core/sca_simcontext.h"
#include "scams/impl/core/sca_object_manager.h"
#include "scams/impl/synchronization/sca_synchronization_layer.h"
#include "scams/impl/solver/linear/sca_linear_solver.h"
#include <algorithm>
#include <functional>

namespace sca_core
{
namespace sca_implementation
{


event_and_list2ev::event_and_list2ev(
		const sc_core::sc_event_and_list& list)
{
	method_started = false;

	and_list = list;

	sc_core::sc_spawn_options opt;
	opt.spawn_method();

	and_proc_h = sc_core::sc_spawn(
	sc_bind(&event_and_list2ev::and_event_list_process,this),
	sc_core::sc_gen_unique_name("and_event_list_process"),&opt);
}


event_and_list2ev::~event_and_list2ev()
{
	if(and_proc_h.valid())
	{
		and_proc_h.kill();
	}

	method_started=false;
}




void event_and_list2ev::and_event_list_process()
{
	if (!method_started)
	{
		next_trigger(and_list);
		method_started = true;
		this->cancel();

		return;
	}


	method_started = false;
	this->notify();
}


//////////////////////////////////////

sca_synchronization_layer_process::sca_synchronization_layer_process(
		sca_synchronization_alg::sca_cluster_objT* ccluster)
{
	cluster = ccluster;
	csync_data=&(cluster->csync_data);

	collect_profile_data=sca_core::sca_implementation::sca_get_curr_simcontext()->is_performance_data_collection_enabled();

	this->duration=std::chrono::duration<double>::zero();

	sc_core::sc_spawn_options opt;
	opt.spawn_method();

	sc_core::sc_spawn(
			sc_bind(&sca_synchronization_layer_process::cluster_process_control,this),
			sc_core::sc_gen_unique_name("cluster_process_control"),&opt);
}



//checks consistent setting of accept_attribut_changes and
//attribute_changes_allowed
void sca_synchronization_layer_process::change_attributes_check()
{
	csync_data->attribute_change_flags_changed=false;

	cluster->n_accept_attribute_changes=0;
	cluster->n_allowed_attribute_changes=0;
	cluster->last_not_accept_changes=NULL;
	cluster->last_attribute_changes=NULL;

	for(sca_synchronization_alg::sca_cluster_objT::iterator cit=cluster->begin();
				cit!=cluster->end();++cit)
	{
		//initial change attribute check -> flags transfered to sync_obj
		(*cit)->initialize_sync_obj();

		//initial attribute change check
		if((*cit)->attribute_changes_allowed)
		{
			cluster->n_allowed_attribute_changes++;
			//if more than one module allows attribute changes and does
			//not accept attribute changes, the stored module should be different
			//to the stored module for not accept attribute changes
			//otherwise the error message will become useless
			if((cluster->last_attribute_changes==NULL)||
					(*cit)->accepts_attribute_changes)
			{
				cluster->last_attribute_changes=(*cit);
			}
		}


		if((*cit)->accepts_attribute_changes) cluster->n_accept_attribute_changes++;
		else
		{
			if((cluster->last_not_accept_changes==NULL)||
					cluster->last_attribute_changes!=(*cit))
			{   //for the error message we will have a different module
				cluster->last_not_accept_changes=(*cit);
			}
		}

	}


	if( (cluster->n_allowed_attribute_changes>0) &&
	    (cluster->n_accept_attribute_changes < (long)cluster->size()))
	{
		//this will be an error except the only module which allows
		//attribute changes is the only module which does not accept
		//attribute changes
		if( (cluster->n_allowed_attribute_changes!=1)  ||
		    (cluster->last_attribute_changes!=cluster->last_not_accept_changes) ||
		    (cluster->n_accept_attribute_changes!=(long)cluster->size()-1))
		{
			std::ostringstream str;
			str << std::endl;
			str << "TDF module: " << cluster->last_attribute_changes->sync_if->get_name();
			str << " has set does_attribute changes(), " << std::endl;
			str << "however the module: ";
			str << cluster->last_not_accept_changes->sync_if->get_name();
			str << " in the same cluster has not set accept_attribute_changes()";
			SC_REPORT_ERROR("SystemC-AMS",str.str().c_str());
		}
	}

	//if it was no error and one module allows attribute changes
	if(cluster->n_allowed_attribute_changes>0) csync_data->is_dynamic_tdf=true;
	else                                       csync_data->is_dynamic_tdf=false;

	//all modules accept attribute changes
	if(cluster->n_accept_attribute_changes == (long)cluster->size())
		csync_data->attribute_changes_allowed=true;
	else
		csync_data->attribute_changes_allowed=false;

	//all modules except one allow attribute changes
	if(cluster->n_accept_attribute_changes == (long)cluster->size()-1)
		csync_data->attribute_changes_may_allowed=true;
	else
		csync_data->attribute_changes_may_allowed=false;


}

////////////////////////////////////////////////////////
///
///          cluster_initialization
///                   |
///             (next_trigger)
///                   |
///                  \ /
///            cluster_first_start
///                   |
///                  \ /
///        --->cluster_pre_scheduling
///        |          |<--------------------
///        |         \ /                   |
///        |   cluster_scheduling  - >  next_trigger
///        |          |
///        |         \ /
///        |   cluster_post_scheduling (static)
///        |          |
///        |      next_trigger
///        |          |
///        |         \ /
///        ----cluster_pre_next_scheduling (static)
///


void sca_synchronization_layer_process::cluster_process_control()
{
#ifndef DISABLE_PERFORMANCE_STATISTICS
	if(this->collect_profile_data)
	{
		this->activation_cnt++;
		start = std::chrono::high_resolution_clock::now();
	}
#endif

	switch(this->process_state)
	{
	case INITIALIZATION:
		cluster_initialization();
		break;
	case FIRST_START:
		cluster_first_start();
		break;
	case CONTINUE_SCHEDULING:
		cluster_scheduling();
		break;
	case PRE_NEXT_SCHEDULING:
		cluster_pre_next_scheduling();
		break;
	case STATIC_PRE_NEXT_SCHEDULING:
		static_cluster_pre_next_scheduling();
		break;
	default:
		SC_REPORT_ERROR("SystemC-AMS","Not possible error");
		return;
	}

#ifndef DISABLE_PERFORMANCE_STATISTICS
	if(this->collect_profile_data)
	{
		if((this->process_state==PRE_NEXT_SCHEDULING)|| (this->process_state==STATIC_PRE_NEXT_SCHEDULING))
		{
			this->cluster_finished_executions_cnt++;
		}

		this->duration+= std::chrono::high_resolution_clock::now()-start;
	}
#endif
}

//////////////////////////////////////

void sca_synchronization_layer_process::cluster_initialization()
{
	csync_data->cluster_start_time = NOT_VALID_SCA_TIME();

	csync_data->cluster_start_time = csync_data->requested_next_cluster_start_time;

	//last cluster period is always last non zero time
	if(csync_data->cluster_period!=sc_core::SC_ZERO_TIME)
	{
		csync_data->last_last_cluster_period=csync_data->last_cluster_period;
		csync_data->last_cluster_period = csync_data->cluster_period;
	}

	csync_data->current_cluster_timestep = csync_data->cluster_period;


	////////////////////////////////////////////////////////////////////////////////
	// wait for first start

	bool wait_for_time=false;
	bool wait_for_events=false;
	sca_core::sca_time first_activation_time;

	for(sca_synchronization_alg::sca_cluster_objT::iterator cit=cluster->begin();
				cit!=cluster->end();++cit)
	{
		sca_synchronization_obj_if* sync_if=(*cit)->sync_if;
		sca_core::sca_implementation::sca_synchronization_object_data* solver_time_data;
		solver_time_data=sync_if->get_synchronization_object_data();


		if(solver_time_data->first_activation_requested)
		{
			//collect all events
			for(unsigned long i=0;i<solver_time_data->first_activation_events.size();i++)
			{
				this->ev_vec.push_back(solver_time_data->first_activation_events[i]);
				wait_for_events=true;
			}
			solver_time_data->first_activation_events.clear();

			//collect all events from event_and_list processes
			for(unsigned long i=0;i<solver_time_data->first_activation_event_and_lists.size();i++)
			{
				this->ev_vec.push_back(&(*solver_time_data->first_activation_event_and_lists[i]));
				//store for deleting the converter events
				this->ev_and_list_vec.push_back(solver_time_data->first_activation_event_and_lists[i]);
				wait_for_events=true;
			}
			solver_time_data->first_activation_event_and_lists.clear();

			//collect all event_or_lists
			for(unsigned long i=0;i<solver_time_data->first_activation_event_or_lists.size();i++)
			{
				this->ev_or_list_vec.push_back(solver_time_data->first_activation_event_or_lists[i]);
				wait_for_events=true;
			}
			solver_time_data->first_activation_event_or_lists.clear();

			//resolve first activation time -> earliest wins if specified
			if(solver_time_data->first_activation_time_requested)
			{
				if(!wait_for_time)
				{
					first_activation_time=solver_time_data->first_activation_time;
					wait_for_time=true;
				}
				else
				{
					if(first_activation_time > solver_time_data->first_activation_time)
					{
						  first_activation_time=solver_time_data->first_activation_time;
					}
				}
			}
		}
	}


	//resolve max time step
	if(first_activation_time > csync_data->cluster_max_time_step)
	{
		first_activation_time=csync_data->cluster_max_time_step;
		wait_for_time=true;
	}


	if(wait_for_events)
	{
		if(this->ev_list.size()>0)  //empty list
		{
			sc_core::sc_event_or_list evtmp;
			this->ev_list.swap(evtmp); //empty list
		}

		//create event or least form all events

		//add all events to the or list

		for(unsigned int i=0;i < ev_vec.size(); i++)
		{
			this->ev_list|=*this->ev_vec[i];
		}

		//add all or lists to the or list
		for(unsigned int i=0;i < ev_or_list_vec.size(); i++)
		{
			this->ev_list|=this->ev_or_list_vec[i];
		}

		//wait for the list
		if(wait_for_time) sc_core::next_trigger(first_activation_time, this->ev_list);
		else              sc_core::next_trigger(this->ev_list);


	}
	else //if no list specified wait for time only if specified (otherwise no wait)
	{
		if( (wait_for_time) && (first_activation_time>sc_core::SC_ZERO_TIME))
		{
			sc_core::next_trigger(first_activation_time);
		}
		else
		{
			cluster_first_start();
			return;
		}
	}



	this->process_state=FIRST_START;

	return;
}

////////////////////////////////////////////

void sca_synchronization_layer_process::cluster_first_start()
{
	//clear list memory
	this->ev_or_list_vec.clear();
	this->ev_vec.clear();

	for(std::size_t i=0;i<this->ev_and_list_vec.size();++i)
	{
		delete this->ev_and_list_vec[i];
	}
	this->ev_and_list_vec.clear();

	this->last_start_time=sc_core::sc_time_stamp();
	this->csync_data->cluster_start_time=this->last_start_time;
	this->csync_data->last_cluster_start_time=sc_core::SC_ZERO_TIME;
	this->csync_data->last_last_cluster_start_time=sc_core::SC_ZERO_TIME;

	this->csync_data->cluster_end_time=last_start_time+csync_data->cluster_period -
				csync_data->cluster_period/csync_data->max_port_samples_per_period;


	this->csync_data->last_max_calls_per_period=csync_data->max_calls_per_period;

	//////////////////////////////////////////////////////////

	for(sca_synchronization_alg::sca_cluster_objT::iterator cit=this->cluster->begin();
				cit!=this->cluster->end();++cit)
	{
			//store linear solver to be able to store/restore check points
			sca_core::sca_implementation::sca_linear_solver* ls;
			ls=dynamic_cast<sca_core::sca_implementation::sca_linear_solver*>((*cit)->sync_if);
			if(ls!=NULL)
			{
				this->linear_solver.push_back(ls);
			}

	}

	change_attributes_check();

	//first cluster execution
	csync_data->cluster_execution_cnt=0;

	//call initialize methods
	for_each(
				this->cluster->begin(),
				this->cluster->end(),
				std::mem_fn(&sca_synchronization_alg::sca_sync_objT::call_init_method)
		    );

	cluster_pre_scheduling();
}


void sca_synchronization_layer_process::cluster_pre_scheduling()
{
	bool& all_traces_initialized =
					sca_core::sca_implementation::sca_get_curr_simcontext()->all_traces_initialized();

	//run processing methods
	if (!all_traces_initialized)
	{
		if(!sca_ac_analysis::sca_ac_is_running())
		{

			all_traces_initialized = true;

			std::vector<sca_util::sca_implementation::sca_trace_file_base*>* traces;

			traces=sca_core::sca_implementation::sca_get_curr_simcontext()->get_trace_list();

			for_each(traces->begin(),traces->end(),
		              std::mem_fn(&sca_util::sca_implementation::sca_trace_file_base::initialize));
		}
	}


	//for new dtdf features -> if we have a zero timestep we restore the
	//analog solver - otherwise we store a checkpoint
	if(csync_data->current_cluster_timestep==sc_core::SC_ZERO_TIME)
	{
		//restore checkpoint of analog solvers
		for_each(linear_solver.begin(),linear_solver.end(),
				              std::mem_fn(&sca_core::sca_implementation::sca_linear_solver::restore_checkpoint));
	}
	else
	{
		//store checkpoints for analog solvers
		for_each(linear_solver.begin(),linear_solver.end(),
	              std::mem_fn(&sca_core::sca_implementation::sca_linear_solver::store_checkpoint));
	}


	this->cluster_start_time=sc_core::sc_time_stamp();
	this->static_cluster_next_start_time=this->cluster_start_time+cluster->csync_data.cluster_period;


	csync_data->cluster_start_event.notify(sc_core::SC_ZERO_TIME); //required for interactive trace callbacks

	this->schedule_program_position=cluster->systemc_synchronizer.schedule_program.begin();
	this->schedule_position=cluster->scheduling_list.begin();

	cluster_scheduling();
}

///////////////////////////////////////
///

//simplified fast variant for static cluster
inline void sca_synchronization_layer_process::static_cluster_post_scheduling()
{
	//last cluster period is always last non zero time
	if(csync_data->cluster_period!=sc_core::SC_ZERO_TIME)
	{
		csync_data->last_last_cluster_period=csync_data->last_cluster_period;
		csync_data->last_cluster_period=csync_data->cluster_period;
	}

	sc_core::next_trigger(this->static_cluster_next_start_time-sc_core::sc_time_stamp());
	this->process_state=STATIC_PRE_NEXT_SCHEDULING;
}


//simplified fast variant for static cluster
void sca_synchronization_layer_process::static_cluster_pre_next_scheduling()
{
	this->last_start_time=sc_core::sc_time_stamp();
	this->cluster_start_time=this->last_start_time;
	this->static_cluster_next_start_time=this->cluster_start_time+cluster->csync_data.cluster_period;

	csync_data->cluster_start_event.notify(sc_core::SC_ZERO_TIME); //required for interactive trace callbacks

	this->schedule_program_position=cluster->systemc_synchronizer.schedule_program.begin();
	this->schedule_position=cluster->scheduling_list.begin();

	//next cluster execution starts
	csync_data->cluster_execution_cnt++;


	//we are now at cluster start time -> last_start_time is up to here current start time
	csync_data->last_last_cluster_start_time=csync_data->last_cluster_start_time;
	csync_data->last_cluster_start_time=csync_data->cluster_start_time;
	csync_data->cluster_start_time=this->cluster_start_time;

	csync_data->last_last_cluster_end_time=csync_data->last_cluster_end_time;
	csync_data->last_cluster_end_time=csync_data->cluster_end_time;


	//for multi rate the end time corresponds to the time of the last module call
	//the module with the highest rate (smallest time step)
	csync_data->cluster_end_time=last_start_time+csync_data->cluster_period -
			csync_data->cluster_period/csync_data->max_port_samples_per_period;

	csync_data->last_max_calls_per_period=csync_data->max_calls_per_period;

	for_each(active_reinitialize_vec.begin(),active_reinitialize_vec.end(),
			std::mem_fn(&sca_synchronization_alg::sca_sync_objT::
			call_reinit_method));

	bool& all_traces_initialized =
					sca_core::sca_implementation::sca_get_curr_simcontext()->all_traces_initialized();

	//run processing methods
	if (!all_traces_initialized)
	{
		if(!sca_ac_analysis::sca_ac_is_running())
		{

			all_traces_initialized = true;

			std::vector<sca_util::sca_implementation::sca_trace_file_base*>* traces;

			traces=sca_core::sca_implementation::sca_get_curr_simcontext()->get_trace_list();

			for_each(traces->begin(),traces->end(),
		              std::mem_fn(&sca_util::sca_implementation::sca_trace_file_base::initialize));
		}
	}




	cluster_scheduling();
}


void sca_synchronization_layer_process::cluster_scheduling()
{
	csync_data->cluster_executes=true;

	//run scheduling list in slices
	const std::uint64_t& scaled_period_time=cluster->systemc_synchronizer.scaled_time_period;
	const sca_core::sca_time& cluster_period=cluster->csync_data.cluster_period;


	std::uint64_t current_scaled_time=0;
	while(this->schedule_program_position != cluster->systemc_synchronizer.schedule_program.end())
	{
		auto& pstep = *this->schedule_program_position;

		if((pstep.first>current_scaled_time) && this->process_state!=CONTINUE_SCHEDULING)
		{
			//progress SystemC time
			sca_core::sca_time cstime=sc_core::sc_time_stamp()-this->cluster_start_time;  //SystemC time after cluster start
			sca_core::sca_time wtime=(pstep.first*cluster_period)/scaled_period_time-cstime;

			current_scaled_time=pstep.first;

			if(wtime>sc_core::SC_ZERO_TIME)
			{
				//std::cout << sc_core::sc_time_stamp() << " progress SystemC by: " << wtime << std::endl;
				sc_core::next_trigger(wtime);

				this->process_state=CONTINUE_SCHEDULING;
				return;
			}
		}

		this->process_state=SCHEDULING;

		for(const auto& act : pstep.second)
		{
			switch(act.action)
			{
			case sca_synchronization_alg::sca_systemc_sync_obj::action_struct::READ_SC:
			{
				act.port->port->read_from_sc();
				break;
			}
			case sca_synchronization_alg::sca_systemc_sync_obj::action_struct::SCHEDULE:
			{
				//execute all processing methods
				for_each(this->schedule_position,act.slice_end,
						              std::mem_fn(&sca_synchronization_alg::schedule_element::run));

				this->schedule_position=act.slice_end;
				break;
			}
			case sca_synchronization_alg::sca_systemc_sync_obj::action_struct::WRITE_SC:
			{
				act.port->port->write_to_sc();
				break;
			}
			default:
			{
				SC_REPORT_ERROR("SystemC-AMS","Should not be possible");
			}
			}
		}

		this->schedule_program_position++;
	}


	//////////////////////////////////////////////////////////


	csync_data->cluster_executes=false;

	//trace results
	for_each(csync_data->traces.begin(),csync_data->traces.end(),
	              std::mem_fn(&sca_util::sca_implementation::sca_trace_object_data::trace));

	csync_data->cluster_end_event.notify(sc_core::SC_ZERO_TIME); //required for interactive trace callbacks


	if(this->cluster_definitly_static)
	{
		static_cluster_post_scheduling();
	}
	else
	{
		this->call_change_attribute_methods();
		//cluster_post_scheduling();

		if(csync_data->is_dynamic_tdf || was_dynamic || is_first)
		{
			cluster_post_scheduling();
			was_dynamic=!is_first;
			is_first=false;
		}
		else
		{
			static_cluster_post_scheduling();
		}

	}

}

///////////////////////////////////////////////////

void sca_synchronization_layer_process::call_change_attribute_methods()
{
	///////////////////////////////////////////////////////////////////


	// new SystemC-AMS 2.0 dtdf extension
	csync_data->attributes_changed=false;

	//call all implemented change attributes methods
	if(this->first_ch_attr_call)  //in the first call we collect the available methods
	{
		for(sca_synchronization_alg::sca_cluster_objT::iterator cit=cluster->begin();
					cit!=cluster->end();++cit)
		{
			//check for implemented change_attributes
			if(!(*cit)->call_change_attributes_method())
			{
				this->active_ch_attr_vec.push_back(*cit);
			}
			else
			{
				if((*cit)->attribute_changes_allowed)
				{
					std::ostringstream str;
					str << "Module: " << (*cit)->sync_if->get_name();
					str << " has called does_attribute_changes(), but ";
					str << "not implemented the callback change_attributes()";
					SC_REPORT_WARNING("SystemC-AMS",str.str().c_str());
				}
			}
		}

		//if no change_attributes method is available - the cluster must be static
		if(this->active_ch_attr_vec.empty())
		{
			this->cluster_definitly_static=true;
		}

		this->first_ch_attr_call=false;
	}
	else //afterwards we call the available methods only
	{

		for_each(this->active_ch_attr_vec.begin(),this->active_ch_attr_vec.end(),
			std::mem_fn(&sca_synchronization_alg::sca_sync_objT::
					call_change_attributes_method));
	}

	/////////////////////////////////////////////////////////////////

	//if any does_attribute_change/accept_attribute_change, ...
	//changes the state the check is re-performed
	if(csync_data->attribute_change_flags_changed) change_attributes_check();
}

void sca_synchronization_layer_process::cluster_post_scheduling()
{
	//last cluster period is always last non zero time
	if(csync_data->cluster_period!=sc_core::SC_ZERO_TIME)
	{
		csync_data->last_last_cluster_period=csync_data->last_cluster_period;
		csync_data->last_cluster_period=csync_data->cluster_period;
	}


	//before the last execution the cluster was rescheduled
	//we must update scheduling variable
	if(csync_data->last_rescheduled)
	{
		for(sca_synchronization_alg::sca_cluster_objT::iterator cit=cluster->begin();
								cit!=cluster->end();++cit)
		{
			(*cit)->object_data->last_last_calls_per_period=(*cit)->object_data->last_calls_per_period;
			(*cit)->object_data->last_calls_per_period=(*cit)->object_data->calls_per_period;
		}

		csync_data->last_rescheduled=csync_data->rescheduled;
		csync_data->rescheduled=false;
	}


	//if rescheduling requested due rate or delay change - reschedule cluster
	if(csync_data->rescheduling_requested)
	{
		//update last variable
		for(sca_synchronization_alg::sca_cluster_objT::iterator cit=cluster->begin();
								cit!=cluster->end();++cit)
		{
			(*cit)->object_data->last_calls_per_period=(*cit)->object_data->calls_per_period;
		}

		csync_data->rescheduling_requested=false;
		csync_data->attributes_changed    =true;

		sca_get_curr_simcontext()->
				get_sca_object_manager()->
				get_synchronization_if()->reanalyze_cluster_timing(csync_data->cluster_id);

		csync_data->rescheduled=true;
		csync_data->last_rescheduled=true;
	}

	/////////////////////////////////////////////////////////////////
	//wait for next start



		this->reinitialize_timing=false;


		sca_core::sca_time ctime = sc_core::sc_time_stamp();

		//default (static case) the old period/timestep equals to the new
		sca_core::sca_time next_start_time = last_start_time + csync_data->cluster_period;

		//in this case the timestep remains constant
		this->expected_start_time=last_start_time + csync_data->current_cluster_timestep;


		if(csync_data->rescheduled)
		{
			//the next start time is calculated for the highest rate (smallest timestep)
			//of a sample in the cluster
			next_start_time = csync_data->cluster_end_time +
					csync_data->cluster_period/csync_data->max_port_samples_per_period;
			csync_data->attributes_changed=true;
		}


		//set_max_timestep
		if (csync_data->request_new_max_timestep)
		{
			if(csync_data->current_cluster_timestep!=csync_data->new_max_timestep)
			{
				csync_data->cluster_period=csync_data->new_max_timestep;

				//the next start time is calculated for the highest rate (smallest timestep)
				//of a sample in the cluster
				next_start_time = csync_data->cluster_end_time +
						csync_data->cluster_period/csync_data->max_port_samples_per_period;

				csync_data->attributes_changed=true;
				csync_data->cluster_max_time_step=csync_data->new_max_timestep;
			}
			csync_data->request_new_max_timestep = false;
		}

		//set_timestep (overrules set_max_timestep (otherwise it was an error)
		if(csync_data->change_timestep)
		{
			if(csync_data->new_timestep>csync_data->cluster_max_time_step)
			{
				std::ostringstream str;
				str << "timestep: " << csync_data->new_timestep;
				str << " of module: " << csync_data->new_timestep_obj->name();
				str << " set by set_timestep is larger than the maximum timestep: ";
				str << csync_data->cluster_max_time_step;
				str << " set by set_max_timestep of module: ";
				str << csync_data->max_timestep_obj->name();
				SC_REPORT_ERROR("SystemC-AMS",str.str().c_str());
			}


			if(csync_data->current_cluster_timestep!=csync_data->new_timestep)
			{
				csync_data->cluster_period=csync_data->new_timestep;
				//the next start time is calculated for the highest rate (smallest timestep)
				//of a sample in the cluster
				next_start_time = csync_data->cluster_end_time +
						csync_data->cluster_period/csync_data->max_port_samples_per_period;
				csync_data->attributes_changed=true;

				reinitialize_timing=true;

			}
			csync_data->change_timestep=false;
		}


		this->time_out_available=false;

		//request_next_activation
		if (csync_data->request_new_start_time)
		{
			time_out_available=true;

			next_start_time = csync_data->requested_next_cluster_start_time;

			//the max timestep is always related to the highest sample rate in the
			//cluster (the smallest timestep between sample)
			sca_core::sca_time time_limit=csync_data->cluster_end_time+
					csync_data->cluster_max_time_step/csync_data->max_port_samples_per_period;

			//limit timestep to max_timestep
			if(next_start_time>time_limit)
			{
				next_start_time=time_limit;
			}

			csync_data->request_new_start_time = false;
		}



		//request_next_activation with event
		if( (csync_data->reactivity_events.size() != 0) ||
		    (csync_data->reactivity_event_or_lists.size() != 0) ||
			(csync_data->reactivity_event_and_lists.size() != 0)  )
		{
			if(csync_data->cluster_max_time_step!=sca_core::sca_max_time())
			{
				sca_core::sca_time ntime=last_start_time + csync_data->cluster_max_time_step;
				if(!time_out_available || (ntime<next_start_time)) next_start_time=ntime;
				time_out_available=true;
			}

			//create global or list

			//add all events to or list

			if(this->ev_list.size()>0)  //empty list
			{
				sc_core::sc_event_or_list evtmp;
				this->ev_list.swap(evtmp); //empty list
			}

			for(unsigned int i=0;i < csync_data->reactivity_events.size(); i++)
			{
				this->ev_list|=*csync_data->reactivity_events[i];
			}

			//add all or lists to or list
			for(unsigned int i=0;i < csync_data->reactivity_event_or_lists.size(); i++)
			{
				this->ev_list|=csync_data->reactivity_event_or_lists[i];
			}

			//add events from and list processes to or list
			for(unsigned int i=0;i < csync_data->reactivity_event_and_lists.size(); i++)
			{
				this->ev_list|=*csync_data->reactivity_event_and_lists[i];
			}

			//wait on global event or list
			if(time_out_available) sc_core::next_trigger(next_start_time - ctime, ev_list);
			else                   sc_core::next_trigger(ev_list);


		}
		else //if no list specified wait on time
		{
			sc_core::next_trigger(next_start_time - ctime);
		}



		this->process_state=PRE_NEXT_SCHEDULING;
		return;
}

//////////////////////////////////////////////////////////////////

void sca_synchronization_layer_process::cluster_pre_next_scheduling()
{
	//clear list memory
	csync_data->reactivity_events.clear(); //delete event list
	csync_data->reactivity_event_or_lists.clear();

	for(std::size_t i=0;i<csync_data->reactivity_event_and_lists.size();++i)
	{
		delete csync_data->reactivity_event_and_lists[i];
	}
	csync_data->reactivity_event_and_lists.clear();

	////////////////////////////////////

	//we are now at cluster start time -> last_start_time is up to here current start time
	last_start_time=sc_core::sc_time_stamp();
	csync_data->current_cluster_timestep=last_start_time-csync_data->cluster_start_time; //store last period
	csync_data->last_last_cluster_start_time=csync_data->last_cluster_start_time;
	csync_data->last_cluster_start_time=csync_data->cluster_start_time;
	csync_data->cluster_start_time=last_start_time;

	csync_data->last_last_cluster_end_time=csync_data->last_cluster_end_time;

	//if no timestep the last cluster end time remains -> on this way the
	//timestep is repeated and get_lat_timestep returns the last non zero value
	if(csync_data->current_cluster_timestep>sc_core::SC_ZERO_TIME)
	{
		csync_data->last_cluster_end_time=csync_data->cluster_end_time;
	}


	//for multi rate the end time corresponds to the time of the last module call
	//the module with the highest rate (smallest time step)
	csync_data->cluster_end_time=last_start_time+csync_data->cluster_period -
			csync_data->cluster_period/csync_data->max_port_samples_per_period;


	csync_data->last_max_calls_per_period=csync_data->max_calls_per_period;


	//rate and delay changes will be detected during set in change_attributes->
	//if there is no rate change and the cluster_period and start time remains
	// -> there is no attribute change in the cluster
	if(this->expected_start_time!=this->last_start_time)
	{
		csync_data->attributes_changed=true;
	}


	if((csync_data->attributes_changed) && !csync_data->rescheduled)
	{
		if(sca_get_curr_simcontext()->
					get_sca_object_manager()->
					get_synchronization_if()->reinitialize_cluster_timing(csync_data->cluster_id))
		{
			csync_data->rescheduled=true;
		}
	}

	//now we schedule if required -> thus the timing can influence the scheduling
	if(csync_data->rescheduled)
	{
		sca_get_curr_simcontext()->
			get_sca_object_manager()->
			get_synchronization_if()->reschedule_cluster(csync_data->cluster_id);

		sca_get_curr_simcontext()->
							get_sca_object_manager()->
							get_synchronization_if()->reinitialize_cluster_datastructures(csync_data->cluster_id);
	}


	//////////////////////////////////////////////////////////////////

	//next cluster execution starts
	csync_data->cluster_execution_cnt++;

	//create unique id to enforce re-calculations if required
	if(csync_data->attributes_changed)
	{
		csync_data->attribute_change_id++;
	}

	//execute re-initialize
	//call all implemented re-initialize methods
	if(this->first_reinitialize_call) //first call we check whether method is available
	{
		for(sca_synchronization_alg::sca_cluster_objT::iterator cit=cluster->begin();
				cit!=cluster->end();++cit)
		{
			//check for implemented change_attributes
			if(!(*cit)->call_reinit_method())
			{
				this->active_reinitialize_vec.push_back(*cit);
			}
		}

		this->first_reinitialize_call=false;
	}
	else //afterwards we call available methods only
	{

		for_each(active_reinitialize_vec.begin(),active_reinitialize_vec.end(),
				std::mem_fn(&sca_synchronization_alg::sca_sync_objT::
				call_reinit_method));
	}

	cluster_pre_scheduling();  //now we start cluster scheduling
}


///////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////



sca_synchronization_layer_process::~sca_synchronization_layer_process()
{
}

}
}

