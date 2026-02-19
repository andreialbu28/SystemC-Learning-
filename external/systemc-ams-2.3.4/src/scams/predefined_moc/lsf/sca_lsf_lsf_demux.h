/*****************************************************************************

    Copyright 2010
    Fraunhofer-Gesellschaft zur Foerderung der angewandten Forschung e.V.

    Copyright 2015-2016   COSEDA Technologies GmbH

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

 sca_lsf_sc_demux.h - linear signal flow demultiplexer controlled by a sc_signal<bool>

 Original Author: Karsten Einwich COSEDA Technologies GmbH

 Created on: 28.01.2020

 SVN Version       :  $Revision: 1681 $
 SVN last checkin  :  $Date: 2014-04-02 03:11:21 -0700 (Wed, 02 Apr 2014) $
 SVN checkin by    :  $Author: karsten $
 SVN Id            :  $Id: sca_lsf_de_demux.h 1681 2014-04-02 10:11:21Z karsten $

 *****************************************************************************/
/*
 * LRM clause 4.2.1.24.
 * The class sca_lsf::sc_core::sca_demux shall implement a primitive module for
 * the LSF MoC that realizes the routing of an LSF&nbsp;input signal to either
 * one of two LSF output signals controlled by a discrete-event signal
 * (demultiplexer). The primitive shall contribute the following equations to
 * the equation system:
 *
 *        y1(t) = x(t)   ctrl >= threshold
 *        y1(t) = 0      ctrl <  threshold
 *
 *        y2(t) = 0      ctrl >= threshold
 *        y2(t) = x(t)   ctrl <  threshold
 *
 */

/*****************************************************************************/

#ifndef SCA_LSF_LSF_DEMUX_H_
#define SCA_LSF_LSF_DEMUX_H_

namespace sca_lsf
{


//class sca_demux : public implementation-derived-from sca_core::sca_module
class sca_lsf_demux: public sca_lsf::sca_module
{
public:
	sca_lsf::sca_in x; // LSF input

	sca_lsf::sca_out y1; // LSF outputs
	sca_lsf::sca_out y2;

	sca_lsf::sca_in ctrl; // lsf control

	sca_core::sca_parameter<double> threshold;

	virtual const char* kind() const;

	explicit sca_lsf_demux(sc_core::sc_module_name,double threshold_=0.0);

private:

	void matrix_stamps();
};


} // namespace sca_lsf


#endif /* SCA_LSF_LSF_DEMUX_H_ */
