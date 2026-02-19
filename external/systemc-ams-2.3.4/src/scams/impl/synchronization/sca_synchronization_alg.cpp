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

 sca_synchronization_alg.cpp - description

 Original Author: Karsten Einwich Fraunhofer IIS/EAS Dresden

 Created on: 26.08.2009

 SVN Version       :  $Revision: 2320 $
 SVN last checkin  :  $Date: 2023-06-08 12:47:38 +0000 (Thu, 08 Jun 2023) $
 SVN checkin by    :  $Author: pehrlich $
 SVN Id            :  $Id: sca_synchronization_alg.cpp 2320 2023-06-08 12:47:38Z pehrlich $

 *****************************************************************************/

/*****************************************************************************/

#include <systemc-ams>
#include "scams/impl/synchronization/sca_synchronization_alg.h"
#include "scams/impl/solver/linear/sca_linear_solver.h"
#include "scams/impl/synchronization/sca_synchronization_layer_process.h"
#include "scams/impl/util/tracing/sca_trace_file_base.h"
#include "scams/impl/core/sca_simcontext.h"

#include<algorithm>
#include <functional>
#include<climits>

using namespace sca_util;
using namespace sca_util::sca_implementation;

namespace sca_core
{
namespace sca_implementation
{

sca_synchronization_alg::sca_sync_objT::sca_sync_objT()
{
	//cluster_id = -1;
	attribute_changes_allowed=false;
	accepts_attribute_changes=true;

	reinit_obj=NULL;

	reinit_meth=NULL;
    multiple=0;
    nout=0;
    object_data=NULL;
    sync_if=NULL;
    nin=0;

    current_context_ref=NULL;

    systemc_synchronizer=NULL;

    graph_order_id=0;

    next_schedule_element=0;
}

sca_synchronization_alg::sca_synchronization_alg()
{
	schedule_list_length = 0;
	scaled_time_lcm = 1;
	scheduling_list_warning_printed = false;

	sync_obj_mem = NULL;

	current_context=NULL;
}

sca_synchronization_alg::~sca_synchronization_alg()
{
	for (std::vector<sca_cluster_objT*>::iterator cit = clusters.begin(); cit
			!= clusters.end(); ++cit)
	{
		delete (*cit)->csync_mod;
		(*cit)->csync_mod=NULL;
		delete *cit;
		(*cit)=NULL;
	}

	delete[] sync_obj_mem;
	sync_obj_mem=NULL;
}


/** Initializes datastructures and calculates the scheduling list */
void sca_synchronization_alg::initialize(std::vector<
		sca_synchronization_obj_if*>& solvers)
{
	//allocate memory for synchronization cur_algorithm objects to perform
	//scheduling graph setup
	sync_obj_mem = new sca_sync_objT[solvers.size()];

	int id_cnt = 0;
	for (std::vector<sca_synchronization_obj_if*>::iterator sit =
			solvers.begin(); sit != solvers.end(); ++sit, ++id_cnt)
	{
		sync_objs.push_back(*sit);
		(*sit) -> get_synchronization_object_data()->id = id_cnt;

		//initialize objects for analyzer to speed up (re-) analyzing process
		sync_obj_mem[id_cnt].init(*sit);
		sync_obj_mem[id_cnt].current_context_ref=&current_context;
	}

	check_closed_graph();
	cluster();
#ifdef SCA_IMPLEMENTATION_DEBUG

	std::cout << "\t\tClustering finished: " << clusters.size()
	<< " cluster found" << std::endl;
#endif

	analyse_all_sample_rates();

#ifdef SCA_IMPLEMENTATION_DEBUG

	for(unsigned long i=0;i<clusters.size();i++)
	{
		if(clusters[i]->dead_cluster)
		std::cout << "!!!!!!!!!!! Cluster dead !!!!!!!!!!!!!!!" << std::endl;
		clusters[i]->print();
		std::cout << std::endl
		<< "------------------------------------------------------" << endl;
	}
#endif

	initialize_ports();


	initialize_systemc_synchronizers();



	generate_all_scheduling_lists();

#ifdef SCA_IMPLEMENTATION_DEBUG

	for(unsigned long i=0;i<clusters.size();i++)
	{
		clusters[i]->print_schedule_list();
		std::cout << std::endl
		<< "------------------------------------------------------" << std::endl;
	}
#endif

	std::ostringstream sta_str;
	sta_str << std::endl;
	sta_str << "\t" << clusters.size() << " dataflow clusters instantiated"
			<< std::endl;

	unsigned long& info_mask(
	    sca_core::sca_implementation::sca_get_curr_simcontext()->
	    get_information_mask());


	for (unsigned long i = 0; i < clusters.size(); i++)
	{

		if (!clusters[i]->dead_cluster)
		{
		  if(info_mask & sca_util::sca_info::sca_tdf_solver.mask)
		  {
			 unsigned long to_systemc=0;
			 unsigned long from_systemc=0;

			 for(sca_cluster_objT::iterator it=  clusters[i]->begin();
			 it!= clusters[i]->end();   it++)
			 {
			 to_systemc+=(*it)->sync_if->get_number_of_sync_ports(SCA_TO_SYSTEMC);
			 from_systemc+=(*it)->sync_if->get_number_of_sync_ports(SCA_FROM_SYSTEMC);
			 }

			 sta_str << "\t  cluster " << i << ":" << std::endl;
			 sta_str << "\t\t" << clusters[i]->size() << " dataflow modules/solver, ";
			 sta_str << "contains e.g. module: " << (*(clusters[i]->begin()))->sync_if->get_name_associated_names(5) << std::endl;
			 sta_str << "\t\t" << clusters[i]->schedule_list_length << " elements in schedule list,  " << std::endl;
			 sta_str << "\t\t" << clusters[i]->T_cluster<< " cluster period, " << std::endl;
			 sta_str << "\t\t"<< "ratio to lowest:  "  << clusters[i]->calls_per_period_max;
			 sta_str <<"              e.g. module: "<< clusters[i]->mod_name_period_max << std::endl;
			 sta_str << "\t\tratio to highest: "<< clusters[i]->calls_per_period_min << " sample time ";
			 sta_str <<" e.g. module: " << clusters[i]->mod_name_period_min << std::endl;
			 sta_str << "\t\t" << to_systemc << " connections to SystemC de, ";
			 sta_str << from_systemc << " connections from SystemC de";
			 sta_str << std::endl;
	           }

			clusters[i]->csync_mod = new sca_synchronization_layer_process(clusters[i]);
		}
	}


	if(info_mask & sca_util::sca_info::sca_tdf_solver.mask)
	{
	  SC_REPORT_INFO("SystemC-AMS",sta_str.str().c_str());
	}

}

///////////////////////////////////////////////////////////////////////////////

//Paranoid check which avoids some checking in the
//following steps
void sca_synchronization_alg::check_closed_graph()
{
	sca_synchronization_obj_if* sync_obj;
	sca_prim_channel* scaif;
	unsigned long nch;

	//for all sync-objects
	for (std::vector<sca_synchronization_obj_if*>::iterator sit =
			sync_objs.begin(); sit != sync_objs.end(); sit++)
	{
		//start wit inports (from analog)
		sca_port_direction_enum dir =SCA_FROM_ANALOG;
		do //for both directions
		{
			//check whether input - channels have one driver only
			nch = (*sit)->get_number_of_sync_ports(dir);

			for (unsigned long i = 0; i < nch; i++) //for all channels of one direction
			{
				//get channel
				scaif = dynamic_cast<sca_prim_channel*> ((*sit)->get_sync_port(
						dir, i)->channel);

				////// check that the connected channel has one driver only //////
				unsigned long ndriver =
						(unsigned long)(scaif->get_connected_solvers(true).size());
				if ((ndriver > 1) | (ndriver < 1))
				{
					std::ostringstream str;
					if (ndriver > 1)
						str
								<< "sca_tdf::sca_signal has more than one driver "
								<< "the following modules are connected to the signal: "
								<< std::endl;
					else
						str << "sca_tdf::sca_signal has no driver "
								<< "the following modules are connected to the channel: "
								<< std::endl;

					unsigned long nmod =
							scaif->get_number_of_connected_modules();
					for (unsigned long j = 0; j < nmod; j++)
					{
						str << "\t"
								<< scaif->get_connected_module_list()[j]->name()
								<< std::endl;
					}
					str << std::endl;
					SC_REPORT_ERROR("SystemC-AMS",str.str().c_str());
				}

				////// check that the connected channel has at least one sink //////
				unsigned long nsinks =
						(unsigned long)(scaif->get_connected_solvers(false).size());
				//        if(nsinks<1)
				//        {
				//          std::ostringstream str;
				//          str << "sca_synchronization channel has no sink "
				//              << "the following modules are connected to the channel: " << std::endl;
				//
				//          unsigned long nmod=scaif->get_number_of_connected_modules();
				//          for(unsigned long j=0;j<nmod;j++)
				//          {
				//            str << "\t"<< scaif->get_connected_module_list()[j]->name()<< std::endl;
				//          }
				//          str << std::endl;
				//          SC_REPORT_ERROR("SystemC-AMS",str.str().c_str());
				//        }

				//check driver solver first - only one exists
				sync_obj = scaif->get_connected_solvers(true)[0];
				sca_sync_obj_listT::iterator it;
				long k = -1;
				while (k < (long) nsinks)
				{
					//then check all sink solver
					if (k >= 0)
						sync_obj = scaif->get_connected_solvers(false)[k];
					k++;

					// check that the solver connected via the channel
					//  is also a member of the list
					it = std::find(sync_objs.begin(), sync_objs.end(), sync_obj);
					if (it == sync_objs.end())
					{
						std::ostringstream str;
						str << "Connected sca_synchronization object is not "
								<< "in the synchronization object list: "
								<< std::endl;
						SC_REPORT_ERROR("SystemC-AMS",str.str().c_str());
					}

					// check that the connected solver is only included one time
					it = std::find(++it, sync_objs.end(), sync_obj);
					if (it != sync_objs.end())
					{
						std::ostringstream str;
						str
								<< "Error: connected sca_synchronization object is more than "
								<< "one time in the synchronization object list: "
								<< std::endl;
						SC_REPORT_ERROR("SystemC-AMS",str.str().c_str());
					}
				}
			} //end for all channels


			//repeat with outports (to analog)
			if (dir == SCA_TO_ANALOG)
				dir = SCA_FROM_ANALOG;
			else
				dir = SCA_TO_ANALOG;

		} while (dir != SCA_FROM_ANALOG);
	} //next synchronization object
}

///////////////////////////////////////////////////////////////////////////////

void sca_synchronization_alg::cluster()
{

	//reset all variables to allow re-clustering
	for_each(sync_objs.begin(),sync_objs.end(),
	              std::mem_fn(&sca_synchronization_obj_if::reset));


	//std::vector<sca_synchronization_obj_if*>
	sca_sync_obj_listT::iterator sync_obj;

	sync_obj=sync_objs.begin();
	while (sync_obj != sync_objs.end())
	{
		//create new cluster
		long cluster_id;
		sca_cluster_objT* cluster_tmp = new sca_cluster_objT;
		sca_cluster_synchronization_data* cluster_data=&(cluster_tmp->csync_data);

		cluster_tmp->fwd_cnt=0;
		cluster_tmp->bwd_cnt=0;

		cluster_id = (long)(clusters.size());
		clusters.push_back(cluster_tmp);
		cluster_tmp->id=cluster_id;

		cluster_tmp->dead_cluster = false;
		cluster_tmp->csync_data.cluster_id=cluster_id;

		//get reference to sync object memory
		sca_sync_objT* current_obj;
		current_obj = &(sync_obj_mem[(*sync_obj)->get_synchronization_object_data()->id]);

		sca_synchronization_object_data* object_data=current_obj->object_data;

		//store cluster id and add obj to cluster
		object_data->cluster_id = cluster_id;
		object_data->sync_id = 0;
		cluster_tmp->push_back(current_obj);

		if (cluster_data->dead_cluster)
		{
			cluster_tmp->dead_cluster = true;
#ifdef SCA_IMPLEMENTATION_DEBUG

			std::cout << "----- dead module : " << (*sync_obj)->get_name_associated_names(5) << std::endl;
#endif

		}

		//rekursive method which moves all connected objs
		//to the cluster
		move_connected_objs(cluster_tmp, cluster_id, current_obj);


		//find not yet clustered object - start at last object -
		//before all objects must be clustered
		sync_obj = find_if(sync_obj, sync_objs.end(), std::mem_fn(
				&sca_synchronization_obj_if::not_clustered));

	}
}

///////////////////////////////////////////////////////////////////////////////
// if not yet done the sync_obj will be moved from the global to the cluster
// list -> than recursive call for the moved object
sca_synchronization_alg::sca_sync_objT*
sca_synchronization_alg::move_obj_if_not_done(
		sca_synchronization_obj_if* sync_obj, sca_cluster_objT* cluster_,
		long cluster_id,
		std::vector<sca_sync_objT*>& todo_list)
{
	sca_cluster_synchronization_data* cluster_data=&(cluster_->csync_data);
	sca_synchronization_object_data* object_data=sync_obj->get_synchronization_object_data();

	if (object_data->cluster_id < 0) //not yet found
	{
		sca_sync_objT* next_obj = &(sync_obj_mem[object_data->id]);

		//store cluster id and position in cluster
		object_data->cluster_id = cluster_id;
		object_data->sync_id = cluster_->size();
		cluster_->push_back(next_obj);     //store object in cluster

		//mechanism to support pre-ordering -> give chained modules
		//pre-ceding numbers
		if(cluster_->fwd_dir)
		{
			cluster_ ->fwd_cnt++;
			next_obj->graph_order_id=cluster_ ->fwd_cnt;
		}
		else
		{
			cluster_ ->bwd_cnt--;
			next_obj->graph_order_id=cluster_ ->bwd_cnt;
		}

		if (object_data->dead_cluster)
			cluster_->dead_cluster = true;

		//recursive method which moves all connected objects to the cluster
		//move_connected_objs(cluster_, cluster_id, next_obj);

		//prevent resize in every iteration
		if(todo_list.capacity()<=todo_list.size())
		{
			todo_list.reserve(todo_list.size()+1000);
		}
		todo_list.push_back(next_obj);

		return next_obj;
	}
	else if (cluster_data->dead_cluster)
	{
#ifdef SCA_IMPLEMENTATION_DEBUG
		std::cout << "----- dead module : " << sync_obj->get_name_associated_names(5) << std::endl;
#endif

		clusters[object_data->cluster_id]->dead_cluster = true;
	}

	return (*cluster_)[object_data->sync_id];
}

///////////////////////////////////////////////////////////////////////////////

void sca_synchronization_alg::move_connected_objs(sca_cluster_objT* cluster_,
		long cluster_id, sca_sync_objT* current_obj_)
{

	sca_cluster_synchronization_data* cluster_data=&(cluster_->csync_data);

	std::vector<sca_sync_objT*> todo_list;
	todo_list.reserve(1000); //prevent resize
	todo_list.push_back(current_obj_);

	while(todo_list.size()>0)
	{

		sca_sync_objT* current_obj=todo_list.back();
		todo_list.pop_back();

		//go first in the dirction of all outports
		unsigned long nop;
		nop = current_obj->sync_if->get_number_of_sync_ports(SCA_TO_ANALOG);

		for (unsigned long opnr = 0; opnr < nop; opnr++)
		{
			//get current outport
			sca_synchronization_port_data* cport;
			cport = current_obj->sync_if->get_sync_port(SCA_TO_ANALOG, opnr);

			//get sampling rate of current port
			long port_rate = *(cport -> rate);

			//initialize for scheduling analysis
			current_obj->out_rates[opnr] = port_rate;

			//get channel connected to current port
			//dynamic cast checked during add_port
			sca_prim_channel* scaif =
					dynamic_cast<sca_prim_channel*> (cport -> channel);

			if(cluster_id!=scaif->cluster_id)
			{
				scaif->cluster_id=cluster_id;
				cluster_->channels.push_back(scaif);
			}

			//an outport drives one or more inports but no other outport
			//get number of connected inports
			unsigned long ncip = (unsigned long)(scaif -> get_connected_sync_ports(false).size());
			current_obj->next_inports[opnr].resize(ncip);
			for (unsigned long cipnr = 0; cipnr < ncip; cipnr++)
			{
				//get next connected inport
				sca_synchronization_port_data *ciport =
						scaif->get_connected_sync_ports(false)[cipnr];

				//get solver which is of the sync_port
				sca_synchronization_obj_if* con_sync_obj = ciport -> parent_obj;

				if (port_rate != 0)
				{
					sca_sync_objT* con_alg_obj;
					//if not yet done it moves the object and all connected objects to
					//the cluster and returns a pointer to the corresponding
					//synchronization cur_algorithm object
					cluster_->fwd_dir=true;
					con_alg_obj = move_obj_if_not_done(con_sync_obj, cluster_, cluster_id,todo_list);

					//initialize datastructure for scheduling analysis
					//connect references to connected inports
					current_obj->next_inports[opnr][cipnr]
													= &(con_alg_obj->sample_inports[ciport->sync_port_number]);
					//initialize sample counter with number of delays
					*(current_obj->next_inports[opnr][cipnr]) += *(cport -> delay);
				}
				else
				{
#ifdef SCA_IMPLEMENTATION_DEBUG
				std::cout << "----- dead module : " << con_sync_obj->get_name_associated_names(5) << std::endl;
#endif
					sca_synchronization_object_data* object_data=con_sync_obj->get_synchronization_object_data();

					//dead clusters not yet supported
					std::ostringstream str;
					str << std::endl;
					str << "Rate=0 not allowed for port: "
						<< cport->port->sca_name() << std::endl << std::endl;
					SC_REPORT_ERROR("SystemC-AMS",str.str().c_str());

					cluster_data->dead_cluster = true;
					if (object_data->cluster_id >= 0)
						clusters[object_data->cluster_id]->dead_cluster = true;
				}
			} //for(cipnr=0;cipnr<ncip;cipnr++) -- for all connected inports
		} //for(unsigned long opnr=0;opnr<nop;opnr++) -- for all outports


		//now do the same for all inports
		unsigned long nip;
		nip = current_obj->sync_if->get_number_of_sync_ports(SCA_FROM_ANALOG);

		for (unsigned long ipnr = 0; ipnr < nip; ipnr++)
		{
			//get driving outport
			sca_synchronization_port_data* cport;
			cport = current_obj->sync_if->get_sync_port(SCA_FROM_ANALOG, ipnr);

			//get sampling rate of current port
			long port_rate = *(cport -> rate);

			current_obj->in_rates[ipnr] = port_rate;
			current_obj->sample_inports[ipnr] += *(cport -> delay);

			//get channel connected to current port
			sca_prim_channel* scaif =
					dynamic_cast<sca_prim_channel*> (cport -> channel);

			//an inport is driven by one outport only

			//get connected outport
			sca_synchronization_port_data *coport = scaif->get_connected_sync_ports(true)[0];

			//get solver of the sync outport
			sca_synchronization_obj_if* con_sync_obj = coport -> parent_obj;
			sca_synchronization_object_data* object_data=con_sync_obj->get_synchronization_object_data();

			if (port_rate != 0)
			{
				cluster_->fwd_dir=false;
				move_obj_if_not_done(con_sync_obj, cluster_, cluster_id,todo_list);
			}
			else
			{
				cluster_data->dead_cluster = true;
				if (object_data->cluster_id >= 0)
					clusters[object_data->cluster_id]->dead_cluster = true;

				std::ostringstream str;
				str << std::endl;
				str << "rate=0 not allowed for port: " << cport->port->sca_name()
					<< std::endl << std::endl;
				SC_REPORT_ERROR("SystemC-AMS",str.str().c_str());
			}

		} //for(unsigned long ipnr=0;ipnr<nip;ipnr++) -- for all inports
	} //while(todo_list.size()>0)
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////


unsigned long sca_synchronization_alg::sca_systemc_sync_obj::add_from_systemc_port(sca_synchronization_port_data* port,unsigned long calls_per_period)
{
	unsigned long id=scaled_time_per_call_from_systemc.size();
	scaled_time_per_call_from_systemc.resize(id+1);
	scaled_next_call_max_time_from_systemc.resize(id+1);
	scaled_next_from_systemc_time.resize(id+1);
	ports_from_systemc.resize(id+1);

	ports_from_systemc[id]=port;

	scale_time_to_lcm_for_rate(*port->rate); //scale time to lcm with rate to prevent rounding errors

	scaled_time_per_call_from_systemc[id]=scaled_time_period/calls_per_period; //no rounding error can occur due scaled_time_period is lcm
	std::uint64_t scaled_time_per_sc_step=scaled_time_per_call_from_systemc[id]/(*port->rate);

	//the minimum timestep can be used to calculate the maximum SC progress (cluster_period-mimimum step)
	if((this->scaled_min_timestep==0) || (this->scaled_min_timestep>scaled_time_per_sc_step))
	{
		this->scaled_min_timestep=scaled_time_per_sc_step;
	}

	scaled_next_call_max_time_from_systemc[id]=(scaled_time_per_sc_step*((*port->rate)-1) - (*port->delay)*scaled_time_per_sc_step);
	scaled_next_from_systemc_time[id]=0;

	return id;
}

unsigned long sca_synchronization_alg::sca_systemc_sync_obj::add_to_systemc_port(sca_synchronization_port_data* port,unsigned long calls_per_period)
{
	unsigned long id=scaled_time_per_call_to_systemc.size();
	scaled_time_per_call_to_systemc.resize(id+1);
	scaled_time_reached_to_systemc.resize(id+1);
	ports_to_systemc.resize(id+1);

	ports_to_systemc[id]=port;

	scale_time_to_lcm_for_rate(*port->rate); //scale time to lcm with rate to prevent rounding errors

	scaled_time_per_call_to_systemc[id]=scaled_time_period/calls_per_period; //no rounding error can occur due scaled_time_period is lcm

	std::uint64_t scaled_time_per_sc_step=scaled_time_per_call_to_systemc[id]/(*port->rate);

	//the minimum timestep can be used to calculate the maximum SC progress (cluster_period-mimimum step)
	if((this->scaled_min_timestep==0) || (this->scaled_min_timestep>scaled_time_per_sc_step))
	{
		this->scaled_min_timestep=scaled_time_per_sc_step;
	}


	scaled_time_reached_to_systemc[id]= (*port->delay)*scaled_time_per_sc_step;

	return id;
}



inline bool sca_synchronization_alg::sca_systemc_sync_obj::is_read_from_systemc_possible(unsigned long id)
{
	return scaled_next_call_max_time_from_systemc[id] <= (std::int64_t)scaled_time_reached;
}


void sca_synchronization_alg::sca_systemc_sync_obj::read_from_systemc(unsigned long id)
{
	unsigned long scaled_time_start=scaled_next_from_systemc_time[id];

	scaled_next_from_systemc_time[id]+=scaled_time_per_call_from_systemc[id];
	scaled_next_call_max_time_from_systemc[id]+=scaled_time_per_call_from_systemc[id];

	 sca_synchronization_port_data* port=ports_from_systemc[id];


	std::uint64_t scaled_time_per_sc_step=scaled_time_per_call_from_systemc[id]/(*port->rate);

	//add read synchronization points
	//the sample delay is realized by the read -> a delay does not change the synchronization time points
	std::uint64_t sct=scaled_time_start;
	for(unsigned long i=0;i<(*port->rate);++i)
	{
		action_struct action;
		action.action=action_struct::READ_SC;
		action.port=port;
		this->schedule_program[sct].push_back(action);

		sct+=scaled_time_per_sc_step;
	}

}


void sca_synchronization_alg::sca_systemc_sync_obj::write_to_systemc(unsigned long id)
{
	unsigned long scaled_time_start=scaled_time_reached_to_systemc[id];
	scaled_time_reached_to_systemc[id]+=scaled_time_per_call_to_systemc[id];

	 sca_synchronization_port_data* port=ports_to_systemc[id];

	std::uint64_t scaled_time_per_sc_step=scaled_time_per_call_to_systemc[id]/(*port->rate);
	scaled_time_start-=(*port->delay)*scaled_time_per_sc_step;

	//add write synchronization points
	//the sample delay is realized by the write -> it does not change the synchronization time points

	std::uint64_t sct=scaled_time_start;
	for(unsigned long i=0;i<(*port->rate);++i)
	{
		action_struct action;
		action.action=action_struct::WRITE_SC;
		action.port=port;
		this->schedule_program[sct].push_back(action);

		sct+=scaled_time_per_sc_step;
	}

}


std::uint64_t sca_synchronization_alg::sca_systemc_sync_obj::run_as_far_as_possible()
{
	std::uint64_t max_time=scaled_time_period;
	for(const long& rtime : scaled_time_reached_to_systemc)
	{
		if(rtime<0) max_time=0;
		else if((std::uint64_t)rtime < max_time) max_time=(std::uint64_t)rtime;
	}

	//we run not beyond the current cluster period
	if(max_time>(scaled_time_period-scaled_min_timestep)) max_time=scaled_time_period-scaled_min_timestep;

	scaled_time_reached=max_time;

	return scaled_time_reached;
}


void sca_synchronization_alg::sca_systemc_sync_obj::scale_time_to_lcm_for_rate(unsigned long rate)
{
	std::uint64_t lcm=sca_synchronization_alg::analyse_sample_rates_lcm(rate,scaled_time_period);

	std::uint64_t fac=lcm/scaled_time_period;

	if(fac<=1) return;

	scaled_time_period*=fac;
	scaled_time_reached*=fac;
	scaled_min_timestep*=fac;

	for(auto& sct : scaled_time_per_call_to_systemc) sct*=fac;
	for(auto& sct : scaled_time_reached_to_systemc) sct*=fac;

	for(auto& sct : scaled_time_per_call_from_systemc) sct*=fac;
	for(auto& sct : scaled_next_call_max_time_from_systemc) sct*=fac;

}

void sca_synchronization_alg::sca_systemc_sync_obj::sort_program_actions()
{
	for(auto& tp : schedule_program)
	{
		std::sort(tp.second.begin(),tp.second.end(),[](const action_struct& a,const action_struct& b)
				{
					if(a.action==b.action) return false;
					if(a.action==action_struct::READ_SC) return true; //begin of list
					if(a.action==action_struct::WRITE_SC) return false; //end of list

					//a must be schedule
					if(b.action==action_struct::WRITE_SC) return true; //schedule before write

					return false; //booth schedule
				});
	}
}



void sca_synchronization_alg::sca_systemc_sync_obj::reset()
{
	scaled_time_per_call_to_systemc.clear();
	scaled_time_reached_to_systemc.clear();

	scaled_time_per_call_from_systemc.clear();
	scaled_next_call_max_time_from_systemc.clear();

	schedule_program.clear();

	scaled_min_timestep=0;
}

void sca_synchronization_alg::initialize_ports(sca_cluster_objT* cluster)
{
	for(sca_sync_objT* sobj : *cluster)
	{
		sobj->systemc_synchronizer=&cluster->systemc_synchronizer;
		for(std::size_t nisc=0;nisc<sobj->sync_if->get_number_of_sync_ports(SCA_FROM_SYSTEMC);++nisc)
		{
			sca_synchronization_port_data* sport=sobj->sync_if->get_sync_port(SCA_FROM_SYSTEMC, nisc);
			sport->port->initialize_port();
		}

		for(std::size_t nosc=0;nosc<sobj->sync_if->get_number_of_sync_ports(SCA_TO_SYSTEMC);++nosc)
		{
			sca_synchronization_port_data* sport=sobj->sync_if->get_sync_port(SCA_TO_SYSTEMC, nosc);
			sport->port->initialize_port();
		}
	}
}

bool sca_synchronization_alg::reinitialize_port_timings(sca_cluster_objT* cluster)
{
	bool request_reschedule=false;

	for(sca_sync_objT* sobj : *cluster)
	{
		sobj->systemc_synchronizer=&cluster->systemc_synchronizer;
		for(std::size_t nisc=0;nisc<sobj->sync_if->get_number_of_sync_ports(SCA_FROM_SYSTEMC);++nisc)
		{
			sca_synchronization_port_data* sport=sobj->sync_if->get_sync_port(SCA_FROM_SYSTEMC, nisc);
			if(sport->port->reinitialize_port_timing())  request_reschedule=true;
		}

		for(std::size_t nosc=0;nosc<sobj->sync_if->get_number_of_sync_ports(SCA_TO_SYSTEMC);++nosc)
		{
			sca_synchronization_port_data* sport=sobj->sync_if->get_sync_port(SCA_TO_SYSTEMC, nosc);
			if(sport->port->reinitialize_port_timing())  request_reschedule=true;
		}
	}

	return request_reschedule;
}

void sca_synchronization_alg::reinitialize_ports(sca_cluster_objT* cluster)
{
	for(sca_sync_objT* sobj : *cluster)
	{
		sobj->systemc_synchronizer=&cluster->systemc_synchronizer;
		for(std::size_t nisc=0;nisc<sobj->sync_if->get_number_of_sync_ports(SCA_FROM_SYSTEMC);++nisc)
		{
			sca_synchronization_port_data* sport=sobj->sync_if->get_sync_port(SCA_FROM_SYSTEMC, nisc);
			sport->port->reinitialize_port();
		}

		for(std::size_t nosc=0;nosc<sobj->sync_if->get_number_of_sync_ports(SCA_TO_SYSTEMC);++nosc)
		{
			sca_synchronization_port_data* sport=sobj->sync_if->get_sync_port(SCA_TO_SYSTEMC, nosc);
			sport->port->reinitialize_port();
		}
	}
}


void sca_synchronization_alg::initialize_systemc_synchronizer(sca_cluster_objT* cluster)
{
		cluster->systemc_synchronizer.scaled_time_period=cluster->scaled_time_lcm;
		cluster->systemc_synchronizer.scaled_systemc_time_reached=0;

		cluster->systemc_synchronizer.scaled_time_reached=0;

		for(sca_sync_objT* sobj : *cluster)
		{
			sobj->systemc_synchronizer=&cluster->systemc_synchronizer;

			sobj->from_systemc_ports_ids.clear();
			for(std::size_t nisc=0;nisc<sobj->sync_if->get_number_of_sync_ports(SCA_FROM_SYSTEMC);++nisc)
			{
				sca_synchronization_port_data* sport=sobj->sync_if->get_sync_port(SCA_FROM_SYSTEMC, nisc);
				unsigned long id=cluster->systemc_synchronizer.add_from_systemc_port(sport,sobj->object_data->calls_per_period);
				sobj->from_systemc_ports_ids.push_back(id);
			}

			sobj->to_systemc_ports_ids.clear();
			for(std::size_t nosc=0;nosc<sobj->sync_if->get_number_of_sync_ports(SCA_TO_SYSTEMC);++nosc)
			{
				sca_synchronization_port_data* sport=sobj->sync_if->get_sync_port(SCA_TO_SYSTEMC, nosc);
				unsigned long id=cluster->systemc_synchronizer.add_to_systemc_port(sport,sobj->object_data->calls_per_period);
				sobj->to_systemc_ports_ids.push_back(id);
			}
		}
}


void sca_synchronization_alg::initialize_ports()
{
	for(std::size_t i=0;i<clusters.size();++i)
	{
		initialize_ports(clusters[i]);
	}
}

void sca_synchronization_alg::initialize_systemc_synchronizers()
{
	for(std::size_t i=0;i<clusters.size();++i)
	{
		initialize_systemc_synchronizer(clusters[i]);
	}
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
inline sc_dt::uint64 sca_synchronization_alg::analyse_sample_rates_gcd(
		sc_dt::uint64 x, sc_dt::uint64 y)
{
	if (y == 0)
		return x;
	else
		return analyse_sample_rates_gcd(y, x % y);
}

// lowest common multiple
inline unsigned long sca_synchronization_alg::analyse_sample_rates_lcm(
		unsigned long n1, unsigned long n2)
{
	if ((n1 == 0) || (n2 == 0))
		return 0;
	return ((sc_dt::uint64) n1 * (sc_dt::uint64) n2)
			/ analyse_sample_rates_gcd(n1, n2);
}

///////////////////////////////////////////////////////////////////////////////

inline bool sca_synchronization_alg::analyse_sample_rates_first_obj(
		sca_cluster_objT* current_cluster,
		unsigned long& nin, unsigned long& nout, long& multiplier,
		sca_synchronization_alg::sca_sync_objT* const& obj,
		sca_cluster_objT::iterator& sit)
{
	sca_prim_channel* ch;
	sca_synchronization_port_data* sport;

	long port_rate;
	bool p_found = false;

	//start with multiplier 1
	multiplier = 1;

	sca_synchronization_object_data* sync_obj_data;
	sync_obj_data=obj->sync_if->get_synchronization_object_data();

	//assign object calls -> try 1 for the first object
	sync_obj_data->calls_per_period = 1;
	obj->multiple = multiplier;

	check_sample_time_consistency(current_cluster,obj, NULL, multiplier);

	//start on first inport
	for (unsigned long i = 0; i < nin; i++)
	{
		sport = obj->sync_if-> get_sync_port(SCA_FROM_ANALOG, i);
		port_rate = *(sport->rate);

		if (port_rate > 0)
		{
			p_found = true;
			ch = sport->channel;

			ch->max_samples = *(sport->rate);
			ch->multiple = multiplier;
			break;
		}
	}

	for (unsigned long i = 0; (i < nout) & !p_found; i++)
	{
		sport = obj->sync_if-> get_sync_port(SCA_TO_ANALOG, i);
		port_rate = *(sport->rate);

		if (port_rate > 0)
		{
			p_found = true;
			ch = sport->channel;

			ch->max_samples = *(sport->rate);
			ch->multiple = multiplier;
			break;
		}
	}

	if (!p_found)
	{
		//cluster consists of one object only -> no channel to another object
		sit++;
		if (sit != current_cluster->end())
		{
			// Error should not be possible -> Error in cluster() ??
			std::ostringstream str;
			str << "Internal Error in: "
					<< "sca_synchronization_alg::analyse_sample_rates() "
					<< __FILE__ << " line: " << __LINE__ << std::endl;
			SC_REPORT_ERROR("SystemC-AMS",str.str().c_str());
		}
		sit--;

		//such an solver will be called once per cluster period
		sync_obj_data->calls_per_period = 1;
		//obj->sync_if->get_calls_per_period() = 1;

		//in this case we have maybe synchronization ports with a time assigned

		//check timing of synchronization ports
		unsigned long nin_sc = obj->sync_if-> get_number_of_sync_ports(SCA_FROM_SYSTEMC);
		unsigned long nout_sc = obj->sync_if-> get_number_of_sync_ports(SCA_TO_SYSTEMC);

		for (unsigned long i = 0; i < (nin_sc + nout_sc); i++)
		{
			//first for all inports
			if (i < nin_sc)
				sport = obj->sync_if->get_sync_port(SCA_FROM_SYSTEMC, i);
			else
				sport = obj->sync_if->get_sync_port(SCA_TO_SYSTEMC, i - nin_sc);

			check_sample_time_consistency(current_cluster,obj, sport, 1);
		}
		return true; //cluster ready - no more objects
	}

	return false; //there must be more objects
}

///////////////////////////////////////////////////////////////////////////////

inline void sca_synchronization_alg::analyse_sample_rates_calc_obj_calls(
		unsigned long& nin, unsigned long& nout, long& multiplier,
		sca_synchronization_alg::sca_sync_objT* const& obj)
{
	sca_prim_channel* ch;
	sca_synchronization_port_data* sport;

	//search for sync_port channel which is yet calculated
	bool ch_found = false;

	sc_dt::int64 ch_max_sample;
	long port_rate;
	sc_dt::int64 n_sample;

	sca_synchronization_object_data* sync_obj_data;
	sync_obj_data=obj->sync_if->get_synchronization_object_data();

	sync_obj_data->calls_per_period = 0;

	//search on all inports and outports
	for (unsigned long i = 0; i < (nin + nout); i++)
	{
		//search first on all inports
		if (i < nin)
			sport = obj->sync_if-> get_sync_port(SCA_FROM_ANALOG, i);
		else
			sport = obj->sync_if-> get_sync_port(SCA_TO_ANALOG, i - nin);

		port_rate = *(sport->rate);

		// ignore ports with rate zero
		if (port_rate == 0)
			continue;

		ch = sport->channel;
		//ch->multiple > 0; multiplier > 0

		//we must be careful to prevent integer overflows
		//the current multiplier is >= the ch->multiplier and is always
		//a multiple of the ch->multiplier -> thus we can divide first
		//and than multiply -> we get no temporarly oferflow
		ch_max_sample = (sc_dt::int64) (ch->max_samples) * (sc_dt::int64) (multiplier
				/ ch->multiple);

		//if we have nevertheless an overflow we cannot handle this
		//we will need at least one buffer of several Giga - this should
		//exceed usually the memory limits
		if (ch_max_sample >= LONG_MAX)
		{
			std::ostringstream str;
			str << " Bad conditioned synchronization cluster integer "
					<< "limit reached for buffer size of: " << ch->name()
					<< std::endl;
			SC_REPORT_ERROR("SystemC-AMS",str.str().c_str());
		}

		if (ch_max_sample > 0)
		{
			ch_found = true;

			//lowest common multiple
			n_sample = analyse_sample_rates_lcm(ch_max_sample, port_rate);
			if (n_sample < 0)
			{
				std::ostringstream str;
				str
						<< " Error bad conditioned synchronization cluster integer "
						<< "limit reached for buffer size of: " << ch->name()
						<< std::endl;
				SC_REPORT_ERROR("SystemC-AMS",str.str().c_str());
			}

			if (n_sample >= LONG_MAX)
			{
				std::ostringstream str;
				str << " Bad conditioned synchronization cluster integer "
						<< "limit reached for buffer size of: " << ch->name()
						<< std::endl;
				SC_REPORT_ERROR("SystemC-AMS",str.str().c_str());
			}

			multiplier = (n_sample / ch_max_sample) * multiplier;

			ch -> max_samples = n_sample;
			ch -> multiple = multiplier;

			sync_obj_data->calls_per_period = n_sample / port_rate;
			obj->multiple = multiplier;

			break; //obj-call defined can stop
		}
	} //end for all ports

	if (!ch_found)
	{
		// Error should not be possible
		std::ostringstream str;
		str << "Internal Error in: " << __FILE__ << " line: " << __LINE__
				<< " assumption for sync-algorithm seems not to be fullfilled"
				<< " in cluster: " << obj->sync_if->get_synchronization_object_data()->cluster_id << std::endl;
		SC_REPORT_ERROR("SystemC-AMS",str.str().c_str());
	}
}

///////////////////////////////////////////////////////////////////////////////

inline void sca_synchronization_alg::check_sample_time_consistency(
		sca_cluster_objT* current_cluster,
		sca_synchronization_alg::sca_sync_objT* const& obj,
		sca_synchronization_port_data* sport, long multiplier)
{
	long n_sample=0;
	sca_core::sca_time c_timestep(NOT_VALID_SCA_TIME());

	sca_synchronization_object_data* sync_obj_data;
	sync_obj_data=obj->sync_if->get_synchronization_object_data();

	//get global id of last timestep change
	sc_dt::int64 ts_ch_id=current_cluster->csync_data.last_timestep_change_id;

	if (sport != NULL)
	{
		n_sample = (*(sport->rate)) * sync_obj_data->calls_per_period;
		//search for assigned T (sample time)
		c_timestep = *(sport->T_set); //if not set timestep is not valid
		if(ts_ch_id==*(sport->timestep_change_id))
		{
			current_cluster->is_max_timestep=false;
		}
		else
		{
			return;
		}
	}
	else
	{
		sca_synchronization_object_data* solver_time_data=obj->sync_if->get_synchronization_object_data();

		//propagate max timestep
		if(solver_time_data->timestep_max_is_set)
		{
			n_sample = sync_obj_data->calls_per_period;
			sca_core::sca_time mtime=n_sample*solver_time_data->timestep_max_set;
			if(mtime<current_cluster->T_max_cluster * multiplier / current_cluster->T_max_multiplier)
			{
				current_cluster->T_max_cluster=mtime;
				current_cluster->T_max_multiplier=multiplier;
				current_cluster->T_max_last_obj=obj;

				current_cluster->csync_data.max_timestep_obj=solver_time_data->module_min_timestep_set;
			}
		}


		if (solver_time_data->timestep_is_set)
		{
			c_timestep = solver_time_data->timestep_set;
			n_sample = sync_obj_data->calls_per_period;
			current_cluster->is_max_timestep=false;
		}
	}



	if (c_timestep == NOT_VALID_SCA_TIME()) return;


	if (current_cluster->T_last_obj == NULL) //no timestep previously set
	{
		current_cluster->T_last_n_sample = n_sample;
		current_cluster->T_cluster = c_timestep * n_sample;
		current_cluster->T_multiplier = multiplier;
		current_cluster->T_last_obj = obj;
		current_cluster->T_last_port = sport;
		current_cluster->T_last = c_timestep;

		return;
	}

	//check consistency
	sca_core::sca_time T_expect = c_timestep;
	sca_core::sca_time T_ref = current_cluster->T_last;

	sc_dt::uint64 ti_exp = T_expect.value();
	sc_dt::uint64 ti_set = current_cluster->T_last.value();

	//scaling last n_sample value to current multiplier
	long last_n_sample = current_cluster->T_last_n_sample * multiplier
			/ current_cluster->T_multiplier;

	sc_dt::int64 ti_dt;


	//we use the largest assigned sample rate
	//for error calculation we scale always to the smaller T
	//thus the maximum error is +/- 1
	//( ti_exp > ti_set ) -> larger buffer, smaller T
	if (last_n_sample > n_sample)
	{
		if(n_sample==0)
		{
			SC_REPORT_ERROR("SystemC-AMS","Internal error n_sample=0");
			return;
		}

		//scaling to smaller old T
		ti_dt = ti_set - ti_exp / (last_n_sample / n_sample);

		//we store larger T fur cluster
		current_cluster->T_last_n_sample = n_sample;
		current_cluster->T_cluster = T_expect * n_sample;
		current_cluster->T_multiplier = multiplier;
		current_cluster->T_last_obj = obj;
		current_cluster->T_last_port = sport;
		current_cluster->T_last = c_timestep;
	} else
	{
		if(last_n_sample==0)
		{
			SC_REPORT_ERROR("SystemC-AMS","Internal error last_n_sample=0");
			return;
		}

		//scaling to smaller current T
		ti_dt = ti_set / (n_sample / last_n_sample) - ti_exp;
	}

	//we allow 2 resolution time units errors
	//(one for double conversion; second for divider)
	if ((ti_dt > 2) || (ti_dt < -2)) //llabs / abs is platform dependent
	{
		sca_core::sca_time T_cur;
		T_cur = current_cluster->T_cluster * (multiplier
				/ current_cluster->T_multiplier);

		long nk = (last_n_sample > n_sample) ? last_n_sample / n_sample : 1;
		sca_core::sca_time T_expect;
		if (ti_dt < 0)
			T_expect = c_timestep - sc_core::sc_get_time_resolution() * double(
					-ti_dt * nk);
		else
			T_expect = c_timestep + sc_core::sc_get_time_resolution() * double(
					ti_dt * nk);

		long nkr = (last_n_sample < n_sample) ? n_sample / last_n_sample : 1;
		sca_core::sca_time T_ref_expect;
		if (ti_dt < 0)
			T_ref_expect = T_ref + sc_core::sc_get_time_resolution() * double(
					-ti_dt * nkr);
		else
			T_ref_expect = T_ref - sc_core::sc_get_time_resolution() * double(
					ti_dt * nkr);

		std::ostringstream str;
		str << std::endl << std::endl;
		str << "Inconsistency in timestep assignment between" << std::endl;
		if (sport == NULL)
		{
			str << " module: "
					<< obj->sync_if->get_synchronization_object_data()->module_timestep_set->name();
		} else
		{
			str << "\t port: " << sport->port->sca_name();
		}
		str << "\t timestep: " << c_timestep << " (" << 1.0
				/ c_timestep.to_seconds() << " Hz)" << "\t\t   expect: "
				<< T_expect << " (" << 1.0 / (T_expect.to_seconds()) << " Hz)"
				<< std::endl << "\t and ";
		if (current_cluster->T_last_port == NULL)
		{
			str << " module: "
					<< current_cluster->T_last_obj->sync_if->get_synchronization_object_data()->module_timestep_set->name();
		} else
		{
			str << current_cluster->T_last_port->port->sca_name();
		}
		str << "\t T: " << T_ref << " (" << 1.0 / T_ref.to_seconds() << " Hz)"
				<< "\t\tor expect: " << T_ref_expect << " (" << 1.0
				/ T_ref_expect.to_seconds() << " Hz)" << std::endl << std::endl
				<< "\t rate1/rate2= ";
		if (n_sample > last_n_sample)
		{
			str << n_sample / last_n_sample << "/1";
		} else {
			str << "1/" << last_n_sample / n_sample;
		}
		str << "\t dT: ";
		if (ti_dt < 0)
			str << "-" << sc_core::sc_get_time_resolution() * double(-ti_dt)
					<< std::endl;
		else
			str << sc_core::sc_get_time_resolution() * double(ti_dt)
					<< std::endl;
		str << std::endl;

		SC_REPORT_ERROR("SystemC-AMS",str.str().c_str());
	}

}

inline void sca_synchronization_alg::analyse_sample_rates_assign_max_samples(
		sca_cluster_objT* current_cluster,
		unsigned long& nin, unsigned long& nout, long& multiplier,
		sca_synchronization_alg::sca_sync_objT* const& obj)
{
	sca_prim_channel* ch;
	sca_synchronization_port_data* sport;

	sca_synchronization_object_data* sync_obj_data;
	sync_obj_data=obj->sync_if->get_synchronization_object_data();

	sc_dt::int64 ch_max_sample;
	long port_rate;
	sc_dt::int64 n_sample;

	check_sample_time_consistency(current_cluster,obj, NULL, multiplier);

	// assign max_samples to all connected channels
	// if yet assigned check consistency
	for (unsigned long i = 0; i < (nin + nout); i++)
	{
		//search first on all inports
		if (i < nin)
			sport = obj-> sync_if->get_sync_port(SCA_FROM_ANALOG, i);
		else
			sport = obj-> sync_if->get_sync_port(SCA_TO_ANALOG, i - nin);

		port_rate = *(sport->rate);

		ch = sport->channel;
		//multiplier > 0 ; ch->multiple > 0
		ch_max_sample = (sc_dt::int64) ch->max_samples * (sc_dt::int64) (multiplier
				/ ch->multiple);
		n_sample = port_rate * (sc_dt::int64) (sync_obj_data->calls_per_period);

		if ((ch_max_sample >= LONG_MAX) || (n_sample >= LONG_MAX))
		{
			std::ostringstream str;
			str << " Bad conditioned synchronization cluster integer "
					<< "limit reached for buffer size of: " << ch->name()
					<< std::endl;
			SC_REPORT_ERROR("SystemC-AMS",str.str().c_str());
		}

		check_sample_time_consistency(current_cluster,obj, sport, multiplier);

		if (ch_max_sample > 0) //check consistency
		{
			//Due the assumption for the cur_algorithm this condition must be
			//fullfilled, because all assigned
			//max_samples resulting from previously analyzed
			//modules/ports (in other words a change of those max_sample results
			//in a change of all yet assigned max_samples/obj_calls -> due it is
			//at least the second (the first was assigned above) analyzed
			//port/channel of the object consistency can't be achieved)
			if (ch_max_sample != n_sample)
			{
				std::ostringstream str;
				str << "Sample rates are inconsistent in : "
						<< obj -> sync_if->get_name_associated_names(5) << " at ";
				if (i < nin)
					str << "inport " << i << std::endl;
				else
					str << "outport " << i - nin
					<< " ch_max_sample: " << ch_max_sample
					<< " n_sample: " << n_sample<< std::endl;

				SC_REPORT_ERROR("SystemC-AMS",str.str().c_str());
			}
		}
		else //assign max_sample
		{
			ch -> max_samples = n_sample;
			ch -> multiple = multiplier;
		}
	} //for all ports

	//check timing of synchronization ports
	unsigned long nin_sc = obj->sync_if-> get_number_of_sync_ports(SCA_FROM_SYSTEMC);
	unsigned long nout_sc = obj->sync_if-> get_number_of_sync_ports(SCA_TO_SYSTEMC);

	for (unsigned long i = 0; i < (nin_sc + nout_sc); i++)
	{
		//first for all inports
		if (i < nin_sc)
			sport = obj->sync_if->get_sync_port(SCA_FROM_SYSTEMC, i);
		else
			sport = obj->sync_if->get_sync_port(SCA_TO_SYSTEMC, i - nin_sc);

		check_sample_time_consistency(current_cluster,obj, sport, multiplier);
	}
}

///////////////////////////////////////////////////////////////////////////////

inline void sca_synchronization_alg::analyse_sample_rates_calc_def_rates(
		sca_cluster_objT* current_cluster,
		long& multiplier, sca_synchronization_alg::sca_sync_objT * const& obj)
{
	sca_synchronization_object_data* sync_obj_data;
	sync_obj_data=obj->sync_if->get_synchronization_object_data();

	//obj->multiple can't be zero
	sc_dt::int64 calls_per_period;
	calls_per_period = (sc_dt::int64) (sync_obj_data->calls_per_period)
			* (sc_dt::int64) (multiplier / obj->multiple);

	sync_obj_data->calls_per_period = calls_per_period;
	//obj->sync_if->get_calls_per_period() = calls_per_period_timestep_ch_obj;

	//assign calculated module timestep
	obj->sync_if->get_synchronization_object_data()->timestep_calculated=current_cluster->csync_data.cluster_period/calls_per_period;

	//collect some infos which will speed-up the schedule-list build
	schedule_list_length += calls_per_period;
	scaled_time_lcm = analyse_sample_rates_lcm(scaled_time_lcm,
			calls_per_period);

	//this is heuristic - we warn due it can took very long for such lists
	//and may it comes anywhere an bad_alloc exception
	if (schedule_list_length > 10000000)
	{
		if (!scheduling_list_warning_printed)
		{
			std::ostringstream str;
			str << "The cluster containing module: "
					<< obj -> sync_if->get_name_associated_names(5)
					<< " results in an extremely long scheduling list - thus the memory"
					<< " space requirement will be very high - may the cluster is bad conditioned"
					<< " or it has very large sample rate ratios the current length is: "
					<< schedule_list_length;
			SC_REPORT_WARNING("SystemC-AMS",str.str().c_str());
			scheduling_list_warning_printed = true;
		}

		if (schedule_list_length >= LONG_MAX)
		{
			SC_REPORT_ERROR("SystemC-AMS","Maximum scheduling list length exceeded");
		}
	}

	//number of channels to the object (inports)
	unsigned long nin = obj->nin;
	//number of channels from the object (outports)
	unsigned long nout = obj->nout;

	sca_synchronization_port_data* sport;

	//for all inports and outports
	for (unsigned long i = 0; i < (nin + nout); i++)
	{
		//first for all inports
		if (i < nin)
			sport = obj->sync_if->get_sync_port(SCA_FROM_ANALOG, i);
		else
			sport = obj->sync_if->get_sync_port(SCA_TO_ANALOG, i - nin);

		//delays not considered -> considered in concrete buffer implementation
		sc_dt::int64 max_sample = (sc_dt::int64) (sport->channel->max_samples)
				* (multiplier / sport->channel->multiple);

		if (max_sample >= LONG_MAX)
		{
			std::ostringstream str;
			str << " Bad conditioned synchronization cluster integer "
					<< "limit reached for buffer size of: "
					<< sport->channel->name() << std::endl;
			SC_REPORT_ERROR("SystemC-AMS",str.str().c_str());
		}

		//set calculated T of port
		sport->T_calculated = current_cluster->csync_data.cluster_period
				/ max_sample;

		if(max_sample>(sc_dt::int64)(current_cluster->csync_data.max_port_samples_per_period))
		{
			current_cluster->csync_data.max_port_samples_per_period=max_sample;
		}

		*(sport->T_set) = sport->T_calculated;

		if(sport->T_calculated==sc_core::SC_ZERO_TIME)
		{
			std::ostringstream str;
			str << "Timestep of port: " << sport->port->sca_name()
			    << " becomes zero (may resolution time is to high ) "
				<< " current cluster period: " << current_cluster->csync_data.cluster_period
				<< " divider: " << max_sample;
			SC_REPORT_ERROR("SystemC-AMS",str.str().c_str());
			return;
		}

		//ch->multiple should not be 0
		sport->channel->max_samples = max_sample;
		sport->channel->multiple = multiplier;

	}

	//asign timing to synchronization ports
	unsigned long nin_sc = obj->sync_if-> get_number_of_sync_ports(SCA_FROM_SYSTEMC);
	unsigned long nout_sc = obj->sync_if-> get_number_of_sync_ports(SCA_TO_SYSTEMC);

	for (unsigned long i = 0; i < (nin_sc + nout_sc); i++)
	{
		//first for all inports
		if (i < nin_sc)
			sport = obj->sync_if->get_sync_port(SCA_FROM_SYSTEMC, i);
		else
			sport = obj->sync_if->get_sync_port(SCA_TO_SYSTEMC, i - nin_sc);

		sc_dt::uint64 max_sample;
		max_sample=(sync_obj_data->calls_per_period * *(sport->rate));
		//set calculated T of port
		sport->T_calculated = current_cluster->csync_data.cluster_period
				/ max_sample;

		if(max_sample>current_cluster->csync_data.max_port_samples_per_period)
		{
			current_cluster->csync_data.max_port_samples_per_period=max_sample;
		}


		//write calculated values back
		*(sport->T_set) = sport->T_calculated;
	}
}

///////////////////////////////////////////////////////////////////////////////

void sca_synchronization_alg::analyse_sample_rates(sca_cluster_objT* current_cluster)
{
		scaled_time_lcm = 1;
		schedule_list_length = 0;

		//ignore dead clusters
		if (current_cluster->dead_cluster) return;

		//from the first object all others shuold be reachable
		sca_cluster_objT::iterator sit = current_cluster->begin();

		if (sit == current_cluster->end()) //cluster empty
		{
			// Error should not be possible -> Error in cluster() ??
			std::ostringstream str;
			str << "Internal Error in: "
					<< "sca_synchronization_alg::analyse_sample_rates() "
					<< __FILE__ << " line: " << __LINE__ << std::endl;
			SC_REPORT_ERROR("SystemC-AMS",str.str().c_str());
		}

		//Assumption (is fullfilled by sca_synchronization_alg::cluster() ) :
		//         a cluster object can be reached (via in- or outports) from
		//         previosly analyzed cluster object ->
		//         at least one port (except the first module) has a port with a
		//         yet analyzed signal -> no dataflow analyzing will be required

		//current multiplier for sampling rates -> trick to prevent re-calculation
		//of yet calculated rates after a sample rate change with remainder
		//(in such a case the LCM must be calculated and all previously calculated
		//rates/obj-calls must be multiplied)
		//Now for every rate/obj-call the additional the current value of the
		//multiplier is stored, on this way the re-calculation has to performed
		//only once after sample rate analyzis finishing
		long multiplier=0;

		//go through all objects of the current cluster in the order of the list
		// (the order they were push back)
		bool first_obj = true;
		for (sit = current_cluster->begin(); sit != current_cluster->end(); sit++) //go through all objects
		{
			sca_synchronization_alg::sca_sync_objT* obj;

			obj = (*sit);

			//number of channels to the object (inports)
			unsigned long nin = obj->nin;

			//number of channels from the object (outports)
			unsigned long nout = obj->nout;

			if (first_obj) //for the first object no analyzed channel is connected
			{
				first_obj = analyse_sample_rates_first_obj(current_cluster,nin, nout,
						multiplier, obj, sit);
				//only one object in cluster (obj without in and/or outs)
				if (first_obj)
					continue;
			}

			//calculate object calls per period by searching a yet calculated channel
			//and the LCM of the port rate and the assigned max_samples of the channel
			analyse_sample_rates_calc_obj_calls(nin, nout, multiplier, obj);

			//assign to all connected channels the maximum number of samples
			// (port_rate * obj-calls per period) if not yet done otherwise
			// check for consistency
			analyse_sample_rates_assign_max_samples(current_cluster,nin, nout, multiplier, obj);

		} //go through all objects

		schedule_list_length = 0;
		scaled_time_lcm = 1;

		if( (current_cluster->T_max_multiplier == 0) && current_cluster->is_max_timestep)
		{
			std::ostringstream str;
			str
					<< "Error at least one sample period must be assigned per cluster "
					<< std::endl
					<< " the following modules are included in the current cluster"
					<< " (max. 20 printed):" << std::endl;

			long cnt = 0;
			for (sca_cluster_objT::iterator it = current_cluster->begin(); it
					!= current_cluster->end(); it++, cnt++)
			{
				str << "\t" << (*it)->sync_if->get_name_associated_names(10) << std::endl;

				if (cnt >= 20)
					break;
			}

			SC_REPORT_ERROR("SystemC-AMS",str.str().c_str());
		}


		//if set calculate max cluster period
		if(current_cluster->T_max_multiplier > 0)
		{
			current_cluster->csync_data.cluster_max_time_step=
				current_cluster->T_max_cluster * (multiplier / current_cluster->T_max_multiplier);

			current_cluster->T_max_cluster = current_cluster->csync_data.cluster_max_time_step;
			current_cluster->T_max_multiplier = multiplier;
		}
		else
		{
			current_cluster->csync_data.cluster_max_time_step=sca_core::sca_max_time();
		}

		//if set calculate cluster_period
		if(current_cluster->T_multiplier > 0)
		{
			current_cluster->csync_data.cluster_period =
					current_cluster->T_cluster * (multiplier / current_cluster->T_multiplier);

			current_cluster->T_cluster = current_cluster->csync_data.cluster_period;

			current_cluster->T_multiplier = multiplier;
		}

		//if max timestep only
		if(current_cluster->is_max_timestep)
		{
			current_cluster->csync_data.cluster_period = current_cluster->T_max_cluster;
		}
		else
		{
			//chech that max timestep is not hidden by timestep
			if(current_cluster->T_max_multiplier>0)
			{
				if(current_cluster->T_cluster>current_cluster->T_max_cluster)
				{
					std::ostringstream str;
					str << "The maximum timestep set in module: ";
					str << current_cluster->T_max_last_obj->sync_if->get_synchronization_object_data()->module_min_timestep_set->name();
					str << " is smaller than the timestep set in module: ";
					str << current_cluster->T_last_obj->sync_if->get_synchronization_object_data()->module_timestep_set->name();
					SC_REPORT_ERROR("SystemC-AMS",str.str().c_str());
				}
			}
		}


#ifdef SCA_IMPLEMENTATION_DEBUG

		std::cout << "Cluster period: " << current_cluster->csync_data.cluster_period
				<< " max: period: " << current_cluster->csync_data.cluster_max_time_step << std::endl;
#endif

		//now calculate definitive max_samples / calls_per_period_timestep_ch_obj using the multiplier
		for (sit = current_cluster->begin(); sit != current_cluster->end(); sit++) //go through all objects
		{
			sca_synchronization_alg::sca_sync_objT* obj = (*sit);
			analyse_sample_rates_calc_def_rates(current_cluster,multiplier, obj);

			sca_synchronization_object_data* obj_data;
			obj_data=obj->sync_if->get_synchronization_object_data();

			//for statistics
			if (obj_data->calls_per_period
					> current_cluster->calls_per_period_max)
			{
				current_cluster->calls_per_period_max
						= obj_data->calls_per_period;
				current_cluster->mod_name_period_max = obj->sync_if->get_name();
			}
			if (obj_data->calls_per_period
					< current_cluster->calls_per_period_min)
			{
				current_cluster->calls_per_period_min
						= obj_data->calls_per_period;
				current_cluster->mod_name_period_min = obj->sync_if->get_name();
			}

			//sdatas.call_counter           = &(obj->sync_if->get_call_counter());
			//sdatas.calls_per_period_timestep_ch_obj       = &(obj->sync_if->get_calls_per_period());


			obj->sync_if->set_sync_data_references(current_cluster->csync_data);

		}

		current_cluster->csync_data.max_calls_per_period=current_cluster->calls_per_period_max;

		current_cluster->schedule_list_length = schedule_list_length;
		current_cluster->scaled_time_lcm = scaled_time_lcm;
}


///////////////////////////////////////////////////////////////////////////////

void sca_synchronization_alg::analyse_all_sample_rates()
{
	//for all clusters
	std::vector<sca_cluster_objT*>::iterator cit;
	for (cit = clusters.begin(); cit != clusters.end(); ++cit)
	{
		analyse_sample_rates(*cit);

		//initialize last calls per period
		for(sca_cluster_objT::iterator sit = (*cit)->begin();
				sit!=(*cit)->end();++sit)
		{
			(*sit)->object_data->last_last_calls_per_period=
					(*sit)->object_data->last_calls_per_period=
					                  (*sit)->object_data->calls_per_period;

		}
	}

}


///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

void sca_synchronization_alg::analyse_timing()
{
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

inline void sca_synchronization_alg::generate_scheduling_list_expand_list(
		sca_synchronization_alg::sca_cluster_objT& cluster)
{
	try
	{
		if(cluster.scheduling_elements!=NULL)
		{
			delete [] cluster.scheduling_elements;
			cluster.scheduling_elements=NULL;
		}


		//allocate memory for schedule elements
		cluster.scheduling_elements
				= new schedule_element[cluster.schedule_list_length];
		//create scheduling list
		cluster.scheduling_list.clear();
		cluster.scheduling_list.resize(cluster.schedule_list_length);
	} catch (...)
	{
		std::ostringstream str;
		str
				<< "Can't allocate memory for scheduling list - the scheduling list"
				<< " of the cluster containing the module: "
				<< (*cluster.begin())-> sync_if->get_name_associated_names(5) << " is to long "
				<< std::endl
				<< "the cluster is may bad conditioned or has very high sample "
				<< "rate ratios which may have no greatest common divider";
		SC_REPORT_ERROR("SystemC-AMS",str.str().c_str());
	}



	unsigned long cnt = 0;
	for (sca_cluster_objT::iterator it = cluster.begin(); it != cluster.end(); it++)
	{
		sca_sync_objT* obj;
		schedule_element* ele;

		sc_core::sc_object* sched_obj;
		sca_core::sca_implementation::sc_object_method proc_method;
		obj = (*it);
		obj->sync_if->get_processing_method(sched_obj, proc_method);

		sca_synchronization_object_data* sync_obj_data;
		sync_obj_data=obj->sync_if->get_synchronization_object_data();


		unsigned long scaled_time_step;
		unsigned long scaled_cur_time;
		unsigned long calls_per_period;

		calls_per_period = sync_obj_data->calls_per_period;
		scaled_time_step = cluster.scaled_time_lcm / calls_per_period;
		scaled_cur_time = 0;

		obj->schedule_elements.resize(calls_per_period);
		obj->next_schedule_element=0;

		//initialize scheduling list
		for (unsigned long i = 0; i < calls_per_period; i++)
		{
			ele = &(cluster.scheduling_elements[cnt]);
			ele->obj = obj;
			ele->schedule_obj = sched_obj;
			ele->proc_method = proc_method;
			ele->scaled_schedule_time = scaled_cur_time;
			ele->call = i + 1;
			ele->call_counter = &(sync_obj_data->call_counter);
			ele->id_counter   = &(sync_obj_data->id_counter);
			ele->allow_processing_access = &(sync_obj_data->allow_processing_access_flag);
			ele->current_context_ref=&this->current_context;
			*(ele->allow_processing_access) = false;
			obj->schedule_elements[i]=ele;
			cnt++;
			scaled_cur_time += scaled_time_step;
		}
	}
}

///////////////////////////////////////////////////////////////////////////////


inline void sca_synchronization_alg::generate_scheduling_list_schedule(
		sca_synchronization_alg::sca_cluster_objT& cluster)
{
	//copy to list to allow faster remove
	std::list<sca_sync_objT*> tmp_list(cluster.size());
	std::copy(cluster.begin(), cluster.end(), tmp_list.begin());

	//pre-odering using time
	//std::sort(tmp_list.begin(),tmp_list.end(),
	//	   std::mem_fn(&sca_sync_objT::schedule_pre_order_criteria));

	//pre-odering using time
	tmp_list.sort(std::mem_fn(&sca_sync_objT::schedule_pre_order_criteria));

	std::vector<schedule_element*>& scheduling_list(cluster.scheduling_list);

	std::vector<schedule_element*>::iterator sched_executed_pos=scheduling_list.begin();

	std::vector<schedule_element*>::iterator vslit = scheduling_list.begin();

	std::list<sca_sync_objT*>::iterator lslit;


	while (true)
	{
		bool bstart=true;
		std::list<sca_sync_objT*>::iterator lpos=tmp_list.begin();
		do
		{
			//find first schedulable object
			lslit=std::find_if(lpos,tmp_list.end(),
		                  std::mem_fn(&sca_sync_objT::schedule_if_possible));

			if (lslit != tmp_list.end())
			{
				auto& next_element=(*lslit)->next_schedule_element;
				(*vslit++) = (*lslit)->schedule_elements[next_element];
				next_element++;


				lpos=lslit;
				lpos++;

				if(next_element>=(*lslit)->schedule_elements.size())
				{
					(*lslit)->schedule_elements.clear();
					tmp_list.erase(lslit); //remove object from list due all elements scheduled
				}

				bstart=false;
			}
			else
			{
				if(bstart) break;
				else
				{
					bstart=true;
					lpos=tmp_list.begin();
				}
			}
		}while(true);


		//if no further object schedulable all elements scheduled or error
		if (tmp_list.size() != 0)
		{
			sca_synchronization_alg::sca_systemc_sync_obj::action_struct action;
			action.action=sca_synchronization_alg::sca_systemc_sync_obj::action_struct::SCHEDULE;
			action.slice_end=vslit;
			sched_executed_pos=action.slice_end;

			std::uint64_t& schedule_sc_time=cluster.systemc_synchronizer.scaled_systemc_time_reached;
			cluster.systemc_synchronizer.schedule_program[schedule_sc_time].push_back(action);

			//try to synchronize SystemC
			std::uint64_t sct=cluster.systemc_synchronizer.run_as_far_as_possible();
			if(sct>schedule_sc_time)
			{
				schedule_sc_time=sct;
				continue; //if SystemC has progressed we continue otherwise the System is not schedulable
			}
		}

		break;
	}

	if (tmp_list.size() != 0)
	{
		std::ostringstream str;
		str << "System cannot be scheduled -";
		if (sched_executed_pos == scheduling_list.begin())
		{
			str << " no element could be scheduled" << std::endl;
		}
		else
		{
			vslit--;

			if((*vslit)->schedule_obj!=NULL)
			{
				str << " Last scheduled element : " << std::endl;
				str << "                  " << (*vslit)->schedule_obj->name()
					<< std::endl;
			}

			str << std::endl << "     Current schedule list: " << std::endl;
			for (auto it=cluster.scheduling_list.begin(); it!= vslit+1; it++)
			{
				if((*it)->schedule_obj==NULL) continue;
				str << "                                "
						<< (*it)->schedule_obj->name()
						<< std::endl;
			}

			str << std::endl << "     Modules which cannot anymore scheduled: " << std::endl;
			for(auto mod : tmp_list)
			{
				str << "                                "
						<< mod->object_data->sync_obj_name
						<< std::endl;
			}

		}
		SC_REPORT_ERROR("SystemC-AMS",str.str().c_str());
	}

	if(sched_executed_pos!=cluster.scheduling_list.end())
	{
		sca_synchronization_alg::sca_systemc_sync_obj::action_struct action;
		action.action=sca_synchronization_alg::sca_systemc_sync_obj::action_struct::SCHEDULE;
		action.slice_end=vslit;
		sched_executed_pos=action.slice_end;

		std::uint64_t schedule_sc_time=cluster.systemc_synchronizer.scaled_systemc_time_reached;
		cluster.systemc_synchronizer.schedule_program[schedule_sc_time].push_back(action);
	}

	cluster.systemc_synchronizer.sort_program_actions();

	//copy max out buffer usage to channel
	for (sca_cluster_objT::iterator it = cluster.begin(); it != cluster.end(); it++)
	{
		sca_sync_objT* obj;

		obj = (*it);

		for(unsigned long i=0;i<obj->sync_if->get_number_of_sync_ports(SCA_TO_ANALOG);++i)
		{
			sca_synchronization_port_data* scport=obj->sync_if->get_sync_port(SCA_TO_ANALOG, i);
			sca_prim_channel* ch=scport->channel;
			ch->max_buffer_usage=obj->max_out_buffer_usage[i];
		}
	}
}

///////////////////////////////////////////////////////////////////////////////

void sca_synchronization_alg::generate_scheduling_list(sca_cluster_objT* current_cluster)
{
	generate_scheduling_list_expand_list(*current_cluster);
	generate_scheduling_list_schedule(*current_cluster);
}

void sca_synchronization_alg::generate_all_scheduling_lists()
{
	for (unsigned long i = 0; i < clusters.size(); i++)
	{
		generate_scheduling_list(clusters[i]);
	}

}


void sca_synchronization_alg::reanalyze_cluster_timing(unsigned long cluster_id)
{
	clusters[cluster_id]->reset_analyse_data();
	clusters[cluster_id]->systemc_synchronizer.reset();


	analyse_sample_rates(clusters[cluster_id]);
}


///////////////////////////////////////////////////////////////////////////////

void sca_synchronization_alg::reschedule_cluster(unsigned long cluster_id)
{

	//adopt buffers in sync ports
	//clusters[cluster_id]->systemc_synchronizer.reinitialize_sync_ports();

	reinitialize_ports(clusters[cluster_id]);

	initialize_systemc_synchronizer(clusters[cluster_id]);


	generate_scheduling_list(clusters[cluster_id]);

	//resize signal buffer
	for(std::vector<sca_core::sca_prim_channel*>::iterator
			it=clusters[cluster_id]->channels.begin();
			it!=clusters[cluster_id]->channels.end();
			++it)
	{
		(*it)->resize_buffer();
	}

	//adjust callcounter to a first execution of the current cluster execution
	//-> maybe the number of calls per period has been changed
	for(sca_cluster_objT::iterator
			it=clusters[cluster_id]->begin();
			it!=clusters[cluster_id]->end();
			++it)
	{
		//we set call counter after one cluster period
		(*it)->object_data->call_counter=(*it)->object_data->calls_per_period;
	}


}


bool sca_synchronization_alg::reinitialize_custer_timing(unsigned long cluster_id)
{
	//adopt buffers in sync ports
	return this->reinitialize_port_timings(clusters[cluster_id]);
}


void sca_synchronization_alg::reinitialize_custer_datastructures(unsigned long cluster_id)
{
	//adopt buffers in sync ports
	return this->reinitialize_ports(clusters[cluster_id]);
}


void sca_synchronization_alg::finish_simulation()
{
	for(auto cluster : this->clusters)
	{
		cluster -> finish_simulation();
	}
}

void sca_synchronization_alg::sca_cluster_objT::finish_simulation()
{

#ifndef DISABLE_PERFORMANCE_STATISTICS

	unsigned long& info_mask(
	    sca_core::sca_implementation::sca_get_curr_simcontext()->
	    get_information_mask());

	if(info_mask & sca_util::sca_info::sca_tdf_solver.mask)
	{
		double overalltime=sca_get_curr_simcontext()->get_consumed_wallclock_time_in_sec();
		double consumed_time=this->csync_mod->duration.count() ;

		std::ostringstream str;

		str << "Cluster: " << this->id << " : " << std::endl;
		str << "       finished executions : " << this->csync_mod->cluster_finished_executions_cnt << " times" << std::endl;
		str << "       with overall        : " << this->csync_mod->activation_cnt << " SystemC context switches" << std::endl;
		str << "       this all consumed   : " << consumed_time << " s / "<< int(consumed_time/overalltime*100.0+0.5) << "% wall clock time";
		SC_REPORT_INFO("SystemC-AMS",str.str().c_str());
	}

#endif
}


///////////////////////////////////////////////////////////////////////////////

void sca_synchronization_alg::sca_cluster_objT::print()
{
	for (iterator i = begin(); i != end(); i++)
	{
		int nin = (*i)->sync_if-> get_number_of_sync_ports(SCA_FROM_ANALOG);
		int nout = (*i)->sync_if-> get_number_of_sync_ports(SCA_TO_ANALOG);

		std::cout << "\t" << (*i)->sync_if->get_name_associated_names();

		std::cout << "\t" << (*i)->sync_if->get_synchronization_object_data()->calls_per_period;

		std::cout << " \t nin: " << nin << " nout: " << nout << "\t";

		sca_synchronization_port_data* cport;
		for (long cnt = 0; cnt < nin; cnt++)
		{
			cport = (*i)->sync_if->get_sync_port(SCA_FROM_ANALOG, cnt);
			std::cout << "  " << *(cport->rate) << "," << *(cport->delay);
		}
		std::cout << "   | |   ";
		for (long cnt = 0; cnt < nout; cnt++)
		{
			cport = (*i)->sync_if->get_sync_port(SCA_TO_ANALOG, cnt);
			std::cout << "  " << *(cport->rate) << "," << *(cport->delay);
		}
		std::cout << std::endl;
	}
}

///////////////////////////////////////////////////////////////////////////////

void sca_synchronization_alg::sca_cluster_objT::print_schedule_list()
{
	int k = 1;
	for (std::vector<schedule_element*>::iterator sit = scheduling_list.begin(); sit
			!= scheduling_list.end(); sit++, k++)
	{
		std::cout << k << "\t";
		std::cout << (*sit)->obj->sync_if->get_name_associated_names() << "\t";
		std::cout << (*sit)->scaled_schedule_time << "\t";
		std::cout << (*sit)->call << " of "
				<< (*sit)->obj->sync_if->get_synchronization_object_data()->calls_per_period << std::endl;
	}
}

///////////////////////////////////////////////////////////////////////////////

void sca_synchronization_alg::sca_sync_objT::initialize_sync_obj()
{
	sc_core::sc_object* obj;
	sca_core::sca_implementation::sc_object_method meth;

	//used to get module pointer
	sync_if->get_initialization_method(obj,meth);

	//transfer flags to sync_obj
	sca_tdf::sca_module* cmod=dynamic_cast<sca_tdf::sca_module*>(obj);
	if(cmod!=NULL)
	{
	   if(cmod->does_attribute_changes_flag) cmod->attribute_changes_allowed_flag=true;


	   cmod->attribute_changes_allowed_flag=false;
	   cmod->change_attributes_executes_flag=false;

	    attribute_changes_allowed=cmod->does_attribute_changes_flag;
	    accepts_attribute_changes=cmod->accept_attribute_changes_flag;
	}
}

///////////////////////////////////////

void sca_synchronization_alg::sca_sync_objT::call_init_method()
{
	sc_core::sc_object* obj;
	sca_core::sca_implementation::sc_object_method meth;


    sync_if->get_synchronization_object_data()->call_counter = -1;

	sync_if->get_initialization_method(obj,meth);

	sca_tdf::sca_module* cmod=dynamic_cast<sca_tdf::sca_module*>(obj);

	if(cmod!=NULL) cmod->initialize_executes_flag=true;

	*this->current_context_ref=obj;
	if(obj!=NULL)  (obj->*(meth))();
	*this->current_context_ref=NULL;

	if(cmod!=NULL) cmod->initialize_executes_flag=false;

	//after init we have time zero (or cluster start time)
    sync_if->get_synchronization_object_data()->call_counter=0;
}


// new SystemC-AMS 2.0 dtdf extension
bool sca_synchronization_alg::sca_sync_objT::call_change_attributes_method()
{
	sc_core::sc_object* obj;
	sca_core::sca_implementation::sc_object_method meth;


	sync_if->get_change_attributes_method(obj,meth);

	sca_tdf::sca_module* cmod=dynamic_cast<sca_tdf::sca_module*>(obj);
	if(cmod!=NULL)
	{
		//execute change attributes - set/reset allowance flags before/after
		if(cmod->does_attribute_changes_flag) cmod->attribute_changes_allowed_flag=true;
		cmod->change_attributes_executes_flag=true;

		(*cmod->call_counter)--; //call belongs to last execution
		                         //due change_attributes is may not executed we make this correction
		                         //the counter is increased after processing

		*this->current_context_ref=obj;
		(cmod->*(cmod->change_attributes_method))();
		*this->current_context_ref=NULL;

		cmod->validate_port_attributes();

		(*cmod->call_counter)++; //restore call counter

		cmod->change_attributes_executes_flag=false;
		cmod->attribute_changes_allowed_flag=false;


		if(cmod->does_attribute_changes_flag && cmod->no_change_attributes)
		{
			std::ostringstream str;
			str << std::endl << "TDF module: " << cmod->name();
			str << " calls does_attribute_changes(), "<< std::endl;
			str << "but implements no change_attributes() callback method " << std::endl;
			str << "so it will never be able to change any attribute";
			SC_REPORT_WARNING("SystemC-AMS",str.str().c_str());
		}



		if(cmod->has_executed_attribute_change_method && !attribute_changes_allowed)
		{
			std::ostringstream str;
			str << "an attribute changing method ";
			str << "(e.g. request_next_activation, set_timestep, ...)";
			str << " is called in the context of change_attributes()  ";
			str << "of the TDF module: " << cmod->name();
			str << " which has not set does_attribute_changes()";
			SC_REPORT_ERROR("SystemC-AMS",str.str().c_str());
		}
		cmod->has_executed_attribute_change_method=false;

		//for the next execution attribute changes are allowed
		attribute_changes_allowed=cmod->does_attribute_changes_flag;
		accepts_attribute_changes=cmod->accept_attribute_changes_flag;


		return cmod->no_change_attributes;
	}

	return true;

}


//////////////////////////////////////////////////////////////////////////////
void sca_synchronization_alg::sca_sync_objT::reset()
{
	//reset number of available sample
	for (unsigned long ipnr = 0; ipnr < nin; ipnr++)
	{
		sample_inports[ipnr]   = 0;
	}

	for (unsigned long opnr = 0; opnr < nout; opnr++)
	{
		max_out_buffer_usage[opnr]=0;
	}
}


//////////////////////////////////////////////////////////////////////////////

void sca_synchronization_alg::sca_sync_objT::reinit()
{
	for (unsigned long ipnr = 0; ipnr < nin; ipnr++)
	{
		//get driving outport
		sca_synchronization_port_data* cport;
		cport = sync_if->get_sync_port(SCA_FROM_ANALOG, ipnr);

		cport ->channel -> reset_for_reschedule();

		//get sampling rate of current port
		long port_rate = *(cport -> rate);

		//add inport delay (maybe outport delay added before)
		in_rates[ipnr] = port_rate;

		sample_inports[ipnr] += *(cport -> delay);
	}


	//for all outports
	for (unsigned long opnr = 0; opnr < nout; opnr++)
	{
		//get current outport
		sca_synchronization_port_data* cport;
		cport = sync_if->get_sync_port(SCA_TO_ANALOG, opnr);

		cport ->channel -> reset_for_reschedule();

		//get sampling rate of current port
		long port_rate = *(cport -> rate);

		//initialize for scheduling analysis
		out_rates[opnr] = port_rate;

		//for all connected inports - add outport delay (initial sample)
		//-> sample_inports of connected module(s)
		unsigned long ncip=next_inports[opnr].size();
		for(unsigned long i=0;i<ncip;i++)
		{
			*(next_inports[opnr][i]) += *(cport->delay);
		}
	}

}


//////////////////////////////////////////////////////////////////////////////

void sca_synchronization_alg::sca_sync_objT::init(sca_synchronization_obj_if* sync_if_)
{
	sync_if=sync_if_;

	//number of inports
	unsigned long nip;
	nip= sync_if->get_number_of_sync_ports(SCA_FROM_ANALOG);

	//number of outports
	unsigned long nop;
	nop = sync_if->get_number_of_sync_ports(SCA_TO_ANALOG);

	inports.clear();
	in_rates.clear();
	sample_inports.clear();

	out_rates.clear();
	next_inports.clear();
	max_out_buffer_usage.clear();


	inports.resize(nip,0);
	in_rates.resize(nip,0);
	sample_inports.resize(nip,0);
	nin = nip;

	//from outport to the next input ports
	out_rates.resize(nop,0);
	next_inports.resize(nop);
	max_out_buffer_usage.resize(nop,0);

	nout = nop;



	for (unsigned long opnr = 0; opnr < nop; opnr++)
	{
		//get current outport
		sca_synchronization_port_data* cport;
		cport = sync_if->get_sync_port(SCA_TO_ANALOG, opnr);

		//get sampling rate of current port
		long port_rate = *(cport -> rate);

		//initialize for scheduling analysis
		out_rates[opnr] = port_rate;
	}



	for (unsigned long ipnr = 0; ipnr < nip; ipnr++)
	{
		//get driving outport
		sca_synchronization_port_data* cport;
		cport = sync_if->get_sync_port(SCA_FROM_ANALOG, ipnr);

		//get sampling rate of current port
		long port_rate = *(cport -> rate);

		in_rates[ipnr] = port_rate;
	}

	object_data=sync_if-> get_synchronization_object_data();
}


///////////////////////////////////////////////////////////////////////////////



bool sca_synchronization_alg::sca_sync_objT::schedule_if_possible()
{
/*
	std::cout  << "try schedule: " << sync_if->get_name();
	for(unsigned long i=0;i<nin;i++)
	                           std::cout << "   " <<  sample_inports[i] << "/"<< in_rates[i];
	std::cout << std::endl;
*/

	//check schedulability
	for(unsigned long i=0;i<nin;i++)
	{
		if(sample_inports[i] < in_rates[i]) return false;
	}

	//check schedulability due to reached SystemC time
	for(std::size_t idx=0;idx<from_systemc_ports_ids.size();++idx)
	{
		if(!systemc_synchronizer->is_read_from_systemc_possible(from_systemc_ports_ids[idx]))
		{
			return false;
		}
	}

	//schedule SystemC
	for(std::size_t idx=0;idx<from_systemc_ports_ids.size();++idx)
	{
		systemc_synchronizer->read_from_systemc(from_systemc_ports_ids[idx]);
	}

	for(std::size_t idx=0;idx<to_systemc_ports_ids.size();++idx)
	{
		systemc_synchronizer->write_to_systemc(to_systemc_ports_ids[idx]);
	}


	/*
	for(unsigned long i=0;i<nin;i++) std::cout << "schedule read: " << in_rates[i] << ",";
	std::cout << std::endl;
   	for(unsigned long i=0;i<nout;i++) std::cout << "schedule write: " << out_rates[i] << ",";

	std::cout << std::endl << "------------------------------------------------------" << std::endl;
    */

	//schedule inports
	for(unsigned long i=0;i<nin;i++)
	{
		sample_inports[i]-=in_rates[i];
	}

	//schedule outports (write to all connected inports)
	for(unsigned long i=0;i<nout;i++)
	{
		unsigned long& mobusage=this->max_out_buffer_usage[i];

		for(unsigned long j=0;j<next_inports[i].size();j++)
		{
			unsigned long& busage=*(next_inports[i][j]);
			busage+=out_rates[i];

			//calculate the maximum usage of the connected
			//this is will correspond to the minimum buffer size
			if(busage>mobusage)
			{
				mobusage=busage;
			}
		}
	}
    return true;
}


///////////////////////////////////////////////////////////////////////////////


bool sca_synchronization_alg::sca_sync_objT::call_reinit_method()
{
	if(reinit_obj==NULL)
	{
		sync_if->get_reinitialization_method(reinit_obj,reinit_meth);
	}

	sca_tdf::sca_module* cmod=dynamic_cast<sca_tdf::sca_module*>(reinit_obj);


	if(cmod==NULL) return true; //only tdf modules have reinitialize method
	else
	{
		cmod->reinitialize_executes_flag=true;

		*this->current_context_ref=reinit_obj;
		(cmod->*(cmod->reinitialize_method))();
		*this->current_context_ref=NULL;

		cmod->reinitialize_executes_flag=false;
	}

	return cmod->no_reinitialize;
}


}
}
