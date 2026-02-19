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

  sca_lsf_sub.h - linear signalflow subtractor

  Original Author: Karsten Einwich Fraunhofer IIS/EAS Dresden

  Created on: 05.03.2009

   SVN Version       :  $Revision: 1265 $
   SVN last checkin  :  $Date: 2011-11-19 20:43:31 +0000 (Sat, 19 Nov 2011) $
   SVN checkin by    :  $Author: karsten $
   SVN Id            :  $Id: sca_lsf_sub.h 1265 2011-11-19 20:43:31Z karsten $

 *****************************************************************************/
/*
 LRM clause 4.2.1.6.
 The class sca_lsf::sca_sub shall implement a primitive module for the LSF MoC
 that realizes the weighted subtraction of two LSF signals. The primitive shall
 contribute the following equation to the equation system:
 y(t) = k1*x1(t) - k2*x2(t)
 where x1(t) and x2(t) are the two LSF input signals, k1 and k2 are constant
 weighting coefficients, and y(t) is the LSF output signal.
 */


/*****************************************************************************/


#ifndef SCA_LSF_SUB_H_
#define SCA_LSF_SUB_H_

namespace sca_lsf {

  //class sca_sub : public implementation-derived-from sca_core::sca_module
  class sca_sub : public sca_lsf::sca_module
  {
   public:
    sca_lsf::sca_in  x1; // LSF inputs
    sca_lsf::sca_in  x2;

    sca_lsf::sca_out y;  // LSF output

    sca_core::sca_parameter<double> k1; // weighting coefficients
    sca_core::sca_parameter<double> k2;

    virtual const char* kind() const;

    explicit sca_sub(sc_core::sc_module_name, double k1_ = 1.0, double k2_ = 1.0 );

	// begin implementation specific

	private:
		 virtual void matrix_stamps();

	// end implementation specific

  };

} // namespace sca_lsf


#endif /* SCA_LSF_SUB_H_ */
