/*****************************************************************************

    Copyright 2010-2013
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

  sca_synchronization_alg.h - description

  Original Author: Karsten Einwich Fraunhofer IIS/EAS Dresden

  Created on: 25.08.2009

   SVN Version       :  $Revision: 2320 $
   SVN last checkin  :  $Date: 2023-06-08 12:47:38 +0000 (Thu, 08 Jun 2023) $
   SVN checkin by    :  $Author: pehrlich $
   SVN Id            :  $Id: sca_synchronization_alg.h 2320 2023-06-08 12:47:38Z pehrlich $

 *****************************************************************************/

/*****************************************************************************/


#ifndef SCA_SYNCHRONIZATION_ALG_H_
#define SCA_SYNCHRONIZATION_ALG_H_

#include <systemc>
#include "scams/impl/synchronization/sca_synchronization_obj_if.h"


#include<vector>
#include<list>


namespace sca_core
{
namespace sca_implementation
{


/** forward class definition */
class sca_synchronization_layer_process;

/**
  Implements synchronization cur_algorithm. Currently
  a static dataflow scheduler is implemented for
  synchronization.
*/
class sca_synchronization_alg
{

public:

	class sca_systemc_sync_obj;
	class schedule_element;

 class sca_sync_objT
 {
  public:

    sca_synchronization_obj_if*      sync_if;
    sca_synchronization_object_data* object_data;

    //sca_sync_objT* driver;
    std::vector<sca_sync_objT*> inports;

    //id's of the synchronization ports
    //used to determine the SystemC synchronization points for multi-rate
    std::vector<unsigned long> to_systemc_ports_ids;
    std::vector<unsigned long> from_systemc_ports_ids;

    sca_systemc_sync_obj* systemc_synchronizer;

    unsigned long nin;
    std::vector<unsigned long> in_rates;
    std::vector<unsigned long> sample_inports;

    unsigned long nout;
    std::vector<unsigned long> out_rates;
    //outports with references to the connected inports
    std::vector<std::vector<unsigned long*> > next_inports;
    std::vector<unsigned long> max_out_buffer_usage;


    //reschedules cluster with id cluster_id
    void reschedule_cluster(unsigned long cluster_id);

    //initializes sync_obj datastructure especially for attribute change check
    void initialize_sync_obj();

    //calls initialize methods
    void call_init_method();


    // new SystemC-AMS 2.0 dtdf extension
    bool call_reinit_method();

    // new SystemC-AMS 2.0 dtdf extension
    bool call_change_attributes_method();


    /** initializes synchronization object */
    void init(sca_synchronization_obj_if* sync_if_);

    /** resets synchronization object data after rate/delay change */
    void reset();

    /** re-initializes synchronization object after rate/delay change */
    void reinit();

    /** check for schedubility and schedule object */
    bool schedule_if_possible();


    //is used by sca_synchronization_alg during analysis (scheduling list set up)
    long multiple;

    bool attribute_changes_allowed;
    bool accepts_attribute_changes;
    sc_core::sc_object** current_context_ref;

    //order criteria for the object in the graph
    //we try to give objects connected in a chain preceding numbers
    //to achieve a faster scheduling
    std::int64_t graph_order_id;

    std::vector<schedule_element*>  schedule_elements;
    std::size_t next_schedule_element;

    inline bool schedule_pre_order_criteria(sca_sync_objT* obj)
    {
    	if(schedule_elements.size()==obj->schedule_elements.size())
    	{
    		//we try to follow the scheduling graph
    		return this->graph_order_id < obj->graph_order_id;
    	}

    	//the probability of obj with more schedule elements is higher
    	// -> thus they should be at the beginning of the list
    	return this->schedule_elements.size() > obj->schedule_elements.size();
    }

    sca_sync_objT();

  private:

    sc_core::sc_object* reinit_obj;
    sca_core::sca_implementation::sc_object_method reinit_meth;

 };

 ////////////////////////////////////////////////////////////////////////////
 ////////////////////////////////////////////////////////////////////////////



 /**
  * this object represents SystemC and manages the synchronization with
  * the SystemC kernel
  */
 class sca_systemc_sync_obj
 {
 public:

	 /**
	  * add port which comes from SystemC and returns the id
	  * @param
	  */
	 unsigned long add_from_systemc_port(sca_synchronization_port_data*,unsigned long);

	 /**
	  * adds port goes to SystemC and returns the id
	  * @param
	  */
	 unsigned long add_to_systemc_port(sca_synchronization_port_data*,unsigned long);

	 /**
	  * returns true if a read from a port with the id id from SystemC is possible
	  * @param id
	  * @return
	  */
	 bool is_read_from_systemc_possible(unsigned long id);

