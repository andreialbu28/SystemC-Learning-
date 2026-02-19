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

  sca_eln_tdf_isource.cpp - description

  Original Author: Karsten Einwich Fraunhofer IIS/EAS Dresden / COSEDA Technologies

  Created on: 10.11.2009

   SVN Version       :  $Revision: 2320 $
   SVN last checkin  :  $Date: 2023-06-08 12:47:38 +0000 (Thu, 08 Jun 2023) $
   SVN checkin by    :  $Author: pehrlich $
   SVN Id            :  $Id: sca_eln_tdf_isource.cpp 2320 2023-06-08 12:47:38Z pehrlich $

 *****************************************************************************/

/*****************************************************************************/


#include "systemc-ams"
#include "scams/predefined_moc/eln/sca_eln_tdf_isource.h"
#include "scams/impl/util/data_types/sca_method_object.h"
#include "scams/impl/util/data_types/sca_method_list.h"
#include "scams/impl/core/sca_solver_base.h"
#include "scams/impl/predefined_moc/conservative/sca_con_interactive_trace_data.h"

namespace sca_eln
{

namespace sca_tdf
{

sca_isource::	sca_isource(sc_core::sc_module_name, double scale_) :
	p("p"), n("n"), inp("inp"), scale("scale", scale_)
{
    i_value=0.0;

	unit="A";
	domain="I";

	nadd=-1;
}

const char* sca_isource::kind() const
{
	return "sca_eln::sca_tdf::sca_isource";
}


void sca_isource::trace( sc_core::sc_trace_file* tf ) const
{
	std::ostringstream str;
	str << "sc_trace of sca_ln::sca_tdf::sca_isource module not supported for module: " << this->name();
	SC_REPORT_WARNING("SystemC-AMS",str.str().c_str());
}


double sca_isource::i_t()
{

    if(!sca_ac_analysis::sca_ac_is_running())
    {
    	i_value=scale.get() * inp.read();
    	return i_value;
    }
    else
    {
        double val = scale.get() * sca_ac_analysis::sca_ac(inp).real();
        return val;
    }
}


void sca_isource::matrix_stamps()
{
	if(sca_ac_analysis::sca_ac_is_running())
	{
	   nadd = add_equation();

	   B_wr(p, nadd) =  1.0;
	   B_wr(n, nadd) = -1.0;

	   B_wr(nadd, nadd) =  1.0;
	   q(nadd).sub_element(SCA_MPTR(sca_isource::i_t),this);
	}
	else
	{
		q(p).add_element(SCA_MPTR(sca_isource::i_t),this);
		q(n).sub_element(SCA_MPTR(sca_isource::i_t),this);
	}
}

bool sca_isource::trace_init(sca_util::sca_implementation::sca_trace_object_data& data)
{
    //trace will be activated after every complete cluster calculation
    //by teh synchronization layer
    return get_sync_domain()->add_solver_trace(data);
}

void sca_isource::trace(long id,sca_util::sca_implementation::sca_trace_file_base& tf)
{
    sca_core::sca_time ctime = sca_eln::sca_module::get_time();

    double through_value = i_value;

    tf.store_time_stamp(id,ctime,through_value);
}

void sca_isource::trace_interactive()
{
	if(this->trd==NULL) return;

    double through_value = i_value;

	this->trd->store_value(through_value);
	return;
}

sca_util::sca_complex sca_isource::calculate_ac_result(sca_util::sca_complex* res_vec)
{
	return res_vec[nadd];
}



const double& sca_isource::get_typed_trace_value() const
{
	if(this->trd==NULL)
	{
		this->trd=new sca_core::sca_implementation::sca_con_interactive_trace_data(this);

	}

	return this->trd->get_value();
}


const std::string& sca_isource::get_trace_value() const
{
	std::ostringstream str;
	str << this->get_typed_trace_value();

	static std::string trace_value_string;
	trace_value_string=str.str().c_str();
	return trace_value_string;
}





bool sca_isource::register_trace_callback(sca_trace_callback cb,void* cb_arg)
{
	if(this->trd==NULL)
	{
		this->trd=new sca_core::sca_implementation::sca_con_interactive_trace_data(this);
	}


	this->trd->register_trace_callback(cb,cb_arg);

	return true;

}


bool sca_isource::register_trace_callback(sca_util::sca_traceable_object::callback_functor_base& func)
{
	if(this->trd==NULL)
	{
		this->trd=new sca_core::sca_implementation::sca_con_interactive_trace_data(this);
	}


	this->trd->register_trace_callback(func);

	return true;

}


bool sca_isource::remove_trace_callback(sca_util::sca_traceable_object::callback_functor_base& func)
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
void sca_isource::set_unit(const std::string& unit_)
{
	unit=unit_;
}

const std::string& sca_isource::get_unit() const
{
	return unit;
}

void sca_isource::set_unit_prefix(const std::string& prefix_)
{
	unit_prefix=prefix_;
}

const std::string& sca_isource::get_unit_prefix() const
{
	return unit_prefix;
}

void sca_isource::set_domain(const std::string& domain_)
{
	domain=domain_;
}

const std::string& sca_isource::get_domain() const
{
	return domain;
}



} //namespace sca_tdf
} //namespace sca_eln
