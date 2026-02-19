/*****************************************************************************

    Copyright 2010
    Fraunhofer-Gesellschaft zur Foerderung der angewandten Forschung e.V.

    Copyright 2015-2020   COSEDA Technologies GmbH

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

 sca_lsf_gain.h - linear signal flow gain

 Original Author: Karsten Einwich Fraunhofer IIS Dresden / COSEDA Technologies

 Created on: 05.03.2009

 SVN Version       :  $Revision: 2096 $
 SVN last checkin  :  $Date: 2020-01-28 15:42:55 +0000 (Tue, 28 Jan 2020) $
 SVN checkin by    :  $Author: karsten $
 SVN Id            :  $Id: sca_lsf_gain.h 2096 2020-01-28 15:42:55Z karsten $

 *****************************************************************************/
/*
 LRM clause 4.2.1.7.
 The class sca_lsf::sca_gain shall implement a primitive module for the LSF MoC
 that realizes the multiplication of an LSF&nbsp;signal by a constant gain.
 The primitive shall contribute the following equation to the equation system:
 y(t) = k*x(t)
 where k is the constant gain coefficient, x(t is the LSF input signal, and
 y(t) is the LSF output signal.
 */

/*****************************************************************************/

#ifndef SCA_LSF_GAIN_H_
#define SCA_LSF_GAIN_H_

namespace sca_lsf
{

//class sca_gain : public implementation-derived-from sca_core::sca_module
class sca_gain: public sca_lsf::sca_module
{
	typedef sca_util::sca_vector<std::pair<double,double> > sca_pwl_pair_vector;

public:
	sca_lsf::sca_in x; // LSF input

	sca_lsf::sca_out y; // LSF output

	sca_core::sca_parameter<double> k; // gain coefficient
	sca_core::sca_parameter<sca_pwl_pair_vector> pwl_k;


	virtual const char* kind() const;

	explicit sca_gain(sc_core::sc_module_name, double k_ = 1.0,
			const sca_pwl_pair_vector& pwl_k_ = sca_pwl_pair_vector());

// begin implementation specific

private:
	 virtual void matrix_stamps();

	 //pwl controlled sources
	 unsigned int pwl_vector_size;  //number of values in pwl_value

// end implementation specific

};

} // namespace sca_lsf

#endif /* SCA_LSF_GAIN_H_ */
