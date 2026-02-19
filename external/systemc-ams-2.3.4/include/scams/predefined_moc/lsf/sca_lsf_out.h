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

 sca_lsf_out.h - linear signal flow outport

 Original Author: Karsten Einwich Fraunhofer IIS Dresden / COSEDA Technologies

 Created on: 05.03.2009

 SVN Version       :  $Revision: 1973 $
 SVN last checkin  :  $Date: 2016-03-24 15:46:07 +0000 (Thu, 24 Mar 2016) $
 SVN checkin by    :  $Author: karsten $
 SVN Id            :  $Id: sca_lsf_out.h 1973 2016-03-24 15:46:07Z karsten $

 *****************************************************************************/
/*
 LRM clause 4.2.1.4.
 The class sca_lsf::sca_out shall define a port class for the LSF MoC. The
 port shall be bound to a primitve channel of class sca_lsf::sca_signal or a
 port of class sca_lsf::sca_out.

 */

/*****************************************************************************/

#ifndef SCA_LSF_OUT_H_
#define SCA_LSF_OUT_H_

namespace sca_lsf
{

class sca_out: public sca_core::sca_port<sca_lsf::sca_signal_if>
{
public:
	sca_out();
	explicit sca_out(const char*);

	virtual const char* kind() const;

private:
	// Other members
	//implementation-defined

	// Disabled
	sca_out(const sca_lsf::sca_out&);

public:

	const double& get_typed_trace_value() const;


	//begin implementation specific

public:  //otherwise all lsf modules must become friend
     /** returns node number of connected wire*/
	 long get_signal_number();

	 /** allows node number access with terminal name only ->
	   * better look and feel for describing matrix_stamps */
	 operator long();

	 // end implementation specific

};

} // namespace sca_lsf


#endif /* SCA_LSF_OUT_H_ */
