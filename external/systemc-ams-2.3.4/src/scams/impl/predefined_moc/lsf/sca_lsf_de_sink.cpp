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

  sca_lsf_de_sink.cpp - description

  Original Author: Karsten Einwich Fraunhofer IIS/EAS Dresden

  Created on: 07.01.2010

   SVN Version       :  $Revision: 2320 $
   SVN last checkin  :  $Date: 2023-06-08 12:47:38 +0000 (Thu, 08 Jun 2023) $
   SVN checkin by    :  $Author: pehrlich $
   SVN Id            :  $Id: sca_lsf_de_sink.cpp 2320 2023-06-08 12:47:38Z pehrlich $

 *****************************************************************************/

/*****************************************************************************/


#include "systemc-ams"
#include "scams/predefined_moc/lsf/sca_lsf_de_sink.h"
#include "scams/impl/util/data_types/sca_method_object.h"
#include "scams/impl/core/sca_solver_base.h"

namespace sca_lsf
{
namespace sca_de
{

sca_sink::sca_sink(sc_core::sc_module_name, double scale_) :
	x("x"), outp("outp"), scale("scale", scale_)
{

	//IMPROVE: find better solution -> inserts additional port in database
	conv_port=new ::sca_tdf::sca_de::sca_out<double>("converter_port");
	conv_port->bind(outp);

}

const char* sca_sink::kind() const
{
	return "sca_lsf::sca_de::sca_sink";
}


void sca_sink::assign_result()
{
    if(!sca_ac_analysis::sca_ac_is_running())
    {
    	conv_port->write( sca_lsf::sca_module::x(x)*scale.get());
    }
    else
    {
        sca_ac_analysis::sca_implementation::sca_ac(outp) = scale.get() * sca_lsf::sca_module::x(x);
    }
}


void sca_sink::matrix_stamps()
{
	add_method(POST_SOLVE, SCA_VMPTR(sca_sink::assign_result));
}


} //namespace sca_de
} //namespace sca_lsf

