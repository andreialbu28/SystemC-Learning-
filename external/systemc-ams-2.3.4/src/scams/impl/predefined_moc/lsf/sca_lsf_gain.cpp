/*****************************************************************************

    Copyright 2010
    Fraunhofer-Gesellschaft zur Foerderung der angewandten Forschung e.V.

    Copyright 2015-2016
    COSEDA Technologies GmbH

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

  sca_lsf_gain.cpp - description

  Original Author: Karsten Einwich Fraunhofer IIS Dresden / COSEDA Technologies

  Created on: 05.01.2010

   SVN Version       :  $Revision: 1950 $
   SVN last checkin  :  $Date: 2016-03-21 12:45:39 +0000 (Mon, 21 Mar 2016) $
   SVN checkin by    :  $Author: karsten $
   SVN Id            :  $Id: sca_lsf_gain.cpp 1950 2016-03-21 12:45:39Z karsten $

 *****************************************************************************/

/*****************************************************************************/


#include "systemc-ams"
#include "scams/predefined_moc/lsf/sca_lsf_gain.h"
#include "scams/impl/core/sca_solver_base.h"
#include "scams/impl/util/data_types/sca_method_object.h"
#include "scams/impl/util/data_types/sca_method_list.h"


namespace sca_lsf
{
sca_gain::sca_gain(
		sc_core::sc_module_name, double k_,
		const sca_pwl_pair_vector& pwl_k_) :
	x("x"), y("y"), k("k", k_), pwl_k("pwl_k",pwl_k_)
{
	pwl_vector_size=0;
}

const char* sca_gain::kind() const
{
	return "sca_lsf::sca_gain";
}

void sca_gain::matrix_stamps()
{
	const sca_pwl_pair_vector& pwl_vector(pwl_k.get());
	pwl_vector_size = pwl_vector.length();

	if (pwl_vector_size < 2) //no pwl description
	{
		B(y,y) = -1.0;
		B(y,x) =  k;
	}
	else
	{
		B(y,y) = -1.0;
	    B(y,x) =  k;

		add_pwl_stamp_to_B(y,x,pwl_vector);
	}
}


} //namespace sca_eln

