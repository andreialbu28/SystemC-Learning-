/*****************************************************************************

    Copyright 2010
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

  sca_port_base.h - description

  Original Author: Karsten Einwich Fraunhofer IIS/EAS Dresden

  Created on: 15.05.2009

   SVN Version       :  $Revision: 2320 $
   SVN last checkin  :  $Date: 2023-06-08 12:47:38 +0000 (Thu, 08 Jun 2023) $
   SVN checkin by    :  $Author: pehrlich $
   SVN Id            :  $Id: sca_port_base.h 2320 2023-06-08 12:47:38Z pehrlich $

 *****************************************************************************/

/*****************************************************************************/


#ifndef SCA_PORT_BASE_H_
#define SCA_PORT_BASE_H_



namespace sca_core
{

class sca_interface;

namespace sca_implementation
{

class sca_sync_value_handle_base;
class sca_solver_base;

/**
	Abstract base class for sca_ports. This class is required
	to handle ports type and interface independent.
*/
class sca_port_base : public sca_util::sca_traceable_object
{

public:

  /**
  	Returns the sca_module which contains this port (the port is
  	a member of that module).
  */
  sca_module* get_parent_module() const;

  /** for decluster ports we allow to change the associated solver
  */
  sca_solver_base* get_sync_domain();

  void set_sync_domain(sca_solver_base*);

  //only if accessed via abstract base class
  //otherwise a port is derived from sc_object
  const char* sca_name() const;
  const char* sca_basename() const;

  //constructors
  sca_port_base();
  sca_port_base(const char* nm);

  /**
   * adds a trace to the cluster in which the port is instantiated
   * so the specified trace will be executed with the cluster data
   */
  bool add_cluster_trace(sca_util::sca_implementation::sca_trace_object_data& tr_obj);

  /**
  	Returns a pointer to the first bound interface -
  	must be always a sca_interface - if a sc_interface is
  	bound, a pointer to the in-port sca_channel will be
  	returned.
  */
  virtual const sca_core::sca_interface* sca_get_interface() const {return m_sca_if;}
  virtual       sca_core::sca_interface* sca_get_interface()       {return m_sca_if;}
  virtual       sc_core::sc_interface*  sc_get_interface()       = 0;
  virtual const sc_core::sc_interface*  sc_get_interface() const = 0;

  //destructor
  virtual ~sca_port_base();

  /**
  	The enumaration defines the possible port categories.
  	These categories are used for analyzing the communication
  	graphs.
  */
  enum port_typeT
  {
	SCA_NOT_DEFINED_PORT = 0,

	SCA_IN_PORT = 1,
	SCA_OUT_PORT = 2,
	SCA_INOUT_PORT = 3,
	SCA_CONSERVATIVE_PORT = 4,

	SCA_SC_IN_PORT = 9,
	SCA_SC_OUT_PORT = 10,
	SCA_SC_INOUT_PORT = 11,

	SCA_DECL_OUT_PORT = 12
  };

  /** This method returns the port's assigned type. */
  port_typeT  get_port_type() const { return  sca_port_type; }

  /** This method tries to determine the bound sca_interfaces.
      If a sc_interface is bound then a sca - synchronization - channel
      will be created (using the virtual method
      @ref #create_synchronization_channel() ). Therefore, that
      creation method has to be overloaded by a sca_port specialization.
      Else an exception will occur.
  */
  void elaborate_port();

  /** This virtual method will be called after the port has elaborated
      and thus all interfaces/channel are bound. Thus the bound
      interfaces can be accessed inside this method (e.g. to propagate
      port-attributes to the channel).
  */
  virtual void end_of_port_elaboration();

  /** Returns port-number within the parent module. */
  unsigned long get_port_number() const { return port_number; }

  void set_if_id(unsigned long id) { port_if_id = id;   }
  unsigned long get_if_id() const  { return port_if_id; }

  //optional callback - used for output calculation of declustering ports
   virtual void port_processing_method();

   bool elaboration_finished;

   virtual void validate_port_attributes() { return;}


	/** adds callback for introspection */
	virtual bool register_trace_callback(sca_trace_callback,void*) override;
	virtual bool register_trace_callback(callback_functor_base&) override;
	virtual bool remove_trace_callback(callback_functor_base&) override;

	/** returns current value */
	virtual const std::string& get_trace_value() const override;

	virtual bool force_value(const std::string&) override;

	virtual void release_value() override;

	  /**
	   * for converter ports, is executed by the synchronization layer
	   * at each SystemC time to which a value has to be read from the
	   * SystemC domain
	   * For converter ports, this method will be implemented by the port,
	   * otherwise the method is not used
	   */
	  virtual void read_from_sc();

	  /**
	   * for converter ports, is executed by the synchronization layer
	   * at each SystemC time to which a value has to be written to the
	   * SystemC domain
	   * For converter ports, this method will be implemented by the port,
	   * otherwise the method is not used
	   */
	  virtual void write_to_sc();


	  /**
	   * used for converter ports for dtdf to initialize the buffer
	   * is called after timesteps are propagated however before
	   * scheduling (delay values can be adopted)
	   */
	  virtual void initialize_port();


	  /**
	   * used for converter ports for dtdf to check whether the timing has
	   * been changed
	   * is called after timesteps are re-propagated however before
	   * rescheduling (delay values can be adopted)
	   * returns true if re-scheduling is required
	   */
	  virtual bool reinitialize_port_timing();

	  /**
	   * used for converter ports for dtdf to reinitialize the buffer
	   * is called after timesteps are re-propagated and after
	   * rescheduling
	   */
	  virtual void reinitialize_port();

	  //port processing method for decluster ports
	  virtual void port_processing(){}


protected:

  void construct_parent(sc_core::sc_object* pobj);

  port_typeT sca_port_type;

  unsigned long port_number;

  // identification of the port to the bound interface
  unsigned long port_if_id;

  //returns time of parent module
  sc_core::sc_time get_parent_module_time() const;


  //method for converter ports -> registers event on which a value from a
  //sc-signal has to be read to the synchronization layer
  void register_sc_value_trace(const ::sc_core::sc_event& ev,
  			sca_core::sca_implementation::sca_sync_value_handle_base& handle);


  //method for converter ports -> registers handle for writing values to a
   //sc-signal
   void register_sca_schedule(::sc_core::sc_time,
   			sca_core::sca_implementation::sca_sync_value_handle_base& handle);

  //reads sc value from handle at given time
  void get_sc_value_on_time(::sc_core::sc_time time,
		                    sca_sync_value_handle_base& handle);

  //writes sc value from handle at a given time
  void write_sc_value_on_time( sc_core::sc_time current_time,
		                       sc_core::sc_time next_time,
		                      sca_sync_value_handle_base& handle);


  sca_module* parent_module;
  sca_solver_base* sync_domain;

  //bound interfaces
  ::sc_core::sc_interface  *m_if;
  //provided interfaces
  sca_interface *m_sca_if;

  bool port_elaborated;

  //implementation of sca_traceable_object interface
 virtual bool trace_init(sca_util::sca_implementation::sca_trace_object_data& data) override;
 virtual void trace(long id,sca_util::sca_implementation::sca_trace_file_base& tf) override;
 virtual void trace_interactive() override;

 virtual void set_type_info(sca_util::sca_implementation::sca_trace_object_data& data);

};


} // namespace sca_implementation
} // namespace sca_core


#endif /* SCA_PORT_BASE_H_ */
