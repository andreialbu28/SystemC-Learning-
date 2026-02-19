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

 sca_prim_channel.h - base class for all SystemC AMS channels

 Original Author: Karsten Einwich Fraunhofer IIS/EAS Dresden / COSEDA Technologies

 Created on: 03.03.2009

 SVN Version       :  $Revision: 2320 $
 SVN last checkin  :  $Date: 2023-06-08 12:47:38 +0000 (Thu, 08 Jun 2023) $ (UTC)
 SVN checkin by    :  $Author: pehrlich $
 SVN Id            :  $Id: sca_prim_channel.h 2320 2023-06-08 12:47:38Z pehrlich $

 *****************************************************************************/

/* LRM clause 3.2.3
 The class sca_core::sca_prim_channel shall be used as base
 class to derive primitive channels for the predefined models of computation.
 */

/*****************************************************************************/

#ifndef SCA_PRIM_CHANNEL_H_
#define SCA_PRIM_CHANNEL_H_


/////////// begin implementation specific declarations ////////////////
namespace sca_core
{
class sca_module;

namespace sca_implementation
{
class sca_port_base;
class sca_synchronization_obj_if;
struct sca_synchronization_port_data;
class sca_solver_base;
class sca_synchronization_alg;
}
}

namespace sca_util
{
namespace sca_implementation
{
class sca_trace_object_data;
class sca_trace_buffer;
}
}

namespace sca_ac_analysis
{
namespace sca_implementation
{
class sca_ac_domain_db;
}
}

/////////// end implementation specific declarations ////////////////


namespace sca_core
{

//begin LRM copy

//  class sca_prim_channel : public implementation-derived-from sc_core::sc_object,
//                           protected sca_util::sca_traceable_object


class sca_prim_channel: public sc_core::sc_object,
		                public sca_util::sca_traceable_object
{
public:
	virtual const char* kind() const;

protected:
	sca_prim_channel();
	explicit sca_prim_channel(const char*);

	virtual ~sca_prim_channel();

private:
	// Disabled
	sca_prim_channel(const sca_core::sca_prim_channel&);
	sca_core::sca_prim_channel& operator=(const sca_core::sca_prim_channel&);

	////// begin implementation specific section /////////
protected:  //must be protected -> otherwise templates can't access

	friend class sca_core::sca_implementation::sca_object_manager;
    friend class sca_core::sca_implementation::sca_port_base;
    friend class sca_core::sca_implementation::sca_solver_base;
    friend class sca_core::sca_implementation::sca_synchronization_alg;
    friend class sca_ac_analysis::sca_implementation::sca_ac_domain_db;
    friend class sca_lsf::sca_implementation::sca_lsf_view;

	typedef std::vector<sca_core::sca_implementation::sca_port_base*> sca_port_base_listT;
	typedef sca_port_base_listT::iterator sca_port_base_list_iteratorT;

	typedef std::vector<sca_core::sca_module*> sca_module_listT;
	typedef sca_module_listT::iterator sca_module_list_iteratorT;

	typedef std::vector<sca_core::sca_implementation::sca_synchronization_obj_if*> sca_solver_listT;
	typedef std::vector<sca_core::sca_implementation::sca_synchronization_port_data*> sca_sync_port_listT;

	/** Method is called immedately before
	 simulation start -> it can be used to
	 setup the channel datastructures using
	 the results of layer initialization -
	 does nothing at default
	 */
	virtual void end_of_elaboration();

	virtual bool trace_init(sca_util::sca_implementation::sca_trace_object_data& data);
	virtual void trace(long id, sca_util::sca_implementation::sca_trace_file_base& tf);

	/** will be called after rescheduling to enable TDF buffer resize */
	virtual void resize_buffer();

	 //sc trace of sc_core::sc_object to prevent clang warning due overloaded
	 //sca_traceable_object function
	 void trace( sc_core::sc_trace_file* tf ) const;

	//some usefull methods

	//returns connected modules -> every module is inserted once
	//independently whether it is connected more than once
	unsigned long get_number_of_connected_modules() const;
	sca_module_listT& get_connected_module_list();
	const sca_module_listT& get_connected_module_list() const;

	//returns connected ports
	unsigned long get_number_of_connected_ports() const;
	sca_core::sca_implementation::sca_port_base** get_connected_port_list() const;

	void register_port(sca_core::sca_implementation::sca_port_base* port);

	void register_sc_interface(sc_core::sc_interface* channel);

	/**
	 Registers a connected solver. This method will be used if the
	 channel represents a connection among solvers.
	 */
	void register_connected_solver(sca_core::sca_implementation::sca_synchronization_port_data* solver_id);

	/**
	 This method returns a list of solvers connected to the channel, if the
	 channel represents a connection among solvers. If con_from_solver is
	 false the returned list consists of solvers, which have the channel as
	 inport, otherwise as outport.
	 */
	sca_solver_listT& get_connected_solvers(bool con_from_solver);

	/**
	 This method returns a list of synchronization ports connected to the channel, if the
	 channel represents a connection among solvers. If con_from_solver is
	 false the returned list consists of inports otherwise as outport.
	 */
	sca_sync_port_listT& get_connected_sync_ports(bool con_from_solver);

	long get_samples_per_period() const
	{
		return max_samples;
	}

	/** Returns the synchronization interface of the cluster to which the
	 * channel assigned - a channel can be assigned to one if only due if the
	 * channel connects modules the modules will be assigned to the same cluster
	 * and thus they will have the same synchronization interface
	 */
	sca_core::sca_implementation::sca_synchronization_obj_if* get_synchronization_if() const;

	// debug functionality
	void print_connected_module_names(std::ostream& ostr);

	sca_port_base_listT port_list;
	sca_module_listT module_list;

	sc_core::sc_interface* systemc_channel;

	sca_solver_listT from_solver;
	sca_sync_port_listT from_solver_sync_port;

	sca_solver_listT to_solver;
	sca_sync_port_listT to_solver_sync_port;

	// members for synchronization layer
	long max_samples;
	long multiple;
	bool remainder_flag;
	long max_buffer_usage;

	long cluster_id;

	void construct();
	void reset_for_reschedule();

	////// end implementation specific section /////////

};

} // namespace sca_core

//end LRM copy

#endif /* SCA_PRIM_CHANNEL_H_ */
