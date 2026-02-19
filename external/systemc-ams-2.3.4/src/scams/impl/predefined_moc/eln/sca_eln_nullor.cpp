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

  sca_eln_nullor.cpp - description

  Original Author: Karsten Einwich Fraunhofer IIS/EAS Dresden

  Created on: 10.11.2009

   SVN Version       :  $Revision: 2149 $
   SVN last checkin  :  $Date: 2020-12-18 08:51:03 +0000 (Fri, 18 Dec 2020) $
   SVN checkin by    :  $Author: karsten $
   SVN Id            :  $Id: sca_eln_nullor.cpp 2149 2020-12-18 08:51:03Z karsten $

 *****************************************************************************/

/*****************************************************************************/


#include "systemc-ams"
#include "scams/predefined_moc/eln/sca_eln_nullor.h"

namespace sca_eln
{

sca_nullor::sca_nullor(sc_core::sc_module_name) :
	nip("nip"), nin("nin"), nop("nop"), non("non")
{
	nadd=-1;
}


const char* sca_nullor::kind() const
{
	return "sca_eln::sca_nullor";
}

void sca_nullor::matrix_stamps()
{
    nadd = add_equation();

    // v(nip)-v(nin) = 0!
    B_wr(nadd, nip) =  1.0;
    B_wr(nadd, nin) += -1.0;

    // i(nop) += i(nadd), i(non) -= i(nadd)
    B_wr(nop, nadd) =  1.0;
    B_wr(non, nadd) += -1.0;
}

} //namespace sca_eln

