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

 sca_lsf_ss.h - linear signal flow state space

 Original Author: Karsten Einwich Fraunhofer IIS/EAS Dresden

 Created on: 05.03.2009

 SVN Version       :  $Revision: 1305 $
 SVN last checkin  :  $Date: 2012-04-11 15:13:49 +0000 (Wed, 11 Apr 2012) $
 SVN checkin by    :  $Author: karsten $
 SVN Id            :  $Id: sca_lsf_ss.h 1305 2012-04-11 15:13:49Z karsten $

 *****************************************************************************/
/*
 * LRM clause 4.2.1.14.
 * The class sca_lsf::sca_ss shall implement a primitive module for the LSF MoC
 * that realizes a system whose behavior is defined by single-input
 * single-output state-space equations. The primitive shall contribute the
 * following equation to the equation system:
 *     ds(t)/dt = A*s(t) + B*x(t)
 *     y(t)     = C*s(t) + D*x(t)
 * where s(t) is the state vector, x(t) is the LSF input signal, and y(t) is the
 * LSF output signal. A is a n-by-n matrix, where n is the number of states,
 * B and C are vectors of size n, and D is a real value.
 */

/*****************************************************************************/

#ifndef SCA_LSF_SS_H_
#define SCA_LSF_SS_H_

namespace sca_lsf
{

//class sca_ss : public implementation-derived-from sca_core::sca_module
class sca_ss: public sca_lsf::sca_module
{
public:
	sca_lsf::sca_in x; // LSF input

	sca_lsf::sca_out y; // LSF output

	sca_core::sca_parameter<sca_util::sca_matrix<double> > a; // matrix A of size n-by-n
	sca_core::sca_parameter<sca_util::sca_matrix<double> > b; // matrix B with one column of size n
	sca_core::sca_parameter<sca_util::sca_matrix<double> > c; // matrix C with one row of size n
	sca_core::sca_parameter<sca_util::sca_matrix<double> > d; // matrix D of size 1

	sca_core::sca_parameter<sca_core::sca_time> delay;

	virtual const char* kind() const;

	explicit sca_ss(sc_core::sc_module_name,
			const sca_util::sca_matrix<double>& a_ = sca_util::sca_matrix<double>(),
			const sca_util::sca_matrix<double>& b_ = sca_util::sca_matrix<double>(),
		    const sca_util::sca_matrix<double>& c_ = sca_util::sca_matrix<double>(),
			const sca_util::sca_matrix<double>& d_ = sca_util::sca_matrix<double>(),
			const sca_core::sca_time& delay_= sc_core::SC_ZERO_TIME);


	// begin implementation specific

private:
	virtual void matrix_stamps();

	std::vector<long> add_eq;
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


	// end implementation specific

};

} // namespace sca_lsf

#endif /* SCA_LSF_SS_H_ */
