/*****************************************************************************

    Copyright 2010-2013
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

 sca_tdf_ltf_zp.h - linear transfer function in zero/pole form

 Original Author: Karsten Einwich Fraunhofer IIS/EAS Dresden

 Created on: 04.03.2009

 SVN Version       :  $Revision: 1908 $
 SVN last checkin  :  $Date: 2016-02-15 17:59:13 +0000 (Mon, 15 Feb 2016) $ (UTC)
 SVN checkin by    :  $Author: karsten $
 SVN Id            :  $Id: sca_tdf_ltf_zp.h 1908 2016-02-15 17:59:13Z karsten $

 *****************************************************************************/
/*
 LRM clause 4.1.4.3.
 The class sca_tdf::sca_ltf_zp shall implement a scaled continuous-time
 linear transfer function of the Laplace-domain variable s in
 the zero-pole form:
 H(s) = k* ( (s-zeros[0]) * (s-zeros[1]) * ... * (s-zeros[M-1]) ) /
           ( (s-poles[0]) * (s-poles[1]) * ... * (s-poles[N-1]) )
 where k is the constant gain of the transfer function, M and N are the number
 of zeros and poles, respectively, and zeros[i] and poles[i] are complex-valued
 zeros and poles, respectively. If M or N is zero, the corresponding numerator
 or denominator term shall be a constant 1.
 */

/*****************************************************************************/

#ifndef SCA_TDF_LTF_ZP_H_
#define SCA_TDF_LTF_ZP_H_

namespace sca_tdf
{


//begin implementation specific
namespace sca_implementation
{
class sca_ct_ltf_zp_proxy;
}

//end implementation specific

//class sca_ltf_zp : public implementation-derived-from sc_core::sc_object
class sca_ltf_zp: public ::sc_core::sc_object
{
public:
	sca_ltf_zp();
	explicit sca_ltf_zp(const char*);

	virtual const char* kind() const;

	void set_max_delay(const sca_core::sca_time& );
	void set_max_delay(double , sc_core::sc_time_unit );

	double estimate_next_value() const;

	void enable_iterations();


	sca_tdf::sca_ct_proxy& calculate(
			const sca_util::sca_vector<sca_util::sca_complex>& zeros,
			const sca_util::sca_vector<sca_util::sca_complex>& poles,
			sca_util::sca_vector<double>& state,
			double input,
			double k = 1.0,
			const sca_core::sca_time& tstep =sca_core::sca_max_time());

	sca_tdf::sca_ct_proxy& calculate(
			const sca_util::sca_vector<sca_util::sca_complex>& zeros,
			const sca_util::sca_vector<sca_util::sca_complex>& poles,
			const sca_core::sca_time& delay,
			sca_util::sca_vector<double>& state,
			double input,
			double k = 1.0,
			const sca_core::sca_time& tstep = sca_core::sca_max_time());

	sca_tdf::sca_ct_proxy& calculate(
			const sca_util::sca_vector<sca_util::sca_complex>& zeros,
			const sca_util::sca_vector<sca_util::sca_complex>& poles,
			sca_util::sca_vector<double>& state,
			const sca_util::sca_vector<double>& input,
			double k = 1.0,
			const sca_core::sca_time& tstep = sca_core::sca_max_time());

	sca_tdf::sca_ct_proxy& calculate(
			const sca_util::sca_vector<sca_util::sca_complex>& zeros,
			const sca_util::sca_vector<sca_util::sca_complex>& poles,
			const sca_core::sca_time& delay,
			sca_util::sca_vector<double>& state,
			const sca_util::sca_vector<double>& input,
			double k = 1.0,
			const sca_core::sca_time& tstep = sca_core::sca_max_time());


	sca_tdf::sca_ct_proxy& calculate(
			const sca_util::sca_vector<sca_util::sca_complex>& zeros,
			const sca_util::sca_vector<sca_util::sca_complex>& poles,
			sca_util::sca_vector<double>& state,
			const sca_tdf::sca_in<double>& input,
			double k = 1.0);

	sca_tdf::sca_ct_proxy& calculate(
			const sca_util::sca_vector<sca_util::sca_complex>& zeros,
			const sca_util::sca_vector<sca_util::sca_complex>& poles,
			const sca_core::sca_time& delay,
			sca_util::sca_vector<double>& state,
			const sca_tdf::sca_in<double>& input,
			double k = 1.0);


	sca_tdf::sca_ct_proxy& calculate(
			const sca_util::sca_vector<sca_util::sca_complex>& zeros,
			const sca_util::sca_vector<sca_util::sca_complex>& poles,
			sca_util::sca_vector<double>& state,
			const sca_tdf::sca_de::sca_in<double>& input,
			double k = 1.0);

