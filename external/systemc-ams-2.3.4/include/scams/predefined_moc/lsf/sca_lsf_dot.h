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

 sca_lsf_dot.h - linear signal flow derivation

 Original Author: Karsten Einwich Fraunhofer IIS/EAS Dresden

 Created on: 05.03.2009

 SVN Version       :  $Revision: 1265 $
 SVN last checkin  :  $Date: 2011-11-19 20:43:31 +0000 (Sat, 19 Nov 2011) $
 SVN checkin by    :  $Author: karsten $
 SVN Id            :  $Id: sca_lsf_dot.h 1265 2011-11-19 20:43:31Z karsten $

 *****************************************************************************/
/*
 LRM claus 4.2.1.8.
 The class sca_lsf::sca_dot shall implement a primitive module for the LSF MoC
 that realizes the scaled first-order time derivative of an LSF signal.
 The primitive shall contribute the following equation to the equation system:
 y(t) = k* dx(t)/dt
 where k is the constant scale coefficient, x(t) is the LSF input signal, and
 y(t) is the LSF output signal.
 */

/*****************************************************************************/

#ifndef SCA_LSF_DOT_H_
#define SCA_LSF_DOT_H_

namespace sca_lsf
{

//class sca_dot : public implementation-derived-from sca_core::sca_module
class sca_dot: public sca_lsf::sca_module
{
public:
	sca_lsf::sca_in x; // LSF input

	sca_lsf::sca_out y; // LSF output

	sca_core::sca_parameter<double> k; // scale coefficient

	virtual const char* kind() const;

	explicit sca_dot(sc_core::sc_module_name, double k_ = 1.0);


	// begin implementation specific

	private:
		 virtual void matrix_stamps();

		 void d0_init();
		 bool first_call;

	// end implementation specific

};

} // namespace sca_lsf

#endif /* SCA_LSF_DOT_H_ */
