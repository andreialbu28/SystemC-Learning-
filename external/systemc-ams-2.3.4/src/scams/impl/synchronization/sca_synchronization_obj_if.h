/*****************************************************************************

    Copyright 2010
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

 sca_synchronization_obj_if.h - description

 Original Author: Karsten Einwich Fraunhofer IIS Dresden / COSEDA Technologies

 Created on: 14.05.2009

 SVN Version       :  $Revision: 2320 $
 SVN last checkin  :  $Date: 2023-06-08 12:47:38 +0000 (Thu, 08 Jun 2023) $
 SVN checkin by    :  $Author: pehrlich $
 SVN Id            :  $Id: sca_synchronization_obj_if.h 2320 2023-06-08 12:47:38Z pehrlich $

 *****************************************************************************/

/*****************************************************************************/

#ifndef SCA_SYNCHRONIZATION_OBJ_IF_H_
#define SCA_SYNCHRONIZATION_OBJ_IF_H_


#include <systemc-ams>
#include "scams/impl/synchronization/sca_sync_value_handle.h"
#include "scams/impl/util/tracing/sca_trace_object_data.h"

namespace sca_core
{

class sca_prim_channel;

namespace sca_implementation
{

class sca_port_base;
class sca_sync_alg_objT;
class sca_solver_base;
class sca_synchronization_obj_if;
class sca_sync_value_handle_base;

typedef void (::sc_core::sc_object::*sc_object_method)();

//object derived from sc_event which fires if the and list fires
// -> creates a single event from an and list
class event_and_list2ev : public sc_core::sc_event
{
public:

	event_and_list2ev(const sc_core::sc_event_and_list& list);
	~event_and_list2ev();

private:

	void and_event_list_process();

	sc_core::sc_process_handle and_proc_h;
	bool method_started;
	sc_core::sc_event_and_list and_list;
};

/** structure which contains all data for cluster time and attribute change handling
 *
 */
struct sca_cluster_synchronization_data
{
	// "static" cluster execution time resulting
	// from set_timestep/set_max_timestep (not request_next_activation)
	// used e.g. for calculating port timesteps for sample_id>0
	sca_core::sca_time cluster_period;
	sca_core::sca_time last_cluster_period;
	sca_core::sca_time last_last_cluster_period;

	// "dynamic" cluster timestep -> cluster_start_time - last_cluster_start_time
	// -> differs from cluster_period if
	// request_next_activation changes the cluster_start_time
	sca_core::sca_time current_cluster_timestep;

	//startime of the cluster execution
	sca_core::sca_time cluster_start_time;
	sca_core::sca_time last_cluster_start_time;
	sca_core::sca_time last_last_cluster_start_time;

	//end time of the cluster execution
	sca_core::sca_time cluster_end_time;
	sca_core::sca_time last_cluster_end_time;
	sca_core::sca_time last_last_cluster_end_time;

	//is true as long as the cluster executes
	bool              cluster_executes;

	sc_core::sc_event cluster_start_event; //TODO remove????????????????? interactive tracing
	sc_core::sc_event cluster_end_event; //TODO remove????????????????? interactive tracing

	sca_core::sca_time cluster_max_time_step;


	sca_core::sca_time requested_next_cluster_start_time;

	//will be used to calculate cluster duration (period-period/max_calls)
	//the maximum number of executions of a module in the cluster
	sc_dt::uint64    max_calls_per_period;
	sc_dt::uint64    last_max_calls_per_period;

	//maximum processed samples of a port per cluster period
	//will be used to calculate the cluster execution end time
	sc_dt::uint64    max_port_samples_per_period;

	bool request_new_start_time;
	bool request_new_max_timestep;

	bool change_timestep;
	sca_core::sca_time new_timestep;
	sca_core::sca_time new_max_timestep;
	sc_core::sc_object* new_timestep_obj;
	sc_core::sc_object* max_timestep_obj;
	//calls per period of the object which has a timestep change
	long calls_per_period_timestep_ch_obj;



