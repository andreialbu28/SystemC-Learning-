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

 sca_solver_manager.h - description

 Original Author: Karsten Einwich Fraunhofer IIS/EAS Dresden

 Created on: 14.05.2009

 SVN Version       :  $Revision: 1265 $
 SVN last checkin  :  $Date: 2011-11-19 20:43:31 +0000 (Sat, 19 Nov 2011) $
 SVN checkin by    :  $Author: karsten $
 SVN Id            :  $Id: sca_solver_manager.h 1265 2011-11-19 20:43:31Z karsten $

 *****************************************************************************/

/*****************************************************************************/

#ifndef SCA_SOLVER_MANAGER_H_
#define SCA_SOLVER_MANAGER_H_

#include <vector>

namespace sca_core
{

namespace sca_implementation
{

class sca_object_manager;
class sca_solver_base;

/**
 Class for managing solver instances.
 */
class sca_solver_manager
{

	typedef std::vector<sca_solver_base*> sca_solver_listT;
	typedef std::vector<sca_solver_base*>::iterator sca_solver_listItT;

public:

	sca_solver_manager();
	~sca_solver_manager();

	void push_back_solver(sca_solver_base* solver);

	sca_solver_listT& get_solvers();

private:

	//the object manager is allowed to access
	friend class sca_object_manager;

	sca_solver_listT solver_list;

};

} // namespace sca_implementation
} // namespace sca_core

#endif /* SCA_SOLVER_MANAGER_H_ */
