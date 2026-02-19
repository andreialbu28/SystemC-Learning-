/*****************************************************************************

    Copyright 2010-2012
    Fraunhofer-Gesellschaft zur Foerderung der angewandten Forschung e.V.

    Copyright 2015-2016
    COSEDA Technologies GmbH

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

 sca_tdf_ss.h - linear state space system

 Original Author: Karsten Einwich Fraunhofer / COSEDA Technologies

 Created on: 04.03.2009

 SVN Version       :  $Revision: 1908 $
 SVN last checkin  :  $Date: 2016-02-15 17:59:13 +0000 (Mon, 15 Feb 2016) $ (UTC)
 SVN checkin by    :  $Author: karsten $
 SVN Id            :  $Id: sca_tdf_ss.h 1908 2016-02-15 17:59:13Z karsten $

 *****************************************************************************/
/*
 LRM clause 4.1.4.5.
 The class sca_tdf::sca_ss shall implement a system whose behavior is defined
 by the following state-space equations:
 ds(t)/dt = A*s(t) + B*x(t)
 y(t)     = C*s(t) + D*x(t)
 where s(t) is the state vector, x(t) is the input vector, and y(t) is the
 output vector. A, B, C, and D are matrices having the following
 characteristics:
 * A is a n-by-n matrix, where n is the number of states.
 * B is a n-by-m matrix, where m is the number of inputs.
 * C and D are r-by-n matrices,  where r is the number of outputs.
 */

/*****************************************************************************/

#ifndef SCA_TDF_SS_H_
#define SCA_TDF_SS_H_

namespace sca_tdf
{

//begin implementation specific
namespace sca_implementation
{
class sca_ct_vector_ss_proxy;
}

//end implementation specific

//class sca_ss : public implementation-derived-from sc_core::sc_object
class sca_ss: public ::sc_core::sc_object
{
public:
	sca_ss();
	explicit sca_ss(const char*);

	virtual const char* kind() const;

	void set_max_delay(const sca_core::sca_time& );
	void set_max_delay(double , sc_core::sc_time_unit );

	sca_util::sca_vector<double>  estimate_next_value() const;

	void enable_iterations();


	sca_tdf::sca_ct_vector_proxy& calculate(
			const sca_util::sca_matrix<double>& a, // matrix A
			const sca_util::sca_matrix<double>& b, // matrix B
			const sca_util::sca_matrix<double>& c, // matrix C
			const sca_util::sca_matrix<double>& d, // matrix D
			sca_util::sca_vector<double>& s, // state vector s(t)
			const sca_util::sca_vector<double>& x, // input vector x(t)
			const sca_core::sca_time& tstep = sca_core::sca_max_time());

	sca_tdf::sca_ct_vector_proxy& calculate(
			const sca_util::sca_matrix<double>& a, // matrix A
			const sca_util::sca_matrix<double>& b, // matrix B
			const sca_util::sca_matrix<double>& c, // matrix C
			const sca_util::sca_matrix<double>& d, // matrix D
			const sca_core::sca_time& delay,
			sca_util::sca_vector<double>& s, // state vector s(t)
			const sca_util::sca_vector<double>& x, // input vector x(t)
			const sca_core::sca_time& tstep = sca_core::sca_max_time());

	sca_tdf::sca_ct_vector_proxy& calculate(
			const sca_util::sca_matrix<double>& a,
			const sca_util::sca_matrix<double>& b,
			const sca_util::sca_matrix<double>& c,
			const sca_util::sca_matrix<double>& d,
			sca_util::sca_vector<double>& s,
			const sca_util::sca_matrix<double>& x,
			const sca_core::sca_time& tstep =sca_core::sca_max_time());

	sca_tdf::sca_ct_vector_proxy& calculate(
			const sca_util::sca_matrix<double>& a,
			const sca_util::sca_matrix<double>& b,
			const sca_util::sca_matrix<double>& c,
			const sca_util::sca_matrix<double>& d,
			const sca_core::sca_time& delay,
			sca_util::sca_vector<double>& s,
			const sca_util::sca_matrix<double>& x,
			const sca_core::sca_time& tstep = sca_core::sca_max_time());