	 /**
	  * reads from port with the id id from SystemC
	  * @param id
	  */
	 void read_from_systemc(unsigned long id);

	 /**
	  * write to port with id id to SystemC
	  * @param id
	  */
	 void write_to_systemc(unsigned long id);

	 /**
	  * increases time as far as possible
	  * returns the scaled time which was reached from cluster period start
	  * @return
	  */
	 std::uint64_t run_as_far_as_possible();

	 /**
	  * scale all time values to become multiple of the rate
	  * this will permit division by rate without rounding error
	  * @param rate
	  */
	 void scale_time_to_lcm_for_rate(unsigned long rate);

	 void reset();



	 std::vector<unsigned long> scaled_time_per_call_to_systemc;
	 std::vector<unsigned long> scaled_time_per_call_from_systemc;

	 std::vector<long> scaled_time_reached_to_systemc;         //before the call time 0 not reached -> negative
	 std::vector<long> scaled_next_call_max_time_from_systemc; //if delay>0 scaled time can be negative
	 std::vector<unsigned long> scaled_next_from_systemc_time; //time when the next read from SystemC is scheduled (without delay)

	 std::vector<sca_synchronization_port_data*> ports_to_systemc;
	 std::vector<sca_synchronization_port_data*> ports_from_systemc;

	 std::uint64_t scaled_time_period=0;
	 std::int64_t scaled_time_reached=0;

	 std::uint64_t scaled_systemc_time_reached=0;

	 std::uint64_t scaled_max_read_write_time=0;

	 std::uint64_t scaled_min_timestep=0;

     struct action_struct
	 {
    	 enum action_enum
		 {
    		 NONE,
    		 READ_SC,
			 WRITE_SC,
			 SCHEDULE
		 };

    	 action_enum action=NONE;


    	sca_synchronization_port_data* port=NULL;

    	//defines the breakpoints in the schedule list (element after break) and the
    	//scaled time SystemC has to progress before continue the schedule
    	std::vector<schedule_element*>::iterator slice_end;

	 };

     //synchronization scheme within schedule period
     //scaled time from period start with corresponding actions
     std::map<std::uint64_t,std::vector<action_struct> > schedule_program;

     //sorts the actions for each time point -> first read, schedule, write
     void sort_program_actions();

 };


 class sca_cluster_objT;

 void initialize_systemc_synchronizers();
 void initialize_systemc_synchronizer(sca_cluster_objT* cluster);
 void initialize_ports();
 void initialize_ports(sca_cluster_objT* cluster);
 bool reinitialize_port_timings(sca_cluster_objT* cluster);
 void reinitialize_ports(sca_cluster_objT* cluster);

 ////////////////////////////////////////////////////////////////////////////
 ////////////////////////////////////////////////////////////////////////////


 //memory for synchronization objects
 //to save allocation and initialization time
 sca_sync_objT* sync_obj_mem;

 /** class which stores informations regarding scheduling time
 */

 class schedule_element
 {
 public:

	sc_core::sc_object* schedule_obj;
  	sc_object_method    proc_method;

  	sc_core::sc_object** current_context_ref;

  	unsigned long call;

  	sc_dt::int64* call_counter;
  	sc_dt::int64* id_counter;

  	bool*      allow_processing_access;

  	sca_sync_objT* obj;

  	//pre-odering criteria
   	inline  bool less_time (schedule_element* ele1)
 	{
   			if(this->scaled_schedule_time == ele1->scaled_schedule_time)
   			{
   				return this->obj->graph_order_id < ele1-> obj->graph_order_id;
   			}
 			return (this->scaled_schedule_time < ele1->scaled_schedule_time);
 	}

	 inline bool schedule_if_possible()
	 {
	 	return(obj->schedule_if_possible());
	 }

	 inline void run()
	 {

	 	(*allow_processing_access)=true;
	 	*this->current_context_ref=schedule_obj;
	 	if(schedule_obj!=NULL) (schedule_obj->*(proc_method))();
	 	*this->current_context_ref=NULL;
	 	(*allow_processing_access)=false;

	 	(*call_counter)++; //will be adjusted after re-scheduling to be able
	 	                   //to detect first call of the cluster execution
	 	(*id_counter)++;   //counts execution to create a unique increasing id
	 }


  	unsigned long scaled_schedule_time;
 };



/**
  Class for encapsulating synchronization cluster data
*/
class sca_cluster_objT
{
	 std::vector<sca_sync_objT*>  obj_list;

public:

	 std::vector<sca_sync_objT*>& get_obj_list() {return obj_list;}

	 long id;
	 sca_synchronization_alg::sca_systemc_sync_obj systemc_synchronizer;

