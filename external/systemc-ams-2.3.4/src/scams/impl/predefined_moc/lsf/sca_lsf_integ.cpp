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

  sca_lsf_integ.cpp - description

  Original Author: Karsten Einwich Fraunhofer IIS/EAS Dresden

  Created on: 05.01.2010

   SVN Version       :  $Revision: 1376 $
   SVN last checkin  :  $Date: 2012-08-09 08:28:06 +0000 (Thu, 09 Aug 2012) $
   SVN checkin by    :  $Author: reuther $
   SVN Id            :  $Id: sca_lsf_integ.cpp 1376 2012-08-09 08:28:06Z reuther $

 *****************************************************************************/

/*****************************************************************************/


#include "systemc-ams"
#include "scams/predefined_moc/lsf/sca_lsf_integ.h"
#include "scams/impl/util/data_types/sca_method_object.h"
#include "scams/impl/util/data_types/sca_method_list.h"
#include "scams/impl/core/sca_solver_base.h"

namespace sca_lsf
{
sca_integ::sca_integ(sc_core::sc_module_name, double k_, double y0_) :
	x("x"), y("y"), k("k", k_), y0("y0", y0_)
{
	first_call=true;
}

const char* sca_integ::kind() const
{
	return "sca_lsf::sca_integ";
}

void sca_integ::y0_init()
{
	B(y,y) =  0.0;
	A(y,y) = -1.0;
	B(y,x) =  k;
	q(y).set_value(0.0);
	request_reinit_and_decomposition_A();
	first_call=false;

	remove_method(POST_SOLVE, SCA_VMPTR(sca_integ::y0_init));
}

void sca_integ::matrix_stamps()
{
	if(first_call && !sca_ac_analysis::sca_ac_is_running())
	{
		B(y,y) = -1.0;
		q(y).set_value(y0);
		add_method(POST_SOLVE, SCA_VMPTR(sca_integ::y0_init));
	}
	else
	{
		A(y,y) = -1.0;
		B(y,x) =  k;
	}
}


} //namespace sca_eln

