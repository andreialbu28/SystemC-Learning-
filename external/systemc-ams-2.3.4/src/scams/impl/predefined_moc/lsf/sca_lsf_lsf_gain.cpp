/*****************************************************************************

    Copyright 2010
    Fraunhofer-Gesellschaft zur Foerderung der angewandten Forschung e.V.

    Copyright 2015-2020   COSEDA Technologies GmbH


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

  sca_lsf_lsf_gain.cpp - description

  Original Author: Karsten Einwich COSEDA Technologies GmbH

  Created on: 28.01.2020

   SVN Version       :  $Revision: 1681 $
   SVN last checkin  :  $Date: 2014-04-02 03:11:21 -0700 (Wed, 02 Apr 2014) $
   SVN checkin by    :  $Author: karsten $
   SVN Id            :  $Id: sca_lsf_de_gain.cpp 1681 2014-04-02 10:11:21Z karsten $

 *****************************************************************************/

/*****************************************************************************/


#include "systemc-ams"
#include "scams/predefined_moc/lsf/sca_lsf_lsf_gain.h"
#include "scams/impl/util/data_types/sca_method_object.h"
#include "scams/impl/util/data_types/sca_method_list.h"
#include "scams/impl/core/sca_solver_base.h"
#include <limits>

namespace sca_lsf
{

sca_lsf_gain::sca_lsf_gain(sc_core::sc_module_name, const sca_pwl_pair_vector& pwc_gain_) :
	inp("inp"), x("x"), y("y"), pwc_gain("pwc_gain",pwc_gain_)
{

}

const char* sca_lsf_gain::kind() const
{
	return "sca_lsf::sca_lsf_gain";
}

///////////////////////////////////////////////////////




void sca_lsf_gain::matrix_stamps()
{
    B(y,y)  =  -1.0;

	B(y,x) = 1.0;
    add_pwl_b_stamp_to_B(y,x,inp,pwc_gain.get());
}

} //namespace sca_lsf


