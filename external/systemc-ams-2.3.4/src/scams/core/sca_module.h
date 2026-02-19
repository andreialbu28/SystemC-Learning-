/*****************************************************************************

    Copyright 2010-2013
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

 sca_module.h - base class for all sca_modules

 Original Author: Karsten Einwich Fraunhofer IIS Dresden / COSEDA Technologies

 Created on: 03.03.2009

 SVN Version       :  $Revision: 2160 $
 SVN last checkin  :  $Date: 2021-06-21 11:17:32 +0000 (Mon, 21 Jun 2021) $ (UTC)
 SVN checkin by    :  $Author: karsten $
 SVN Id            :  $Id: sca_module.h 2160 2021-06-21 11:17:32Z karsten $

 *****************************************************************************/

/*
 LRM clause 3.2.1
 The class sca_core::sca_module shall define the base class to
 derive primitive modules for the predefined models of computation.
 */

/*****************************************************************************/

#ifndef SCA_MODULE_H_
#define SCA_MODULE_H_

//// begin implementation specific declarations //////////////

namespace sca_core
{

class sca_prim_channel;

namespace sca_implementation
{
class sca_port_base;
class sca_solver_base;
class sca_view_manager;
class sca_object_manager;
class sca_view_base;
class sca_conservative_module;
class sca_conservative_view;

}
}

namespace sca_tdf
{
class sca_module;

namespace sca_implementation
{
  class sca_tdf_signal_impl_base;
  class sca_tdf_view;
  class sca_trace_variable_base;
}
}

namespace sca_eln
{
class sca_module;
class sca_node;

namespace sca_implementation
{
class sca_eln_view;
}
}

namespace sca_lsf
{
class sca_module;
class sca_signal;

namespace sca_implementation
{
class sca_lsf_view;
}
}



namespace sca_ac_analysis
{
namespace sca_implementation
{
class sca_ac_domain_solver;
}
}


//// end implementation specific declarations //////////////


//begin LRM copy
namespace sca_core
{

class sca_module: public sc_core::sc_module
{
public:
	virtual const char* kind() const;

	virtual void set_timestep(const sca_core::sca_time&);
	virtual void set_timestep(double, sc_core::sc_time_unit);

	virtual void set_max_timestep(const sca_core::sca_time&);
	virtual void set_max_timestep(double, sc_core::sc_time_unit);

	virtual sca_core::sca_time get_timestep() const;
	virtual sca_core::sca_time get_max_timestep() const;

	void set_solver_parameter(const std::string& val,const std::string& par);
	std::string get_solver_parameter(const std::string& name) const;
	std::string get_solver_name() const;


	template<class T>
	void set_user_specific_solver()
	{
		if(user_solver_handler!=NULL) delete user_solver_handler;
		user_solver_handler=new sca_core::sca_implementation::sca_user_solver_handler<T>();
	}


protected:

	sca_module();

	virtual ~sca_module();


	//////////// begin implementation specific section ////////////////

	explicit sca_module(const sc_core::sc_module_name&);
	explicit sca_module(const char*);
	explicit sca_module(const std::string&);

	unsigned long register_port(sca_core::sca_implementation::sca_port_base* port);

private:

	friend class sca_core::sca_implementation::sca_port_base;
	friend class sca_core::sca_implementation::sca_solver_base;
	friend class sca_core::sca_implementation::sca_view_manager;
	friend class sca_core::sca_implementation::sca_object_manager;
	friend class sca_tdf::sca_implementation::sca_tdf_signal_impl_base;
	friend class sca_core::sca_prim_channel;
	friend class sca_tdf::sca_module;
	friend class sca_eln::sca_module;
	friend class sca_lsf::sca_module;
	friend class sca_core::sca_implementation::sca_conservative_module;
	friend class sca_core::sca_implementation::sca_conservative_view;
	friend class sca_tdf::sca_implementation::sca_tdf_view;
	friend class sca_eln::sca_implementation::sca_eln_view;
	friend class sca_lsf::sca_implementation::sca_lsf_view;
	friend class sca_tdf::sca_implementation::sca_trace_variable_base;
	friend class sca_eln::sca_node;
	friend class sca_lsf::sca_signal;
	friend class sca_ac_analysis::sca_implementation::sca_ac_domain_solver;

	typedef std::vector<sca_core::sca_implementation::sca_port_base*> sca_port_base_listT;
	typedef sca_port_base_listT::iterator sca_port_base_list_iteratorT;

	sca_core::sca_implementation::sca_solver_base* sync_domain;



	sca_core::sca_implementation::sca_user_solver_handler_base* user_solver_handler;


	//!!!! required due private sc_module portlist !!!!!!
	sca_port_base_listT port_list;