	std::vector<const sc_core::sc_event*>   reactivity_events;
	std::vector<sc_core::sc_event_or_list>  reactivity_event_or_lists;
	std::vector<event_and_list2ev*>         reactivity_event_and_lists;
	sca_core::sca_time                      reactivity_event_time_out;

	std::vector<sca_util::sca_implementation::sca_trace_object_data*> traces;

	bool is_dynamic_tdf;
	bool attribute_changes_allowed;
	bool attribute_changes_may_allowed;
	bool attributes_changed;

	bool attribute_change_flags_changed;

	//counts attribute changes - used as id to detect whether
	//calculations are updated
	unsigned long attribute_change_id;

	//is used by sca_synchronization_alg during analysis (scheduling list set up)
	bool dead_cluster;


	bool rescheduling_requested;
	bool rescheduled;        //cluster was rescheduled
	bool last_rescheduled;

	long cluster_id;

	//counts cluster executions - can be used as id for changes (e.g. timestep)
	sc_dt::int64 cluster_execution_cnt;

	//cluster execution counter of the last timestep change
	sc_dt::int64 last_timestep_change_id;


	sca_cluster_synchronization_data()
	{
		max_calls_per_period=0;
		last_max_calls_per_period=0;
		max_port_samples_per_period=0;

		cluster_executes=false;

		request_new_start_time=false;
		request_new_max_timestep=false;
		change_timestep=false;
		new_timestep_obj=NULL;
		max_timestep_obj=NULL;
		calls_per_period_timestep_ch_obj=0;

		is_dynamic_tdf=false;
		attribute_changes_allowed=false;
		attribute_changes_may_allowed=false;
		attributes_changed=false;
		attribute_change_flags_changed=false;

		attribute_change_id=0;

		dead_cluster=false;

		rescheduling_requested=false;
		rescheduled=false;
		last_rescheduled=false;

		cluster_id=-1;

		cluster_execution_cnt   =-1; //never executed
		last_timestep_change_id =-1; //maybe assignment in set_attributes
		                             //the default of the port id is -2,
		                             //thus, not assigned will be detected
	}
};


/** structure with contains all synchronization object / solver specific
 * time data are instantiated by sca_solver_base
 */

struct sca_synchronization_object_data
{
	sca_core::sca_time timestep_set;
	sca_core::sca_time timestep_max_set;
	sca_core::sca_time timestep_calculated;
	sca_core::sca_module* module_timestep_set;
	bool               timestep_is_set;
	bool               timestep_max_is_set;
	sca_core::sca_module* module_min_timestep_set;
	sca_core::sca_time    min_timestep_set;

	bool first_activation_time_requested;
	bool first_activation_requested;
	sca_core::sca_time first_activation_time;
	std::vector<const sc_core::sc_event*>  first_activation_events;
	std::vector<sc_core::sc_event_or_list> first_activation_event_or_lists;
	std::vector<event_and_list2ev*>        first_activation_event_and_lists;

	long cluster_id;

	//position in cluster list
	unsigned long sync_id;

	//position in overall list
	unsigned long id;

	std::string sync_obj_name;

	//will be adjusted after re-scheduling to be able
	//to detect first call of the cluster execution
	sc_dt::int64 call_counter;
	//counts execution to create a unique increasing id
	//will not be changed after re-scheduling
	sc_dt::int64 id_counter;

	long calls_per_period;
	long last_calls_per_period;
	long last_last_calls_per_period;
	bool allow_processing_access_flag;

	bool dead_cluster;

	sca_synchronization_object_data()
	{
		module_timestep_set=NULL;
		timestep_is_set=false;
		timestep_max_is_set=false;
		module_min_timestep_set=NULL;

		first_activation_time_requested=false;
		first_activation_requested=false;

		cluster_id=-1;
		sync_id=0;
		id=0;

		call_counter=0;
		id_counter=0;

		calls_per_period=0;
		last_calls_per_period=0;
		last_last_calls_per_period=0;
		allow_processing_access_flag=false;

		dead_cluster=false;
	}

};



/** Enumeration for port direction
 *
 */
enum sca_port_direction_enum
{
	SCA_NOT_DEFINED_DIRECTION,

