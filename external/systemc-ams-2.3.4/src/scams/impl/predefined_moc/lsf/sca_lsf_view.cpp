/*****************************************************************************

    Copyright 2010
    Fraunhofer-Gesellschaft zur Foerderung der angewandten Forschung e.V.


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

  sca_lsf_view.cpp - description

  Original Author: Karsten Einwich Fraunhofer IIS/EAS Dresden

  Created on: 05.01.2010

   SVN Version       :  $Revision: 2155 $
   SVN last checkin  :  $Date: 2021-05-31 11:26:39 +0000 (Mon, 31 May 2021) $
   SVN checkin by    :  $Author: karsten $
   SVN Id            :  $Id: sca_lsf_view.cpp 2155 2021-05-31 11:26:39Z karsten $

 *****************************************************************************/

/*****************************************************************************/


#include "systemc-ams"
#include "scams/impl/predefined_moc/lsf/sca_lsf_view.h"
#include "scams/predefined_moc/lsf/sca_lsf_module.h"
#include "scams/predefined_moc/tdf/sca_tdf_signal_if.h"
#include "scams/impl/solver/linear/sca_linear_equation_if.h"
#include "scams/impl/solver/linear/sca_linear_solver.h"

namespace sca_lsf
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
		sca_lsf::sca_module* lmod = dynamic_cast<sca_lsf::sca_module*> (*mit);
		lmod->add_equations.resize(0);
		lmod->matrix_stamps();
	}
}

////////////////////////////////////////////////////////////////

void sca_lsf_view::setup_equations()
{
#ifdef SCA_IMPLEMENTATION_DEBUG
	std::cout << "Setup equations for sca_linnet_view: "
	<< conservative_clusters.size() << std::endl;
#endif

	cluster_nets();

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


		////// perform driver check ////////////////

		std::vector<sca_core::sca_implementation::sca_conservative_signal*>  signals=(*cit)->channels;
		for(unsigned long i=0;i<signals.size();i++)
		{
			unsigned long n=signals[i]->get_number_of_connected_ports();
			sca_core::sca_implementation::sca_port_base** ports=signals[i]->get_connected_port_list();

			sca_core::sca_implementation::sca_port_base* first_driver=NULL;
			for(unsigned long j=0;j<n;j++)
			{
				if(ports[j]->get_port_type()==sca_core::sca_implementation::sca_port_base::SCA_OUT_PORT)
				{
					if(first_driver==NULL)
					{
						first_driver=ports[j];
					}
					else
					{
						std::ostringstream str;
						str << "lsf signal: " << signals[i]->name()
							<< " has more than one driver first driver: "
							<< first_driver->sca_name() << " second driver: "
							<< ports[j]->sca_name();
						SC_REPORT_ERROR("SystemC-AMS",str.str().c_str());
					}
				}
			}

			if((first_driver==NULL) && (n>0))
			{
				std::ostringstream str;
				str << "lsf signal: " << signals[i]->name() << " has no driver";
				SC_REPORT_ERROR("SystemC-AMS",str.str().c_str());
				return;
			}
		}
		////////// end driver check //////////////


		eqs = new sca_core::sca_implementation::sca_linear_equation_system;
		lcl->eqs = eqs;

        //activate sparse mode
		eqs->A.set_sparse_mode();
		eqs->B.set_sparse_mode();

		eqs->A.unset_auto_resizable();
		eqs->B.unset_auto_resizable();
		eqs->q.unset_auto_resizable();

		//generate solver instance

		std::vector<sca_core::sca_module*> tmp_mod;
		for (sca_core::sca_implementation::sca_conservative_cluster::iterator
				mit = lcl->begin(); mit != lcl->end(); ++mit) tmp_mod.push_back(*mit);

		std::vector<sca_core::sca_interface*> tmp_chan;
		for(unsigned long i=0;i<lcl->channels.size();i++) tmp_chan.push_back(lcl->channels[i]);

		solver = new sca_core::sca_implementation::sca_linear_solver(tmp_mod,tmp_chan);
		solver_time_data=solver->get_synchronization_object_data();


#ifdef SCA_IMPLEMENTATION_DEBUG
		std::cout << "   sca_linear solver created" << std::endl;
#endif

		//setup equation system by adding all modules matrix stamps
		for (sca_core::sca_implementation::sca_conservative_cluster::iterator
				mit = lcl->begin(); mit != lcl->end(); ++mit)
		{
			sca_lsf::sca_module* lmod =
					dynamic_cast<sca_lsf::sca_module*> (*mit);

			if (lmod == NULL)
			{
				std::ostringstream str;
				str << "Module: " << (*mit)->name()
						<< " is assigned to sca_lsf_view."
						<< " However it is not a sca_lsf::sca_module."
						<< std::endl;
				SC_REPORT_ERROR("SystemC-AMS",str.str().c_str());
				return;
			}
			else
				lmod->set_equations(
						eqs,
						solver->get_state_vector(),
						solver->current_time,
						lcl->view_params,
						solver->call_id);

			//timestep module->sync object propagation

			lmod->timestep_elaborated=true;  //forbid further setting
			if (lmod->timestep_is_set)
			{
				if (solver_time_data->timestep_is_set)
				{
					sc_dt::int64 diff = lmod->timestep_set.value()
							- solver_time_data->timestep_set.value();
					if ((diff > 1) || (diff < -1))
					{
						std::ostringstream str;
						str << "Inconsistent timestep between module: "
								<< lmod->name() << ": "
								<< lmod->timestep_set << " and module: ";
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
						solver_time_data->timestep_set = lmod->timestep_set;
						solver_time_data->module_timestep_set = lmod;

						//prevent drifting
						sc_dt::int64 diff_min = lmod->timestep_set.value()
								- solver_time_data->min_timestep_set.value();
						if (diff_min > 1)
						{
							std::ostringstream str;
							str << "Inconsistent timestep between module: "
									<< lmod->name() << " timestep: "
									<< lmod->timestep_set << " and module: ";
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
						if (lmod->timestep_set < solver_time_data->min_timestep_set)
						{
							solver_time_data->module_min_timestep_set = lmod;
							solver_time_data->min_timestep_set = lmod->timestep_set;
						}
					}
				}
				else
				{
					solver_time_data->timestep_is_set = true;
					solver_time_data->timestep_set = lmod->timestep_set;
					solver_time_data->module_timestep_set = lmod;
					solver_time_data->module_min_timestep_set = lmod;
					solver_time_data->min_timestep_set = lmod->timestep_set;
				}
			}

			//end timestep module->sync object propagation

		} // for all modules


		solver->assign_equation_system(*lcl);

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

} // namespace sca_implementation
} // namespace sca_lsf