	const sca_port_base_listT& get_port_list() const;
	sca_port_base_listT& get_port_list();

	long get_view_id();

	void construct();


	/** Requests first activation time point
	 */
	void request_first_activation(const sca_core::sca_time& mtime);

	/** Requests first activation on event
	 */
	void request_first_activation(const sc_core::sc_event& ev);
	void request_first_activation(const sc_core::sc_event_or_list& evlist);
	void request_first_activation(const sc_core::sc_event_and_list& evlist);

public:

	/**
	 * gets unique id for the current computation cluster
	 * if the module is not (yet) associated to a cluster -1 is returned
	 */
	long get_cluster_id();

	/** Returns reference flag for allowing access to ports for processing phase
		 */
	bool* get_allow_processing_access_flag_ref();

	/** Returns flag for allowing access to ports for processing phase
		 */
	bool is_processing_executing() const;

	/** Returns flag for allowing access for intializing e.g. ports
		 */
	bool is_initialize_executing() const;

	/** Returns flag for allowing access for reintializing e.g. ports
		 */
	bool is_reinitialize_executing() const;

	/** Returns flag for allowing access for setting attributes
		 */
	bool is_set_attributes_executing() const;


	/** Returns flag for allowing access for setting attributes
		 */
	bool is_ac_processing_executing() const;


	/** Returns an interface to the synchronization cluster to which
	 * the module is assigned - a sca_module is assigned to exactly
	 * one cluster (others should make no sense ???)
	 */
	sca_core::sca_implementation::sca_solver_base* get_sync_domain() const;

	/** Returns call counter of the current sync domain*/
	sc_dt::int64 get_call_counter();


	/** returns calls per cluster execution of the current sync domain */
	long get_calls_per_period();

	sca_core::sca_time sca_get_time() const;

	/**
	 * Requests maximum absolute next time, returns number of calls
	 * before request can be accepted and in mtime the time which was
	 * accepted - if there are other requests the module is maybe
	 * called earlier
	 */
	long sca_next_max_time(sc_core::sc_time mtime);

	/** Requests next maximum time step, returns number of calls before
	 * the new time step can be accepted - if there are other requests, the
	 * resulting time step is maybe smaller, the time step will be hold until
	 * other requests occur
	 */
	long sca_next_max_time_step(sc_core::sc_time mtime);


	/** Registers an event which triggers next cluster activation and
	 * returns the earliest time when the event can be accepted -
	 * events before will be ignored
	 */
	sc_core::sc_time sca_synchronize_on_event(const sc_core::sc_event& ev);

	/**
	 * Synchronizes cluster activation with an event on a converter port
	 * the module is called latest the event occurs (may it is called
	 * earlier due cluster_period or next_start_time); the return time
	 * is the earliest time when the event can be accepted (due to
	 * multi-rate systems)
	 */
	sc_core::sc_time sca_synchronize_on_event(
			const sca_core::sca_implementation::sca_port_base& iport);

	//the view manger is allowed to access the assigned view

	sca_core::sca_implementation::sca_view_base* view_interface;
	long view_id;

	//elaborates the sca_module
	//permit overloading to add elaboration actions - overloaded method must call this base method
	virtual void elaborate();
	virtual void end_of_sca_elaboration();


	sca_core::sca_time timestep_set;
	bool               timestep_is_set;

	sca_core::sca_time timestep_max_set;
	bool               timestep_max_is_set;

	bool               timestep_elaborated;

	bool               attribute_changes_allowed_flag;

	bool               set_attributes_executes_flag;
	bool               initialize_executes_flag;
	bool               reinitialize_executes_flag;
	bool               change_attributes_executes_flag;
	bool               ac_processing_executes_flag;



	sca_core::sca_time first_activation_time;
	bool first_activation_time_requested;

	std::vector<const sc_core::sc_event*>  first_activation_events;
	std::vector<sc_core::sc_event_or_list> first_activation_event_or_lists;
	std::vector<sc_core::sc_event_and_list> first_activation_event_and_lists;
	bool first_activation_requested;

	/** Returns flag for allowing access for change  attributes
		 */
	bool is_change_attributes_executing() const;
	bool are_attribute_changes_enabled() const;

	//vectors for pending solver parameter
	std::vector<std::string> solver_parameter;
	std::vector<std::string> solver_parameter_values;


	/////////// end implementation specific section //////////////////////


};

//#define SCA_CTOR(name)       implementation-defined name( sc_core::sc_module_name )
#define SCA_CTOR(name)       name( sc_core::sc_module_name )

} // namespace sca_core


//end LRM copy

#endif /* SCA_MODULE_H_ */
