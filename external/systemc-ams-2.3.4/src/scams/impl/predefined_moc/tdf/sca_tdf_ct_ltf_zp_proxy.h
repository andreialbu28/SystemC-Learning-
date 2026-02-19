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

 sca_tdf_ct_ltf_zp_proxy.h - description

 Original Author: Karsten Einwich Fraunhofer IIS/EAS Dresden

 Created on: 15.12.2009

 SVN Version       :  $Revision: 1265 $
 SVN last checkin  :  $Date: 2011-11-19 20:43:31 +0000 (Sat, 19 Nov 2011) $
 SVN checkin by    :  $Author: karsten $
 SVN Id            :  $Id: sca_tdf_ct_ltf_zp_proxy.h 1265 2011-11-19 20:43:31Z karsten $

 *****************************************************************************/

/*****************************************************************************/

#ifndef SCA_TDF_CT_LTF_ZP_PROXY_H_
#define SCA_TDF_CT_LTF_ZP_PROXY_H_

namespace sca_tdf
{
namespace sca_implementation
{
class sca_ct_ltf_zp_proxy: public sca_tdf::sca_implementation::sca_ct_ltf_nd_proxy
{
public:

	void register_zp(const sca_util::sca_vector<sca_util::sca_complex>& zeros,
			const sca_util::sca_vector<sca_util::sca_complex>& poles,
			sca_core::sca_time ct_delay,
			sca_util::sca_vector<double>& state, double input, double k,
			sca_core::sca_time tstep);

	void register_zp(const sca_util::sca_vector<sca_util::sca_complex>& zeros,
			const sca_util::sca_vector<sca_util::sca_complex>& poles,
			sca_core::sca_time ct_delay,
			sca_util::sca_vector<double>& state, const sca_util::sca_vector<
					double>& input, double k, sca_core::sca_time tstep);

	void register_zp(const sca_util::sca_vector<sca_util::sca_complex>& zeros,
			const sca_util::sca_vector<sca_util::sca_complex>& poles,
			sca_core::sca_time ct_delay,
			sca_util::sca_vector<double>& state,
			const sca_tdf::sca_in<double>& input, double k);

	void register_zp(const sca_util::sca_vector<sca_util::sca_complex>& zeros,
			const sca_util::sca_vector<sca_util::sca_complex>& poles,
			sca_core::sca_time ct_delay,
			sca_util::sca_vector<double>& state, const sca_tdf::sca_de::sca_in<
					double>& input, double k);

	void register_zp(const sca_util::sca_vector<sca_util::sca_complex>& zeros,
			const sca_util::sca_vector<sca_util::sca_complex>& poles,
			sca_core::sca_time ct_delay,
			double input, double k, sca_core::sca_time tstep);

	void register_zp(const sca_util::sca_vector<sca_util::sca_complex>& zeros,
			const sca_util::sca_vector<sca_util::sca_complex>& poles,
			sca_core::sca_time ct_delay,
			const sca_util::sca_vector<double>& input, double k,
			sca_core::sca_time tstep);

	void register_zp(const sca_util::sca_vector<sca_util::sca_complex>& zeros,
			const sca_util::sca_vector<sca_util::sca_complex>& poles,
			sca_core::sca_time ct_delay,
			const sca_tdf::sca_in<double>& input, double k);

	void register_zp(const sca_util::sca_vector<sca_util::sca_complex>& zeros,
			const sca_util::sca_vector<sca_util::sca_complex>& poles,
			sca_core::sca_time ct_delay,
			const sca_tdf::sca_de::sca_in<double>& input, double k);

	sca_ct_ltf_zp_proxy(::sc_core::sc_object* ltf_object);

private:

	const sca_util::sca_vector<sca_util::sca_complex>* zeros_old_ref;
	const sca_util::sca_vector<sca_util::sca_complex>* poles_old_ref;

	sca_util::sca_vector<sca_util::sca_complex> zeros_old;
	sca_util::sca_vector<sca_util::sca_complex> poles_old;

	bool coeff_changed(
			const sca_util::sca_vector<sca_util::sca_complex>& zeros,
			const sca_util::sca_vector<sca_util::sca_complex>& poles);

	//overload base class function to prevent doubled check
	bool coeff_changed(
			const sca_util::sca_vector<double>& num,
			const sca_util::sca_vector<double>& den);

	void calculate_nd_coefficients();

};

} //namespace sca_implementation
} //namespace sca_tdf
#endif /* SCA_TDF_CT_LTF_ZP_PROXY_H_ */
