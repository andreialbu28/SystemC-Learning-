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

  sca_lsf_de_gain.cpp - description

  Original Author: Karsten Einwich Fraunhofer IIS/EAS Dresden

  Created on: 07.01.2010

   SVN Version       :  $Revision: 1681 $
   SVN last checkin  :  $Date: 2014-04-02 10:11:21 +0000 (Wed, 02 Apr 2014) $
   SVN checkin by    :  $Author: karsten $
   SVN Id            :  $Id: sca_lsf_de_gain.cpp 1681 2014-04-02 10:11:21Z karsten $

 *****************************************************************************/

/*****************************************************************************/


#include "systemc-ams"
#include "scams/predefined_moc/lsf/sca_lsf_de_gain.h"
#include "scams/impl/util/data_types/sca_method_object.h"
#include "scams/impl/util/data_types/sca_method_list.h"
#include "scams/impl/core/sca_solver_base.h"
#include <limits>

namespace sca_lsf
{
namespace sca_de
{

sca_gain::sca_gain(sc_core::sc_module_name, double scale_) :
	inp("inp"), x("x"), y("y"), scale("scale",scale_)
{
    //IMPROVE: find better solution -> inserts additional port in database
    conv_port=new ::sca_tdf::sca_de::sca_in<double>("converter_port");
    conv_port->bind(inp);

    ctrl_val=false;
    ctrl_old=false;
    ctrl_pre_val = false;
}

const char* sca_gain::kind() const
{
	return "sca_lsf::sca_de::sca_gain";
}

///////////////////////////////////////////////////////

void sca_gain::update_ctrl()
{
	if(get_time()==sc_core::SC_ZERO_TIME) read_ctrl();

	if(ctrl_old!=ctrl_val)
	{
		enable_b_change=true;
		continous=true;

		ctrl_old=ctrl_val;

		B(y,x) = ctrl_val*scale.get();
	}
}

void sca_gain::read_ctrl()
{
    ctrl_val = conv_port->read();
}


void sca_gain::matrix_stamps()
{
    B(y,y)  =  -1.0;

	B(y,x) = ctrl_val*scale.get();

	add_method(PRE_SOLVE, SCA_VMPTR(sca_gain::update_ctrl));
	add_method(POST_SOLVE, SCA_VMPTR(sca_gain::read_ctrl));
}

}
}


