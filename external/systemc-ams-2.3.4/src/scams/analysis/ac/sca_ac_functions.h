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

 sca_ac_functions.h - utility functions for ac and ac-noise modeling

 Original Author: Karsten Einwich Fraunhofer IIS/EAS Dresden

 Created on: 08.03.2009

 SVN Version       :  $Revision: 2144 $
 SVN last checkin  :  $Date: 2020-06-23 05:36:36 +0000 (Tue, 23 Jun 2020) $
 SVN checkin by    :  $Author: karsten $
 SVN Id            :  $Id: sca_ac_functions.h 2144 2020-06-23 05:36:36Z karsten $

 *****************************************************************************/
/*
 * LRM clause 5.2.2.1.
 * The function sca_ac::sca_ac applied to ports of class sca_tdf::sca_in shall
 * return a const reference to a value of type sca_util::sca_complex of the
 * corresponding port.
 * The function sca_ac::sca_ac applied to ports of class sca_tdf::sca_out shall
 * return a reference to a value of type sca_util::sca_complex to allow
 * assignment of a contribution to this port.
 * It shall be an error if the functions are called outside the context of the
 * member function sca_tdf::sca_module::ac_processing or its equivalent
 * registered member function.
 *
 * LRM clause 5.2.2.2.
 * The function sca_ac::sca_ac_noise applied to port of class sca_tdf::sca_out
 * shall return a reference to a value of type sca_util::sca_complex to allow
 * assignment of a noise contribution to this port.
 * It shall be an error if the function is called outside the context of the
 * member function sca_tdf::sca_module::ac_processing or its equivalent
 * registered member function.
 *
 * LRM clause 5.2.2.3.
 * The function sca_ac::sca_ac_analysis shall return true while performing a
 * small-signal frequency-domain or a small-signal noise simulation and false
 * otherwise.
 *
 * LRM clause 5.2.2.4.
 * The function sca_ac::sca_ac_noise_is_running shall return true while performing
 * a small-signal frequency-domain noise simulation and false otherwise.
 *
 * LRM clause 5.2.2.5.
 * The function sca_ac::sca_ac_f shall return the current frequency in hertz.
 * It shall be an error if the function is called outside the context of the
 * member function sca_tdf::sca_module::ac_processing or its equivalent
 * registered member function.
 *
 * LRM clause 5.2.2.6.
 * The function sca_ac::sca_ac_w shall return the current angular frequency in
 * radian per second (rad/s).
 * It shall be an error if the function is called outside the context of the
 * member function sca_tdf::sca_module::ac_processing or its equivalent
 * registered member function.
 *
 * LRM clause 5.2.2.7.
 * The function sca_ac::sca_ac_s shall return the complex value of the Laplace
 * operator s**n = (jw)**n .
 * It shall be an error if the function is called outside the context of the
 * member function sca_tdf::sca_module::ac_processing or its equivalent
 * registered member function.
 *
 * LRM clause 5.2.2.8.
 * The functions sca_ac::sca_ac_z shall return the complex value of the
 * z operator z**n = e**(jw*n*tstep). If not specified, the argument tstep shall
 * be set to the value returned by the member function get_timestep of the
 * module of class sca_tdf::sca_module, in which the function is called.
 * It shall be an error if the function is called outside the context of the
 * member function sca_tdf::sca_module::ac_processing or its equivalent
 * registered member function.
 *
 * LRM clause 5.2.2.9.
 * The function sca_ac::sca_ac_delay shall return the complex value of the
 * continuous time delay ( e**(-jw*delay) ).
 * It shall be an error if the function is called outside the context of the
 * member function sca_tdf::sca_module::ac_processing or its equivalent
 * registered member function.
 *
 * LRM clause 5.2.2.10.
 * The function sca_ac::sca_ac_ltf_nd shall return the complex value of the
 * linear transfer function of the Laplace-domain variable s in
 * numerator-denumerator form (see clause 4.1.4.2. sca_tdf::sca_ltf_nd) with,
 * s**i = (jw)**i multiplied by the complex value input>.
 * It shall be an error if the function is called outside the context of the
 * member function sca_tdf::sca_module::ac_processing or its equivalent
 * registered member function.
 *
 * LRM clause 5.2.2.11.
 * The function sca_ac::sca_ac_ltf_zp shall return the complex value of the
 * linear transfer function of the Laplace-domain variable s in zero-pole form
 * (see 4.1.4.3. sca_tdf::sca_ltf_zp) with, multiplied by the complex value
 * input.
 * It shall be an error if the function is called outside the context of the
 * member function sca_tdf::sca_module::ac_processing or its equivalent
 * registered member function.
 *
 * LRM clause 5.2.2.12.
 * The functions sca_ac::sca_ac_ss shall return the complex vector y of the
 * state-space equation system with (d/dt)**i = (jw)**i. The function with the
 * complex vector input as argument shall multiply the complex vector y with
 * input. It shall be an error if the matrix and vector sizes are inconsistent
 * (see 4.1.4.5. sca_tdf::sca_ss"/>) .
 * It shall be an error if the function is called outside the context of the
 * member function sca_tdf::sca_module::ac_processing or its equivalent
 * registered member function.
 */

/*****************************************************************************/

#ifndef SCA_AC_FUNCTIONS_H_
#define SCA_AC_FUNCTIONS_H_

