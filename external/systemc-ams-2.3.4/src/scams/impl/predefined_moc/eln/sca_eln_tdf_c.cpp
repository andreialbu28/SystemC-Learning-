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

  sca_eln_tdf_c.cpp - description

  Original Author: Karsten Einwich Fraunhofer IIS/EAS Dresden / COSEDA Technologies

  Created on: 10.11.2009

   SVN Version       :  $Revision: 2320 $
   SVN last checkin  :  $Date: 2023-06-08 12:47:38 +0000 (Thu, 08 Jun 2023) $
   SVN checkin by    :  $Author: pehrlich $
   SVN Id            :  $Id: sca_eln_tdf_c.cpp 2320 2023-06-08 12:47:38Z pehrlich $

 *****************************************************************************/

/*****************************************************************************/


#include "systemc-ams"
#include "scams/predefined_moc/eln/sca_eln_tdf_c.h"
#include "scams/impl/util/data_types/sca_method_object.h"
#include "scams/impl/util/data_types/sca_method_list.h"
#include "scams/impl/core/sca_solver_base.h"
#include <limits>
#include "scams/impl/util/data_types/almost_equal.h"
#include "scams/impl/predefined_moc/conservative/sca_con_interactive_trace_data.h"


namespace sca_eln
{

namespace sca_tdf
{

sca_c::	sca_c(sc_core::sc_module_name, double scale_, double q0_) :
	p("p"), n("n"), inp("inp"), scale("scale", scale_), q0("q0", q0_)
{
    curr_value = 1.0;

    nadd2= -1;
    nadd=-1;

    dc_init=true;

	unit="A";
	domain="I";
}


const char* sca_c::kind() const
{
	return "sca_eln::sca_tdf::sca_c";
}

void sca_c::trace( sc_core::sc_trace_file* tf ) const
{
	std::ostringstream str;
	str << "sc_trace of sca_ln::sca_tdf::sca_c module not supported for module: " << this->name();
	SC_REPORT_WARNING("SystemC-AMS",str.str().c_str());
}


void sca_c::read_cval()
{
	double inp_val = scale * inp.read();

	if(!sca_util::sca_implementation::almost_equal(inp_val,curr_value))
	{
		enable_b_change=true;
		continous=true;

		B_wr(nadd2, p) = inp_val;
		B_wr(nadd2, n) = -inp_val;

		if(!sca_ac_analysis::sca_ac_is_running() && dc_init)
		{
			if (q0.get() != sca_util::SCA_UNDEFINED)
			{
				q(nadd).set_value(-q0.get() / inp_val);
			}
		}

		curr_value=inp_val;
	}
}



void sca_c::matrix_stamps()
{
	//nadd - current
	nadd  = add_equation();
	//nadd2 - charge
	nadd2 = add_equation();

	add_method(PRE_SOLVE, SCA_VMPTR(sca_c::read_cval));

	if(!sca_ac_analysis::sca_ac_is_running() && dc_init)
	{
		if (q0.get() == sca_util::SCA_UNDEFINED)
		{
			B_wr(nadd, nadd) = -1.0; //i=0

			B_wr(nadd2, nadd2) = -1.0; // q=C*v
			B_wr(nadd2, p) = curr_value;
			B_wr(nadd2, n) = -curr_value;
		}
		else
		{
			B_wr(p, nadd) = 1.0;
			B_wr(n, nadd) = -1.0;

			// 0 = v - q0/c   - nadd -> i
			B_wr(nadd, p) = 1.0;
			B_wr(nadd, n) = -1.0;
			q(nadd).set_value(-q0.get() / curr_value);

			// q = C * v
			B_wr(nadd2, nadd2) = -1.0;
			B_wr(nadd2, p) = curr_value;
			B_wr(nadd2, n) = -curr_value;
		}

		add_method(POST_SOLVE, SCA_VMPTR(sca_c::post_solve));
	}
	else  //for restore for / after AC
	{
		B_wr(p,nadd) = 1.0;
		B_wr(n,nadd) = -1.0;

	    // i = dq
	    B_wr(nadd,nadd)   = -1.0;
	    A(nadd,nadd2)  =  1.0;

	    // q = C * v
	    B_wr(nadd2, nadd2) = -1.0;
	    B_wr(nadd2, p) =  curr_value;
	    B_wr(nadd2, n) = -curr_value;
	}

}


void sca_c::post_solve() //set capacitor stamps after first step
{
	//reset dc stamps
	B_wr(nadd, nadd)  = 0.0;
	B_wr(nadd2,nadd2) = 0.0;
	B_wr(nadd2,p)     = 0.0;
	B_wr(nadd2,n)     = 0.0;
    B_wr(nadd,p)      = 0.0;
    B_wr(nadd,n)      = 0.0;
    q(nadd).set_value(0.0);

    remove_method(POST_SOLVE, SCA_VMPTR(sca_c::post_solve));

	//set transient stamps
	B_wr(p,nadd) = 1.0;
	B_wr(n,nadd) = -1.0;

    // i = dq
    B_wr(nadd,nadd)   = -1.0;
    A(nadd,nadd2)  =  1.0;

    // q = C * v
    B_wr(nadd2, nadd2) = -1.0;
    B_wr(nadd2, p) =  curr_value;
    B_wr(nadd2, n) = -curr_value;

    request_reinit(1);

	dc_init=false;

}


bool sca_c::trace_init(sca_util::sca_implementation::sca_trace_object_data& data)
{
    //trace will be activated after every complete cluster calculation
    //by teh synchronization layer
    return get_sync_domain()->add_solver_trace(data);
}

void sca_c::trace(long id,sca_util::sca_implementation::sca_trace_file_base& tf)
{
    sca_core::sca_time ctime = sca_eln::sca_module::get_time();

    double through_value = x(nadd);

    tf.store_time_stamp(id,ctime,through_value);
}

void sca_c::trace_interactive()
{
	if(this->trd==NULL) return;

    double through_value = x(nadd);

	this->trd->store_value(through_value);
	return;
}

sca_util::sca_complex sca_c::calculate_ac_result(sca_util::sca_complex* res_vec)
{
	return res_vec[nadd];
}



const double& sca_c::get_typed_trace_value() const
{
	if(this->trd==NULL)
	{
		this->trd=new sca_core::sca_implementation::sca_con_interactive_trace_data(this);

	}

	return this->trd->get_value();
}


const std::string& sca_c::get_trace_value() const
{
	std::ostringstream str;
	str << this->get_typed_trace_value();

	static std::string trace_value_string;
	trace_value_string=str.str().c_str();
	return trace_value_string;
}





bool sca_c::register_trace_callback(sca_trace_callback cb,void* cb_arg)
{
	if(this->trd==NULL)
	{
		this->trd=new sca_core::sca_implementation::sca_con_interactive_trace_data(this);
	}


	this->trd->register_trace_callback(cb,cb_arg);

	return true;

}


bool sca_c::register_trace_callback(sca_util::sca_traceable_object::callback_functor_base& func)
{
	if(this->trd==NULL)
	{
		this->trd=new sca_core::sca_implementation::sca_con_interactive_trace_data(this);
	}


	this->trd->register_trace_callback(func);

	return true;

}


bool sca_c::remove_trace_callback(sca_util::sca_traceable_object::callback_functor_base& func)
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
void sca_c::set_unit(const std::string& unit_)
{
	unit=unit_;
}

const std::string& sca_c::get_unit() const
{
	return unit;
}

void sca_c::set_unit_prefix(const std::string& prefix_)
{
	unit_prefix=prefix_;
}

const std::string& sca_c::get_unit_prefix() const
{
	return unit_prefix;
}

void sca_c::set_domain(const std::string& domain_)
{
	domain=domain_;
}

const std::string& sca_c::get_domain() const
{
	return domain;
}



} //namespace sca_tdf
} //namespace sca_eln
