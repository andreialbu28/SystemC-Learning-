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

  sca_view_manager.h - description

  Original Author: Karsten Einwich Fraunhofer IIS/EAS Dresden

  Created on: 14.05.2009

   SVN Version       :  $Revision: 1265 $
   SVN last checkin  :  $Date: 2011-11-19 20:43:31 +0000 (Sat, 19 Nov 2011) $
   SVN checkin by    :  $Author: karsten $
   SVN Id            :  $Id: sca_view_manager.h 1265 2011-11-19 20:43:31Z karsten $

 *****************************************************************************/

/*****************************************************************************/


#ifndef SCA_VIEW_MANAGER_H_
#define SCA_VIEW_MANAGER_H_

#include<vector>

namespace sca_core
{

class sca_module;


namespace sca_implementation
{

class sca_view_object;
class sca_object_manager;

////////////////////////////////////////////////////

/**
	Class for manging view instances
*/
class sca_view_manager
{

	typedef std::vector<sca_view_object*> sca_view_listT;
	typedef std::vector<sca_view_object*>::iterator sca_view_listItT;


public:

	sca_view_manager();
	~sca_view_manager();

	void push_back_module(sca_core::sca_module*);

private:

	//the object-manger is allowed to access
	friend class sca_object_manager;

	sca_view_listT  view_list;

};

} //namespace sca_implementation

} //namespace sca_core

#endif /* SCA_VIEW_MANAGER_H_ */
