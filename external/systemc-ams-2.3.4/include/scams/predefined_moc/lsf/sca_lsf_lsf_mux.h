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

 sca_lsf_sc_mux.h - linear signal flow multiplexer controlled by a sc_signal<bool>

 Original Author: Karsten Einwich COSEDA Technologies GmbH

 Created on: 28.01.2020

 SVN Version       :  $Revision: 1681 $
 SVN last checkin  :  $Date: 2014-04-02 03:11:21 -0700 (Wed, 02 Apr 2014) $
 SVN checkin by    :  $Author: karsten $
 SVN Id            :  $Id: sca_lsf_de_mux.h 1681 2014-04-02 10:11:21Z karsten $

 *****************************************************************************/
/*
 * LRM clause 4.2.1.23.
 * The class sca_lsf::sc_core::sca_mux shall implement a primitive module for
 * the LSF MoC that realizes the selection of one of two LSF signals by a
 * discrete-event control signal (multiplexer). The primitive shall contribute
 * the following equation to the equation system:
 *
 *        y(t) = x1(t)  ctrl>=threshold
 *        y(t) = x2(t)  ctrl< threshold
 *
 */

/*****************************************************************************/

#ifndef SCA_LSF_LSF_MUX_H_
#define SCA_LSF_LSF_MUX_H_

namespace sca_lsf
{


//class sca_mux : public implementation-derived-from sca_core::sca_module
class sca_lsf_mux: public sca_lsf::sca_module
{
public:
	sca_lsf::sca_in x1; // LSF inputs
	sca_lsf::sca_in x2;

	sca_lsf::sca_out y; // LSF output

	sca_lsf::sca_in ctrl; // lsf control

	sca_core::sca_parameter<double> threshold;


	virtual const char* kind() const;

	explicit sca_lsf_mux(sc_core::sc_module_name, double threshold_=0.0);

private:

	void matrix_stamps();

};


} // namespace sca_lsf

#endif /* SCA_LSF_LSF_MUX_H_ */
