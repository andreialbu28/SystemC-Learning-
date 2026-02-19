/*****************************************************************************

    Copyright 2010-2014
    Fraunhofer-Gesellschaft zur Foerderung der angewandten Forschung e.V.

    Copyright 2015-2020
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

  sca_eln_vcvs.cpp - description

  Original Author: Karsten Einwich Fraunhofer IIS Dresden / COSEDA Technologies

  Created on: 10.11.2009

   SVN Version       :  $Revision: 2149 $
   SVN last checkin  :  $Date: 2020-12-18 08:51:03 +0000 (Fri, 18 Dec 2020) $
   SVN checkin by    :  $Author: karsten $
   SVN Id            :  $Id: sca_eln_vcvs.cpp 2149 2020-12-18 08:51:03Z karsten $

 *****************************************************************************/

/*****************************************************************************/

#include "systemc-ams"
#include "scams/predefined_moc/eln/sca_eln_vcvs.h"
#include "scams/impl/util/data_types/sca_method_object.h"
#include "scams/impl/util/data_types/sca_method_list.h"
#include "scams/impl/core/sca_solver_base.h"

namespace sca_eln
{

sca_vcvs::sca_vcvs(
		sc_core::sc_module_name, double value_,
		const sca_pwl_pair_vector& pwl_value_) :
	ncp("ncp"), ncn("ncn"), np("np"), nn("nn"), value("value", value_), pwl_value("pwl_value", pwl_value_)
{
	nadd1=-1;
	nadd2=-1;

	pwl_vector_size=0;
}

const char* sca_vcvs::kind() const
{
	return "sca_eln::sca_vcvs";
}

void sca_vcvs::matrix_stamps()
{
	const sca_pwl_pair_vector& pwl_vector(pwl_value.get());
	pwl_vector_size = pwl_vector.length();


	if (pwl_vector_size < 2) //no pwl description
	{
		nadd1 = add_equation();

		B_wr(np,nadd1)  =  1.0;
		B_wr(nn,nadd1)  += -1.0;

		B_wr(nadd1,np)  =  1.0;
		B_wr(nadd1,nn)  += -1.0;

		B_wr(nadd1,ncp) += -value;
		B_wr(nadd1,ncn) +=  value;
	}
	else //pwl description
	{

		nadd1 = add_equation(2);       // output voltage
		nadd2 = nadd1 + 1;             //- control voltage

		B_wr(np,nadd1)  =  1.0;
		B_wr(nn,nadd1)  += -1.0;

		B_wr(nadd1,np)  = -1.0;       // 0 = -v(np) + v(nn) + f(v(nadd2))
		B_wr(nadd1,nn)  +=  1.0;

		B_wr(nadd2,nadd2) = -1.0; // v(nadd2) = v(ncp) - v(ncn)
		B_wr(nadd2,ncp)   =  1.0;
		B_wr(nadd2,ncn)   += -1.0;

		B_wr(nadd1,nadd2) = value;  // v(nadd1) = -value * (v(ncn) - v(ncp))
		add_pwl_stamp_to_B(nadd1,nadd2,pwl_vector);  // v(nadd1) = - f(nadd2)

	}

}

} //namespace sca_eln

