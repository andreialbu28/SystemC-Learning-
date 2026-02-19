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

 sca_eln_terminal.h - electrical linear net terminal

 Original Author: Karsten Einwich Fraunhofer IIS Dresden / COSEDA Technologies

 Created on: 07.03.2009

 SVN Version       :  $Revision: 2106 $
 SVN last checkin  :  $Date: 2020-02-26 15:58:39 +0000 (Wed, 26 Feb 2020) $ (UTC)
 SVN checkin by    :  $Author: karsten $
 SVN Id            :  $Id: sca_eln_terminal.h 2106 2020-02-26 15:58:39Z karsten $

 *****************************************************************************/
/*
 * LRM clause 4.3.2.1.
 * The class sca_eln::sca_terminal shall define a port class for the ELN MoC.
 * The port shall be bound to a primitive channel of class sca_eln::sca_node or
 * sca_eln::sca_node_ref or to a port of class sca_eln::sca_terminal.
 */

/*****************************************************************************/

#ifndef SCA_ELN_TERMINAL_H_
#define SCA_ELN_TERMINAL_H_

namespace sca_eln
{

//class sca_terminal : public implementation-derived-from sca_core::sca_port< sca_eln::sca_node_if >
class sca_terminal: public sca_core::sca_port<sca_eln::sca_node_if>
{
public:
	sca_terminal();
	explicit sca_terminal(const char* name_);

	virtual const char* kind() const;

	const double& get_typed_trace_value() const;

private:
	// Other members
	//implementation-defined

	// Disabled
	sca_terminal(const sca_eln::sca_terminal&);


	//begin implementation specific

public:  //otherwise all eln modules must become friend
     /** returns node number of connected wire*/
	 long get_node_number();

	 /** allows node number access with terminal name only ->
	   * better look and feel for describing matrix_stamps */
	 operator long();

	 // end implementation specific
};

} // namespace sca_eln


#endif /* SCA_ELN_TERMINAL_H_ */
