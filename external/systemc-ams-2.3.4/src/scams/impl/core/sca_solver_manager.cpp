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

 sca_solver_manager.cpp - description

 Original Author: Karsten Einwich Fraunhofer IIS/EAS Dresden

 Created on: 14.05.2009

 SVN Version       :  $Revision: 1265 $
 SVN last checkin  :  $Date: 2011-11-19 20:43:31 +0000 (Sat, 19 Nov 2011) $
 SVN checkin by    :  $Author: karsten $
 SVN Id            :  $Id: sca_solver_manager.cpp 1265 2011-11-19 20:43:31Z karsten $

 *****************************************************************************/

/*****************************************************************************/

#include "sca_solver_manager.h"

#include "scams/impl/core/sca_solver_base.h"

namespace sca_core
{

namespace sca_implementation
{

class sca_solver_base;

/////////////////////////////////////////////////

sca_solver_manager::sca_solver_manager()
{
}

/////////////////////////////////////////////////

sca_solver_manager::~sca_solver_manager()
{
	for (sca_solver_listItT it = solver_list.begin(); it != solver_list.end(); ++it)
	{
		delete *it;
	}
}

/////////////////////////////////////////////////

void sca_solver_manager::push_back_solver(sca_solver_base* solver)
{
	solver_list.push_back(solver);
}

sca_solver_manager::sca_solver_listT& sca_solver_manager::get_solvers()
{
	return solver_list;
}


} //namespace sca_implementation
}// namespace sca_core

/////////////////////////////////////////////////
