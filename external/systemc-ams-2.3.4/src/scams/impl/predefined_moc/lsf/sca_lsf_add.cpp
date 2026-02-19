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

  sca_lsf_add.cpp - description

  Original Author: Karsten Einwich Fraunhofer IIS/EAS Dresden

  Created on: 05.01.2010

   SVN Version       :  $Revision: 1265 $
   SVN last checkin  :  $Date: 2011-11-19 20:43:31 +0000 (Sat, 19 Nov 2011) $
   SVN checkin by    :  $Author: karsten $
   SVN Id            :  $Id: sca_lsf_add.cpp 1265 2011-11-19 20:43:31Z karsten $

 *****************************************************************************/

/*****************************************************************************/


#include "systemc-ams"
#include "scams/predefined_moc/lsf/sca_lsf_add.h"
#include "scams/impl/core/sca_solver_base.h"

namespace sca_lsf
{
sca_add::sca_add(sc_core::sc_module_name, double k1_, double k2_ ):
	x1("x1"), x2("x2"), y("y"), k1("k1", k1_), k2("k2", k2_)
{
}

const char* sca_add::kind() const
{
	return "sca_lsf::sca_add";
}

void sca_add::matrix_stamps()
{
    B(y,y)  = -1.0;
    B(y,x1) = k1;
    B(y,x2) = k2;
}


} //namespace sca_lsf