	sca_tdf::sca_ct_vector_proxy& calculate(
			const sca_util::sca_matrix<double>& a,
			const sca_util::sca_matrix<double>& b,
			const sca_util::sca_matrix<double>& c,
			const sca_util::sca_matrix<double>& d,
			sca_util::sca_vector<double>& s,
			const sca_tdf::sca_in<sca_util::sca_vector<double> >& x);

	sca_tdf::sca_ct_vector_proxy& calculate(
			const sca_util::sca_matrix<double>& a,
			const sca_util::sca_matrix<double>& b,
			const sca_util::sca_matrix<double>& c,
			const sca_util::sca_matrix<double>& d,
			const sca_core::sca_time& delay,
			sca_util::sca_vector<double>& s,
			const sca_tdf::sca_in<sca_util::sca_vector<double> >& x);

	sca_tdf::sca_ct_vector_proxy& calculate(
			const sca_util::sca_matrix<double>& a,
			const sca_util::sca_matrix<double>& b,
			const sca_util::sca_matrix<double>& c,
			const sca_util::sca_matrix<double>& d,
			sca_util::sca_vector<double>& s,
			const sca_tdf::sca_de::sca_in<sca_util::sca_vector<double> >& x);

	sca_tdf::sca_ct_vector_proxy& calculate(
			const sca_util::sca_matrix<double>& a,
			const sca_util::sca_matrix<double>& b,
			const sca_util::sca_matrix<double>& c,
			const sca_util::sca_matrix<double>& d,
			const sca_core::sca_time& delay,
			sca_util::sca_vector<double>& s,
			const sca_tdf::sca_de::sca_in<sca_util::sca_vector<double> >& x);

	sca_tdf::sca_ct_vector_proxy& calculate(
			const sca_util::sca_matrix<double>& a,
			const sca_util::sca_matrix<double>& b,
			const sca_util::sca_matrix<double>& c,
			const sca_util::sca_matrix<double>& d,
			const sca_util::sca_vector<double>& x,
			const sca_core::sca_time& tstep = sca_core::sca_max_time());

	sca_tdf::sca_ct_vector_proxy& calculate(
			const sca_util::sca_matrix<double>& a,
			const sca_util::sca_matrix<double>& b,
			const sca_util::sca_matrix<double>& c,
			const sca_util::sca_matrix<double>& d,
			const sca_core::sca_time& delay,
			const sca_util::sca_vector<double>& x,
			const sca_core::sca_time& tstep = sca_core::sca_max_time());

	sca_tdf::sca_ct_vector_proxy& calculate(
			const sca_util::sca_matrix<double>& a,
			const sca_util::sca_matrix<double>& b,
			const sca_util::sca_matrix<double>& c,
			const sca_util::sca_matrix<double>& d,
			const sca_util::sca_matrix<double>& x,
			const sca_core::sca_time& tstep =sca_core::sca_max_time());

	sca_tdf::sca_ct_vector_proxy& calculate(
			const sca_util::sca_matrix<double>& a,
			const sca_util::sca_matrix<double>& b,
			const sca_util::sca_matrix<double>& c,
			const sca_util::sca_matrix<double>& d,
			const sca_core::sca_time& delay,
			const sca_util::sca_matrix<double>& x,
			const sca_core::sca_time& tstep = sca_core::sca_max_time());

	sca_tdf::sca_ct_vector_proxy& calculate(
			const sca_util::sca_matrix<double>& a,
			const sca_util::sca_matrix<double>& b,
			const sca_util::sca_matrix<double>& c,
			const sca_util::sca_matrix<double>& d,
			const sca_tdf::sca_in<sca_util::sca_vector<double> >& x);

