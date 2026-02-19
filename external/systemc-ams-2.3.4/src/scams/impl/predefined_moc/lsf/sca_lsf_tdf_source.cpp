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

  sca_lsf_dot.cpp - description

  Original Author: Karsten Einwich Fraunhofer IIS/EAS Dresden

  Created on: 05.01.2010

   SVN Version       :  $Revision: 1265 $
   SVN last checkin  :  $Date: 2011-11-19 20:43:31 +0000 (Sat, 19 Nov 2011) $
   SVN checkin by    :  $Author: karsten $
   SVN Id            :  $Id: sca_lsf_tdf_source.cpp 1265 2011-11-19 20:43:31Z karsten $

 *****************************************************************************/

/*****************************************************************************/


#include "systemc-ams"
#include "scams/predefined_moc/lsf/sca_lsf_tdf_source.h"
#include "scams/impl/util/data_types/sca_method_object.h"
#include "scams/impl/util/data_types/sca_method_list.h"
#include "scams/impl/core/sca_solver_base.h"

namespace sca_lsf
{
namespace sca_tdf
{

sca_source::sca_source(sc_core::sc_module_name, double scale_) :
	inp("inp"), y("y"), scale("scale", scale_)
{
}

const char* sca_source::kind() const
{
	return "sca_lsf::sca_tdf::sca_source";
}

void sca_source::matrix_stamps()
{
    B(y,y)  =  -1.0;

    q(y).add_element(SCA_MPTR(sca_source::value_t),this);
}

double sca_source::value_t()
{
    if(!sca_ac_analysis::sca_ac_is_running())
    {
    	return scale * inp.read();
    }
    else
    {
        double val = scale * sca_ac_analysis::sca_ac(inp).real();
        return val;
    }
}


} //namespace sca_tdf
} //namespace sca_lsf

