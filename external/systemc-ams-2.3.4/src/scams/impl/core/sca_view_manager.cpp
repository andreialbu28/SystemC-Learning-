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

  sca_view_manager.cpp - description

  Original Author: Karsten Einwich Fraunhofer IIS/EAS Dresden

  Created on: 14.05.2009

   SVN Version       :  $Revision: 1523 $
   SVN last checkin  :  $Date: 2013-02-17 20:36:57 +0000 (Sun, 17 Feb 2013) $
   SVN checkin by    :  $Author: karsten $
   SVN Id            :  $Id: sca_view_manager.cpp 1523 2013-02-17 20:36:57Z karsten $

 *****************************************************************************/

/*****************************************************************************/

#include <systemc-ams>
#include "scams/impl/core/sca_view_manager.h"
#include "scams/core/sca_module.h"

#include "scams/impl/core/sca_view_object.h"
#include "scams/impl/core/sca_view_base.h"

namespace sca_core
{

namespace sca_implementation
{

class sca_view_base;
class sca_view_object;

sca_view_manager::sca_view_manager()
{
}


///////////////////////////////////////////////////

sca_view_manager::~sca_view_manager()
{
	for(sca_view_listItT it=view_list.begin(); it!=view_list.end(); ++it)
	{
		delete *it;
	}
}


////////////////////////////////////////////////////


void sca_view_manager::push_back_module(sca_core::sca_module* module)
{
	sca_view_base* mview_interface=module->view_interface;

	bool view_exist=false;
	for(sca_view_listItT it=view_list.begin(); it!=view_list.end(); ++it)
	{
		if( mview_interface->is_equal((*it)->view_interface) )
		{
			(*it)->push_back_module(module);
			module->view_id = (*it)->get_view_id();
			view_exist=true;
			break;
		}
	}

	//add new view to list
	if(!view_exist)
	{
		sca_view_object* new_view= mview_interface->create_view();
		new_view->push_back_module(module);
		new_view->set_view_id((unsigned long)(view_list.size()));
		module->view_id = (long)(view_list.size());
		view_list.push_back(new_view);
	}

}

} // namespace sca_implementation

} // namespace sca_core

////////////////////////////////////////////////////
