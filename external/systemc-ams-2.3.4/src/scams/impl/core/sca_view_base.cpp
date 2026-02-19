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

  sca_view_base.cpp - description

  Original Author: Karsten Einwich Fraunhofer IIS/EAS Dresden

  Created on: 25.08.2009

   SVN Version       :  $Revision: 1265 $
   SVN last checkin  :  $Date: 2011-11-19 20:43:31 +0000 (Sat, 19 Nov 2011) $
   SVN checkin by    :  $Author: karsten $
   SVN Id            :  $Id: sca_view_base.cpp 1265 2011-11-19 20:43:31Z karsten $

 *****************************************************************************/

/*****************************************************************************/

#include <systemc>
#include "scams/impl/core/sca_simcontext.h"
#include "scams/impl/core/sca_view_base.h"
#include "scams/impl/core/sca_view_object.h"

#include <typeinfo>

namespace sca_core
{

namespace sca_implementation
{

sca_view_base::sca_view_base()
{
	name = "no_view_name_set";

	view_datas = NULL;
	solvers    = NULL;
}

///////////////////////////////////////////////////////////

sca_view_base::~sca_view_base()
{
}

///////////////////////////////////////////////////////////

bool sca_view_base::is_equal(sca_view_base* view_interface)
{
	if(view_interface==NULL) return false;

	//assume that the equal views have the same type
	if(typeid(*view_interface) == typeid(*this) ) return true;
	else                                          return false;
}

///////////////////////////////////////////////////////////

sca_view_object* sca_view_base::create_view()
{
	solvers = sca_get_curr_simcontext()->get_sca_solver_manager();

	view_datas = new sca_view_object(this);
 	return   view_datas;
}


}
}
