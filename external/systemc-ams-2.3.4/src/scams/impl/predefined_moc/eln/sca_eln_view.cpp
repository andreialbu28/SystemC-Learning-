/*****************************************************************************

    Copyright 2010-2014
    Fraunhofer-Gesellschaft zur Foerderung der angewandten Forschung e.V.

    Copyright 2015-2020
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

 sca_eln_view.cpp - description

 Original Author: Karsten Einwich Fraunhofer IIS/EAS Dresden

 Created on: 09.11.2009

 SVN Version       :  $Revision: 2155 $
 SVN last checkin  :  $Date: 2021-05-31 11:26:39 +0000 (Mon, 31 May 2021) $
 SVN checkin by    :  $Author: karsten $
 SVN Id            :  $Id: sca_eln_view.cpp 2155 2021-05-31 11:26:39Z karsten $

 *****************************************************************************/

/*****************************************************************************/

#include "systemc-ams"
#include "scams/impl/predefined_moc/eln/sca_eln_view.h"
#include "scams/predefined_moc/eln/sca_eln_module.h"
#include "scams/predefined_moc/tdf/sca_tdf_signal_if.h"
#include "scams/impl/solver/linear/sca_linear_equation_if.h"
#include "scams/impl/solver/linear/sca_linear_solver.h"
#include "scams/impl/solver/user_solver/sca_generic_user_solver.h"