	 std::vector<sca_core::sca_prim_channel*> channels;

	 long n_allowed_attribute_changes;
	 sca_sync_objT* last_attribute_changes;
	 long n_accept_attribute_changes;
	 sca_sync_objT* last_not_accept_changes;

     typedef std::vector<sca_sync_objT*>::iterator iterator;

     bool dead_cluster;

     sca_synchronization_layer_process* csync_mod;

     //data for statistics
     sc_dt::int64 calls_per_period_max;
     std::string  mod_name_period_max;
     sc_dt::int64 calls_per_period_min;
     std::string  mod_name_period_min;

     void push_back(sca_sync_objT* obj) { obj_list.push_back(obj); }

     inline iterator end()    { return obj_list.end();   }
     inline iterator begin()  { return obj_list.begin(); }
     inline std::size_t size() const { return obj_list.size(); }

     sca_sync_objT*& operator [] (unsigned long n) { return obj_list[n]; }

     sca_cluster_synchronization_data csync_data;

     unsigned long schedule_list_length;
     unsigned long scaled_time_lcm;


     sca_core::sca_time        T_cluster;         //period of cluster
     sca_core::sca_time        T_max_cluster;     //maximum timestep
     unsigned long  T_multiplier;      //multiplier of T_cluster assignment
     unsigned long  T_max_multiplier;      //multiplier of T_cluster assignment
     sca_sync_objT* T_last_obj;        //reference to obj which assigned T_cluster
     sca_sync_objT* T_max_last_obj;        //reference to obj which assigned T_cluster
     long           T_last_n_sample;   //number of sample
      //reference to port which assigned T_cluster
     sca_synchronization_port_data* T_last_port;
     //time of last analyzed object / port
     sca_core::sca_time T_last;

     bool is_max_timestep;

     std::int64_t fwd_cnt;
     std::int64_t bwd_cnt;
     bool fwd_dir;

     //pointer to memory of schedule elements - to speed up
     //sorting by copy pointers instead of complete classes
     schedule_element* scheduling_elements;

     std::vector<schedule_element*> scheduling_list;


     //fast remove possible
     std::list<schedule_element*> scheduling_list_tmp;

     void finish_simulation();

     //for debugging
     void print();
     void print_schedule_list();

     sca_cluster_objT()
     {
        dead_cluster = false;
        csync_mod    = NULL;

        csync_data.last_cluster_period = NOT_VALID_SCA_TIME();

        csync_data.cluster_period       = sc_core::SC_ZERO_TIME;
        csync_data.requested_next_cluster_start_time   = sc_core::SC_ZERO_TIME;

        csync_data.request_new_start_time    = false;
        csync_data.request_new_max_timestep        = false;
        csync_data.change_timestep           = false;

        csync_data.max_calls_per_period      = 0;
        csync_data.new_timestep_obj          = NULL;
        csync_data.max_timestep_obj          = NULL;

        csync_data.is_dynamic_tdf=false;
        csync_data.attribute_changes_allowed=false;
        csync_data.attribute_changes_may_allowed=false;
        csync_data.attributes_changed=false;

        csync_data.max_port_samples_per_period=1;


        schedule_list_length = 0;
        scaled_time_lcm      = 0;

        T_last_obj  = NULL;
        T_max_last_obj  = NULL;
        T_last_port = NULL;

        T_multiplier=0;
        T_max_multiplier=0;
        T_cluster=sc_core::SC_ZERO_TIME;
        T_max_cluster=sca_core::sca_max_time();
        T_last_n_sample=0;

        calls_per_period_max=0;
        calls_per_period_min=0x7fffffff;

        n_allowed_attribute_changes=0;
        n_accept_attribute_changes=0;
        last_attribute_changes=NULL;
        last_not_accept_changes=NULL;

        is_max_timestep=true;

        scheduling_elements=NULL;

        id=-1;

        fwd_cnt=0;
        bwd_cnt=0;
        fwd_dir=false;
     }

