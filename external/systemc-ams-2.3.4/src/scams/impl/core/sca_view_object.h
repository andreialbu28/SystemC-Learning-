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

 sca_view_object.h - description

 Original Author: Karsten Einwich Fraunhofer IIS/EAS Dresden

 Created on: 14.05.2009

 SVN Version       :  $Revision: 1920 $
 SVN last checkin  :  $Date: 2016-02-25 12:43:37 +0000 (Thu, 25 Feb 2016) $
 SVN checkin by    :  $Author: karsten $
 SVN Id            :  $Id: sca_view_object.h 1920 2016-02-25 12:43:37Z karsten $

 *****************************************************************************/

/*****************************************************************************/

#ifndef SCA_VIEW_OBJECT_H_
#define SCA_VIEW_OBJECT_H_


#include <vector>
#include <string>

///////////////////////////////////////////////////////


namespace sca_core
{

class sca_module;

namespace sca_implementation
{

class sca_view_manager;
class sca_solver_base;
class sca_view_base;

//////////////////////////////////////////////////////

/**
 Class for View instance.
 */
class sca_view_object
{

public:

	typedef std::vector<sca_core::sca_module*> sca_module_listT;
	typedef std::vector<sca_core::sca_module*>::iterator
			sca_module_list_iteratorT;

	typedef std::vector<sca_solver_base*> sca_solver_listT;
	typedef std::vector<sca_solver_base*>::iterator sca_solver_list_iteratorT;

public:
	sca_view_object(sca_view_base* interf);
	~sca_view_object();

	/** Assigns a module to this view. */
	void push_back_module(sca_module* module);

	/**
	 Stores references to a solver instance which is
	 created/used by this view.
	 */
	void push_back_solver(sca_solver_base* solver);

	void setup_equations();

	sca_module_listT& get_module_list()
	{
		return module_list;
	}

	/**
	 Returns the id of the current view
	 */
	long get_view_id()
	{
		return view_id;
	}

	void set_view_id(unsigned long id)
	{
		view_id = id;
	}

private:

	sca_module_listT module_list;
	sca_solver_listT solver_list;

	friend class sca_view_manager;
	sca_view_base* view_interface;

	std::string view_name;

	long view_id;

	//disabled
	sca_view_object()
	{
		view_interface=NULL;
		view_id=0;
	}

};

} //namespace sca_implementation
} //namespace sca_core

#endif /* SCA_VIEW_OBJECT_H_ */
