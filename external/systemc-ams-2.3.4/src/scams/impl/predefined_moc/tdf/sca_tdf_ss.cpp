/*****************************************************************************

    Copyright 2010-2013
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

  sca_tdf_ss.cpp - description

  Original Author: Karsten Einwich Fraunhofer / COSEDA Technologies

  Created on: 30.12.2009

   SVN Version       :  $Revision: 1908 $
   SVN last checkin  :  $Date: 2016-02-15 17:59:13 +0000 (Mon, 15 Feb 2016) $
   SVN checkin by    :  $Author: karsten $
   SVN Id            :  $Id: sca_tdf_ss.cpp 1908 2016-02-15 17:59:13Z karsten $

 *****************************************************************************/

/*****************************************************************************/

#include "systemc-ams"
#include "scams/predefined_moc/tdf/sca_tdf_ss.h"
#include "scams/impl/predefined_moc/tdf/sca_tdf_ct_vector_ss_proxy.h"

namespace sca_tdf
{

sca_ss::sca_ss(): sc_core::sc_object(sc_core::sc_gen_unique_name("sca_ss"))
{
	proxy = new sca_tdf::sca_implementation::sca_ct_vector_ss_proxy(this);
}

sca_ss::sca_ss(const char* nm) : sc_core::sc_object(nm)
{
	proxy = new sca_tdf::sca_implementation::sca_ct_vector_ss_proxy(this);
}

const char* sca_ss::kind() const
{
	return "sca_tdf::sca_ss";
}

void sca_ss::set_max_delay(const sca_core::sca_time& ct_delay)
{
	proxy->set_max_delay(ct_delay);
}

void sca_ss::set_max_delay(double ct_delay, sc_core::sc_time_unit unit)
{
	set_max_delay(sca_core::sca_time(ct_delay,unit));
}


sca_tdf::sca_ct_vector_proxy& sca_ss::calculate(
		    const sca_util::sca_matrix<double>& a, // matrix A
			const sca_util::sca_matrix<double>& b, // matrix B
			const sca_util::sca_matrix<double>& c, // matrix C
			const sca_util::sca_matrix<double>& d, // matrix D
			sca_util::sca_vector<double>& s, // state vector s(t)
			const sca_util::sca_vector<double>& x, // input vector x(t)
			const sca_core::sca_time& tstep)
{
	proxy->register_ss(a,b,c,d, sc_core::SC_ZERO_TIME, s, x, tstep);
	return *proxy;
}


sca_tdf::sca_ct_vector_proxy& sca_ss::calculate(
		const sca_util::sca_matrix<double>& a, // matrix A
					const sca_util::sca_matrix<double>& b, // matrix B
					const sca_util::sca_matrix<double>& c, // matrix C
					const sca_util::sca_matrix<double>& d, // matrix D
					const sca_core::sca_time& ct_delay,           //continous time delay
					sca_util::sca_vector<double>& s, // state vector s(t)
					const sca_util::sca_vector<double>& x, // input vector x(t)
					const sca_core::sca_time& tstep)
{
	proxy->register_ss(a,b,c,d, ct_delay, s, x, tstep);
	return *proxy;
}



sca_tdf::sca_ct_vector_proxy& sca_ss::calculate(
		const sca_util::sca_matrix<double>& a,
		const sca_util::sca_matrix<double>& b,
		const sca_util::sca_matrix<double>& c,
		const sca_util::sca_matrix<double>& d,
		sca_util::sca_vector<double>& s,
		const sca_util::sca_matrix<double>& x,
		const sca_core::sca_time& tstep)
{
	proxy->register_ss(a,b,c,d, sc_core::SC_ZERO_TIME, s, x, tstep);
	return *proxy;
}


sca_tdf::sca_ct_vector_proxy& sca_ss::calculate(
		const sca_util::sca_matrix<double>& a,
					const sca_util::sca_matrix<double>& b,
					const sca_util::sca_matrix<double>& c,
					const sca_util::sca_matrix<double>& d,
					const sca_core::sca_time& ct_delay,
					sca_util::sca_vector<double>& s,
					const sca_util::sca_matrix<double>& x,
					const sca_core::sca_time& tstep)
{
	proxy->register_ss(a,b,c,d, ct_delay, s, x, tstep);
	return *proxy;
}



sca_tdf::sca_ct_vector_proxy& sca_ss::calculate(
		const sca_util::sca_matrix<double>& a,
		const sca_util::sca_matrix<double>& b,
		const sca_util::sca_matrix<double>& c,
		const sca_util::sca_matrix<double>& d,
		sca_util::sca_vector<double>& s,
		const sca_tdf::sca_in<sca_util::sca_vector<double> >& x)
{
	proxy->register_ss(a,b,c,d, sc_core::SC_ZERO_TIME, s, x);
	return *proxy;
}


sca_tdf::sca_ct_vector_proxy& sca_ss::calculate(
		const sca_util::sca_matrix<double>& a,
		const sca_util::sca_matrix<double>& b,
		const sca_util::sca_matrix<double>& c,
		const sca_util::sca_matrix<double>& d,
		const sca_core::sca_time& ct_delay,
		sca_util::sca_vector<double>& s,
		const sca_tdf::sca_in<sca_util::sca_vector<double> >& x)
{
	proxy->register_ss(a,b,c,d, ct_delay, s, x);
	return *proxy;
}




sca_tdf::sca_ct_vector_proxy& sca_ss::calculate(
		const sca_util::sca_matrix<double>& a,
		const sca_util::sca_matrix<double>& b,
		const sca_util::sca_matrix<double>& c,
		const sca_util::sca_matrix<double>& d,
		sca_util::sca_vector<double>& s,
		const sca_tdf::sca_de::sca_in<sca_util::sca_vector<double> >& x)
{
	proxy->register_ss(a,b,c,d, sc_core::SC_ZERO_TIME, s, x);
	return *proxy;
}


sca_tdf::sca_ct_vector_proxy& sca_ss::calculate(
		const sca_util::sca_matrix<double>& a,
		const sca_util::sca_matrix<double>& b,
		const sca_util::sca_matrix<double>& c,
		const sca_util::sca_matrix<double>& d,
		const sca_core::sca_time& ct_delay,
		sca_util::sca_vector<double>& s,
		const sca_tdf::sca_de::sca_in<sca_util::sca_vector<double> >& x)
{
	proxy->register_ss(a,b,c,d, ct_delay, s, x);
	return *proxy;
}


sca_tdf::sca_ct_vector_proxy& sca_ss::calculate(
		const sca_util::sca_matrix<double>& a,
		const sca_util::sca_matrix<double>& b,
		const sca_util::sca_matrix<double>& c,
		const sca_util::sca_matrix<double>& d,
		const sca_util::sca_vector<double>& x,
		const sca_core::sca_time& tstep)
{
	proxy->register_ss(a,b,c,d, sc_core::SC_ZERO_TIME, x, tstep);
	return *proxy;
}


sca_tdf::sca_ct_vector_proxy& sca_ss::calculate(
		const sca_util::sca_matrix<double>& a,
		const sca_util::sca_matrix<double>& b,
		const sca_util::sca_matrix<double>& c,
		const sca_util::sca_matrix<double>& d,
		const sca_core::sca_time& ct_delay,
		const sca_util::sca_vector<double>& x,
		const sca_core::sca_time& tstep)
{
	proxy->register_ss(a,b,c,d, ct_delay, x, tstep);
	return *proxy;
}


sca_tdf::sca_ct_vector_proxy& sca_ss::calculate(
		const sca_util::sca_matrix<double>& a,
		const sca_util::sca_matrix<double>& b,
		const sca_util::sca_matrix<double>& c,
		const sca_util::sca_matrix<double>& d,
		const sca_util::sca_matrix<double>& x,
		const sca_core::sca_time& tstep)
{
	proxy->register_ss(a,b,c,d, sc_core::SC_ZERO_TIME, x, tstep);
	return *proxy;
}


sca_tdf::sca_ct_vector_proxy& sca_ss::calculate(
		const sca_util::sca_matrix<double>& a,
		const sca_util::sca_matrix<double>& b,
		const sca_util::sca_matrix<double>& c,
		const sca_util::sca_matrix<double>& d,
		const sca_core::sca_time& ct_delay,
		const sca_util::sca_matrix<double>& x,
		const sca_core::sca_time& tstep)
{
	proxy->register_ss(a,b,c,d, ct_delay, x, tstep);
	return *proxy;
}



sca_tdf::sca_ct_vector_proxy& sca_ss::calculate(
		const sca_util::sca_matrix<double>& a,
		const sca_util::sca_matrix<double>& b,
		const sca_util::sca_matrix<double>& c,
		const sca_util::sca_matrix<double>& d,
		const sca_tdf::sca_in<sca_util::sca_vector<double> >& x)
{
	proxy->register_ss(a,b,c,d, sc_core::SC_ZERO_TIME, x);
	return *proxy;
}


sca_tdf::sca_ct_vector_proxy& sca_ss::calculate(
		const sca_util::sca_matrix<double>& a,
		const sca_util::sca_matrix<double>& b,
		const sca_util::sca_matrix<double>& c,
		const sca_util::sca_matrix<double>& d,
		const sca_core::sca_time& ct_delay,
		const sca_tdf::sca_in<sca_util::sca_vector<double> >& x)
{
	proxy->register_ss(a,b,c,d, ct_delay, x);
	return *proxy;
}



sca_tdf::sca_ct_vector_proxy& sca_ss::calculate(
		const sca_util::sca_matrix<double>& a,
		const sca_util::sca_matrix<double>& b,
		const sca_util::sca_matrix<double>& c,
		const sca_util::sca_matrix<double>& d,
		const sca_tdf::sca_de::sca_in<sca_util::sca_vector<double> >& x)
{
	proxy->register_ss(a,b,c,d, sc_core::SC_ZERO_TIME, x);
	return *proxy;
}


sca_tdf::sca_ct_vector_proxy& sca_ss::calculate(
		const sca_util::sca_matrix<double>& a,
		const sca_util::sca_matrix<double>& b,
		const sca_util::sca_matrix<double>& c,
		const sca_util::sca_matrix<double>& d,
		const sca_core::sca_time& ct_delay,
		const sca_tdf::sca_de::sca_in<sca_util::sca_vector<double> >& x)
{
	proxy->register_ss(a,b,c,d, ct_delay, x);
	return *proxy;
}



////////////////////////////////////////////////////////////////


sca_tdf::sca_ct_vector_proxy& sca_ss::operator()(const sca_util::sca_matrix<double>& a,
		const sca_util::sca_matrix<double>& b, const sca_util::sca_matrix<
				double>& c, const sca_util::sca_matrix<double>& d,
		sca_util::sca_vector<double>& s,
		const sca_util::sca_vector<double>& x, const sca_core::sca_time& tstep)
{
	return calculate(a, b, c, d, s, x, tstep);
}


sca_tdf::sca_ct_vector_proxy& sca_ss::operator()(
		const sca_util::sca_matrix<double>& a,
		const sca_util::sca_matrix<double>& b,
		const sca_util::sca_matrix<double>& c,
		const sca_util::sca_matrix<double>& d,
		const sca_core::sca_time& ct_delay,
		sca_util::sca_vector<double>& s,
		const sca_util::sca_vector<double>& x, const sca_core::sca_time& tstep)
{
	return calculate(a, b, c, d, ct_delay, s, x, tstep);
}

sca_tdf::sca_ct_vector_proxy& sca_ss::operator()(const sca_util::sca_matrix<double>& a,
		const sca_util::sca_matrix<double>& b, const sca_util::sca_matrix<
				double>& c, const sca_util::sca_matrix<double>& d,
		sca_util::sca_vector<double>& s,
		const sca_util::sca_matrix<double>& x, const sca_core::sca_time& tstep)
{
	return calculate(a, b, c, d, s, x, tstep);
}


sca_tdf::sca_ct_vector_proxy& sca_ss::operator()(
		const sca_util::sca_matrix<double>& a,
		const sca_util::sca_matrix<double>& b,
		const sca_util::sca_matrix<double>& c,
		const sca_util::sca_matrix<double>& d,
		const sca_core::sca_time& ct_delay,
		sca_util::sca_vector<double>& s,
		const sca_util::sca_matrix<double>& x, const sca_core::sca_time& tstep)
{
	return calculate(a, b, c, d, ct_delay, s, x, tstep);
}


sca_tdf::sca_ct_vector_proxy& sca_ss::operator()(
		const sca_util::sca_matrix<double>& a,
		const sca_util::sca_matrix<double>& b,
		const sca_util::sca_matrix<double>& c,
		const sca_util::sca_matrix<double>& d,
		sca_util::sca_vector<double>& s,
		const sca_tdf::sca_in<sca_util::sca_vector<double> >& x)
{
	return calculate(a, b, c, d, s, x);
}


sca_tdf::sca_ct_vector_proxy& sca_ss::operator()(
		const sca_util::sca_matrix<double>& a,
		const sca_util::sca_matrix<double>& b,
		const sca_util::sca_matrix<double>& c,
		const sca_util::sca_matrix<double>& d,
		const sca_core::sca_time& ct_delay,
		sca_util::sca_vector<double>& s,
		const sca_tdf::sca_in<sca_util::sca_vector<double> >& x)
{
	return calculate(a, b, c, d, ct_delay, s, x);
}


sca_tdf::sca_ct_vector_proxy& sca_ss::operator()(
		const sca_util::sca_matrix<double>& a,
		const sca_util::sca_matrix<double>& b,
		const sca_util::sca_matrix<double>& c,
		const sca_util::sca_matrix<double>& d,
		sca_util::sca_vector<double>& s,
		const sca_tdf::sca_de::sca_in<sca_util::sca_vector<double> >& x)
{
	return calculate(a, b, c, d, s, x);
}

sca_tdf::sca_ct_vector_proxy& sca_ss::operator()(
		const sca_util::sca_matrix<double>& a,
		const sca_util::sca_matrix<double>& b,
		const sca_util::sca_matrix<double>& c,
		const sca_util::sca_matrix<double>& d,
		const sca_core::sca_time& ct_delay,
		sca_util::sca_vector<double>& s,
		const sca_tdf::sca_de::sca_in<sca_util::sca_vector<double> >& x)
{
	return calculate(a, b, c, d, ct_delay, s, x);
}



sca_tdf::sca_ct_vector_proxy& sca_ss::operator()(const sca_util::sca_matrix<double>& a,
		const sca_util::sca_matrix<double>& b, const sca_util::sca_matrix<
				double>& c, const sca_util::sca_matrix<double>& d,
		const sca_util::sca_vector<double>& x, const sca_core::sca_time& tstep)
{
	return calculate(a, b, c, d, x, tstep);
}


sca_tdf::sca_ct_vector_proxy& sca_ss::operator()(
		const sca_util::sca_matrix<double>& a,
		const sca_util::sca_matrix<double>& b,
		const sca_util::sca_matrix<double>& c,
		const sca_util::sca_matrix<double>& d,
		const sca_core::sca_time& ct_delay,
		const sca_util::sca_vector<double>& x,
		const sca_core::sca_time& tstep)
{
	return calculate(a, b, c, d, ct_delay, x, tstep);
}




sca_tdf::sca_ct_vector_proxy& sca_ss::operator()(const sca_util::sca_matrix<double>& a,
		const sca_util::sca_matrix<double>& b, const sca_util::sca_matrix<
				double>& c, const sca_util::sca_matrix<double>& d,
		const sca_util::sca_matrix<double>& x, const sca_core::sca_time& tstep)
{
	return calculate(a, b, c, d, x, tstep);
}


sca_tdf::sca_ct_vector_proxy& sca_ss::operator()(
		const sca_util::sca_matrix<double>& a,
		const sca_util::sca_matrix<double>& b,
		const sca_util::sca_matrix<double>& c,
		const sca_util::sca_matrix<double>& d,
		const sca_core::sca_time& ct_delay,
		const sca_util::sca_matrix<double>& x,
		const sca_core::sca_time& tstep)
{
	return calculate(a, b, c, d, ct_delay, x, tstep);
}



sca_tdf::sca_ct_vector_proxy& sca_ss::operator()(const sca_util::sca_matrix<double>& a,
		const sca_util::sca_matrix<double>& b, const sca_util::sca_matrix<
				double>& c, const sca_util::sca_matrix<double>& d,
		const sca_tdf::sca_in<sca_util::sca_vector<double> >& x)
{
	return calculate(a, b, c, d, x);
}


sca_tdf::sca_ct_vector_proxy& sca_ss::operator()(
		const sca_util::sca_matrix<double>& a,
		const sca_util::sca_matrix<double>& b,
		const sca_util::sca_matrix<double>& c,
		const sca_util::sca_matrix<double>& d,
		const sca_core::sca_time& ct_delay,
		const sca_tdf::sca_in<sca_util::sca_vector<double> >& x)
{
	return calculate(a, b, c, d, ct_delay, x);
}


sca_tdf::sca_ct_vector_proxy& sca_ss::operator()(const sca_util::sca_matrix<double>& a,
		const sca_util::sca_matrix<double>& b, const sca_util::sca_matrix<
				double>& c, const sca_util::sca_matrix<double>& d,
		const sca_tdf::sca_de::sca_in<sca_util::sca_vector<double> >& x)
{
	return calculate(a, b, c, d, x);
}


sca_tdf::sca_ct_vector_proxy& sca_ss::operator()(
		const sca_util::sca_matrix<double>& a,
		const sca_util::sca_matrix<double>& b,
		const sca_util::sca_matrix<double>& c,
		const sca_util::sca_matrix<double>& d,
		const sca_core::sca_time& ct_delay,
		const sca_tdf::sca_de::sca_in<sca_util::sca_vector<double> >& x)
{
	return calculate(a, b, c, d, ct_delay, x);
}



sca_util::sca_vector<double>  sca_ss::estimate_next_value() const
{
	return proxy->estimate_next_value();
}

void sca_ss::enable_iterations()
{
	proxy->enable_iterations();
}

} //namespace sca_tdf


