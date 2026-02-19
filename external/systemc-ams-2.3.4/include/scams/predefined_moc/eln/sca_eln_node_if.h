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

 sca_eln_node_if.h - interface for electrical linear nodes

 Original Author: Karsten Einwich Fraunhofer IIS/EAS Dresden

 Created on: 07.03.2009

 SVN Version       :  $Revision: 2106 $
 SVN last checkin  :  $Date: 2020-02-26 15:58:39 +0000 (Wed, 26 Feb 2020) $ (UTC)
 SVN checkin by    :  $Author: karsten $
 SVN Id            :  $Id: sca_eln_node_if.h 2106 2020-02-26 15:58:39Z karsten $

 *****************************************************************************/
/*
 * LRM clause 4.3.1.1.
 * The class sca_eln::sca_node_if shall define an interface proper for the
 * primitive channels of class sca_eln::sca_node and sca_eln::sca_node_ref. The
 * interface class member functions are implementation-defined.
 */

/*****************************************************************************/

#ifndef SCA_ELN_NODE_IF_H_
#define SCA_ELN_NODE_IF_H_

namespace sca_eln
{

//class sca_node_if : public implementation-derived-from sca_core::sca_interface
class sca_node_if: virtual public sca_core::sca_interface //sca_core::sca_implementation::sca_conservative_interface
{
public:

	//begin implementation specific
	virtual long get_node_number() const=0; 
	//end implementation specific

protected:
	sca_node_if(){}



private:
	// Other members
	//implementation-defined

	// Disabled
	sca_node_if(const sca_eln::sca_node_if&);
	sca_eln::sca_node_if& operator=(const sca_eln::sca_node_if&);
};

} // namespace sca_eln

#endif /* SCA_ELN_NODE_IF_H_ */
