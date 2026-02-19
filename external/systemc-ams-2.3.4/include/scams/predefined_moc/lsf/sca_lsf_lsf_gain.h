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

 sca_lsf_sc_gain.h - linear signal flow gain controlled by a sc_signal<double>

 Original Author: Karsten Einwich COSEDA Technologies GmbH

 Created on: 28.01.2020

 SVN Version       :  $Revision: 1681 $
 SVN last checkin  :  $Date: 2014-04-02 03:11:21 -0700 (Wed, 02 Apr 2014) $
 SVN checkin by    :  $Author: karsten $
 SVN Id            :  $Id: sca_lsf_de_gain.h 1681 2014-04-02 10:11:21Z karsten $

 *****************************************************************************/
/*
 * LRM clause 4.2.1.20.
 * The class sca_lsf::sc_core::sca_gain shall implement a primitive module for
 * the LSF MoC that realizes the scaled multiplication of a discrete-event input
 * signal by an LSF input signal. The primitive shall contribute the following
 * equation to the equation system:
 *
 *        y(t) = fpwc(inp) * x(t)
 *
 * where scale is the constant scale coefficient, inp is the discrete-event
 * input signal that shall be interpreted as a discrete-time signal, x(t) is the
 * LSF input signal, and y(t) is the LSF output signal.
 *
 */

/*****************************************************************************/

#ifndef SCA_LSF_LSF_SC_GAIN_H_
#define SCA_LSF_LSF_SC_GAIN_H_

namespace sca_lsf
{

//class sca_gain : public implementation-derived-from sca_core::sca_module
class sca_lsf_gain: public sca_lsf::sca_module
{
	typedef sca_util::sca_vector<std::pair<double,double> > sca_pwl_pair_vector;

public:

	sca_lsf::sca_in inp; // lsf input

	sca_lsf::sca_in x; // LSF input

	sca_lsf::sca_out y; // LSF output

	sca_core::sca_parameter<sca_pwl_pair_vector> pwc_gain;

	virtual const char* kind() const;

	explicit sca_lsf_gain(sc_core::sc_module_name, const sca_pwl_pair_vector& pwc_gain_ = {{0.0,1.0}});


private:

	void matrix_stamps();
};

} // namespace sca_lsf

#endif /* SCA_LSF_LSF_GAIN_H_ */
