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

 sca_tdf_ltf_nd.cpp - description

 Original Author: Karsten Einwich Fraunhofer IIS/EAS Dresden

 Created on: 09.10.2009

 SVN Version       :  $Revision: 1563 $
 SVN last checkin  :  $Date: 2013-05-03 15:06:50 +0000 (Fri, 03 May 2013) $
 SVN checkin by    :  $Author: karsten $
 SVN Id            :  $Id: sca_tdf_ltf_nd.cpp 1563 2013-05-03 15:06:50Z karsten $

 *****************************************************************************/

/*****************************************************************************/

#include "systemc-ams"
#include "scams/predefined_moc/tdf/sca_tdf_ltf_nd.h"
#include "scams/impl/predefined_moc/tdf/sca_tdf_ct_ltf_nd_proxy.h"

namespace sca_tdf
{

sca_ltf_nd::sca_ltf_nd(): sc_core::sc_object(sc_core::sc_gen_unique_name("sca_ltf_nd"))
{
	proxy = new sca_tdf::sca_implementation::sca_ct_ltf_nd_proxy(this);
}

sca_ltf_nd::sca_ltf_nd(const char* nm) :
	::sc_core::sc_object(nm)
{
	proxy = new sca_tdf::sca_implementation::sca_ct_ltf_nd_proxy(this);
}

const char* sca_ltf_nd::kind() const
{
	return "sca_tdf::sca_ltf_nd";
}

void sca_ltf_nd::set_max_delay(const sca_core::sca_time& ct_delay)
{
	proxy->set_max_delay(ct_delay);
}
void sca_ltf_nd::set_max_delay(double ct_delay, sc_core::sc_time_unit unit)
{
	set_max_delay(sca_core::sca_time(ct_delay,unit));
}


sca_tdf::sca_ct_proxy& sca_ltf_nd::calculate(
		const sca_util::sca_vector<double>& num, const sca_util::sca_vector<
				double>& den, sca_util::sca_vector<double>& state,
		double input, double k, const sca_core::sca_time& tstep)
{
	proxy->register_nd(num, den, sc_core::SC_ZERO_TIME, state, input, k, tstep);
	return *proxy;
}


sca_tdf::sca_ct_proxy& sca_ltf_nd::calculate(
		const sca_util::sca_vector<double>& num, const sca_util::sca_vector<
				double>& den, const sca_core::sca_time& ct_delay, sca_util::sca_vector<double>& state,
		double input, double k, const sca_core::sca_time& tstep)
{
	proxy->register_nd(num, den, ct_delay, state, input, k, tstep);
	return *proxy;
}



sca_tdf::sca_ct_proxy& sca_ltf_nd::calculate(const sca_util::sca_vector<double>& num,
		const sca_util::sca_vector<double>& den,
		sca_util::sca_vector<double>& state,
		const sca_util::sca_vector<double>& input, double k,
		const sca_core::sca_time& tstep)
{
	proxy->register_nd(num, den, sc_core::SC_ZERO_TIME, state, input, k, tstep);
	return *proxy;
}


sca_tdf::sca_ct_proxy& sca_ltf_nd::calculate(const sca_util::sca_vector<double>& num,
		const sca_util::sca_vector<double>& den, const sca_core::sca_time& ct_delay,
		sca_util::sca_vector<double>& state,
		const sca_util::sca_vector<double>& input, double k,
		const sca_core::sca_time& tstep)
{
	proxy->register_nd(num, den, ct_delay, state, input, k, tstep);
	return *proxy;
}



sca_tdf::sca_ct_proxy& sca_ltf_nd::calculate(const sca_util::sca_vector<double>& num,
		const sca_util::sca_vector<double>& den,
		sca_util::sca_vector<double>& state,
		const sca_tdf::sca_in<double>& input, double k)
{
	proxy->register_nd(num, den, sc_core::SC_ZERO_TIME, state, input, k);
	return *proxy;
}


sca_tdf::sca_ct_proxy& sca_ltf_nd::calculate(const sca_util::sca_vector<double>& num,
		const sca_util::sca_vector<double>& den,
		const sca_core::sca_time& ct_delay,
		sca_util::sca_vector<double>& state,
		const sca_tdf::sca_in<double>& input, double k)
{
	proxy->register_nd(num, den, ct_delay, state, input, k);
	return *proxy;
}




sca_tdf::sca_ct_proxy& sca_ltf_nd::calculate(const sca_util::sca_vector<double>& num,
		const sca_util::sca_vector<double>& den,
		sca_util::sca_vector<double>& state, const sca_tdf::sca_de::sca_in<
				double>& input, double k)
{
	proxy->register_nd(num, den, sc_core::SC_ZERO_TIME, state, input, k);
	return *proxy;
}


sca_tdf::sca_ct_proxy& sca_ltf_nd::calculate(const sca_util::sca_vector<double>& num,
		const sca_util::sca_vector<double>& den,
		const sca_core::sca_time& ct_delay,
		sca_util::sca_vector<double>& state, const sca_tdf::sca_de::sca_in<
				double>& input, double k)
{
	proxy->register_nd(num, den, ct_delay, state, input, k);
	return *proxy;
}


sca_tdf::sca_ct_proxy& sca_ltf_nd::calculate(const sca_util::sca_vector<double>& num,
		const sca_util::sca_vector<double>& den, double input, double k,
		const sca_core::sca_time& tstep)
{
	proxy->register_nd(num, den, sc_core::SC_ZERO_TIME, input, k, tstep);
	return *proxy;
}


sca_tdf::sca_ct_proxy& sca_ltf_nd::calculate(const sca_util::sca_vector<double>& num,
		const sca_util::sca_vector<double>& den, const sca_core::sca_time& ct_delay, double input, double k,
		const sca_core::sca_time& tstep)
{
	proxy->register_nd(num, den, ct_delay, input, k, tstep);
	return *proxy;
}


sca_tdf::sca_ct_proxy& sca_ltf_nd::calculate(const sca_util::sca_vector<double>& num,
		const sca_util::sca_vector<double>& den, const sca_util::sca_vector<
				double>& input, double k, const sca_core::sca_time& tstep)
{
	proxy->register_nd(num, den, sc_core::SC_ZERO_TIME, input, k, tstep);
	return *proxy;
}


sca_tdf::sca_ct_proxy& sca_ltf_nd::calculate(const sca_util::sca_vector<double>& num,
		const sca_util::sca_vector<double>& den, const sca_core::sca_time& ct_delay, const sca_util::sca_vector<
				double>& input, double k, const sca_core::sca_time& tstep)
{
	proxy->register_nd(num, den, ct_delay, input, k, tstep);
	return *proxy;
}



sca_tdf::sca_ct_proxy& sca_ltf_nd::calculate(const sca_util::sca_vector<double>& num,
		const sca_util::sca_vector<double>& den,
		const sca_tdf::sca_in<double>& input, double k)
{
	proxy->register_nd(num, den, sc_core::SC_ZERO_TIME, input, k);
	return *proxy;
}


sca_tdf::sca_ct_proxy& sca_ltf_nd::calculate(const sca_util::sca_vector<double>& num,
		const sca_util::sca_vector<double>& den, const sca_core::sca_time& ct_delay,
		const sca_tdf::sca_in<double>& input, double k)
{
	proxy->register_nd(num, den, ct_delay, input, k);
	return *proxy;
}



sca_tdf::sca_ct_proxy& sca_ltf_nd::calculate(const sca_util::sca_vector<double>& num,
		const sca_util::sca_vector<double>& den,
		const sca_tdf::sca_de::sca_in<double>& input, double k)
{
	proxy->register_nd(num, den, sc_core::SC_ZERO_TIME, input, k);
	return *proxy;
}


sca_tdf::sca_ct_proxy& sca_ltf_nd::calculate(const sca_util::sca_vector<double>& num,
		const sca_util::sca_vector<double>& den, const sca_core::sca_time& ct_delay,
		const sca_tdf::sca_de::sca_in<double>& input, double k)
{
	proxy->register_nd(num, den, ct_delay, input, k);
	return *proxy;
}



////////////////////////////////////////////////////////////////


sca_tdf::sca_ct_proxy& sca_ltf_nd::operator()(const sca_util::sca_vector<double>& num,
		const sca_util::sca_vector<double>& den,
		sca_util::sca_vector<double>& state, double input, double k ,
		const sca_core::sca_time& tstep)
{
	return calculate(num, den, state, input, k, tstep);
}


sca_tdf::sca_ct_proxy& sca_ltf_nd::operator()(const sca_util::sca_vector<double>& num,
		const sca_util::sca_vector<double>& den, const sca_core::sca_time& ct_delay,
		sca_util::sca_vector<double>& state, double input, double k ,
		const sca_core::sca_time& tstep)
{
	return calculate(num, den, ct_delay,state, input, k, tstep);
}


sca_tdf::sca_ct_proxy& sca_ltf_nd::operator()(const sca_util::sca_vector<double>& num,
		const sca_util::sca_vector<double>& den,
		sca_util::sca_vector<double>& state,
		const sca_util::sca_vector<double>& input, double k ,
		const sca_core::sca_time& tstep)
{
	return calculate(num, den, state, input, k, tstep);
}


sca_tdf::sca_ct_proxy& sca_ltf_nd::operator()(const sca_util::sca_vector<double>& num,
		const sca_util::sca_vector<double>& den, const sca_core::sca_time& ct_delay,
		sca_util::sca_vector<double>& state,
		const sca_util::sca_vector<double>& input, double k ,
		const sca_core::sca_time& tstep)
{
	return calculate(num, den, ct_delay, state, input, k, tstep);
}




sca_tdf::sca_ct_proxy& sca_ltf_nd::operator()(const sca_util::sca_vector<double>& num,
		const sca_util::sca_vector<double>& den,
		sca_util::sca_vector<double>& state,
		const sca_tdf::sca_in<double>& input, double k )
{
	return calculate(num, den, state, input, k);
}


sca_tdf::sca_ct_proxy& sca_ltf_nd::operator()(const sca_util::sca_vector<double>& num,
		const sca_util::sca_vector<double>& den, const sca_core::sca_time& ct_delay,
		sca_util::sca_vector<double>& state,
		const sca_tdf::sca_in<double>& input, double k )
{
	return calculate(num, den, ct_delay, state, input, k);
}


sca_tdf::sca_ct_proxy& sca_ltf_nd::operator()(const sca_util::sca_vector<double>& num,
		const sca_util::sca_vector<double>& den,
		sca_util::sca_vector<double>& state, const sca_tdf::sca_de::sca_in<
				double>& input, double k )
{
	return calculate(num, den, state, input, k);
}


sca_tdf::sca_ct_proxy& sca_ltf_nd::operator()(const sca_util::sca_vector<double>& num,
		const sca_util::sca_vector<double>& den, const sca_core::sca_time& ct_delay,
		sca_util::sca_vector<double>& state, const sca_tdf::sca_de::sca_in<
				double>& input, double k )
{
	return calculate(num, den, ct_delay, state, input, k);
}

sca_tdf::sca_ct_proxy& sca_ltf_nd::operator()(const sca_util::sca_vector<double>& num,
		const sca_util::sca_vector<double>& den, double input, double k ,
		const sca_core::sca_time& tstep )
{
	return calculate(num, den, input, k, tstep);
}


sca_tdf::sca_ct_proxy& sca_ltf_nd::operator()(const sca_util::sca_vector<double>& num,
		const sca_util::sca_vector<double>& den, const sca_core::sca_time& ct_delay, double input, double k ,
		const sca_core::sca_time& tstep )
{
	return calculate(num, den, ct_delay, input, k, tstep);
}

sca_tdf::sca_ct_proxy& sca_ltf_nd::operator()(const sca_util::sca_vector<double>& num,
		const sca_util::sca_vector<double>& den, const sca_util::sca_vector<
				double>& input, double k, const sca_core::sca_time& tstep )
{
	return calculate(num, den, input, k, tstep);
}


sca_tdf::sca_ct_proxy& sca_ltf_nd::operator()(const sca_util::sca_vector<double>& num,
		const sca_util::sca_vector<double>& den, const sca_core::sca_time& ct_delay, const sca_util::sca_vector<
				double>& input, double k, const sca_core::sca_time& tstep )
{
	return calculate(num, den, ct_delay, input, k, tstep);
}


sca_tdf::sca_ct_proxy& sca_ltf_nd::operator()(const sca_util::sca_vector<double>& num,
		const sca_util::sca_vector<double>& den,
		const sca_tdf::sca_in<double>& input, double k)
{
	return calculate(num, den, input, k);
}


sca_tdf::sca_ct_proxy& sca_ltf_nd::operator()(const sca_util::sca_vector<double>& num,
		const sca_util::sca_vector<double>& den, const sca_core::sca_time& ct_delay,
		const sca_tdf::sca_in<double>& input, double k)
{
	return calculate(num, den, ct_delay, input, k);
}



sca_tdf::sca_ct_proxy& sca_ltf_nd::operator()(const sca_util::sca_vector<double>& num,
		const sca_util::sca_vector<double>& den,
		const sca_tdf::sca_de::sca_in<double>& input, double k )
{
	return calculate(num, den, input, k);
}

sca_tdf::sca_ct_proxy& sca_ltf_nd::operator()(const sca_util::sca_vector<double>& num,
		const sca_util::sca_vector<double>& den, const sca_core::sca_time& ct_delay,
		const sca_tdf::sca_de::sca_in<double>& input, double k )
{
	return calculate(num, den, ct_delay, input, k);
}

double sca_ltf_nd::estimate_next_value() const
{
	return proxy->estimate_next_value();
}

void sca_ltf_nd::enable_iterations()
{
	proxy->enable_iterations();
}

}
