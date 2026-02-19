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

 sca_eln_gyrator.cpp - description

 Original Author: Karsten Einwich Fraunhofer IIS/EAS Dresden

 Created on: 10.11.2009

 SVN Version       :  $Revision: 2106 $
 SVN last checkin  :  $Date: 2020-02-26 15:58:39 +0000 (Wed, 26 Feb 2020) $
 SVN checkin by    :  $Author: karsten $
 SVN Id            :  $Id: sca_eln_gyrator.cpp 2106 2020-02-26 15:58:39Z karsten $

 *****************************************************************************/

/*****************************************************************************/

#include "systemc-ams"
#include "scams/predefined_moc/eln/sca_eln_gyrator.h"

namespace sca_eln
{

sca_gyrator::sca_gyrator(sc_core::sc_module_name, double g1_,
		double g2_) :
	p1("p1"), n1("n1"), p2("p2"), n2("n2"), g1("g1", g1_), g2("g2", g2_)
{
}


const char* sca_gyrator::kind() const
{
	return "sca_eln::sca_gyrator";
}

void sca_gyrator::matrix_stamps()
{
    B_wr(p2, p1) += -g1;
    B_wr(p2, n1) +=  g1;
    B_wr(n2, p1) +=  g1;
    B_wr(n2, n1) += -g1;
    B_wr(p1, p2) +=  g2;
    B_wr(p1, n2) += -g2;
    B_wr(n1, p2) += -g2;
    B_wr(n1, n2) +=  g2;
}

} //namespace sca_eln

