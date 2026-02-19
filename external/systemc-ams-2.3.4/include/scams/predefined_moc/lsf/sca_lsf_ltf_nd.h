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

 sca_lsf_ltf_nd.h - linear signal flow transfer function in numerator/denumerator form

 Original Author: Karsten Einwich Fraunhofer IIS/EAS Dresden

 Created on: 05.03.2009

 SVN Version       :  $Revision: 1305 $
 SVN last checkin  :  $Date: 2012-04-11 15:13:49 +0000 (Wed, 11 Apr 2012) $
 SVN checkin by    :  $Author: karsten $
 SVN Id            :  $Id: sca_lsf_ltf_nd.h 1305 2012-04-11 15:13:49Z karsten $

 *****************************************************************************/
/*
 *   LRM clause 4.2.1.12.
 *   The class sca_lsf::sca_ltf_nd shall implement a primitive module for the
 *   LSF MoC that realizes a scaled Laplace transfer function in the time-domain
 *   in the numerator-denominator form. The primitive shall contribute
 *   the following equation to the equation system:
 *      den[N-1]*d**(N-1) * y(t) + ... den[0]*y(t) =
 *   k* num[M-1]*d**(M-1) * x(t) + ... num[0]*x(t)
 *   where k is the constant gain coefficient, M and N are the number of
 *   numerator and denominator coefficients, respectively, indexed with i, x(t)
 *   is the LSF input signal, num[i] and den[i] are real-valued coefficients of
 *   the numerator and denominator, respectively, and y(t)is the LSF
 *   output signal.
 */

/*****************************************************************************/

#ifndef SCA_LSF_LTF_ND_H_
#define SCA_LSF_LTF_ND_H_

namespace sca_lsf
{

//class sca_ltf_nd : public implementation-derived-from sca_core::sca_module
class sca_ltf_nd: public sca_lsf::sca_module
{
public:
	sca_lsf::sca_in x; // LSF input

	sca_lsf::sca_out y; // LSF output

	sca_core::sca_parameter<sca_util::sca_vector<double> > num; // numerator coefficients
	sca_core::sca_parameter<sca_util::sca_vector<double> > den; // denumerator coefficients
	sca_core::sca_parameter<sca_core::sca_time> delay;

	sca_core::sca_parameter<double> k; // gain coefficient

	virtual const char* kind() const;

	explicit sca_ltf_nd(sc_core::sc_module_name,
			const sca_util::sca_vector<double>& num_ =
					sca_util::sca_create_vector(1.0),
			const sca_util::sca_vector<double>& den_ =
					sca_util::sca_create_vector(1.0), double k_ = 1.0);

	sca_ltf_nd(sc_core::sc_module_name,
			const sca_util::sca_vector<double>& num_,
			const sca_util::sca_vector<double>& den_,
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

	// end implementation specific

};

} // namespace sca_lsf

#endif /* SCA_LSF_LTF_ND_H_ */
