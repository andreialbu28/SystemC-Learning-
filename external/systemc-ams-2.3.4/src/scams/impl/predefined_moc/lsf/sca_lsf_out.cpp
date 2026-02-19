/*****************************************************************************

    Copyright 2010
    Fraunhofer-Gesellschaft zur Foerderung der angewandten Forschung e.V.

	Copyright 2015-2016
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

  sca_lsf_out.cpp - description

  Original Author: Karsten Einwich Fraunhofer IIS Dresden / COSEDA Technologies

  Created on: 05.01.2010

   SVN Version       :  $Revision: 1973 $
   SVN last checkin  :  $Date: 2016-03-24 15:46:07 +0000 (Thu, 24 Mar 2016) $
   SVN checkin by    :  $Author: karsten $
   SVN Id            :  $Id: sca_lsf_out.cpp 1973 2016-03-24 15:46:07Z karsten $

 *****************************************************************************/

/*****************************************************************************/


#include "systemc-ams"
#include "scams/predefined_moc/lsf/sca_lsf_out.h"

namespace sca_lsf
{

const double& sca_out::get_typed_trace_value() const
{
	const sc_core::sc_interface* scif=this->get_interface();
	const sca_lsf::sca_signal* scasig=
			dynamic_cast<const sca_lsf::sca_signal*>(scif);

	if(scasig==NULL)
	{
		std::ostringstream str;
		str << "get_(typed)_trace_value() called for not (yet) bound port: ";
		str << this->name() << " - the result will be undefined";
		SC_REPORT_WARNING("SystemC-AMS",str.str().c_str());
		static const double dummy(0.0);
		return dummy;
	}
	return scasig->get_typed_trace_value();
}


sca_out::sca_out() :
	sca_core::sca_port<sca_lsf::sca_signal_if>(sc_core::sc_gen_unique_name("sca_lsf_out"))
{
		sca_port_type = SCA_OUT_PORT;
}


sca_out::sca_out(const char* name_) :
	sca_core::sca_port<sca_lsf::sca_signal_if>(name_)
{
		sca_port_type = SCA_OUT_PORT;
}

const char* sca_out::kind() const
{
	return "sca_lsf::sca_out";
}


/** returns node number of connected wire*/
long sca_out::get_signal_number()
{
	return (*this)-> get_node_number();
}

 /** allows node number access with terminal name only ->
  * better look and feel for describing matrix_stamps */
sca_out::operator long()
{
	return this->get_signal_number();
}



sca_out::sca_out(const sca_lsf::sca_out&)
{
}


} // namespace sca_lsf


