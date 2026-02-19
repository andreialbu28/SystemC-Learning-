/*****************************************************************************

    Copyright 2010
    Fraunhofer-Gesellschaft zur Foerderung der angewandten Forschung e.V.

	Copyright 2015-2022
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

  sca_eln_terminal.cpp - description

  Original Author: Karsten Einwich Fraunhofer IIS Dresden / COSEDA Technologies

  Created on: 10.11.2009

   SVN Version       :  $Revision: 2320 $
   SVN last checkin  :  $Date: 2023-06-08 12:47:38 +0000 (Thu, 08 Jun 2023) $
   SVN checkin by    :  $Author: pehrlich $
   SVN Id            :  $Id: sca_eln_terminal.cpp 2320 2023-06-08 12:47:38Z pehrlich $

 *****************************************************************************/

/*****************************************************************************/

#include "systemc-ams"
#include "scams/predefined_moc/eln/sca_eln_terminal.h"

namespace sca_eln
{


const double& sca_terminal::get_typed_trace_value() const
{
	const sc_core::sc_interface* scif=this->get_interface();
	const sca_eln::sca_node* scasig=
			dynamic_cast<const sca_eln::sca_node*>(scif);

	if(scasig==NULL)
	{
		std::ostringstream str;
		str << "get_(typed)_trace_value() called for not (yet) bound terminal: ";
		str << this->name() << " - the result will be undefined";
		SC_REPORT_WARNING("SystemC-AMS",str.str().c_str());
		static const double dummy(0.0);
		return dummy;
	}
	return scasig->get_typed_trace_value();
}

sca_terminal::sca_terminal() :
	sca_core::sca_port<sca_eln::sca_node_if>(sc_core::sc_gen_unique_name("sca_eln_terminal"))
{
		sca_port_type = SCA_CONSERVATIVE_PORT;
}


sca_terminal::sca_terminal(const char* name_) :
	sca_core::sca_port<sca_eln::sca_node_if>(name_)
{
		sca_port_type = SCA_CONSERVATIVE_PORT;
}

const char* sca_terminal::kind() const
{
	return "sca_eln::sca_terminal";
}


/** returns node number of connected wire*/
long sca_terminal::get_node_number()
{
	return (*this)-> get_node_number();
}


 /** allows node number access with terminal name only ->
  * better look and feel for describing matrix_stamps */
sca_terminal::operator long()
{
	return this->get_node_number();
}



sca_terminal::sca_terminal(const sca_eln::sca_terminal&)
{
}


} // namespace sca_eln
