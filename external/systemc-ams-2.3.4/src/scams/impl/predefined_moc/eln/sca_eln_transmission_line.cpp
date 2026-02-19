/*****************************************************************************

    Copyright 2010-2012
    Fraunhofer-Gesellschaft zur Foerderung der angewandten Forschung e.V.

    Copyright 2015-2020
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

 sca_eln_transmission_line.cpp - description

 Original Author: Karsten Einwich Fraunhofer IIS/EAS Dresden

 Created on: Jan 22, 2010

 SVN Version       :  $Revision: 2149 $
 SVN last checkin  :  $Date: 2020-12-18 08:51:03 +0000 (Fri, 18 Dec 2020) $
 SVN checkin by    :  $Author: karsten $
 SVN Id            :  $Id: sca_eln_transmission_line.cpp 2149 2020-12-18 08:51:03Z karsten $

 *****************************************************************************/

/*****************************************************************************/

#include "systemc-ams"
#include "scams/predefined_moc/eln/sca_eln_transmission_line.h"
#include "scams/impl/util/data_types/sca_method_object.h"
#include "scams/impl/util/data_types/sca_method_list.h"
#include "scams/impl/core/sca_solver_base.h"

namespace sca_eln
{

sca_transmission_line::sca_transmission_line(sc_core::sc_module_name,
		double z0_, const sca_core::sca_time& delay_, double delta0_) :
	a1("a1"), b1("b1"), a2("a2"), b2("b2"), z0("z0", z0_), delay("delay",
			delay_), delta0("delta0", delta0_)
{
	delay_buffer_va1b1 = NULL;
	delay_buffer_va2b2 = NULL;
	delay_buffer_size = 0;
	delay_cnt = 0;
	nsample = 0;
	initialized = false;
	scale_last_sample = 0.0;

	dt_sec=0.0;
	delay_sec=0.0;
	dt_delay=0.0;
	k_diss=0.0;

	nadd2=-1;
	nadd1=-1;

	last_sample_va1b1=0.0;
	last_sample_va2b2=0.0;

}

const char* sca_transmission_line::kind() const
{
	return "sca_eln::sca_transmission_line";
}

////////////////

double sca_transmission_line::v_line1()
{
	if (get_time() < delay.get())
	{
		return 0.0;
	}

	if (nsample == 0)
	{
		return last_sample_va1b1 * scale_last_sample;
	}
	else
	{
		double vc = delay_buffer_va1b1[(delay_cnt - (nsample - 1))
				% delay_buffer_size];
		double vl = delay_buffer_va1b1[(delay_cnt - nsample)
				% delay_buffer_size];

		double va1b1 = (vc - vl) / dt_sec * dt_delay + vl;

		return va1b1;
	}
}

/////////////////

double sca_transmission_line::v_line2()
{
	if (get_time() < delay.get())
	{
		return 0.0;
	}

	if (nsample == 0)
	{
		return last_sample_va2b2 * scale_last_sample;
	}
	else
	{
		double vc = delay_buffer_va2b2[(delay_cnt - (nsample - 1))
				% delay_buffer_size];
		double vl = delay_buffer_va2b2[(delay_cnt - nsample)
				% delay_buffer_size];

		double va2b2 = (vc - vl) / dt_sec * dt_delay + vl;

		return va2b2;
	}
}

///////////////////

void sca_transmission_line::init_delays()
{
	nsample = (unsigned long)(delay.get().value() / get_timestep().value());

	delay_sec = delay.get().to_seconds();
	dt_sec = get_timestep().to_seconds();

	k_diss = exp(-delta0.get() * delay_sec);

	double z0_val = z0.get();

	if (nsample == 0)
	{
		//linear interpolation - enough ??
		// (x-xlast)/dt * (dt-delay) + xlast =
		// x/dt*(dt-delay)  + xlast*(1-(dt-delay)/dt) = xlast*delay/dt
		B_wr(nadd1, a2) =   k_diss * (dt_sec - delay_sec) / dt_sec; //va1b1= k*va2b2(..)
		B_wr(nadd1, b2) =  -k_diss * (dt_sec - delay_sec) / dt_sec;

		B_wr(nadd1, nadd2) = k_diss * z0_val * (dt_sec - delay_sec) / dt_sec; // + z0ia2b2


		B_wr(nadd2, a1) =  k_diss * (dt_sec - delay_sec) / dt_sec; //va2b2= k*va1b1(..)
		B_wr(nadd2, b1) = -k_diss * (dt_sec - delay_sec) / dt_sec;

		B_wr(nadd2, nadd1) = k_diss * z0_val * (dt_sec - delay_sec) / dt_sec; // + z0ia1b1


		scale_last_sample=delay_sec/dt_sec * k_diss;
	}
	else
	{

		if (delay_buffer_va1b1 != NULL)
		{
			if (delay_buffer_size == (nsample + 1))
				return;

			delete[] delay_buffer_va1b1;
			delay_buffer_va1b1 = NULL;

			delete[] delay_buffer_va2b2;
			delay_buffer_va2b2 = NULL;

			delay_buffer_size = 0;
			delay_cnt = 0;
		}

		delay_buffer_va1b1 = new double[nsample + 1];
		delay_buffer_va2b2 = new double[nsample + 1];

		if ((delay_buffer_va2b2 == NULL) || (delay_buffer_va1b1 == NULL))
		{
			std::ostringstream str;
			str << "Can't allocate memory for delay buffer in: " << name();
			SC_REPORT_ERROR("SystemC-AMS",str.str().c_str());
		}

		for (unsigned long i = 0; i <= nsample; i++)
		{
			delay_buffer_va1b1[i] = 0.0;
			delay_buffer_va2b2[i] = 0.0;
		}

		delay_buffer_size = nsample + 1;
		delay_cnt = nsample + 1;

		dt_delay = (nsample + 1) * dt_sec - delay_sec;
	}
}

///////////////////

void sca_transmission_line::pre_solve()
{
	if (!initialized)
	{
		initialized = true;
		last_sample_va1b1 = 0.0;
		last_sample_va2b2 = 0.0;
		request_reinit();
	}
}

///////////////////

void sca_transmission_line::post_solve()
{
	if (nsample == 0)
	{
		last_sample_va1b1 = (x(a2) - x(b2) + z0.get() * x(nadd2));
		last_sample_va2b2 = (x(a1) - x(b1) + z0.get() * x(nadd1));
	}
	else
	{
		delay_cnt++;
		unsigned long pos = delay_cnt % delay_buffer_size;

		delay_buffer_va1b1[pos] = k_diss
				* (x(a2) - x(b2) + z0.get() * x(nadd2));
		delay_buffer_va2b2[pos] = k_diss
				* (x(a1) - x(b1) + z0.get() * x(nadd1));
	}
}

void sca_transmission_line::matrix_stamps()
{
	double z0_val = z0.get();

	if(fabs(z0_val)<1e-12)
	{
		std::ostringstream str;
		str << "z0 should have a reasonable value (greater 1e-12) in: "
		    << name();
		SC_REPORT_WARNING("SystemC-AMS",str.str().c_str());
	}

	nadd1 = add_equation();

	B_wr(nadd1,nadd1) = z0_val;  // z0*ia1 = ...
	B_wr(nadd1, a1)   += -1.0;    // v(a1,b1) ...
	B_wr(nadd1, b1)   +=  1.0;
	B_wr(a1, nadd1)   +=  1.0;    //i(a1)
	B_wr(b1, nadd1)   += -1.0;    //i(b1)

	nadd2 = add_equation();

	B_wr(nadd2,nadd2) = z0_val; // z0*ia2 = ...
	B_wr(nadd2, a2)   += -1.0;   // v(a2,b2) = ...
	B_wr(nadd2, b2)   +=  1.0;
	B_wr(a2, nadd2)   +=  1.0;   // i(a2)
	B_wr(b2, nadd2)   += -1.0;   // i(b2)


	if(!sca_ac_analysis::sca_ac_is_running())
	{
		// + v_line1
		q(nadd1).add_element(SCA_MPTR(sca_transmission_line::v_line1), this);

		// + v_line2
		q(nadd2).add_element(SCA_MPTR(sca_transmission_line::v_line2), this);

		add_method(POST_SOLVE, SCA_VMPTR(sca_transmission_line::post_solve));

		init_delays();

		if(!initialized)
		{
			add_method(PRE_SOLVE, SCA_VMPTR(sca_transmission_line::pre_solve));
		}
	}
	else
	{
    	double w=sca_ac_analysis::sca_ac_w();
    	delay_sec=delay.get().to_seconds();

    	//e**(-jw*delay) = cos(-w*delay) + j*sin(-w*delay)

		B_wr(nadd1, a2) =   k_diss   * cos(-w*delay_sec); //va1b1= k*va2b2(..)
		A(nadd1, a2) =   k_diss/w * sin(-w*delay_sec);

		B_wr(nadd1, b2) =  -k_diss *   cos(-w*delay_sec);
		A(nadd1, b2) =  -k_diss/w * sin(-w*delay_sec);

		B_wr(nadd1, nadd2) = k_diss   * z0_val * cos(-w*delay_sec); // + z0ia2b2
		A(nadd1, nadd2) = k_diss/w * z0_val * sin(-w*delay_sec);

		B_wr(nadd2, a1) =  k_diss   * cos(-w*delay_sec); //va2b2= k*va1b1(..)
		A(nadd2, a1) =  k_diss/w * sin(-w*delay_sec);

		B_wr(nadd2, b1) = -k_diss   * cos(-w*delay_sec);
		A(nadd2, b1) = -k_diss/w * sin(-w*delay_sec);

		B_wr(nadd2, nadd1) = k_diss   * z0_val * cos(-w*delay_sec); // + z0ia1b1
		A(nadd2, nadd1) = k_diss/w * z0_val * sin(-w*delay_sec);
	}

}

} //namespace sca_eln


