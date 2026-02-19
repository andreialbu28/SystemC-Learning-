/*****************************************************************************

    Copyright 2010-2013
    Fraunhofer-Gesellschaft zur Foerderung der angewandten Forschung e.V.

    Copyright 2015-2022
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

 sca_eln_vsource.cpp - description

 Original Author: Karsten Einwich Fraunhofer IIS/EAS Dresden / COSEDA Technologies

 Created on: 12.11.2009

 SVN Version       :  $Revision: 2320 $
 SVN last checkin  :  $Date: 2023-06-08 12:47:38 +0000 (Thu, 08 Jun 2023) $
 SVN checkin by    :  $Author: pehrlich $
 SVN Id            :  $Id: sca_eln_vsource.cpp 2320 2023-06-08 12:47:38Z pehrlich $

 *****************************************************************************/

/*****************************************************************************/

#include "systemc-ams"
#include "scams/predefined_moc/eln/sca_eln_vsource.h"
#include "scams/impl/util/data_types/sca_method_object.h"
#include "scams/impl/util/data_types/sca_method_list.h"
#include "scams/impl/core/sca_solver_base.h"
#include "scams/impl/predefined_moc/conservative/sca_con_interactive_trace_data.h"

#include "scams/impl/analysis/ac/sca_ac_domain_globals.h"
#include "scams/impl/analysis/ac/sca_ac_domain_db.h"
#include "scams/impl/analysis/ac/sca_ac_domain_eq.h"

using namespace sca_ac_analysis;
using namespace sca_implementation;

namespace sca_eln
{

sca_vsource::sca_vsource(sc_core::sc_module_name, double init_value_,
		double offset_, double amplitude_, double frequency_, double phase_,
		const sca_core::sca_time& delay_, double ac_amplitude_, double ac_phase_,
		double ac_noise_amplitude_) :
	p("p"), n("n"), init_value("init_value", init_value_), offset("offset",
			offset_), amplitude("amplitude", amplitude_), frequency(
			"frequency", frequency_), phase("phase", phase_), delay("delay",
			delay_), ac_amplitude("ac_amplitude", ac_amplitude_), ac_phase(
			"ac_phase", ac_phase_), ac_noise_amplitude("ac_noise_amplitude",
			ac_noise_amplitude_)
{
	nadd=-1;

	unit="A";
	domain="I";
}

const char* sca_vsource::kind() const
{
	return "sca_eln::sca_vsource";
}


void sca_vsource::trace( sc_core::sc_trace_file* tf ) const
{
	std::ostringstream str;
	str << "sc_trace of sca_ln::sca_vsource module not supported for module: " << this->name();
	SC_REPORT_WARNING("SystemC-AMS",str.str().c_str());
}

double sca_vsource::v_t()
{
    if (get_time() < delay)
    		return init_value;

    double tmp;
    tmp = offset + amplitude * sin(2.0 * M_PI * frequency * (sca_get_time()
    			- delay.get().to_seconds()) + phase);
    return tmp;
}


void sca_vsource::matrix_stamps()
{
	if(!sca_ac_analysis::sca_ac_is_running() )
	{
		nadd = add_equation();

		B_wr(nadd, p)  =  1.0;
		B_wr(nadd, n)  = -1.0;
		B_wr(p, nadd)  =  1.0;
		B_wr(n, nadd)  = -1.0;

		q(nadd).sub_element(SCA_MPTR(sca_vsource::v_t), this);
	}
    else
    {
    	nadd = add_equation();

    	B_wr(p, nadd)  =  1.0;
    	B_wr(n, nadd)  = -1.0;


    	if(sca_ac_analysis::sca_ac_noise_is_running())
    	{
        	B_wr(nadd, p) =  1.0;
        	B_wr(nadd, n) = -1.0;

        	if(get_ac_database().is_initialized() && get_ac_database().set_src_value())
        	{
        		long id;
        		id=get_ac_database().get_start_of_add_equations(this->get_sync_domain());

        		if(id>=0)
        		{
        			get_ac_database().get_equations().get_noise_src_names()(id+nadd)=this->name();
        			get_ac_database().get_equations().get_Bgnoise()(id+nadd)=ac_noise_amplitude.get();
        		}
        	}
    	}
    	else
    	{
            sca_util::sca_complex phase_cmpl(0,(M_PI/180) * ac_phase.get());

            phase_cmpl = exp(-phase_cmpl);

            B_wr(nadd, p) =  phase_cmpl.real();
            B_wr(nadd, n) = -phase_cmpl.real();
            A(nadd, p)    =  phase_cmpl.imag()/sca_ac_analysis::sca_ac_w();
            A(nadd, n)    = -phase_cmpl.imag()/sca_ac_analysis::sca_ac_w();

            q(nadd).set_value(-ac_amplitude.get());
    	}
    }

}

bool sca_vsource::trace_init(sca_util::sca_implementation::sca_trace_object_data& data)
{
    //trace will be activated after every complete cluster calculation
    //by teh synchronization layer
    return get_sync_domain()->add_solver_trace(data);
}

void sca_vsource::trace(long id,sca_util::sca_implementation::sca_trace_file_base& tf)
{
    sca_core::sca_time ctime = sca_eln::sca_module::get_time();

    double through_value = x(nadd);

    tf.store_time_stamp(id,ctime,through_value);
}

void sca_vsource::trace_interactive()
{
	if(this->trd!=NULL) return;

    double through_value = x(nadd);

	this->trd->store_value(through_value);
	return;
}



sca_util::sca_complex sca_vsource::calculate_ac_result(sca_util::sca_complex* res_vec)
{
	return res_vec[nadd];
}


const double& sca_vsource::get_typed_trace_value() const
{
	if(this->trd==NULL)
	{
		this->trd=new sca_core::sca_implementation::sca_con_interactive_trace_data(this);

	}

	return this->trd->get_value();
}


const std::string& sca_vsource::get_trace_value() const
{
	std::ostringstream str;
	str << this->get_typed_trace_value();

	static std::string trace_value_string;
	trace_value_string=str.str().c_str();
	return trace_value_string;
}





bool sca_vsource::register_trace_callback(sca_trace_callback cb,void* cb_arg)
{
	if(this->trd==NULL)
	{
		this->trd=new sca_core::sca_implementation::sca_con_interactive_trace_data(this);
	}


	this->trd->register_trace_callback(cb,cb_arg);

	return true;

}



bool sca_vsource::register_trace_callback(sca_util::sca_traceable_object::callback_functor_base& func)
{
	if(this->trd==NULL)
	{
		this->trd=new sca_core::sca_implementation::sca_con_interactive_trace_data(this);
	}


	this->trd->register_trace_callback(func);

	return true;

}


bool sca_vsource::remove_trace_callback(sca_util::sca_traceable_object::callback_functor_base& func)
{
	if(this->trd==NULL)
	{
		return false;
	}


	return this->trd->remove_trace_callback(func);

}

/**
   * experimental physical domain interface
*/
void sca_vsource::set_unit(const std::string& unit_)
{
	unit=unit_;
}

const std::string& sca_vsource::get_unit() const
{
	return unit;
}

void sca_vsource::set_unit_prefix(const std::string& prefix_)
{
	unit_prefix=prefix_;
}

const std::string& sca_vsource::get_unit_prefix() const
{
	return unit_prefix;
}

void sca_vsource::set_domain(const std::string& domain_)
{
	domain=domain_;
}

const std::string& sca_vsource::get_domain() const
{
	return domain;
}


} //namespace sca_eln
