/*****************************************************************************

    Copyright 2010-2014
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

  sca_lsf_lsf_mux.cpp - description

  Original Author: Karsten Einwich COSEDA Technologies GmbH

  Created on: 28.01.2020

   SVN Version       :  $Revision: 1681 $
   SVN last checkin  :  $Date: 2014-04-02 03:11:21 -0700 (Wed, 02 Apr 2014) $
   SVN checkin by    :  $Author: karsten $
   SVN Id            :  $Id: sca_lsf_de_mux.cpp 1681 2014-04-02 10:11:21Z karsten $

 *****************************************************************************/

/*****************************************************************************/


#include "systemc-ams"
#include "scams/predefined_moc/lsf/sca_lsf_lsf_mux.h"
#include "scams/impl/util/data_types/sca_method_object.h"
#include "scams/impl/util/data_types/sca_method_list.h"
#include "scams/impl/core/sca_solver_base.h"


namespace sca_lsf
{

sca_lsf_mux::sca_lsf_mux(sc_core::sc_module_name, double threshold_) :
	x1("x1"), x2("x2"), y("y"), ctrl("ctrl"),threshold("threshold",threshold_)
{
}

const char* sca_lsf_mux::kind() const
{
	return "sca_lsf::sca_lsf_mux";
}


void sca_lsf_mux::matrix_stamps()
{
    B(y,y)  =  -1.0;

	B(y,x1) = 1.0;
	B(y,x2) = 0.0;


	add_pwl_b_stamp_to_B(y, x1, ctrl, sca_util::sca_vector<std::pair<double, double> >({{threshold.get() - 1.0, 0.0}, { threshold.get(), 1.0 }}));
	add_pwl_b_stamp_to_B(y, x2, ctrl, sca_util::sca_vector<std::pair<double, double> >({{threshold.get() - 1.0, 1.0}, { threshold.get(), 0.0 }}));

}


} //namespace sca_lsf

