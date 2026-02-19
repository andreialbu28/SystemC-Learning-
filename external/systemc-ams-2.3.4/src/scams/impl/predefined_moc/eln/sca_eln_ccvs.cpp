/*****************************************************************************

    Copyright 2010
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

  sca_eln_ccvs.cpp - description

  Original Author: Karsten Einwich Fraunhofer IIS Dresden / COSEDA Technologies

  Created on: 10.11.2009

   SVN Version       :  $Revision: 2320 $
   SVN last checkin  :  $Date: 2023-06-08 12:47:38 +0000 (Thu, 08 Jun 2023) $
   SVN checkin by    :  $Author: pehrlich $
   SVN Id            :  $Id: sca_eln_ccvs.cpp 2320 2023-06-08 12:47:38Z pehrlich $

 *****************************************************************************/

/*****************************************************************************/


#include "systemc-ams"
#include "scams/predefined_moc/eln/sca_eln_ccvs.h"
#include "scams/impl/util/data_types/sca_method_object.h"
#include "scams/impl/util/data_types/sca_method_list.h"

namespace sca_eln
{

sca_ccvs::	sca_ccvs(
		sc_core::sc_module_name, double value_,
		const sca_pwl_pair_vector& pwl_value_) :
	ncp("ncp"), ncn("ncn"), np("np"), nn("nn"), value("value", value_), pwl_value("pwl_value", pwl_value_)
{
	nadd1=-1;
	nadd2=-1;

	pwl_vector_size=0;
}


const char* sca_ccvs::kind() const
{
	return "sca_eln::sca_ccvs";
}

void sca_ccvs::matrix_stamps()
{
	const sca_pwl_pair_vector& pwl_vector(pwl_value.get());
	pwl_vector_size = pwl_vector.length();

	nadd1 = add_equation();
	nadd2 = add_equation();

	B_wr(ncp,nadd1) =  1.0;
	B_wr(ncn,nadd1) += -1.0;
	B_wr(nadd1,ncp) =  1.0;
	B_wr(nadd1,ncn) += -1.0;

	B_wr(np,nadd2)  =  -1.0;
	B_wr(nn,nadd2)  += 1.0;
	B_wr(nadd2,np)  =  -1.0;
	B_wr(nadd2,nn)  += 1.0;

	B_wr(nadd2,nadd1) = value;

	if (pwl_vector_size >= 2) //no pwl description
	{
		add_pwl_stamp_to_B(nadd2,nadd1,pwl_vector);
	}
}

} //namespace sca_eln

