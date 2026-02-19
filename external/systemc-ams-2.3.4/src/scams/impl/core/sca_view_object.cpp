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

  sca_view_object.cpp - description

  Original Author: Karsten Einwich Fraunhofer IIS/EAS Dresden

  Created on: 25.08.2009

   SVN Version       :  $Revision: 1920 $
   SVN last checkin  :  $Date: 2016-02-25 12:43:37 +0000 (Thu, 25 Feb 2016) $
   SVN checkin by    :  $Author: karsten $
   SVN Id            :  $Id: sca_view_object.cpp 1920 2016-02-25 12:43:37Z karsten $

 *****************************************************************************/

/*****************************************************************************/


#include "scams/impl/core/sca_view_object.h"
#include "scams/impl/core/sca_view_base.h"


namespace sca_core
{
namespace sca_implementation
{

////////////////////////////////////////////////////////////

sca_view_object::sca_view_object(sca_core::sca_implementation::sca_view_base* interf)
{
	view_interface = interf;
	view_name      = interf->get_name();
	view_id=0;
}

////////////////////////////////////////////////////////////

sca_view_object::~sca_view_object()
{
}

////////////////////////////////////////////////////////////

void sca_view_object::push_back_module(sca_core::sca_module* module)
{
	module_list.push_back(module);
}

////////////////////////////////////////////////////////////

void sca_view_object::setup_equations()
{
  //call interface method of the view
  view_interface->setup_equations();
}

////////////////////////////////////////////////////////////

void sca_view_object::push_back_solver(sca_core::sca_implementation::sca_solver_base* solver)
{
  solver_list.push_back(solver);
}

////////////////////////////////////////////////////////////

}
}
