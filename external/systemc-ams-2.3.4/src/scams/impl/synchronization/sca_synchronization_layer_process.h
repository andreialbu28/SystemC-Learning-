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

  sca_synchronization_layer_process.h - description

  Original Author: Karsten Einwich Fraunhofer IIS/EAS Dresden

  Created on: 26.08.2009

   SVN Version       :  $Revision: 2320 $
   SVN last checkin  :  $Date: 2023-06-08 12:47:38 +0000 (Thu, 08 Jun 2023) $
   SVN checkin by    :  $Author: pehrlich $
   SVN Id            :  $Id: sca_synchronization_layer_process.h 2320 2023-06-08 12:47:38Z pehrlich $

 *****************************************************************************/

/*****************************************************************************/


#ifndef SCA_SYNCHRONIZATION_LAYER_PROCESS_H_
#define SCA_SYNCHRONIZATION_LAYER_PROCESS_H_


#include <systemc>
#include <chrono>

#include "scams/impl/synchronization/sca_synchronization_alg.h"

namespace sca_core
{
namespace sca_implementation
{


class sca_synchronization_layer_process
{

public:

	std::uint64_t activation_cnt=0;
	std::uint64_t cluster_finished_executions_cnt=0;
	std::chrono::time_point<std::chrono::high_resolution_clock> start;
	std::chrono::duration<double> duration;

	bool collect_profile_data;

  sca_synchronization_layer_process(
                    sca_synchronization_alg::sca_cluster_objT* ccluster
                    );

  ~sca_synchronization_layer_process();

 protected:

  void cluster_process_control();


 private:

      sca_synchronization_alg::sca_cluster_objT* cluster;

      sca_cluster_synchronization_data* csync_data;

      sca_core::sca_time cluster_start_time;
      sca_core::sca_time static_cluster_next_start_time;
      sca_core::sca_time last_start_time;


      void change_attributes_check();

      enum cluster_process_states
	  {
    	  INITIALIZATION,
		  FIRST_START,
		  SCHEDULING,
		  CONTINUE_SCHEDULING,
		  PRE_NEXT_SCHEDULING,
		  STATIC_PRE_NEXT_SCHEDULING
	  };


      void call_change_attribute_methods();

      void cluster_initialization();
      void cluster_first_start();
      void cluster_pre_scheduling();
      void cluster_pre_next_scheduling();
      void cluster_scheduling();
      void cluster_post_scheduling();

      //simplified fast variant for static cluster
      void static_cluster_post_scheduling();
      void static_cluster_pre_next_scheduling();

      cluster_process_states process_state=INITIALIZATION;

  	std::vector<const sc_core::sc_event*> ev_vec;
  	std::vector<sc_core::sc_event_or_list> ev_or_list_vec;
  	std::vector<event_and_list2ev*>        ev_and_list_vec;

	sc_core::sc_event_or_list ev_list;

	bool first_ch_attr_call=true;
	bool first_reinitialize_call=true;
	bool reinitialize_timing=false;
	bool time_out_available=false;
	sca_core::sca_time expected_start_time;

	bool cluster_definitly_static=false;

	std::vector<sca_core::sca_implementation::sca_linear_solver*> linear_solver;
	std::vector<sca_synchronization_alg::sca_sync_objT*> active_ch_attr_vec;
	std::vector<sca_synchronization_alg::sca_sync_objT*> active_reinitialize_vec;

	std::map<std::uint64_t,std::vector<sca_synchronization_alg::sca_systemc_sync_obj::action_struct> >::iterator schedule_program_position;
	std::vector<sca_synchronization_alg::schedule_element*>::iterator schedule_position;

	bool was_dynamic=false;
	bool is_first=true;

};


}
}


#endif /* SCA_SYNCHRONIZATION_LAYER_PROCESS_H_ */
