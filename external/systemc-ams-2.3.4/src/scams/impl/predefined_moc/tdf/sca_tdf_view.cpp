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

  sca_tdf_view.cpp - description

  Original Author: Karsten Einwich Fraunhofer IIS Dresden / COSEDA Technologies

  Created on: 25.08.2009

   SVN Version       :  $Revision: 2320 $
   SVN last checkin  :  $Date: 2023-06-08 12:47:38 +0000 (Thu, 08 Jun 2023) $
   SVN checkin by    :  $Author: pehrlich $
   SVN Id            :  $Id: sca_tdf_view.cpp 2320 2023-06-08 12:47:38Z pehrlich $

 *****************************************************************************/

/*****************************************************************************/


#include "scams/impl/predefined_moc/tdf/sca_tdf_view.h"

#include "systemc-ams"
#include "scams/impl/core/sca_view_object.h"
#include "scams/impl/solver/tdf/sca_tdf_solver.h"
#include "scams/predefined_moc/tdf/sca_tdf_module.h"
#include "scams/impl/predefined_moc/tdf/sca_tdf_signal_impl_base.h"


#include<typeinfo>



namespace sca_tdf
{

namespace sca_implementation
{

////////////////////////////////////////////////

sca_tdf_view::sca_tdf_view()
{
	constant_zero=0;
	constant_one=1;
}

////////////////////////////////////////////////

sca_tdf_view::~sca_tdf_view()
{}

////////////////////////////////////////////////

void sca_tdf_view::setup_equations()
{
    sca_tdf_solver* solver;
	sca_core::sca_implementation::sca_synchronization_object_data* solver_time_data;


    for(sca_core::sca_implementation::sca_view_object::sca_module_list_iteratorT mit=
              view_datas->get_module_list().begin();
         mit!=view_datas->get_module_list().end(); ++mit
       )
    {
        sca_tdf::sca_module* sdfm=dynamic_cast<sca_tdf::sca_module*>(*mit);

        if(sdfm==NULL)
        {
            /**
              this should not be occur due the sca_sdf-view is only
              used by sca_sdf_modules
            */

            std::ostringstream str;
            str << "Error: Module: " << (*mit)->name() << " has instantiated a tdf-view "
            << " however it is not a sca_tdf::sca_module" << std::endl;
            ::sc_core::sc_report_handler::report( \
            	    ::sc_core::SC_ERROR, "SystemC-AMS", str.str().c_str(), __FILE__, __LINE__ );
            return;
        }


        //every module represents a solver instance
		std::vector<sca_core::sca_module*> tmp_mod;
		tmp_mod.push_back(sdfm);

		//no channels associated
		std::vector<sca_core::sca_interface*> tmp_chan;

		solver = new sca_tdf::sca_implementation::sca_tdf_solver(tmp_mod,tmp_chan);
		solver_time_data=solver->get_synchronization_object_data();

        //initialize modules callcounter with solver reference
        sdfm->call_counter = &(solver->get_call_counter());

        // register solver methods
        solver->register_methods(sdfm,
                                 sdfm->init_method,
                                 sdfm->reinitialize_method,
                                 sdfm->change_attributes_method,
                                 sdfm->sig_proc_method,
                                 sdfm->post_method);


        //call registered attribute method of current module
        sdfm->set_attributes_executes_flag=true;

        (sdfm->*(sdfm->attr_method))();
        sdfm->validate_port_attributes();

        sdfm->set_attributes_executes_flag=false;

        //propagate module timestep settings
        sdfm->timestep_elaborated=true; //forbid further setting
        if((sdfm->timestep_is_set)||(sdfm->timestep_max_is_set))
        {
        	solver_time_data->timestep_is_set = sdfm->timestep_is_set;
        	solver_time_data->timestep_max_is_set = sdfm->timestep_max_is_set;
        	solver_time_data->timestep_set = sdfm->timestep_set;
        	solver_time_data->timestep_max_set = sdfm->timestep_max_set;
        	solver_time_data->module_timestep_set = sdfm;
        	solver_time_data->module_min_timestep_set = sdfm;         //not required for tdf
        	solver_time_data->min_timestep_set = sdfm->timestep_set;  //not required for tdf

        	if(solver_time_data->timestep_is_set&&solver_time_data->timestep_max_is_set)
        	{
        		if(solver_time_data->timestep_max_set<solver_time_data->timestep_set)
        		{
        			std::ostringstream str;
        			str << "The maximum timestep set with set_max_timestep: ";
        			str << solver_time_data->timestep_max_set;
        			str << " is smaller than the timestep set with set_timestep: ";
        			str << solver_time_data->timestep_set;
        			str << " for module: " << sdfm->name();
        			SC_REPORT_ERROR("SytemC-AMS",str.str().c_str());
        		}
        	}
        }


        solver_time_data->first_activation_requested=sdfm->first_activation_requested;
        solver_time_data->first_activation_time_requested=sdfm->first_activation_time_requested;
        solver_time_data->first_activation_time=sdfm->first_activation_time;
        solver_time_data->first_activation_events=sdfm->first_activation_events;

        for(std::size_t i=0;i<sdfm->first_activation_event_and_lists.size();++i)
        {
        	solver_time_data->first_activation_event_and_lists.push_back(
        			new sca_core::sca_implementation::event_and_list2ev(
        					sdfm->first_activation_event_and_lists[i]));
        }
        solver_time_data->first_activation_event_or_lists=sdfm->first_activation_event_or_lists;

        //for each port
        for(sca_core::sca_module::sca_port_base_list_iteratorT pit=
                  (*mit)->get_port_list().begin();
             pit!=(*mit)->get_port_list().end(); ++pit
           )
        {
            sca_core::sca_interface* sca_if = (*pit)->sca_get_interface();

            sca_tdf_signal_impl_base* sdf_if =
                dynamic_cast<sca_tdf_signal_impl_base*>(sca_if);


            unsigned long port_id = (*pit)->get_if_id();
            sca_tdf::sca_implementation::sca_port_attributes* attr;
            attr = dynamic_cast<sca_tdf::sca_implementation::sca_port_attributes*>(*pit);

            (*pit)->elaboration_finished=true;



            //sort ports
            switch((*pit)->get_port_type())
            {
            case sca_core::sca_implementation::sca_port_base::SCA_IN_PORT:

                if(sdf_if==NULL)
                {
                    std::ostringstream str;
                    str  << "Error tdf module: " << (*mit)->name() << " has instantiated a non TDF port: "
                    << (*pit)->sca_name() << std::endl;
                    ::sc_core::sc_report_handler::report( \
                     	    ::sc_core::SC_ERROR, "SystemC-AMS", str.str().c_str(), __FILE__, __LINE__ );
                    return;
                }


                solver->push_back_from_analog(*pit,
                		sca_if,
                		sdf_if->get_rate(port_id),
                		sdf_if->get_delay(port_id),
                		sdf_if->get_timestep_calculated_ref(port_id),
                		sdf_if->get_timestep_change_id(port_id)
                                             );
                break;

            case sca_core::sca_implementation::sca_port_base::SCA_OUT_PORT:

                if(sdf_if==NULL)
                {
                    std::ostringstream str;
                    str  << "Error tdf module: " << (*mit)->name() << " has instantiated a non TDF port: "
                    << (*pit)->sca_name() << std::endl;
                    sc_core::sc_report_handler::report( \
                     	    ::sc_core::SC_ERROR, "SystemC-AMS", str.str().c_str(), __FILE__, __LINE__ );
                    return;
                }

                solver->push_back_to_analog(  *pit,
                		sca_if,
                   		sdf_if->get_rate(port_id),
                    		sdf_if->get_delay(port_id),
                    		sdf_if->get_timestep_calculated_ref(port_id),
                    		sdf_if->get_timestep_change_id(port_id)
                                           );
                break;

            case sca_core::sca_implementation::sca_port_base::SCA_SC_IN_PORT:
                if(attr==NULL)
                {
                    ::sc_core::sc_report_handler::report( \
                      	    ::sc_core::SC_ERROR, "SystemC-AMS",
                      	    "Internal error (should not be possible)",
                      	    __FILE__, __LINE__ );
                	return;
                }


                solver->push_back_from_systemc( *pit,
                		NULL,
                        attr->rate,
                         attr->delay,
                         attr->timestep_calculated,
                         attr->timestep_change_id
                                              );
                break;

            case sca_core::sca_implementation::sca_port_base::SCA_SC_OUT_PORT:

                if(attr==NULL)
                {
                    ::sc_core::sc_report_handler::report( \
                      	    ::sc_core::SC_ERROR, "SystemC-AMS",
                      	    "Internal error (should not be possible)",
                      	    __FILE__, __LINE__ );
                	return;
                }

                solver->push_back_to_systemc( *pit,
                		NULL,
                        attr->rate,
                         attr->delay,
                         attr->timestep_calculated,
                         attr->timestep_change_id
                                            );
                break;

            case sca_core::sca_implementation::sca_port_base::SCA_DECL_OUT_PORT:
            {

                if(sdf_if==NULL)
                {
                    ::sc_core::sc_report_handler::report( \
                      	    ::sc_core::SC_ERROR, "SystemC-AMS",
                      	    "Internal error (should not be possible)",
                      	    __FILE__, __LINE__ );
                	return;
                }


                //for a decluster port we introduce a synchronization point to SystemC
                //in the current cluster -> the write_to_sc callback of the port will be called at all
                //sample time points

                solver->push_back_to_systemc( *pit,
                		NULL,
                         attr->rate,
                         attr->get_delay_internal(),
                         attr->timestep_calculated,
                         attr->timestep_change_id
                                            );


                //in the declustered cluster we instantiate an additional solver (dataflow module)
                //with a synchronization point from SystemC -> the write_to_sc callbacks
                //of the port will be called at all sample time points

                //create separate solver instance for declustering
                std::vector<sca_core::sca_module*> empty_list; //no module belongs to this solver
                sca_tdf_solver* dec_solver=new sca_tdf::sca_implementation::sca_tdf_solver(empty_list,tmp_chan);
                //dec_solver->register_methods(dynamic_cast<sc_core::sc_object*>(*pit), NULL, NULL, NULL, NULL, NULL);

                //dataflow outport to the declustered cluster
                dec_solver->push_back_to_analog( *pit,
                		sca_if,
                		constant_one,  //rate should be 1
                		constant_zero, //delay should be 0
                		sdf_if->get_timestep_calculated_ref(port_id),
                		sdf_if->get_timestep_change_id(port_id)
                                                            );

                sca_core::sca_implementation::sca_port_base* pp=*pit;
                dec_solver->register_processing_method([=](){pp->port_processing();});

                //change sync domain for this port
                (*pit)->set_sync_domain(dec_solver);

                //synchronization point to SystemC
                // read_from_sc port callbacks are called to the corresponding
                //SystemC time points
                dec_solver->push_back_from_systemc( *pit,
                 		NULL,
						constant_one,  //rate should be 1
						constant_zero, //delay should be 0
                 		sdf_if->get_timestep_calculated_ref(port_id),
                 		sdf_if->get_timestep_change_id(port_id)
                                                             );

                break;
            }

            default:

            	std::ostringstream str;
            	str << " not allowed port instantiated in a tdf module for port: ";
            	str <<  (*pit)->sca_name();
                ::sc_core::sc_report_handler::report( \
                 	    ::sc_core::SC_ERROR, "SystemC-AMS", str.str().c_str(), __FILE__, __LINE__ );

            }
        }
    }
}


}
}
////////////////////////////////////////////////
