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

 sca_lsf_delay.h - linear signal flow delay

 Original Author: Karsten Einwich Fraunhofer IIS/EAS Dresden

 Created on: 05.03.2009

 SVN Version       :  $Revision: 1305 $
 SVN last checkin  :  $Date: 2012-04-11 15:13:49 +0000 (Wed, 11 Apr 2012) $
 SVN checkin by    :  $Author: karsten $
 SVN Id            :  $Id: sca_lsf_delay.h 1305 2012-04-11 15:13:49Z karsten $

 *****************************************************************************/

/*
 LRM clause 4.2.1.10.
 The class sca_lsf::sca_delay shall implement a primitive module for the
 LSF MoC that generates a scaled time-delayed version of an LSF signal.
 The primitive shall contribute the following equation to the equation system:
 y(t) = y0            t<=delay
 y(t) = k*x(t-delay)  t>delay
 where t is the time, delay is the time delay in second, k is the constant
 scale coefficient, x(t) is the LSF input signal, y0 is the output value before
 the delay is in effect, and y(t) is the LSF output signal.
 */

/*****************************************************************************/

#ifndef SCA_LSF_DELAY_H_
#define SCA_LSF_DELAY_H_

namespace sca_lsf
{

//class sca_delay : public implementation-derived-from sca_core::sca_module
class sca_delay: public sca_lsf::sca_module
{
public:
	sca_lsf::sca_in x; // LSF input

	sca_lsf::sca_out y; // LSF output

	sca_core::sca_parameter<sca_core::sca_time> delay; // time delay
	sca_core::sca_parameter<double> k; // scale coefficient
	sca_core::sca_parameter<double> y0; // output value before delay is in effect

	virtual const char* kind() const;

	explicit sca_delay(sc_core::sc_module_name,
			const sca_core::sca_time& delay_=sc_core::SC_ZERO_TIME,
			double k_ = 1.0, double y0_ = 0.0);


	// begin implementation specific

private:
		 virtual void matrix_stamps();

		 double*       delay_buffer;
		 unsigned long delay_buffer_size;
		 unsigned long delay_cnt;
		 unsigned long nsample;
		 double        last_sample;
		 bool          initialized;
		 double        scale_last_sample;

		 double delay_sec, dt_sec, dt_delay;

		 void pre_solve();
		 void post_solve();
		 double q_t();
		 void init_delay();

	// end implementation specific

};

} // namespace sca_lsf


#endif /* SCA_LSF_DELAY_H_ */