	sca_tdf::sca_ct_proxy& calculate(
			const sca_util::sca_vector<sca_util::sca_complex>& zeros,
			const sca_util::sca_vector<sca_util::sca_complex>& poles,
			const sca_core::sca_time& delay,
			sca_util::sca_vector<double>& state,
			const sca_tdf::sca_de::sca_in<double>& input,
			double k = 1.0);


	sca_tdf::sca_ct_proxy& calculate(
			const sca_util::sca_vector<sca_util::sca_complex>& zeros,
			const sca_util::sca_vector<sca_util::sca_complex>& poles,
			double input,
			double k = 1.0,
			const sca_core::sca_time& tstep = sca_core::sca_max_time());

	sca_tdf::sca_ct_proxy& calculate(
			const sca_util::sca_vector<sca_util::sca_complex>& zeros,
			const sca_util::sca_vector<sca_util::sca_complex>& poles,
			const sca_core::sca_time& delay,
			double input,
			double k = 1.0,
			const sca_core::sca_time& tstep = sca_core::sca_max_time());


	sca_tdf::sca_ct_proxy& calculate(
			const sca_util::sca_vector<sca_util::sca_complex>& zeros,
			const sca_util::sca_vector<sca_util::sca_complex>& poles,
			const sca_util::sca_vector<double>& input,
			double k = 1.0,
			const sca_core::sca_time& tstep = sca_core::sca_max_time());

	sca_tdf::sca_ct_proxy& calculate(
			const sca_util::sca_vector<sca_util::sca_complex>& zeros,
			const sca_util::sca_vector<sca_util::sca_complex>& poles,
			const sca_core::sca_time& delay,
			const sca_util::sca_vector<double>& input,
			double k = 1.0,
			const sca_core::sca_time& tstep = sca_core::sca_max_time());


	sca_tdf::sca_ct_proxy& calculate(
			const sca_util::sca_vector<sca_util::sca_complex>& zeros,
			const sca_util::sca_vector<sca_util::sca_complex>& poles,
			const sca_tdf::sca_in<double>& input,
			double k = 1.0);

	sca_tdf::sca_ct_proxy& calculate(
			const sca_util::sca_vector<sca_util::sca_complex>& zeros,
			const sca_util::sca_vector<sca_util::sca_complex>& poles,
			const sca_core::sca_time& delay,
			const sca_tdf::sca_in<double>& input,
			double k = 1.0);


	sca_tdf::sca_ct_proxy& calculate(
			const sca_util::sca_vector<sca_util::sca_complex>& zeros,
			const sca_util::sca_vector<sca_util::sca_complex>& poles,
			const sca_tdf::sca_de::sca_in<double>& input,
			double k = 1.0);

	sca_tdf::sca_ct_proxy& calculate(
			const sca_util::sca_vector<sca_util::sca_complex>& zeros,
			const sca_util::sca_vector<sca_util::sca_complex>& poles,
			const sca_core::sca_time& delay,
			const sca_tdf::sca_de::sca_in<double>& input,
			double k = 1.0);


	sca_tdf::sca_ct_proxy& operator()(
			const sca_util::sca_vector<sca_util::sca_complex>& zeros,
			const sca_util::sca_vector<sca_util::sca_complex>& poles,
			sca_util::sca_vector<double>& state,
			double input,
			double k = 1.0,
			const sca_core::sca_time& tstep = sca_core::sca_max_time());


	sca_tdf::sca_ct_proxy& operator()(
			const sca_util::sca_vector<sca_util::sca_complex>& zeros,
			const sca_util::sca_vector<sca_util::sca_complex>& poles,
			const sca_core::sca_time& delay,
			sca_util::sca_vector<double>& state,
			double input,
			double k = 1.0,
			const sca_core::sca_time& tstep = sca_core::sca_max_time());


	sca_tdf::sca_ct_proxy& operator()(
			const sca_util::sca_vector<sca_util::sca_complex>& zeros,
			const sca_util::sca_vector<sca_util::sca_complex>& poles,
			sca_util::sca_vector<double>& state,
			const sca_util::sca_vector<double>& input,
			double k = 1.0,
			const sca_core::sca_time& tstep =sca_core::sca_max_time());