namespace sca_ac_analysis
{

template<class T>
const sca_util::sca_complex& sca_ac(const sca_tdf::sca_in<T>&);

template<class T>
const sca_util::sca_complex& sca_ac(const sca_tdf::sc_in<T>&);

template<class T>
const sca_util::sca_complex& sca_ac(const sc_core::sc_in<T>&);


template<class T>
sca_util::sca_complex& sca_ac(const sca_tdf::sc_out<T>&);

template<class T>
sca_util::sca_complex& sca_ac(const sc_core::sc_out<T>&);





template<class T>
sca_util::sca_complex& sca_ac(const sca_tdf::sca_out_base<T>&);



template<class T>
sca_util::sca_complex& sca_ac_noise(const sca_tdf::sca_out_base<T>&);

template<class T>
sca_util::sca_complex& sca_ac_noise(const sc_core::sc_out<T>&);

template<class T>
sca_util::sca_complex& sca_ac_noise(const sca_tdf::sc_out<T>&);



bool sca_ac_is_running();

bool sca_ac_noise_is_running();

double sca_ac_f();

double sca_ac_w();

sca_util::sca_complex sca_ac_s(long n = 1);


sca_util::sca_complex sca_ac_z(long n, const sca_core::sca_time& tstep);
sca_util::sca_complex sca_ac_z(long n = 1);

sca_util::sca_complex sca_ac_delay(const sca_core::sca_time& delay);


sca_util::sca_complex sca_ac_ltf_nd(
		const sca_util::sca_vector<double>& num,
		const sca_util::sca_vector<double>& den,
		const sca_util::sca_complex& input = 1.0,
		double k = 1.0);

sca_util::sca_complex sca_ac_ltf_nd(
		const sca_util::sca_vector<double>& num,
		const sca_util::sca_vector<double>& den,
		const sca_core::sca_time& delay,
		const sca_util::sca_complex& input = 1.0,
		double k = 1.0);


sca_util::sca_complex sca_ac_ltf_zp(
		const sca_util::sca_vector<sca_util::sca_complex>& zeros,
		const sca_util::sca_vector<sca_util::sca_complex>& poles,
		const sca_util::sca_complex& input = 1.0,
		double k = 1.0);

sca_util::sca_complex sca_ac_ltf_zp(
		const sca_util::sca_vector<sca_util::sca_complex>& zeros,
		const sca_util::sca_vector<sca_util::sca_complex>& poles,
		const sca_core::sca_time& delay,
		const sca_util::sca_complex& input = 1.0,
		double k = 1.0);


sca_util::sca_vector<sca_util::sca_complex> sca_ac_ss(
		const sca_util::sca_matrix<double>& a,
		const sca_util::sca_matrix<double>& b,
		const sca_util::sca_matrix<double>& c,
		const sca_util::sca_matrix<double>& d,
		const sca_util::sca_vector<sca_util::sca_complex>& input);

sca_util::sca_vector<sca_util::sca_complex> sca_ac_ss(
		const sca_util::sca_matrix<double>& a,
		const sca_util::sca_matrix<double>& b,
		const sca_util::sca_matrix<double>& c,
		const sca_util::sca_matrix<double>& d);


sca_util::sca_vector<sca_util::sca_complex> sca_ac_ss(
		const sca_util::sca_matrix<double>& a,
		const sca_util::sca_matrix<double>& b,
		const sca_util::sca_matrix<double>& c,
		const sca_util::sca_matrix<double>& d,
		const sca_core::sca_time& delay,
		const sca_util::sca_vector<sca_util::sca_complex>& input);


sca_util::sca_vector<sca_util::sca_complex> sca_ac_ss(
		const sca_util::sca_matrix<double>& a,
		const sca_util::sca_matrix<double>& b,
		const sca_util::sca_matrix<double>& c,
		const sca_util::sca_matrix<double>& d,
		const sca_core::sca_time& delay);

// begin implementation specific


namespace sca_implementation
{
sca_util::sca_complex& sca_ac(const sc_core::sc_port_base& port);
sca_util::sca_complex& sca_ac_noise(const sc_core::sc_port_base& port, std::string nm="");
}

//LRM clause 5.2.2.1.
template<class T>
inline sca_util::sca_complex& sca_ac(const sca_tdf::sca_out_base<T>& port)
{
	return sca_implementation::sca_ac(port);
}


template<class T>
inline sca_util::sca_complex& sca_ac(const sc_core::sc_out<T>& port)
{
	return sca_implementation::sca_ac(port);
}

template<class T>
inline sca_util::sca_complex& sca_ac(const sca_tdf::sc_out<T>& port)
{
	return sca_implementation::sca_ac(port);
}

//LRM clause 5.2.2.2.
template<class T>
inline sca_util::sca_complex& sca_ac_noise(const sca_tdf::sca_out_base<T>& port)
{
	return sca_implementation::sca_ac_noise(port);
}


template<class T>
inline sca_util::sca_complex& sca_ac_noise(const sc_core::sc_out<T>& port)
{
	return sca_implementation::sca_ac_noise(port);
}

template<class T>
inline sca_util::sca_complex& sca_ac_noise(const sca_tdf::sc_out<T>& port)
{
	return sca_implementation::sca_ac_noise(port);
}

template<class T>
inline const sca_util::sca_complex& sca_ac(const sca_tdf::sca_in<T>& port)
{
	return sca_implementation::sca_ac(port);
}


template<class T>
inline const sca_util::sca_complex& sca_ac(const sca_tdf::sc_in<T>& port)
{
	return sca_implementation::sca_ac(port);
}


template<class T>
inline const sca_util::sca_complex& sca_ac(const sc_core::sc_in<T>& port)
{
	return sca_implementation::sca_ac(port);
}



// end implementation specific

} //namespace sca_ac_analysis

#endif /* SCA_AC_FUNCTIONS_H_ */
