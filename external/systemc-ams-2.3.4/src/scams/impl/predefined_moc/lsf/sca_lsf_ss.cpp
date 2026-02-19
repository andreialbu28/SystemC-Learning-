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

  sca_lsf_ss.cpp - description

  Original Author: Karsten Einwich Fraunhofer / COSEDA Technologies

  Created on: 08.01.2010

   SVN Version       :  $Revision: 1909 $
   SVN last checkin  :  $Date: 2016-02-16 10:09:52 +0000 (Tue, 16 Feb 2016) $
   SVN checkin by    :  $Author: karsten $
   SVN Id            :  $Id: sca_lsf_ss.cpp 1909 2016-02-16 10:09:52Z karsten $

 *****************************************************************************/

/*****************************************************************************/

#include "systemc-ams"
#include "scams/predefined_moc/lsf/sca_lsf_ss.h"
#include "scams/impl/util/data_types/sca_method_object.h"
#include "scams/impl/util/data_types/sca_method_list.h"
#include "scams/impl/core/sca_solver_base.h"

namespace sca_lsf
{
sca_ss::sca_ss(sc_core::sc_module_name,
		const sca_util::sca_matrix<double>& a_,
		const sca_util::sca_matrix<double>& b_,
		const sca_util::sca_matrix<double>& c_,
		const sca_util::sca_matrix<double>& d_,
		const sca_core::sca_time& delay_):
		x("x"),
		y("y"),
		a("a", a_),
		b("b", b_),
		c("c", c_),
		d("d", d_),
		delay("delay",delay_)
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

const char* sca_ss::kind() const
{
	return "sca_lsf::sca_ss";
}

void sca_ss::matrix_stamps()
{
	if(a.get().n_cols()!=a.get().n_rows())
	{
		std::ostringstream str;
		str << "Matrix A must be square (dimension number of states) in sca_lsf::sca_ss : "
		    << name();
		SC_REPORT_ERROR("SystemC-AMS",str.str().c_str());
	}

	if(b.get().n_rows()!=a.get().n_rows())
	{
		std::ostringstream str;
		str << "Number of rows of matrix B must be equal to number of states "
		       "(dimension of matrix A) in sca_lsf::sca_ss : " << name();
		SC_REPORT_ERROR("SystemC-AMS",str.str().c_str());
	}

	if(b.get().n_cols()!=1)
	{
		std::ostringstream str;
		str << "Number of columns of matrix B must be 1 (number of inputs)"
		       "in sca_lsf::sca_ss : " << name();
		SC_REPORT_ERROR("SystemC-AMS",str.str().c_str());
	}

	if(c.get().n_cols()!=a.get().n_rows())
	{
		std::ostringstream str;
		str << "Number of columns of matrix C (number of states) must be equal"
		       " to dimension of matrix A in sca_lsf::sca_ss : " << name();
		SC_REPORT_ERROR("SystemC-AMS",str.str().c_str());
	}

	if(c.get().n_rows()!=1)
	{
		std::ostringstream str;
		str << "Number of rows of matrix C (outputs) must be 1"
		       " in sca_lsf::sca_ss : " << name();
		SC_REPORT_ERROR("SystemC-AMS",str.str().c_str());
	}

	if(d.get().n_rows()!=1)
	{
		std::ostringstream str;
		str << "Number of rows of matrix D (inputs) must be 1"
		       " in sca_lsf::sca_ss : " << name();
		SC_REPORT_ERROR("SystemC-AMS",str.str().c_str());
	}


	if(d.get().n_cols()!=1)
	{
		std::ostringstream str;
		str << "Number of columns of matrix D (outputs) must be 1"
		       " in sca_lsf::sca_ss : " << name();
		SC_REPORT_ERROR("SystemC-AMS",str.str().c_str());
	}

	add_eq.resize(a.get().n_cols());

	for(unsigned long i=0;i<add_eq.size();i++) // add equations
	{
		add_eq[i]=add_equation();
	}


	for(unsigned long i=0;i<add_eq.size();i++)
	{
		long nadd=add_eq[i];

		if(!dc_init || sca_ac_analysis::sca_ac_is_running())
		{
			A(nadd,nadd) = -1.0;   // ds= ....

			for(unsigned long j=0;j<add_eq.size();j++)
			{
				B(nadd,add_eq[j])=a.get()(i,j); // ds(i)= a(i,0)*s(0) +... a(i,n)*s(n)
			}

			B(nadd,x) = b.get()(i,0);    // ds(i) = a(i,0)*s(0)+... +a(i,n)*s(n)  +  b(i,0)*x

		}
		else
		{
			// s = 0
			B(nadd,nadd) = 1.0;

			add_method(POST_SOLVE, SCA_VMPTR(sca_ss::dc_step_finish));
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


    for(unsigned long j=0;j<add_eq.size();j++)
    {
    	B(nltf_out,add_eq[j]) = c.get()(0,j);  // y= c(0,0)*s+ ... c(0,n)*s
    }

    B(nltf_out,x) = d.get()(0,0);  // y= c(0,0)*s+ ... c(0,n)*s + d(0,0)*x


	if (delay.get() != sc_core::SC_ZERO_TIME)
	{

		if (!sca_ac_analysis::sca_ac_is_running())
		{
			//initialization must be done after setup - otherwise timstep not available
			if (!initialized)
			{
				add_method(PRE_SOLVE, SCA_VMPTR(sca_ss::pre_solve));
			}

			init_delay();
			q(y).add_element(SCA_MPTR(sca_ss::q_t), this);
			add_method(POST_SOLVE, SCA_VMPTR(sca_ss::post_solve));

		}
		else
		{
			double w = sca_ac_analysis::sca_ac_w();
			delay_sec = delay.get().to_seconds();
			//e**(-jw*delay) = cos(-w*delay) + j*sin(-w*delay)
			A(y,nadd_delay) = 1.0 / w * sin(-w * delay_sec); // j*sin(w*delay)
			B(y,nadd_delay) = 1.0 * cos(-w * delay_sec); //   cos(w*delay)
		}
	}

}


void sca_ss::dc_step_finish()
{
	for(unsigned long i=0;i<add_eq.size();i++)
	{
		long nadd=add_eq[i];

		B(nadd,nadd) = 0.0;

		A(nadd,nadd) = -1.0;   // ds= ....

		for(unsigned long j=0;j<add_eq.size();j++)
		{
			B(nadd,add_eq[j])=a.get()(i,j); // ds(i)= a(i,0)*s(0) +... a(i,n)*s(n)
		}

		B(nadd,x) = b.get()(i,0);    // ds(i) = a(i,0)*s(0)+... +a(i,n)*s(n)  +  b(i,0)*x

	}

	remove_method(POST_SOLVE, SCA_VMPTR(sca_ss::dc_step_finish));

	request_reinit_and_decomposition_A();

	dc_init=false;
}



void sca_ss::init_delay()
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

void sca_ss::pre_solve()
{
	if (!initialized)
	{
		initialized = true;
		last_sample = 0.0;
		request_reinit();
	}
}

double sca_ss::q_t()
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

void sca_ss::post_solve()
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
