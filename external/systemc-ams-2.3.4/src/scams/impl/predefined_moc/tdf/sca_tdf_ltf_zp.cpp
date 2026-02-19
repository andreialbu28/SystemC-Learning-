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

  sca_tdf_ltf_zp.cpp - description

  Original Author: Karsten Einwich Fraunhofer IIS/EAS Dresden

  Created on: 16.12.2009

   SVN Version       :  $Revision: 1573 $
   SVN last checkin  :  $Date: 2013-05-09 16:36:22 +0000 (Thu, 09 May 2013) $
   SVN checkin by    :  $Author: karsten $
   SVN Id            :  $Id: sca_tdf_ltf_zp.cpp 1573 2013-05-09 16:36:22Z karsten $

 *****************************************************************************/

/*****************************************************************************/


#include "systemc-ams"
#include "scams/predefined_moc/tdf/sca_tdf_ltf_zp.h"
#include "scams/impl/predefined_moc/tdf/sca_tdf_ct_ltf_nd_proxy.h"
#include "scams/impl/predefined_moc/tdf/sca_tdf_ct_ltf_zp_proxy.h"

namespace sca_tdf
{

sca_ltf_zp::sca_ltf_zp(): sc_core::sc_object(sc_core::sc_gen_unique_name("sca_ltf_zp"))
{
	proxy = new sca_tdf::sca_implementation::sca_ct_ltf_zp_proxy(this);
}

sca_ltf_zp::sca_ltf_zp(const char* nm) :
	::sc_core::sc_object(nm)
{
	proxy = new sca_tdf::sca_implementation::sca_ct_ltf_zp_proxy(this);
}

const char* sca_ltf_zp::kind() const
{
	return "sca_tdf::sca_ltf_zp";
}

void sca_ltf_zp::set_max_delay(const sca_core::sca_time& ct_delay)
{
	proxy->set_max_delay(ct_delay);
}


void sca_ltf_zp::set_max_delay(double ct_delay, sc_core::sc_time_unit unit)
{
	set_max_delay(sca_core::sca_time(ct_delay,unit));
}

sca_tdf::sca_ct_proxy& sca_ltf_zp::calculate(const sca_util::sca_vector<
		sca_util::sca_complex>& zeros, const sca_util::sca_vector<
		sca_util::sca_complex>& poles, sca_util::sca_vector<double>& state,
		double input, double k, const sca_core::sca_time& tstep)
{
	proxy->register_zp(zeros, poles, sc_core::SC_ZERO_TIME,state, input, k, tstep);
	return *proxy;
}


sca_tdf::sca_ct_proxy& sca_ltf_zp::calculate(
		const sca_util::sca_vector<sca_util::sca_complex>& zeros,
		const sca_util::sca_vector<sca_util::sca_complex>& poles,
		const sca_core::sca_time& ct_delay,
		sca_util::sca_vector<double>& state,
		double input,
		double k,
		const sca_core::sca_time& tstep)
{
	proxy->register_zp(zeros, poles, ct_delay,state, input, k, tstep);
	return *proxy;
}


///////

sca_tdf::sca_ct_proxy& sca_ltf_zp::calculate(
		const sca_util::sca_vector<sca_util::sca_complex>& zeros,
		const sca_util::sca_vector<sca_util::sca_complex>& poles,
		sca_util::sca_vector<double>& state,
		const sca_util::sca_vector<double>& input, double k,
		const sca_core::sca_time& tstep)
{
	proxy->register_zp(zeros, poles, sc_core::SC_ZERO_TIME, state, input, k, tstep);
	return *proxy;
}


sca_tdf::sca_ct_proxy& sca_ltf_zp::calculate(
		const sca_util::sca_vector<sca_util::sca_complex>& zeros,
		const sca_util::sca_vector<sca_util::sca_complex>& poles,
		const sca_core::sca_time& ct_delay,
		sca_util::sca_vector<double>& state,
		const sca_util::sca_vector<double>& input, double k,
		const sca_core::sca_time& tstep)
{
	proxy->register_zp(zeros, poles, ct_delay, state, input, k, tstep);
	return *proxy;
}


///////////////

sca_tdf::sca_ct_proxy& sca_ltf_zp::calculate(
		const sca_util::sca_vector<sca_util::sca_complex>& zeros,
		const sca_util::sca_vector<sca_util::sca_complex>& poles,
		sca_util::sca_vector<double>& state,
		const sca_tdf::sca_in<double>& input,
		double k)
{
	proxy->register_zp(zeros, poles, sc_core::SC_ZERO_TIME, state, input, k);
	return *proxy;
}


sca_tdf::sca_ct_proxy& sca_ltf_zp::calculate(
		const sca_util::sca_vector<sca_util::sca_complex>& zeros,
		const sca_util::sca_vector<sca_util::sca_complex>& poles,
		const sca_core::sca_time& ct_delay,
		sca_util::sca_vector<double>& state,
		const sca_tdf::sca_in<double>& input,
		double k)
{
	proxy->register_zp(zeros, poles, ct_delay, state, input, k);
	return *proxy;
}

////////////////


sca_tdf::sca_ct_proxy& sca_ltf_zp::calculate(
		const sca_util::sca_vector<sca_util::sca_complex>& zeros,
		const sca_util::sca_vector<sca_util::sca_complex>& poles,
		sca_util::sca_vector<double>& state,
		const sca_tdf::sca_de::sca_in<double>& input,
		double k)
{
	proxy->register_zp(zeros, poles, sc_core::SC_ZERO_TIME, state, input, k);
	return *proxy;
}


sca_tdf::sca_ct_proxy& sca_ltf_zp::calculate(
		const sca_util::sca_vector<sca_util::sca_complex>& zeros,
		const sca_util::sca_vector<sca_util::sca_complex>& poles,
		const sca_core::sca_time& ct_delay,
		sca_util::sca_vector<double>& state,
		const sca_tdf::sca_de::sca_in<double>& input,
		double k)
{
	proxy->register_zp(zeros, poles, ct_delay, state, input, k);
	return *proxy;
}


////////////

sca_tdf::sca_ct_proxy& sca_ltf_zp::calculate(
		const sca_util::sca_vector<sca_util::sca_complex>& zeros,
		const sca_util::sca_vector<sca_util::sca_complex>& poles,
		double input,
		double k,
		const sca_core::sca_time& tstep)
{
	proxy->register_zp(zeros, poles, sc_core::SC_ZERO_TIME, input, k, tstep);
	return *proxy;
}


sca_tdf::sca_ct_proxy& sca_ltf_zp::calculate(
		const sca_util::sca_vector<sca_util::sca_complex>& zeros,
		const sca_util::sca_vector<sca_util::sca_complex>& poles,
		const sca_core::sca_time& ct_delay,
		double input,
		double k,
		const sca_core::sca_time& tstep)
{
	proxy->register_zp(zeros, poles, ct_delay, input, k, tstep);
	return *proxy;
}


///////////////

sca_tdf::sca_ct_proxy& sca_ltf_zp::calculate(
		const sca_util::sca_vector<sca_util::sca_complex>& zeros,
		const sca_util::sca_vector<sca_util::sca_complex>& poles,
		const sca_util::sca_vector<double>& input,
		double k,
		const sca_core::sca_time& tstep)
{
	proxy->register_zp(zeros, poles, sc_core::SC_ZERO_TIME, input, k, tstep);
	return *proxy;
}


sca_tdf::sca_ct_proxy& sca_ltf_zp::calculate(
		const sca_util::sca_vector<sca_util::sca_complex>& zeros,
		const sca_util::sca_vector<sca_util::sca_complex>& poles,
		const sca_core::sca_time& ct_delay,
		const sca_util::sca_vector<double>& input,
		double k,
		const sca_core::sca_time& tstep)
{
	proxy->register_zp(zeros, poles, ct_delay, input, k, tstep);
	return *proxy;
}



/////////////////

sca_tdf::sca_ct_proxy& sca_ltf_zp::calculate(
		const sca_util::sca_vector<sca_util::sca_complex>& zeros,
		const sca_util::sca_vector<sca_util::sca_complex>& poles,
		const sca_tdf::sca_in<double>& input,
		double k)
{
	proxy->register_zp(zeros, poles, sc_core::SC_ZERO_TIME, input, k);
	return *proxy;
}


sca_tdf::sca_ct_proxy& sca_ltf_zp::calculate(
		const sca_util::sca_vector<sca_util::sca_complex>& zeros,
		const sca_util::sca_vector<sca_util::sca_complex>& poles,
		const sca_core::sca_time& ct_delay,
		const sca_tdf::sca_in<double>& input,
		double k)
{
	proxy->register_zp(zeros, poles, ct_delay, input, k);
	return *proxy;
}


/////////////////


sca_tdf::sca_ct_proxy& sca_ltf_zp::calculate(
		const sca_util::sca_vector<sca_util::sca_complex>& zeros,
		const sca_util::sca_vector<sca_util::sca_complex>& poles,
		const sca_tdf::sca_de::sca_in<double>& input,
		double k)
{
	proxy->register_zp(zeros, poles, sc_core::SC_ZERO_TIME, input, k);
	return *proxy;
}


sca_tdf::sca_ct_proxy& sca_ltf_zp::calculate(
		const sca_util::sca_vector<sca_util::sca_complex>& zeros,
		const sca_util::sca_vector<sca_util::sca_complex>& poles,
		const sca_core::sca_time& ct_delay,
		const sca_tdf::sca_de::sca_in<double>& input,
		double k)
{
	proxy->register_zp(zeros, poles, ct_delay, input, k);
	return *proxy;
}



////////////////////////////////////////////////////////////////


sca_tdf::sca_ct_proxy& sca_ltf_zp::operator()(const sca_util::sca_vector<sca_util::sca_complex>& zeros,
		const sca_util::sca_vector<sca_util::sca_complex>& poles,
		sca_util::sca_vector<double>& state, double input, double k,
		const sca_core::sca_time& tstep)
{
	return calculate(zeros, poles, state, input, k, tstep);
}


sca_tdf::sca_ct_proxy& sca_ltf_zp::operator()(
		const sca_util::sca_vector<sca_util::sca_complex>& zeros,
		const sca_util::sca_vector<sca_util::sca_complex>& poles,
		const sca_core::sca_time& ct_delay,
		sca_util::sca_vector<double>& state, double input, double k,
		const sca_core::sca_time& tstep)
{
	return calculate(zeros, poles, ct_delay, state, input, k, tstep);
}



sca_tdf::sca_ct_proxy& sca_ltf_zp::operator()(const sca_util::sca_vector<sca_util::sca_complex>& zeros,
		const sca_util::sca_vector<sca_util::sca_complex>& poles,
		sca_util::sca_vector<double>& state, const sca_util::sca_vector<
				double>& input, double k, const sca_core::sca_time& tstep )
{
	return calculate(zeros, poles, state, input, k, tstep);
}


sca_tdf::sca_ct_proxy& sca_ltf_zp::operator()(
		const sca_util::sca_vector<sca_util::sca_complex>& zeros,
		const sca_util::sca_vector<sca_util::sca_complex>& poles,
		const sca_core::sca_time& ct_delay,
		sca_util::sca_vector<double>& state, const sca_util::sca_vector<
				double>& input, double k, const sca_core::sca_time& tstep )
{
	return calculate(zeros, poles, ct_delay, state, input, k, tstep);
}


sca_tdf::sca_ct_proxy& sca_ltf_zp::operator()(const sca_util::sca_vector<sca_util::sca_complex>& zeros,
		const sca_util::sca_vector<sca_util::sca_complex>& poles,
		sca_util::sca_vector<double>& state,
		const sca_tdf::sca_in<double>& input, double k)
{
	return calculate(zeros, poles, state, input, k);
}


sca_tdf::sca_ct_proxy& sca_ltf_zp::operator()(
		const sca_util::sca_vector<sca_util::sca_complex>& zeros,
		const sca_util::sca_vector<sca_util::sca_complex>& poles,
		const sca_core::sca_time& ct_delay,
		sca_util::sca_vector<double>& state,
		const sca_tdf::sca_in<double>& input, double k)
{
	return calculate(zeros, poles,ct_delay, state, input, k);
}



sca_tdf::sca_ct_proxy& sca_ltf_zp::operator()(const sca_util::sca_vector<sca_util::sca_complex>& zeros,
		const sca_util::sca_vector<sca_util::sca_complex>& poles,
		sca_util::sca_vector<double>& state,
		const sca_tdf::sca_de::sca_in<double>& input, double k)
{
	return calculate(zeros, poles, state, input, k);
}


sca_tdf::sca_ct_proxy& sca_ltf_zp::operator()(
		const sca_util::sca_vector<sca_util::sca_complex>& zeros,
		const sca_util::sca_vector<sca_util::sca_complex>& poles,
		const sca_core::sca_time& ct_delay,
		sca_util::sca_vector<double>& state,
		const sca_tdf::sca_de::sca_in<double>& input, double k)
{
	return calculate(zeros, poles, ct_delay, state, input, k);
}




sca_tdf::sca_ct_proxy& sca_ltf_zp::operator()(const sca_util::sca_vector<sca_util::sca_complex>& zeros,
		const sca_util::sca_vector<sca_util::sca_complex>& poles,
		double input, double k, const sca_core::sca_time& tstep)
{
	return calculate(zeros, poles, input, k, tstep);
}


sca_tdf::sca_ct_proxy& sca_ltf_zp::operator()(
		const sca_util::sca_vector<sca_util::sca_complex>& zeros,
		const sca_util::sca_vector<sca_util::sca_complex>& poles,
		const sca_core::sca_time& ct_delay,
		double input, double k, const sca_core::sca_time& tstep)
{
	return calculate(zeros, poles, ct_delay, input, k, tstep);
}




sca_tdf::sca_ct_proxy& sca_ltf_zp::operator()(const sca_util::sca_vector<sca_util::sca_complex>& zeros,
		const sca_util::sca_vector<sca_util::sca_complex>& poles,
		const sca_util::sca_vector<double>& input, double k,
		const sca_core::sca_time& tstep)
{
	return calculate(zeros, poles, input, k, tstep);
}


sca_tdf::sca_ct_proxy& sca_ltf_zp::operator()(
		const sca_util::sca_vector<sca_util::sca_complex>& zeros,
		const sca_util::sca_vector<sca_util::sca_complex>& poles,
		const sca_core::sca_time& ct_delay,
		const sca_util::sca_vector<double>& input, double k,
		const sca_core::sca_time& tstep)
{
	return calculate(zeros, poles, ct_delay, input, k, tstep);
}


sca_tdf::sca_ct_proxy& sca_ltf_zp::operator()(const sca_util::sca_vector<sca_util::sca_complex>& zeros,
		const sca_util::sca_vector<sca_util::sca_complex>& poles,
		const sca_tdf::sca_in<double>& input, double k)
{
	return calculate(zeros, poles, input, k);
}


sca_tdf::sca_ct_proxy& sca_ltf_zp::operator()(
		const sca_util::sca_vector<sca_util::sca_complex>& zeros,
		const sca_util::sca_vector<sca_util::sca_complex>& poles,
		const sca_core::sca_time& ct_delay,
		const sca_tdf::sca_in<double>& input, double k)
{
	return calculate(zeros, poles, ct_delay, input, k);
}



sca_tdf::sca_ct_proxy& sca_ltf_zp::operator()(const sca_util::sca_vector<sca_util::sca_complex>& zeros,
		const sca_util::sca_vector<sca_util::sca_complex>& poles,
		const sca_tdf::sca_de::sca_in<double>& input, double k)
{
	return calculate(zeros, poles, input, k);
}


sca_tdf::sca_ct_proxy& sca_ltf_zp::operator()(
		const sca_util::sca_vector<sca_util::sca_complex>& zeros,
		const sca_util::sca_vector<sca_util::sca_complex>& poles,
		const sca_core::sca_time& ct_delay,
		const sca_tdf::sca_de::sca_in<double>& input, double k)
{
	return calculate(zeros, poles, ct_delay, input, k);
}

double sca_ltf_zp::estimate_next_value() const
{
	return proxy->estimate_next_value();
}

void sca_ltf_zp::enable_iterations()
{
	proxy->enable_iterations();
}


}