	sca_tdf::sca_ct_vector_proxy& calculate(
			const sca_util::sca_matrix<double>& a,
			const sca_util::sca_matrix<double>& b,
			const sca_util::sca_matrix<double>& c,
			const sca_util::sca_matrix<double>& d,
			const sca_core::sca_time& delay,
			const sca_tdf::sca_in<sca_util::sca_vector<double> >& x);

	sca_tdf::sca_ct_vector_proxy& calculate(
			const sca_util::sca_matrix<double>& a,
			const sca_util::sca_matrix<double>& b,
			const sca_util::sca_matrix<double>& c,
			const sca_util::sca_matrix<double>& d,
			const sca_tdf::sca_de::sca_in<sca_util::sca_vector<double> >& x);

	sca_tdf::sca_ct_vector_proxy& calculate(
			const sca_util::sca_matrix<double>& a,
			const sca_util::sca_matrix<double>& b,
			const sca_util::sca_matrix<double>& c,
			const sca_util::sca_matrix<double>& d,
			const sca_core::sca_time& delay,
			const sca_tdf::sca_de::sca_in<sca_util::sca_vector<double> >& x);




	sca_tdf::sca_ct_vector_proxy& operator()(
			const sca_util::sca_matrix<double>& a,
			const sca_util::sca_matrix<double>& b,
			const sca_util::sca_matrix<double>& c,
			const sca_util::sca_matrix<double>& d,
			sca_util::sca_vector<double>& s,
			const sca_util::sca_vector<double>& x,
			const sca_core::sca_time& tstep = sca_core::sca_max_time());

	sca_tdf::sca_ct_vector_proxy& operator()(
			const sca_util::sca_matrix<double>& a,
			const sca_util::sca_matrix<double>& b,
			const sca_util::sca_matrix<double>& c,
			const sca_util::sca_matrix<double>& d,
			const sca_core::sca_time& delay,
			sca_util::sca_vector<double>& s,
			const sca_util::sca_vector<double>& x,
			const sca_core::sca_time& tstep = sca_core::sca_max_time());

	sca_tdf::sca_ct_vector_proxy& operator()(
			const sca_util::sca_matrix<double>& a,
			const sca_util::sca_matrix<double>& b,
			const sca_util::sca_matrix<double>& c,
			const sca_util::sca_matrix<double>& d,
			sca_util::sca_vector<double>& s,
			const sca_util::sca_matrix<double>& x,
			const sca_core::sca_time& tstep = sca_core::sca_max_time());


	sca_tdf::sca_ct_vector_proxy& operator()(
			const sca_util::sca_matrix<double>& a,
			const sca_util::sca_matrix<double>& b,
			const sca_util::sca_matrix<double>& c,
			const sca_util::sca_matrix<double>& d,
			const sca_core::sca_time& delay,
			sca_util::sca_vector<double>& s,
			const sca_util::sca_matrix<double>& x,
			const sca_core::sca_time& tstep = sca_core::sca_max_time());


	sca_tdf::sca_ct_vector_proxy& operator()(
			const sca_util::sca_matrix<double>& a,
			const sca_util::sca_matrix<double>& b,
			const sca_util::sca_matrix<double>& c,
			const sca_util::sca_matrix<double>& d,
			sca_util::sca_vector<double>& s,
			const sca_tdf::sca_in<sca_util::sca_vector<double> >& x);

	sca_tdf::sca_ct_vector_proxy& operator()(
			const sca_util::sca_matrix<double>& a,
			const sca_util::sca_matrix<double>& b,
			const sca_util::sca_matrix<double>& c,
			const sca_util::sca_matrix<double>& d,
			const sca_core::sca_time& delay,
			sca_util::sca_vector<double>& s,
			const sca_tdf::sca_in<sca_util::sca_vector<double> >& x);


	sca_tdf::sca_ct_vector_proxy& operator()(
			const sca_util::sca_matrix<double>& a,
			const sca_util::sca_matrix<double>& b,
			const sca_util::sca_matrix<double>& c,
			const sca_util::sca_matrix<double>& d,
			sca_util::sca_vector<double>& s,
			const sca_tdf::sca_de::sca_in<sca_util::sca_vector<double> >& x);