	SCA_TO_ANALOG, //outport to other analog obj
	SCA_FROM_ANALOG, //inport from other analog obj

	SCA_TO_SYSTEMC, //outport to SystemC kernel
	SCA_FROM_SYSTEMC
//inport from SystemC kernel
};


/**
 struct which collects the properties/attributtes
 of a synchronization port
 */
struct sca_synchronization_port_data
{

	//reference to port which represents the sync-port
	sca_port_base* port;

	//reference to number of data items which has to be
	//proceeded per call
	unsigned long* rate;

	//reference to number of data items which are
	//included/written during initialization
	const unsigned long* delay;

	//reference to set time distance of the data items
	sca_core::sca_time* T_set;

	sc_dt::int64* timestep_change_id;

	//reference to the connected channel
	sca_core::sca_prim_channel* channel;

	//direction of the connection (port)
	sca_port_direction_enum dir;

	//calculated  time distance of data items by the sync-cur_algorithm
	sca_core::sca_time T_calculated;

	unsigned long sync_port_number;
	sca_synchronization_obj_if* parent_obj;

};



///////////////////////////////////////////////////////////////////////////////


/**
 Interface class for the synchronization layer. The interface will
 be implemented by @ref #sca_solver_base, due a solver is a
 synchronization object.
 Additional methods for configurating the synchronization
 and to change this configuration during simulation are
 provided. The methods will be implemented by the synchronization.
 */
class sca_synchronization_obj_if
{
public:

	virtual ~sca_synchronization_obj_if()
	{
	}


	/**
	 * Adds trace which is activated (the trace method of the object is called)
	 * after each cluster calculation by the synchronization layer
	 * (traces usually signals between solvers)
	 */
	virtual void add_cluster_trace(sca_util::sca_implementation::sca_trace_object_data& tr_obj) = 0;

	/**
	 * Adds trace which is activated (the trace method of the object is called)
	 * by the specific solver (traces usually signals/values inside a solver)
	 * returns true if successful
	 */
	virtual bool add_solver_trace(sca_util::sca_implementation::sca_trace_object_data& tr_obj) = 0;



	/** registers method which is called after the event ev
	 * will be used to trace an channel from SystemC-kernel
	 * a handle will be returned
	 * abstract interface method implemented in sca_solver_base
	 */
	virtual void register_sc_value_trace(const ::sc_core::sc_event& ev,
			sca_sync_value_handle_base& handle)=0;



	/** registers synchronization from a sca domain to a sc domain -
	 * limits the sc time to the first sca schedule time
	 */
	virtual void register_sca_schedule(::sc_core::sc_time next_time,
			sca_sync_value_handle_base& handle) =0;

	/** writes with write_method a value to a channel at time ctime or maybe
	 * timeless - the SystemC-kernel will suspend (and thus resume the sca -
	 * cluster) not later then next_time - during this activation the mehtod
	 * must be called to remove the old and schedule a new next_time -
	 * otherwise it should give an exception, due we deadlocked
	 * for timeless (ordered) synchronization next_time can be set to a large
	 * value or SC_INVALID_TIME - in this case the sc_channel suspends if a
	 * value required (we have to do nothing therefore)
	 * abstract interface method
	 */
	virtual void write_sc_value(::sc_core::sc_time ctime, ::sc_core::sc_time next_time,
			sca_sync_value_handle_base& handle) = 0;

	//type dependent user method (used in concrete synchronization channel)
	template<class T>
	void write_sc_value(::sc_core::sc_time ctime, ::sc_core::sc_time next_time,
			sca_sync_value_handle<T>& handle, T& value)
	{
		handle.write_tmp(value);
		write_sc_value(ctime, next_time, handle);
	}


