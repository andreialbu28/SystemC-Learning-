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

 sca_lsf_ltf_nd.cpp - description

 Original Author: Karsten Einwich Fraunhofer / COSEDA Technologies

 Created on: 09.01.2010

 SVN Version       :  $Revision: 1930 $
 SVN last checkin  :  $Date: 2016-02-26 16:16:13 +0000 (Fri, 26 Feb 2016) $
 SVN checkin by    :  $Author: karsten $
 SVN Id            :  $Id: sca_lsf_ltf_nd.cpp 1930 2016-02-26 16:16:13Z karsten $

 *****************************************************************************/

/*****************************************************************************/

#include "systemc-ams"
#include "scams/predefined_moc/lsf/sca_lsf_ltf_nd.h"
#include "scams/impl/util/data_types/sca_method_object.h"
#include "scams/impl/util/data_types/sca_method_list.h"
#include "scams/impl/core/sca_solver_base.h"

namespace sca_lsf
{

sca_ltf_nd::sca_ltf_nd(sc_core::sc_module_name, const sca_util::sca_vector<
		double>& num_, const sca_util::sca_vector<double>& den_, double k_) :
	x("x"),
	y("y"),
	num("num", num_),
	den("den", den_),
	delay("delay",sc_core::SC_ZERO_TIME),
	k("k", k_)
{
	dc_init=true;

	n_add=-1;
	nadd_delay=-1;
	scale_last_sample=0.0;
	delay_buffer=NULL;
	delay_cnt=0;
	last_sample=0.0;
	nsample=0;
	dt_sec=0.0;
	delay_sec=0.0;
	initialized=false;
	delay_buffer_size=0;
	dt_delay=0.0;
}

sca_ltf_nd::sca_ltf_nd(sc_core::sc_module_name, const sca_util::sca_vector<
		double>& num_, const sca_util::sca_vector<double>& den_,
		const sca_core::sca_time& delay_, double k_) :
	x("x"),
	y("y"),
	num("num", num_),
	den("den", den_),
	delay("delay", delay_),
	k("k", k_)
{
	dc_init=true;

	n_add=-1;
	nadd_delay=-1;
	scale_last_sample=0.0;
	delay_buffer=NULL;
	delay_cnt=0;
	last_sample=0.0;
	nsample=0;
	dt_sec=0.0;
	delay_sec=0.0;
	initialized=false;
	delay_buffer_size=0;
	dt_delay=0.0;
}

const char* sca_ltf_nd::kind() const
{
	return "sca_lsf::sca_ltf_nd";
}

void sca_ltf_nd::matrix_stamps()
{
	const sca_util::sca_vector<double>& num_ltf(num.get());
	const sca_util::sca_vector<double>& den_ltf(den.get());

	unsigned long num_size = num_ltf.length();
	unsigned long den_size = den_ltf.length();

	if (num_size == 0)
	{
		std::ostringstream str;
		str
				<< "The number of numerator coefficients must be greater than zero in: "
				<< name();
		SC_REPORT_ERROR("SystemC-AMS",str.str().c_str());
	}

	if (den_size == 0)
	{
		std::ostringstream str;
		str
				<< "The number of denumerator coefficients must be greater than zero in: "
				<< name();
		SC_REPORT_ERROR("SystemC-AMS",str.str().c_str());
	}

	unsigned long state_size = (den_size > num_size) ? den_size - 1 : num_size;

	unsigned long number_of_equations = den_size - 1;
	unsigned long number_of_equations2 = 0;
	if (state_size > number_of_equations)
	{
		number_of_equations2 = state_size - number_of_equations;
	}

	//add additional equations
	add_eq.resize(number_of_equations);
	for (unsigned long i = 0; i < number_of_equations; i++)
	{
		add_eq[i] = add_equation();
	}

	add_eq2.resize(number_of_equations2);
	for (unsigned long i = 0; i < number_of_equations2; i++)
	{
		add_eq2[i] = add_equation();
	}

	//prepare matrices Adx+Bx+q(t)=0 for fractional part

	double q_dn = 1.0 / den_ltf(number_of_equations);

	if (number_of_equations > 0)
	{
		if(dc_init && !sca_ac_analysis::sca_ac_is_running())
		{
			B(add_eq[number_of_equations - 1],x) = 0.0;
		}
		else
		{
			B(add_eq[number_of_equations - 1],x) = -q_dn * k.get();
		}

		A(add_eq[0], add_eq[0]) = 1.0;
		B(add_eq[number_of_equations - 1], add_eq[0]) = q_dn * den_ltf(0);
		for (unsigned long i = 1; i < number_of_equations; ++i)
		{
			A(add_eq[i]                       , add_eq[i]) = 1.0;
			B(add_eq[i-1]                     , add_eq[i]) = -1.0;
			B(add_eq[number_of_equations - 1] , add_eq[i] ) = q_dn * den_ltf(i);
		}
	}

	//in the case num.size() > den.size() setup equation for non-fractional part
	if (number_of_equations2 > 0)
	{
		num2_ltf = num_ltf;

		for (long i = long(num_size) - 1; i >= long(number_of_equations); --i)
		{
			num2_ltf(i) *= q_dn;
			double c = num2_ltf(i);
			unsigned long start = i - number_of_equations;
			for (long j = start; j < i; ++j)
				num2_ltf(j) -= c * den_ltf(j - start);
		}

		B(add_eq2[0], add_eq2[state_size - den_size]) = 1.0;
		for (unsigned long i = 1; i < number_of_equations2; ++i)
		{
			A(add_eq2[i], add_eq2[i]) = 1.0;
			B(add_eq2[i], add_eq2[number_of_equations2 - 1]) = -1.0;
		}

		for (unsigned long i = number_of_equations, j = state_size
				- number_of_equations - 1; i < state_size; ++i, --j)
		{
			if(dc_init && !sca_ac_analysis::sca_ac_is_running())
			{
				B(add_eq2[j],x) = 0.0;
				//algebraic equation
				if(j==0) B(add_eq2[j],x) = -num2_ltf(i) * k.get();
			}
			else
			{
				B(add_eq2[j],x) = -num2_ltf(i) * k.get();
			}
		}
	}

	B(y, y) = -1.0; // y = ...

	long nltf_out = y;
	if (delay.get() != sc_core::SC_ZERO_TIME)
	{
		nadd_delay = add_equation();
		nltf_out = nadd_delay;

		B(nltf_out, nltf_out) = -1.0;
	}

	if (number_of_equations2 > 0) //solve non-fractional part
	{
		B(nltf_out,add_eq2[0]) = 1.0;
		for (unsigned long i = 0; i < number_of_equations; ++i)
		{
			B(nltf_out, add_eq[i]) = num2_ltf(i);
		}

	}
	else
	{
		for (unsigned long i = 0; i < num_size; ++i)
		{
			B(nltf_out, add_eq[i]) = num_ltf(i);
		}
	}

	if (delay.get() != sc_core::SC_ZERO_TIME)
	{

		if (!sca_ac_analysis::sca_ac_is_running())
		{
			//initialization must be done after setup - otherwise timstep not available
			if (!initialized)
			{
				add_method(PRE_SOLVE, SCA_VMPTR(sca_ltf_nd::pre_solve));
			}

			init_delay();
			q(y).add_element(SCA_MPTR(sca_ltf_nd::q_t), this);
			add_method(POST_SOLVE, SCA_VMPTR(sca_ltf_nd::post_solve));

		}
		else
		{
			double w = sca_ac_analysis::sca_ac_w();
			delay_sec = delay.get().to_seconds();
			//e**(-jw*delay) = cos(-w*delay) + j*sin(-w*delay)
			A(y, nadd_delay) = 1.0 / w * sin(-w * delay_sec); // j*sin(w*delay)
			B(y, nadd_delay) = 1.0 * cos(-w * delay_sec); //   cos(w*delay)
		}
	}

	if(dc_init && !sca_ac_analysis::sca_ac_is_running())
	{
		add_method(POST_SOLVE, SCA_VMPTR(sca_ltf_nd::dc_step_finish));
	}

}

/////////////////////////////////////

void sca_ltf_nd::dc_step_finish()
{
	const sca_util::sca_vector<double>& num_ltf(num.get());
	const sca_util::sca_vector<double>& den_ltf(den.get());

	unsigned long den_size = den_ltf.length();
	unsigned long num_size = num_ltf.length();

	unsigned long number_of_equations = den_size - 1;

	double q_dn = 1.0 / den_ltf(number_of_equations);

	B(add_eq[number_of_equations - 1],x) = -q_dn * k.get();


	///////////////////

	unsigned long state_size = (den_size > num_size) ? den_size - 1 : num_size;

	for (unsigned long i = number_of_equations, j = state_size
			- number_of_equations - 1; i < state_size; ++i, --j)
	{
		B(add_eq2[j],x) = -num2_ltf(i) * k.get();
	}

	dc_init=false;

	remove_method(POST_SOLVE, SCA_VMPTR(sca_ltf_nd::dc_step_finish));

	request_reinit();
}

void sca_ltf_nd::init_delay()
{
	nsample = (unsigned long)(delay.get().value() / get_timestep().value());

	delay_sec = delay.get().to_seconds();
	dt_sec = get_timestep().to_seconds();

	if (nsample == 0)
	{
		//linear interpolation - enough ??
		// (x-xlast)/dt * (dt-delay) + xlast =
		// x/dt*(dt-delay)  + xlast*(1-(dt-delay)/dt) = xlast*delay/dt
		B(nadd_delay, y) = (dt_sec - delay_sec) / dt_sec;
		scale_last_sample = delay_sec / dt_sec;
	}
	else
	{
		B(nadd_delay, y) = 0.0;

		if (delay_buffer != NULL)
		{
			if (delay_buffer_size == (nsample + 1))
				return;
			delete[] delay_buffer;
			delay_buffer = NULL;
			delay_buffer_size = 0;
			delay_cnt = 0;
		}

		delay_buffer = new double[nsample + 1];
		if (delay_buffer == NULL)
		{
			std::ostringstream str;
			str << "Can't allocate memory for delay buffer in: " << name();
			SC_REPORT_ERROR("SystemC-AMS",str.str().c_str());
		}

		for (unsigned long i = 0; i <= nsample; i++)
		{
			delay_buffer[i] = 0.0;
		}

		delay_buffer_size = nsample + 1;
		delay_cnt = nsample + 1;

		dt_delay = (nsample + 1) * dt_sec - delay_sec;
	}
}

void sca_ltf_nd::pre_solve()
{
	if (!initialized)
	{
		initialized = true;
		last_sample = 0.0;
		request_reinit();
	}
}

double sca_ltf_nd::q_t()
{
	if (nsample == 0)
	{
		return last_sample * scale_last_sample;
	}
	else
	{
		double xc = delay_buffer[(delay_cnt - (nsample - 1))
				% delay_buffer_size];
		double xl = delay_buffer[(delay_cnt - nsample) % delay_buffer_size];

		double out = (xc - xl) / dt_sec * dt_delay + xl;
		return out;
	}
}

void sca_ltf_nd::post_solve()
{
	if (nsample == 0)
	{
		last_sample = sca_lsf::sca_module::x(nadd_delay);
	}
	else
	{
		delay_cnt++;
		delay_buffer[delay_cnt % delay_buffer_size] = sca_lsf::sca_module::x(
				nadd_delay);
	}
}

} //namespace sca_lsf

