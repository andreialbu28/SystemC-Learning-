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

 sca_lsf_ltf_zp.h - linear signal flow transfer function in zero/pole form

 Original Author: Karsten Einwich Fraunhofer IIS/EAS Dresden

 Created on: 05.03.2009

 SVN Version       :  $Revision: 1909 $
 SVN last checkin  :  $Date: 2016-02-16 10:09:52 +0000 (Tue, 16 Feb 2016) $
 SVN checkin by    :  $Author: karsten $
 SVN Id            :  $Id: sca_lsf_ltf_zp.h 1909 2016-02-16 10:09:52Z karsten $

 *****************************************************************************/
/*
 * LRM clause 4.2.1.13.
 * The class sca_lsf::sca_ltf_zp shall implement a primitive module for the
 * LSF MoC that realizes a scaled Laplace transfer function in the time-domain
 * in the zero-pole form. The primitive shall contribute the following equation
 * to the equation system:
 *          (d/dt-poles[N-1])* ...(d/dt-poles[0]) * y(t) =
 *      k * (d/dt-zeros[M-1])* ...(d/dt-zeros[0]) * x(t)
 * where k is the constant gain coefficient, M and N are x(t) is the LSF input
 * signal, zeros[i] and poles[i] are complex-valued zeros and poles,
 * respectively, and y(t) is the LSF output signal.
 */

/*****************************************************************************/

#ifndef SCA_LSF_LTF_ZP_H_
#define SCA_LSF_LTF_ZP_H_

namespace sca_lsf
{

//class sca_ltf_zp : public implementation-derived-from sca_core::sca_module
class sca_ltf_zp: public sca_lsf::sca_module
{
public:
	sca_lsf::sca_in x; // LSF input

	sca_lsf::sca_out y; // LSF output

	sca_core::sca_parameter<sca_util::sca_vector<sca_util::sca_complex> > zeros;
	sca_core::sca_parameter<sca_util::sca_vector<sca_util::sca_complex> > poles;
	sca_core::sca_parameter<sca_core::sca_time> delay;

	sca_core::sca_parameter<double> k; // gain coefficient

	virtual const char* kind() const;

	explicit sca_ltf_zp(sc_core::sc_module_name,
			const sca_util::sca_vector<sca_util::sca_complex>& zeros_ =
					sca_util::sca_vector<sca_util::sca_complex>(),
			const sca_util::sca_vector<sca_util::sca_complex>& poles_ =
					sca_util::sca_vector<sca_util::sca_complex>(),
					double k_ = 1.0);

	sca_ltf_zp(sc_core::sc_module_name,
			const sca_util::sca_vector<sca_util::sca_complex>& zeros_,
			const sca_util::sca_vector<sca_util::sca_complex>& poles_,
			const sca_core::sca_time& delay,
			double k_ = 1.0);

// begin implementation specific

private:
	virtual void matrix_stamps();
	std::vector<long> add_eq, add_eq2;

	long nadd_delay;

	double* delay_buffer;
	unsigned long delay_buffer_size;
	unsigned long delay_cnt;
	unsigned long nsample;
	double last_sample;
	bool initialized;
	double scale_last_sample;

	double delay_sec, dt_sec, dt_delay;

	void pre_solve();
	void post_solve();
	double q_t();
	void init_delay();

	bool dc_init;
	void dc_step_finish();
	sca_util::sca_vector<double> num2_ltf;
	sca_util::sca_vector<double> num_ltf;
	sca_util::sca_vector<double> den_ltf;

// end implementation specific
};

} // namespace sca_lsf


#endif /* SCA_LSF_LTF_ZP_H_ */
