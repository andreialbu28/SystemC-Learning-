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

 sca_lsf_sc_sink.h - converter from linear signal flow to a sc_signal<double>

 Original Author: Karsten Einwich Fraunhofer IIS/EAS Dresden

 Created on: 06.03.2009

 SVN Version       :  $Revision: 1909 $
 SVN last checkin  :  $Date: 2016-02-16 10:09:52 +0000 (Tue, 16 Feb 2016) $
 SVN checkin by    :  $Author: karsten $
 SVN Id            :  $Id: sca_lsf_de_sink.h 1909 2016-02-16 10:09:52Z karsten $

 *****************************************************************************/
/*
 * LRM clause 4.2.1.22.
 * The class sca_lsf::sca_sc::sca_sink shall implement a primitive module for
 * the LSF MoC that realizes a scaled conversion from an LSF signal to a
 * discrete-event signal. The value of the LSF input signal x(t) shall be scaled
 * with coefficient scale and written to the discrete-event output port outp.
 * The primitive shall not contribute any equation to the equation system.
 */

/*****************************************************************************/

#ifndef SCA_LSF_SC_SINK_H_
#define SCA_LSF_SC_SINK_H_

namespace sca_lsf
{

namespace sca_de
{

//class sca_sink : public implementation-derived-from sca_core::sca_module
class sca_sink: public sca_lsf::sca_module
{
public:
	sca_lsf::sca_in x; // LSF input

	sc_core::sc_out<double> outp; // discrete-event output

	sca_core::sca_parameter<double> scale; // scale coefficient

	virtual const char* kind() const;

	explicit sca_sink(sc_core::sc_module_name, double scale_ = 1.0);

private:

	virtual void matrix_stamps();
	void assign_result();

	::sca_tdf::sca_de::sca_out<double>* conv_port;
};

} // namespace sca_de

typedef sca_lsf::sca_de::sca_sink sca_de_sink;

} // namespace sca_lsf


#endif /* SCA_LSF_SC_SINK_H_ */