	sca_tdf::sca_ct_vector_proxy& operator()(
			const sca_util::sca_matrix<double>& a,
			const sca_util::sca_matrix<double>& b,
			const sca_util::sca_matrix<double>& c,
			const sca_util::sca_matrix<double>& d,
			const sca_core::sca_time& delay,
			sca_util::sca_vector<double>& s,
			const sca_tdf::sca_de::sca_in<sca_util::sca_vector<double> >& x);


	sca_tdf::sca_ct_vector_proxy& operator()(
			const sca_util::sca_matrix<double>& a,
			const sca_util::sca_matrix<double>& b,
			const sca_util::sca_matrix<double>& c,
			const sca_util::sca_matrix<double>& d,
			const sca_util::sca_vector<double>& x,
			const sca_core::sca_time& tstep = sca_core::sca_max_time());


	sca_tdf::sca_ct_vector_proxy& operator()(
			const sca_util::sca_matrix<double>& a,
			const sca_util::sca_matrix<double>& b,
			const sca_util::sca_matrix<double>& c,
			const sca_util::sca_matrix<double>& d,
			const sca_core::sca_time& delay,
			const sca_util::sca_vector<double>& x,
			const sca_core::sca_time& tstep = sca_core::sca_max_time());


	sca_tdf::sca_ct_vector_proxy& operator()(
			const sca_util::sca_matrix<double>& a,
			const sca_util::sca_matrix<double>& b,
			const sca_util::sca_matrix<double>& c,
			const sca_util::sca_matrix<double>& d,
			const sca_util::sca_matrix<double>& x,
			const sca_core::sca_time& tstep = sca_core::sca_max_time());


	sca_tdf::sca_ct_vector_proxy& operator()(
			const sca_util::sca_matrix<double>& a,
			const sca_util::sca_matrix<double>& b,
			const sca_util::sca_matrix<double>& c,
			const sca_util::sca_matrix<double>& d,
			const sca_core::sca_time& delay,
			const sca_util::sca_matrix<double>& x,
			const sca_core::sca_time& tstep = sca_core::sca_max_time());


	sca_tdf::sca_ct_vector_proxy& operator()(
			const sca_util::sca_matrix<double>& a,
			const sca_util::sca_matrix<double>& b,
			const sca_util::sca_matrix<double>& c,
			const sca_util::sca_matrix<double>& d,
			const sca_tdf::sca_in<sca_util::sca_vector<double> >& x);

	sca_tdf::sca_ct_vector_proxy& operator()(
			const sca_util::sca_matrix<double>& a,
			const sca_util::sca_matrix<double>& b,
			const sca_util::sca_matrix<double>& c,
			const sca_util::sca_matrix<double>& d,
			const sca_core::sca_time& delay,
			const sca_tdf::sca_in<sca_util::sca_vector<double> >& x);



	sca_tdf::sca_ct_vector_proxy& operator()(
			const sca_util::sca_matrix<double>& a,
			const sca_util::sca_matrix<double>& b,
			const sca_util::sca_matrix<double>& c,
			const sca_util::sca_matrix<double>& d,
			const sca_tdf::sca_de::sca_in<sca_util::sca_vector<double> >& x);


	sca_tdf::sca_ct_vector_proxy& operator()(
			const sca_util::sca_matrix<double>& a,
			const sca_util::sca_matrix<double>& b,
			const sca_util::sca_matrix<double>& c,
			const sca_util::sca_matrix<double>& d,
			const sca_core::sca_time& delay,
			const sca_tdf::sca_de::sca_in<sca_util::sca_vector<double> >& x);

	//begin implementation specific

	private:

		sca_tdf::sca_implementation::sca_ct_vector_ss_proxy* proxy;

	//end implementation specific



};

} // namespace sca_tdf


#endif /* SCA_TDF_SS_H_ */
