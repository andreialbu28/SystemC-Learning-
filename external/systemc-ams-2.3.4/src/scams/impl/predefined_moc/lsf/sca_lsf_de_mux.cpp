/*****************************************************************************

    Copyright 2010-2014
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

  sca_lsf_de_mux.cpp - description

  Original Author: Karsten Einwich Fraunhofer IIS/EAS Dresden

  Created on: 07.01.2010

   SVN Version       :  $Revision: 1681 $
   SVN last checkin  :  $Date: 2014-04-02 10:11:21 +0000 (Wed, 02 Apr 2014) $
   SVN checkin by    :  $Author: karsten $
   SVN Id            :  $Id: sca_lsf_de_mux.cpp 1681 2014-04-02 10:11:21Z karsten $

 *****************************************************************************/

/*****************************************************************************/


#include "systemc-ams"
#include "scams/predefined_moc/lsf/sca_lsf_de_mux.h"
#include "scams/impl/util/data_types/sca_method_object.h"
#include "scams/impl/util/data_types/sca_method_list.h"
#include "scams/impl/core/sca_solver_base.h"


namespace sca_lsf
{
namespace sca_de
{

sca_mux::sca_mux(sc_core::sc_module_name) :
	x1("x1"), x2("x2"), y("y"), ctrl("ctrl")
{
    //IMPROVE: find better solution -> inserts additional port in database
    conv_port=new ::sca_tdf::sca_de::sca_in<bool>("converter_port");
    conv_port->bind(ctrl);

    ctrl_val=false;
    ctrl_old=false;
}

const char* sca_mux::kind() const
{
	return "sca_lsf::sca_de::sca_mux";
}




void sca_mux::update_ctrl()
{
    if(get_time()==sc_core::SC_ZERO_TIME) read_ctrl();


	if(ctrl_val!=ctrl_old)
	{
		enable_b_change=true;
		continous=false;

		ctrl_old=ctrl_val;

		if(!ctrl_val)
		{
			B(y,x1) = 1.0;
			B(y,x2) = 0.0;
		}
		else
		{
			B(y,x1) = 0.0;
			B(y,x2) = 1.0;
		}
	}
}


void sca_mux::read_ctrl()
{
    ctrl_val = conv_port->read();
}


void sca_mux::matrix_stamps()
{
    B(y,y)  =  -1.0;

	if(!ctrl_val)
	{
		B(y,x1) = 1.0;
   		B(y,x2) = 0.0;
	}
	else
	{
		B(y,x1) = 0.0;
   		B(y,x2) = 1.0;
	}

	add_method(PRE_SOLVE, SCA_VMPTR(sca_mux::update_ctrl));
	add_method(POST_SOLVE, SCA_VMPTR(sca_mux::read_ctrl));
}


} //namespace sca_tdf
} //namespace sca_lsf