	/** sets for the next cluster start the start maximal time - if the method
	 *  called from different objects the minimum time will be choosen - the
	 * method returns the number of object calls before the time will be
	 * accepted (in a multirate system the number of object calls in the current
	 * period - in single rate system this number will be always 0)ntime is
	 * setted to the time used if the return value >0
	 * (ntime = ret_val * cluster_period/calls_per_period_timestep_ch_obj)
	 */
	virtual long set_max_start_time(const sca_core::sca_time& ntime) = 0;

	/** sets the maximum time period which will be valid up to the next cluster
	 * start (the time is the cluster_period/(obj calls per cluster) ), if the
	 * time is setted from othe objects of the clusters also the time resulting
	 * from the minimum cluster_period will be used
	 * the method returns the number of object calls in the current period
	 * before the time will be accepted  (in a single rate system this number
	 * will be allways 0)
	 */
	virtual long set_max_period(const sca_core::sca_time& nperiod,sc_core::sc_object*) = 0;

	/** registers an event for activation of the next cluster start
	 * the method returns the time before the event can be
	 * accepted (events before will be ignored) - for single rate always <
	 * cluster period and if no t0 specified it should be equal to the
	 * current time
	 * after restarting the cluster the events will be removed from the list
	 * -> thus the events has to be re-registered every cluster period
	 */
	virtual ::sc_core::sc_time register_activation_event(const sc_core::sc_event& ev)=0;

	/**
	 Returns number of synchronization ports to other synchronization objects
	 */
	virtual unsigned long get_number_of_sync_ports(sca_port_direction_enum direction)=0;

	/**
	 Returns the number synchronization ports to/from an other
	 synchronization object, or systemc discrete event kernel
	 (for the first channel number=0) if number>= number of channels a null
	 pointer is returned.
	 */
	virtual sca_synchronization_port_data* get_sync_port(sca_port_direction_enum direction,
			unsigned long number)=0;

	/**
	 Returns initialization method for synchronization object.
	 */
	virtual void get_initialization_method(::sc_core::sc_object*&, sc_object_method&)=0;


	/**
	 Returns change attributes method for synchronization object.
	 new SystemC-AMS 2.0 dtdf feature
	 */
	virtual void get_reinitialization_method(::sc_core::sc_object*&, sc_object_method&)=0;

	/**
	 Returns change attributes method for synchronization object.
	 new SystemC-AMS 2.0 dtdf feature
	 */
	virtual void get_change_attributes_method(::sc_core::sc_object*&, sc_object_method&)=0;

	/**
	 Returns processing method for synchronization object.
	 */
	virtual void get_processing_method(::sc_core::sc_object*&, sc_object_method&)=0;

	/**
	 Returns post processing method for synchronization object.
	 */
	virtual void get_post_method(::sc_core::sc_object*&, sc_object_method&)=0;


	/** terminates synchronization objects (calls post methods)
	 */
	virtual void terminate()=0;

	/**
	 Resets all states to allow re-analysing of the graphs
	 */
	virtual void reset()=0;

	/**
	 Returns name of synchronization object -> for debugging and
	 error messages
	 */
	virtual const std::string& get_name() const = 0;


	virtual std::string get_name_associated_names(int max_num=-1) const = 0;

	/**
	 * the object is not yet clustered
	 */
	virtual bool not_clustered()=0;


	/**
	 * returns object specific synchronization data - instantiated by
	 * sca_solver_base
	 */
	virtual sca_synchronization_object_data* get_synchronization_object_data()=0;

	/**
	 * Returns the cluster synchronization data of the corresponding cluster
	 * this data are instantiated by the cluster
	 */
	virtual sca_cluster_synchronization_data* get_cluster_synchronization_data()=0;



private:

	friend class sca_synchronization_alg;

	//mechanism to transfer data from the sync-cluster to the solver obj (sca_solver_base)
	virtual void set_sync_data_references(sca_cluster_synchronization_data& crefs) = 0;

};



} //namaespace sca_implementation
} //namespace sca_core


#endif /* SCA_SYNCHRONIZATION_OBJ_IF_H_ */
