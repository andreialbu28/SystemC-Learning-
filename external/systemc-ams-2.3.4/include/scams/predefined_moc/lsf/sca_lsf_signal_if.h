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

 sca_lsf_signal_if.h - linear signal flow signal interface

 Original Author: Karsten Einwich Fraunhofer IIS/EAS Dresden

 Created on: 05.03.2009

 SVN Version       :  $Revision: 1909 $
 SVN last checkin  :  $Date: 2016-02-16 10:09:52 +0000 (Tue, 16 Feb 2016) $
 SVN checkin by    :  $Author: karsten $
 SVN Id            :  $Id: sca_lsf_signal_if.h 1909 2016-02-16 10:09:52Z karsten $

 *****************************************************************************/
/*
 LRM clause 4.2.1.1.
 The class sca_lsf::sca_signal_if shall define an interface proper for a
 primitive channel of class sca_lsf::sca_signal. The interface class member
 functions are implementation-defined.
 */

/*****************************************************************************/

#ifndef SCA_LSF_SIGNAL_IF_H_
#define SCA_LSF_SIGNAL_IF_H_

namespace sca_lsf
{
class sca_out;
class sca_in;

//class sca_signal_if : public implementation-derived-from sca_core::sca_interface
// class sca_signal_if: public sca_core::sca_interface
class sca_signal_if: virtual public sca_core::sca_interface //sca_core::sca_implementation::sca_conservative_interface
{


protected:
	sca_signal_if(){}


private:
	// Other members
	//implementation-defined

	friend class sca_lsf::sca_out;
	friend class sca_lsf::sca_in;

	virtual long get_node_number() const=0;

	// Disabled
	sca_signal_if(const sca_lsf::sca_signal_if&);
	sca_lsf::sca_signal_if& operator=(const sca_lsf::sca_signal_if&);
};

} // namespace sca_lsf

#endif /* SCA_LSF_SIGNAL_IF_H_ */
