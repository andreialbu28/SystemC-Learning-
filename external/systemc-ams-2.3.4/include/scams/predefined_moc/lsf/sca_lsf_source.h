/*****************************************************************************

    Copyright 2010-2012
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

 sca_lsf_source.h - linear signal flow source

 Original Author: Karsten Einwich Fraunhofer IIS/EAS Dresden

 Created on: 05.03.2009

 SVN Version       :  $Revision: 1305 $
 SVN last checkin  :  $Date: 2012-04-11 15:13:49 +0000 (Wed, 11 Apr 2012) $
 SVN checkin by    :  $Author: karsten $
 SVN Id            :  $Id: sca_lsf_source.h 1305 2012-04-11 15:13:49Z karsten $

 *****************************************************************************/

/*
 LRM clause 4.2.1.11.
 The class sca_lsf::sca_source shall implement a primitive module for the
 LSF MoC that realizes a source for an LSF signal. In time-domain simulation,
 the primitive shall contribute the following equation to the equation system:
 y(t) = init_value                                              t<delay
 y(t) = offset + amplitude*sin(2pi * frequency*(t-delay)+phase) t>=delay
 where t is the time, delay is the initial delay in second, init_value
 is the initial value, offset is the offset, amplitude is the source amplitude,
 frequency is the source frequency in hertz, phase is the source phase
 in radian, pi is the pi constant, and y(t) is the LSF output signal.
 Source parameters shall be set to zero by default.
 In small-signal frequency-domain simulation, the primitive shall contribute
 the following equation to the equation system:
 y(f) = ac_amplitude * (cos(ac_phase) + j*sin(ac_phase))
 where f is the simulation frequency, ac_amplitude is the small-signal
 amplitude, and ac_phase is the small-signal phase in radian.
 In small-signal frequency-domain noise simulation, the primitive shall
 contribute the following equation to the equation system:
 y(f) = ac_noise_amplitude
 where f is the simulation frequency, and ac_noise_amplitude is the
 small-signal noise amplitude.
 */

/*****************************************************************************/

#ifndef SCA_LSF_SOURCE_H_
#define SCA_LSF_SOURCE_H_

namespace sca_lsf
{

//class sca_source : public implementation-derived-from sca_core::sca_module
class sca_source: public sca_lsf::sca_module
{
public:
	sca_lsf::sca_out y; // LSF output

	sca_core::sca_parameter<double> init_value;
	sca_core::sca_parameter<double> offset;
	sca_core::sca_parameter<double> amplitude;
	sca_core::sca_parameter<double> frequency;
	sca_core::sca_parameter<double> phase;
	sca_core::sca_parameter<sca_core::sca_time> delay;
	sca_core::sca_parameter<double> ac_amplitude;
	sca_core::sca_parameter<double> ac_phase;
	sca_core::sca_parameter<double> ac_noise_amplitude;

	virtual const char* kind() const;

	explicit sca_source(sc_core::sc_module_name, double init_value_ = 0.0,
			double offset_ = 0.0, double amplitude_ = 0.0, double frequency_ =
					0.0, double phase_ = 0.0,
					const sca_core::sca_time& delay_ = sc_core::SC_ZERO_TIME,
			double ac_amplitude_ = 0.0, double ac_phase_ = 0.0,
			double ac_noise_amplitude_ = 0.0);

	//begin implementation specific

private:
		 virtual void matrix_stamps();
		 double value_t();

	//end implementation specific

};

} // namespace sca_lsf


#endif /* SCA_LSF_SOURCE_H_ */
