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

 sca_lsf_tdf_mux.h - linear signal flow multiplexer controlled by a tdf signal

 Original Author: Karsten Einwich Fraunhofer IIS/EAS Dresden

 Created on: 05.03.2009

 SVN Version       :  $Revision: 1681 $
 SVN last checkin  :  $Date: 2014-04-02 10:11:21 +0000 (Wed, 02 Apr 2014) $
 SVN checkin by    :  $Author: karsten $
 SVN Id            :  $Id: sca_lsf_tdf_mux.h 1681 2014-04-02 10:11:21Z karsten $

 *****************************************************************************/
/*
 * LRM clause 4.2.1.18.
 * The class sca_lsf::sca_tdf::sca_mux shall implement a primitive module for
 * the LSF MoC that realizes the selection of one of two LSF signals by a TDF
 * control signal (multiplexer). The primitive shall contribute the following
 * equation to the equation system:
 *         y(t) = x1(t)   ctrl=false
 *         y(t) = x2(t)   ctrl=true
 * where ctrl is the TDF control signal, x1(t) and x2(t) are the LSF input
 * signals, and y(t) is the LSF output signal.
 */

/*****************************************************************************/

#ifndef SCA_LSF_TDF_MUX_H_
#define SCA_LSF_TDF_MUX_H_

namespace sca_lsf
{

namespace sca_tdf
{

//class sca_mux : public implementation-derived-from sca_core::sca_module
class sca_mux: public sca_lsf::sca_module
{
public:
	sca_lsf::sca_in x1; // LSF inputs
	sca_lsf::sca_in x2;

	sca_lsf::sca_out y; // LSF output

	::sca_tdf::sca_in<bool> ctrl; // TDF control input

	virtual const char* kind() const;

	explicit sca_mux(sc_core::sc_module_name);

private:

	bool ctrl_val, ctrl_old;

	void update_ctrl();
	void read_ctrl();
	void matrix_stamps();
};

} // namespace sca_tdf

typedef sca_lsf::sca_tdf::sca_mux sca_tdf_mux;

} // namespace sca_lsf

#endif /* SCA_LSF_TDF_MUX_H_ */
