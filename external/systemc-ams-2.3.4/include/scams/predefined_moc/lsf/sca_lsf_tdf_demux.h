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

 sca_lsf_tdf_demux.h - linear signal flow demultiplexer controlled by a tdf signal

 Original Author: Karsten Einwich Fraunhofer IIS/EAS Dresden

 Created on: 06.03.2009

 SVN Version       :  $Revision: 1681 $
 SVN last checkin  :  $Date: 2014-04-02 10:11:21 +0000 (Wed, 02 Apr 2014) $
 SVN checkin by    :  $Author: karsten $
 SVN Id            :  $Id: sca_lsf_tdf_demux.h 1681 2014-04-02 10:11:21Z karsten $

 *****************************************************************************/
/*
 * LRM clause 4.2.19.
 * The class sca_lsf::sca_tdf::sca_demux</classname> shall implement a primitive
 * module for the LSF MoC that realizes the routing of an LSF input signal to
 * either one of two LSF output signals controlled by a TDF signal
 * (demultiplexer). The primitive shall contribute the following equations to
 * the equation system:
 *
 *        y1(t) = x(t)   ctrl=flase
 *        y1(t) = 0      ctrl=true
 *
 *        y2(t) = 0      ctrl=false
 *        y2(t) = x(t)   ctrl=true
 *
 * where ctrl is the TDF control signal, x(t) is the LSF input signal, and y1(t)
 * and y2(t) are the LSF output signals.
 */

/*****************************************************************************/

#ifndef SCA_LSF_TDF_DEMUX_H_
#define SCA_LSF_TDF_DEMUX_H_

namespace sca_lsf
{

namespace sca_tdf
{

//class sca_demux : public implementation-derived-from sca_core::sca_module
class sca_demux: public sca_lsf::sca_module
{
public:
	sca_lsf::sca_in x; // LSF input

	sca_lsf::sca_out y1; // LSF outputs
	sca_lsf::sca_out y2;

	::sca_tdf::sca_in<bool> ctrl; // TDF control input

	virtual const char* kind() const;

	explicit sca_demux(sc_core::sc_module_name);

private:

	bool ctrl_val, ctrl_old;

	void update_ctrl();
	void read_ctrl();
	void matrix_stamps();

};

} // namespace sca_tdf

typedef sca_lsf::sca_tdf::sca_demux sca_tdf_demux;

} // namespace sca_lsf


#endif /* SCA_LSF_TDF_DEMUX_H_ */