	sca_tdf::sca_ct_proxy& operator()(
			const sca_util::sca_vector<sca_util::sca_complex>& zeros,
			const sca_util::sca_vector<sca_util::sca_complex>& poles,
			const sca_core::sca_time& ct_delay,
			sca_util::sca_vector<double>& state,
			const sca_util::sca_vector<double>& input,
			double k = 1.0, const sca_core::sca_time& tstep =sca_core::sca_max_time());


	sca_tdf::sca_ct_proxy& operator()(
			const sca_util::sca_vector<sca_util::sca_complex>& zeros,
			const sca_util::sca_vector<sca_util::sca_complex>& poles,
			sca_util::sca_vector<double>& state,
			const sca_tdf::sca_in<double>& input,
			double k = 1.0);


	sca_tdf::sca_ct_proxy& operator()(
			const sca_util::sca_vector<sca_util::sca_complex>& zeros,
			const sca_util::sca_vector<sca_util::sca_complex>& poles,
			const sca_core::sca_time& delay,
			sca_util::sca_vector<double>& state,
			const sca_tdf::sca_in<double>& input,
			double k = 1.0);


	sca_tdf::sca_ct_proxy& operator()(
			const sca_util::sca_vector<sca_util::sca_complex>& zeros,
			const sca_util::sca_vector<sca_util::sca_complex>& poles,
			sca_util::sca_vector<double>& state,
			const sca_tdf::sca_de::sca_in<double>& input,
			double k = 1.0);

	sca_tdf::sca_ct_proxy& operator()(
			const sca_util::sca_vector<sca_util::sca_complex>& zeros,
			const sca_util::sca_vector<sca_util::sca_complex>& poles,
			const sca_core::sca_time& delay,
			sca_util::sca_vector<double>& state,
			const sca_tdf::sca_de::sca_in<double>& input,
			double k = 1.0);


	sca_tdf::sca_ct_proxy& operator()(
			const sca_util::sca_vector<sca_util::sca_complex>& zeros,
			const sca_util::sca_vector<sca_util::sca_complex>& poles,
			double input,
			double k = 1.0,
			const sca_core::sca_time& tstep =sca_core::sca_max_time());

	sca_tdf::sca_ct_proxy& operator()(
			const sca_util::sca_vector<sca_util::sca_complex>& zeros,
			const sca_util::sca_vector<sca_util::sca_complex>& poles,
			const sca_core::sca_time& delay,
			double input,
			double k = 1.0,
			const sca_core::sca_time& tstep = sca_core::sca_max_time());


	sca_tdf::sca_ct_proxy& operator()(
			const sca_util::sca_vector<sca_util::sca_complex>& zeros,
			const sca_util::sca_vector<sca_util::sca_complex>& poles,
			const sca_util::sca_vector<double>& input,
			double k = 1.0,
			const sca_core::sca_time& tstep = sca_core::sca_max_time());

	sca_tdf::sca_ct_proxy& operator()(
			const sca_util::sca_vector<sca_util::sca_complex>& zeros,
			const sca_util::sca_vector<sca_util::sca_complex>& poles,
			const sca_core::sca_time& delay,
			const sca_util::sca_vector<double>& input,
			double k = 1.0,
			const sca_core::sca_time& tstep = sca_core::sca_max_time());


	sca_tdf::sca_ct_proxy& operator()(
			const sca_util::sca_vector<sca_util::sca_complex>& zeros,
			const sca_util::sca_vector<sca_util::sca_complex>& poles,
			const sca_tdf::sca_in<double>& input,
			double k = 1.0);

	sca_tdf::sca_ct_proxy& operator()(
			const sca_util::sca_vector<sca_util::sca_complex>& zeros,
			const sca_util::sca_vector<sca_util::sca_complex>& poles,
			const sca_core::sca_time& delay,
			const sca_tdf::sca_in<double>& input,
			double k = 1.0);


	sca_tdf::sca_ct_proxy& operator()(
			const sca_util::sca_vector<sca_util::sca_complex>& zeros,
			const sca_util::sca_vector<sca_util::sca_complex>& poles,
			const sca_tdf::sca_de::sca_in<double>& input,
			double k = 1.0);

	sca_tdf::sca_ct_proxy& operator()(
			const sca_util::sca_vector<sca_util::sca_complex>& zeros,
			const sca_util::sca_vector<sca_util::sca_complex>& poles,
			const sca_core::sca_time& delay,
			const sca_tdf::sca_de::sca_in<double>& input,
			double k = 1.0);


//begin implementation specific

	private:

		sca_tdf::sca_implementation::sca_ct_ltf_zp_proxy* proxy;

//end implementation specific

};

} // namespace sca_tdf


#endif /* SCA_TDF_LTF_ZP_H_ */