     //resets data for re-scheduling
     void reset_analyse_data()
     {
    	 for (sca_cluster_objT::iterator sit = this->begin();
    			 sit != this->end(); sit++) //go through all objects
    	 {
    		 //reinit datastructure (especially to current rate values)
    		 (*sit)->reset();
    	 }

    	 for (sca_cluster_objT::iterator sit = this->begin();
    			 sit != this->end(); sit++) //go through all objects
    	 {
    		 //reinit datastructure (especially to current rate values)
    		 (*sit)->reinit();
    	 }

         csync_data.cluster_period       = sc_core::SC_ZERO_TIME;

         csync_data.max_calls_per_period      = 0;
         csync_data.new_timestep_obj          = NULL;
         csync_data.max_timestep_obj          = NULL;

         csync_data.max_port_samples_per_period=1;


         schedule_list_length = 0;
         scaled_time_lcm      = 0;

         T_last_obj  = NULL;
         T_max_last_obj  = NULL;
         T_last_port = NULL;

         T_multiplier=0;
         T_max_multiplier=0;
         T_cluster=sc_core::SC_ZERO_TIME;
         T_max_cluster=sca_core::sca_max_time();
         T_last_n_sample=0;

         calls_per_period_max=0;
         calls_per_period_min=0x7fffffff;

         n_allowed_attribute_changes=0;
         n_accept_attribute_changes=0;
         last_attribute_changes=NULL;
         last_not_accept_changes=NULL;

         is_max_timestep=true;

     }

};

 public:

	sca_synchronization_alg();
	~sca_synchronization_alg();

  /** Initializes datastructures and calculates the scheduling list */
  void initialize(std::vector<sca_synchronization_obj_if*>& solvers);

  void reanalyze_cluster_timing(unsigned long cluster_id);
  void reschedule_cluster(unsigned long cluster_id);
  bool reinitialize_custer_timing(unsigned long cluster_id);
  void reinitialize_custer_datastructures(unsigned long cluster_id);

  sc_core::sc_object* get_current_context() { return current_context; }

  void finish_simulation();

 private:

  typedef std::vector<sca_synchronization_obj_if*> sca_sync_obj_listT;


  //list for all (unclustered) objects
  sca_sync_obj_listT sync_objs;

  //list for objects which can't be analyzed in the first step, due zero port_rates
  sca_sync_obj_listT remainder_list;

  //list of synchronization clusters
  std::vector<sca_cluster_objT*> clusters;

  //reference to cluster which is currently analyzed
  //sca_cluster_objT* current_cluster;

  sc_dt::uint64 schedule_list_length;
  sc_dt::uint64 scaled_time_lcm;
  bool scheduling_list_warning_printed;

  sc_core::sc_object* current_context;


  void check_closed_graph();
  void cluster();

  void analyse_all_sample_rates();
  void analyse_sample_rates(sca_cluster_objT* current_cluster);
  bool analyse_sample_rates_first_obj(
		                sca_cluster_objT* current_cluster,
						unsigned long&                             nin,
						unsigned long&                             nout,
						long&                                      multiplier,
						sca_synchronization_alg::sca_sync_objT* const&   obj,
						sca_cluster_objT::iterator&                sit
									);

void analyse_sample_rates_calc_obj_calls (
						unsigned long&                              nin,
						unsigned long&                              nout,
						long&                                       multiplier,
						sca_synchronization_alg::sca_sync_objT* const&    obj
										);

void analyse_sample_rates_assign_max_samples (
		                sca_cluster_objT* current_cluster,
						unsigned long&                              nin,
						unsigned long&                              nout,
						long&                                       multiplier,
                        sca_synchronization_alg::sca_sync_objT* const&    obj
											  );

  void check_sample_time_consistency (
		                        sca_cluster_objT* current_cluster,
                                sca_synchronization_alg::sca_sync_objT* const& obj,
                                sca_synchronization_port_data* sport,
                                long multiplier
                                      );



void analyse_sample_rates_calc_def_rates (
		                sca_cluster_objT* current_cluster,
						long&                                       multiplier,
						sca_synchronization_alg::sca_sync_objT* const&    obj
									     );

  //greatest common divider (recursive function) called by lccm
  static sc_dt::uint64
          analyse_sample_rates_gcd(sc_dt::uint64 x, sc_dt::uint64 y);

  //lowest common multiple (LCM)
  static unsigned long analyse_sample_rates_lcm(unsigned long n1, unsigned long n2);

  void move_connected_objs(sca_cluster_objT* cluster,long cluster_id,
                           sca_sync_objT*  current_obj );
  sca_sync_objT* move_obj_if_not_done(sca_synchronization_obj_if* sync_obj,
										    sca_cluster_objT*    cluster,
                                            long                 cluster_id,
											std::vector<sca_sync_objT*>&);


   void analyse_timing();

   void generate_all_scheduling_lists();
   void generate_scheduling_list(sca_cluster_objT* current_cluster);

   bool comp_time(schedule_element* e1, schedule_element* e2);
   void generate_scheduling_list_expand_list(
							sca_synchronization_alg::sca_cluster_objT& cluster
												                   );
   void generate_scheduling_list_schedule(
							sca_synchronization_alg::sca_cluster_objT& cluster);


};




}
}


#endif /* SCA_SYNCHRONIZATION_ALG_H_ */