namespace sca_eln
{
namespace sca_implementation
{

lin_eqs_cluster::lin_eqs_cluster()
{
	eqs = 0;
	view_params.reset_params();
}

////////////////////////////////////////////////////////////////

lin_eqs_cluster::~lin_eqs_cluster()
{
	delete eqs;
}

////////////////////////////////////////////////////////////////

void lin_eqs_cluster::reinit_equations()
{
	//number of nodes
	unsigned long nnodes = (unsigned long)(channels.size());

	eqs->lin_eq_if_methods.resize(0);
	eqs->reinit_methods.resize(0);

	eqs->A.resize(nnodes, nnodes);
	eqs->B.resize(nnodes, nnodes);
	eqs->q.resize(nnodes);

	eqs->A.reset();
	eqs->B.reset();
	eqs->q.reset();

	eqs->pwl_stamps.clear();
	eqs->B_change.reset();

	for (sca_conservative_cluster::iterator mit = modules.begin(); mit
			!= modules.end(); ++mit)
	{
		sca_eln::sca_module* lmod = dynamic_cast<sca_eln::sca_module*> (*mit);
		lmod->add_equations.resize(0);
		lmod->matrix_stamps();
	}
}

////////////////////////////////////////////////////////////////

void sca_eln_view::setup_equations()
{
#ifdef SCA_IMPLEMENTATION_DEBUG
	std::cout << "Setup equations for sca_linnet_view: "
	<< conservative_clusters.size() << std::endl;
#endif

	this->cluster_nets();

	//for all clusters
	for (ccluster_listT::iterator cit = conservative_clusters.begin(); cit
          != conservative_clusters.end(); ++cit)
	{
		lin_eqs_cluster* lcl = dynamic_cast<lin_eqs_cluster*> (*cit);
		if (lcl == NULL)
		{
			SC_REPORT_ERROR("SystemC-AMS", "Internal Error");
			return;
		}

		//store all modules to a vector and check whether a user specific
		//solver is required
		sca_core::sca_user_solver_base* user_solver=NULL;
		std::vector<sca_core::sca_module*> tmp_mod;
		for (sca_core::sca_implementation::sca_conservative_cluster::iterator
				mit = lcl->begin(); mit != lcl->end(); ++mit)
		{
			tmp_mod.push_back(*mit);

			sca_core::sca_user_solver_base* usolv=NULL;

			if((*mit)->user_solver_handler!=NULL)
			{
				usolv=(*mit)->user_solver_handler->create();
			}


			if(usolv!=NULL)
			{
				usolv->solver_requesting_module=*mit;
				if(user_solver==NULL)
				{
					user_solver=usolv;
				}
				else
				{
					//resolve different solvers
					sca_core::sca_user_solver_base* rsolv;
					rsolv=(*mit)->user_solver_handler->try_cast(user_solver);
					if(rsolv!=NULL)
					{   //last solver has the same type or is derived from the current
						delete usolv;
					}
					else
					{
						rsolv=user_solver->solver_requesting_module->user_solver_handler->try_cast(usolv);
						if(rsolv!=NULL)
						{   //current solver is derived from last solver
							delete user_solver;
							user_solver=rsolv;
						}
						else
						{
							std::ostringstream str;
						    str << "Incompatible user specific solver requested ";
							str << "by modules: " << (*mit)->name() << " and ";
							str << user_solver->solver_requesting_module->name();
						    SC_REPORT_ERROR("SystemC-AMS",str.str().c_str());
							return;
						}
					}
				}
			}
		}

		//store all channels to a vector
		std::vector<sca_core::sca_interface*> tmp_chan;
		for(unsigned long i=0;i<lcl->channels.size();i++) tmp_chan.push_back(lcl->channels[i]);


		if(user_solver!=NULL)
		{
			solver = new sca_core::sca_implementation::sca_generic_user_solver(
					tmp_mod,tmp_chan,user_solver);
		}
		else
		{
			sca_core::sca_implementation::sca_linear_solver* linear_solver;

			//create standard solver
			linear_solver = new sca_core::sca_implementation::sca_linear_solver(tmp_mod,tmp_chan);
			solver=linear_solver;

			eqs = new sca_core::sca_implementation::sca_linear_equation_system;
			lcl->eqs = eqs;

			//activate sparse mode
			eqs->A.set_sparse_mode();
			eqs->B.set_sparse_mode();

			eqs->A.unset_auto_resizable();
			eqs->B.unset_auto_resizable();
			eqs->q.unset_auto_resizable();

			//ignore matrix access with negative indices -> reference node
			eqs->A.set_ignore_negative();
			eqs->B.set_ignore_negative();
			eqs->q.set_ignore_negative();

#ifdef SCA_IMPLEMENTATION_DEBUG
		std::cout << "   sca_linear solver created" << std::endl;
#endif

			//set equation reference to modules
			for (sca_core::sca_implementation::sca_conservative_cluster::iterator
				mit = lcl->begin(); mit != lcl->end(); ++mit)
			{
				sca_eln::sca_module* lmod = dynamic_cast<sca_eln::sca_module*> (*mit);

				if (lmod == NULL)
				{
					std::ostringstream str;
					str << "Module: " << (*mit)->name()
						<< " is assigned to sca_linnet_view."
						<< " However it is not a sca_linnet_module."
						<< std::endl;
					SC_REPORT_ERROR("SystemC-AMS",str.str().c_str());
					return;
				}
				else
				{
					lmod->set_equations(
							eqs,
							linear_solver->get_state_vector(),
							linear_solver->current_time,
							lcl->view_params,
							linear_solver->call_id);
				}
			}

			linear_solver->assign_equation_system(*lcl);

		} //standard solver setup


		solver_time_data=solver->get_synchronization_object_data();


		//timestep module->sync object propagation
		for (sca_core::sca_implementation::sca_conservative_cluster::iterator
			mit = lcl->begin(); mit != lcl->end(); ++mit)
		{

			(*mit)->timestep_elaborated=true;  //forbid further setting
			if ((*mit)->timestep_is_set)
			{
				if (solver_time_data->timestep_is_set)
				{
					sc_dt::int64 diff = (*mit)->timestep_set.value()
							- solver_time_data->timestep_set.value();
					if ((diff > 1) || (diff < -1))
					{
						std::ostringstream str;
						str << "Inconsistent timestep between module: "
								<< (*mit)->name() << ": "
								<< (*mit)->timestep_set << " and module: ";
						if (solver_time_data->module_timestep_set == NULL)
							str << "unknown module";
						else
							str << solver_time_data->module_timestep_set->name();
						str << ": " << solver_time_data->timestep_set
						    << " (modules belong to the same eln cluster "
						    << "thus the timesteps must be equal)";

						SC_REPORT_ERROR("SystemC-AMS",str.str().c_str());
					}

					//we use larger timestep
					if (diff > 0)
					{
						solver_time_data->timestep_set = (*mit)->timestep_set;
						solver_time_data->module_timestep_set = (*mit);

						//prevent drifting
						sc_dt::int64 diff_min = (*mit)->timestep_set.value()
								- solver_time_data->min_timestep_set.value();
						if (diff_min > 1)
						{
							std::ostringstream str;
							str << "Inconsistent timestep between module: "
									<< (*mit)->name() << " timestep: "
									<< (*mit)->timestep_set << " and module: ";
							if (solver_time_data->module_min_timestep_set == NULL)
								str << "unknown module";
							else
								str << solver_time_data->module_min_timestep_set->name();
							str << " timestep: " << solver_time_data->min_timestep_set;

							SC_REPORT_ERROR("SystemC-AMS",str.str().c_str());
						}
					}
					else
					{
						if ((*mit)->timestep_set < solver_time_data->min_timestep_set)
						{
							solver_time_data->module_min_timestep_set = (*mit);
							solver_time_data->min_timestep_set = (*mit)->timestep_set;
						}
					}
				}
				else
				{
					solver_time_data->timestep_is_set = true;
					solver_time_data->timestep_set = (*mit)->timestep_set;
					solver_time_data->module_timestep_set = (*mit);
					solver_time_data->module_min_timestep_set = (*mit);
					solver_time_data->min_timestep_set = (*mit)->timestep_set;
				}
			}

			//end timestep module->sync object propagation

		} // for all modules




		//establish connection to other domains and dt-SystemC
		//sort ports
		for (std::vector<sca_core::sca_implementation::sca_port_base*>::iterator
				pit = (*cit)->foreign_ports.begin(); pit
				!= (*cit)->foreign_ports.end(); ++pit)
		{
			sca_core::sca_interface* sca_if = (*pit)->sca_get_interface();
			(*pit)->elaboration_finished=true;

			::sca_tdf::sca_implementation::sca_tdf_signal_impl_base
			* sdf_if =
			dynamic_cast< ::sca_tdf::sca_implementation::sca_tdf_signal_impl_base*> (sca_if);


			::sca_tdf::sca_implementation::sca_port_attributes* attr;
			attr = dynamic_cast< ::sca_tdf::sca_implementation::sca_port_attributes*>(*pit);


			unsigned long port_id = (*pit)->get_if_id();

			switch ((*pit)->get_port_type())
			{
			case sca_core::sca_implementation::sca_port_base::SCA_IN_PORT:

                if(sdf_if==NULL)
                {
                    std::ostringstream str;
                    str  << "Error module: " << (*pit)->get_parent_module()->name()
                         << " has bound non sca_tdf_interface at port: "
                         << (*pit)->get_port_number() << std::endl;
                    SC_REPORT_ERROR("SystemC-AMS", str.str().c_str());
                    return;
                }

				solver->push_back_from_analog(*pit, sca_if, sdf_if->get_rate(
						port_id), sdf_if->get_delay(port_id), sdf_if->get_timestep_calculated_ref(
						port_id),
						sdf_if->get_timestep_change_id(port_id));
				break;

			case sca_core::sca_implementation::sca_port_base::SCA_OUT_PORT:

                if(sdf_if==NULL)
                {
                    std::ostringstream str;
                    str  << "Error module: " << (*pit)->get_parent_module()->name()
                         << " has bound non sca_tdf_interface at port: "
                    << (*pit)->get_port_number() << std::endl;
                    SC_REPORT_ERROR("SystemC-AMS", str.str().c_str());
                    return;
                }

				solver->push_back_to_analog(*pit, sca_if, sdf_if->get_rate(
						port_id), sdf_if->get_delay(port_id), sdf_if->get_timestep_calculated_ref(
						port_id),
						sdf_if->get_timestep_change_id(port_id));
				break;

			case sca_core::sca_implementation::sca_port_base::SCA_SC_IN_PORT:

	             if(attr==NULL)
	             {
	            	SC_REPORT_ERROR("SystemC-AMS","Internal error (should not be possible)");
	                return;
	             }

				solver->push_back_from_systemc(*pit, NULL,
                        attr->rate,
                         attr->delay,
                         attr->timestep_calculated,
                         attr->timestep_change_id);
				break;

			case sca_core::sca_implementation::sca_port_base::SCA_SC_OUT_PORT:

	             if(attr==NULL)
	             {
	            	SC_REPORT_ERROR("SystemC-AMS","Internal error (should not be possible)");
	                return;
	             }

				solver->push_back_to_systemc(*pit, NULL,
                        attr->rate,
                         attr->delay,
                         attr->timestep_calculated,
                         attr->timestep_change_id);
				break;

			default:

				SC_REPORT_ERROR("SystemC-AMS", "Not supported sca-port direction in linnet view.");
			}
		} //for all foreign ports

	} //for all clusters

}

}
}
