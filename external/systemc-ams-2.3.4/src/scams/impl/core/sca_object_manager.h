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

 sca_object_manager.h - description

 Original Author: Karsten Einwich Fraunhofer IIS/EAS Dresden

 Created on: 14.05.2009

 SVN Version       :  $Revision: 2320 $
 SVN last checkin  :  $Date: 2023-06-08 12:47:38 +0000 (Thu, 08 Jun 2023) $
 SVN checkin by    :  $Author: pehrlich $
 SVN Id            :  $Id: sca_object_manager.h 2320 2023-06-08 12:47:38Z pehrlich $

 *****************************************************************************/

/*****************************************************************************/

#ifndef SCA_OBJECT_MANAGER_H_
#define SCA_OBJECT_MANAGER_H_

//STL usage
#include <vector>
#include <map>
#include <string>

//forward class declarations

namespace sca_util
{
namespace sca_implementation
{
class sca_trace_file_base;
}
}


namespace sc_core
{
class sc_object_manager;
class sc_object;
}

namespace sca_core
{

//forward class declarations
class sca_module;
class sca_interface;
class sca_prim_channel;


namespace sca_implementation
{

//forward class declarations
class sca_simcontext;
class sca_view_manager;
class sca_solver_manager;
class sca_synchronization_layer;

////////////////////////////////////////////////////////////////////////

typedef std::vector<sca_module*> sca_module_listT;
typedef sca_module_listT::iterator sca_module_list_iteratorT;

typedef std::vector<sca_interface*> sca_interface_listT;
typedef sca_interface_listT::iterator sca_interface_list_iteratorT;

typedef std::vector<sca_prim_channel*> sca_channel_listT;
typedef sca_channel_listT::iterator sca_channel_list_iteratorT;

typedef std::vector<std::pair<std::string,std::string> > solver_parameterT;
typedef std::map<std::string, solver_parameterT> solver_default_parameter_listT;

////////////////////////////////////////////////////////////////////////

/** SystemC-AMS global object manager

 */

class sca_object_manager
{
	//no one else than sca_simcontext can construct and
	//destruct the objectmanager
	friend class sca_simcontext;

	//constructor
	sca_object_manager();

public:

	//destructor
	~sca_object_manager();

	sc_core::sc_object* get_current_context();

	void set_default_solver_parameter(
			std::string solver,
			std::string par,
			std::string value
			);

	std::string get_default_solver_parameter(
			const std::string& solver_name,
			const std::string& parameter_name) const;

	std::vector<std::string> get_solver_parameter_names(
			const std::string& solver_name) const;

	std::vector<std::string> get_solver_names() const;

	void insert_module(sca_module* module);
	bool remove_module(sca_module_list_iteratorT instance_number);

	//full path (hierarchical) name
	sca_module_list_iteratorT sca_find_module(const char* name);
	bool remove_module(const char* name);
	bool remove_module(sca_module* module);

	void insert_channel(sca_prim_channel* channel);
	bool remove_channel(sca_channel_list_iteratorT channel_number);

	void insert_interface(sca_interface* interf);
	bool remove_interface(sca_interface_list_iteratorT interface_number);

	//called during end_of_simulation kernel callback
	void finish_simulation();

	sca_module_list_iteratorT get_module_list();
	inline sca_module_list_iteratorT get_module_list_end()
	{
		return sca_module_list.end();
	}

	sca_interface_list_iteratorT get_interface_list();
	inline sca_interface_list_iteratorT get_interface_list_end()
	{
		return sca_interface_list.end();
	}

	sca_channel_list_iteratorT get_channel_list();
	inline sca_channel_list_iteratorT get_channel_list_end()
	{
		return sca_channel_list.end();
	}

	/** Provides access to the synchronization layer. */
	sca_synchronization_layer* get_synchronization_if()
	{
		return synchronization;
	}

	/**
	 Initializes all SystemC-AMS objects - will be called once by the
	 global function @ref void sca_systemc_ams_init() .
	 */
	void systemc_ams_init();

	bool initialized()
	{
		return init_done;
	}

	bool is_object_deleted()
	{
		return object_deleted;
	}

private:

	//reference to main object manager
	sc_core::sc_object_manager* sc_kernel_object_manager;

	sca_module_listT sca_module_list;
	sca_interface_listT sca_interface_list;
	sca_channel_listT sca_channel_list;

	solver_default_parameter_listT solver_default_parameter;

	std::vector<sca_util::sca_implementation::sca_trace_file_base*> trace_list;

	//one view and solver manager
	sca_view_manager* views;
	sca_solver_manager* solvers;
	sca_synchronization_layer* synchronization;

	bool init_done;

	bool object_deleted;

};

} // namespace sca_implementation
} // namespace sca_core

#endif /* SCA_OBJECT_MANAGER_H_ */
