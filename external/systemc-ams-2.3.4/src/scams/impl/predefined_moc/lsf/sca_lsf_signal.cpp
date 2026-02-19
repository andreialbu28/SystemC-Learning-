/*****************************************************************************

    Copyright 2010
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

  sca_lsf_signal.cpp - description

  Original Author: Karsten Einwich Fraunhofer IIS/EAS Dresden / COSEDA Technologies

  Created on: 05.01.2010

   SVN Version       :  $Revision: 2320 $
   SVN last checkin  :  $Date: 2023-06-08 12:47:38 +0000 (Thu, 08 Jun 2023) $
   SVN checkin by    :  $Author: pehrlich $
   SVN Id            :  $Id: sca_lsf_signal.cpp 2320 2023-06-08 12:47:38Z pehrlich $

 *****************************************************************************/

/*****************************************************************************/


#include <systemc-ams>
#include "scams/predefined_moc/lsf/sca_lsf_signal.h"
#include "scams/impl/core/sca_solver_base.h"
#include "scams/impl/analysis/ac/sca_ac_domain_globals.h"
#include "scams/impl/predefined_moc/conservative/sca_con_interactive_trace_data.h"


namespace sca_lsf
{

static const double ZERO_VALUE=0.0;


void sca_signal::print( std::ostream& str) const
{
	str << this->get_typed_trace_value();
}


void sca_signal::dump( std::ostream&  str) const
{
	str << this->kind() << " : " << this->name() << " value: "<< this->get_typed_trace_value();
}


sca_signal::sca_signal() :
	sca_conservative_signal(sc_core::sc_gen_unique_name("sca_lsf_signal"))
{
	reference_node = false;
	sca_ac_analysis::sca_implementation::sca_ac_register_add_eq_arc(this, &node_number, &reference_node);
	lsf_module=NULL;
	trd=NULL;
}

sca_signal::sca_signal(const char* name_) :
	sca_conservative_signal(name_)
{
	reference_node = false;
	sca_ac_analysis::sca_implementation::sca_ac_register_add_eq_arc(this, &node_number, &reference_node);
	lsf_module=NULL;
	trd=NULL;
}

long sca_signal::get_node_number() const
{
	return sca_core::sca_implementation::sca_conservative_signal::get_node_number();
}


void sca_signal::get_connected_lsf_module()
{
	unsigned long nmodules = get_number_of_connected_modules();
	std::vector<sca_core::sca_module*>& modules = get_connected_module_list();

	for (unsigned long i = 0; i < nmodules; ++i)
	{
		lsf_module = dynamic_cast<sca_lsf::sca_module*> (modules[i]);
		if(lsf_module!=NULL) return;
	}
}



/** overloaded sca_conservative_module method */
double* sca_signal::get_result_ref()
{

	if(lsf_module==NULL)
	{
		this->get_connected_lsf_module();
	}

	if(lsf_module==NULL) return NULL;

	return &(lsf_module->x(node_number));
}


const char* sca_signal::kind() const
{
	return "sca_lsf::sca_signal";
}

//Disabled not used
sca_signal::sca_signal(const sca_lsf::sca_signal&)
{
	lsf_module=NULL;
	trd=NULL;
}

///////////////

bool sca_signal::trace_init(
		sca_util::sca_implementation::sca_trace_object_data& data)
{
	unsigned long nmodules = get_number_of_connected_modules();
	std::vector<sca_core::sca_module*>& modules = get_connected_module_list();

	for (unsigned long i = 0; i < nmodules; ++i)
	{
		lsf_module = dynamic_cast<sca_lsf::sca_module*> (modules[i]);
		if (lsf_module != NULL)
		{
			return lsf_module->get_sync_domain()->add_solver_trace(data);
		}
	}
	return false;
}

//is called after cluster was calculated
void sca_signal::trace(long id,
		sca_util::sca_implementation::sca_trace_file_base& tf)
{
	if((id<0)&&(this->trd!=NULL)) //interactive trace
	{
		if(lsf_module==NULL)
		{
			this->get_connected_lsf_module();
		}

		if(lsf_module!=NULL)
		{
			trd->store_value(lsf_module->x(node_number));
		}

		return;
	}


	sca_core::sca_time ctime = lsf_module->get_time();

	tf.store_time_stamp(id, ctime, lsf_module->x(node_number));
}


//is called after cluster was calculated
void sca_signal::trace_interactive()
{
	if(this->trd==NULL) return;

	if(lsf_module==NULL)
	{
		this->get_connected_lsf_module();
	}

	if(lsf_module!=NULL)
	{
		trd->store_value(lsf_module->x(node_number));
	}

	return;
}


///////////////////////////////////////////////////////////////////////////////


const double& sca_signal::get_typed_trace_value() const
{
	if (reference_node) //cumbersome reference node tracing
	{
		return ZERO_VALUE;
	}

	//before end of elaboration signal may not yet connected
	if(sc_core::sc_get_status() &
			(sc_core::SC_ELABORATION | sc_core::SC_BEFORE_END_OF_ELABORATION))
	{
		return ZERO_VALUE;
	}


	if(trd==NULL)
	{
		trd=new sca_core::sca_implementation::sca_con_interactive_trace_data(this);

	}

	return trd->get_value();
}

///////////////////////////////////////////////////////////////////////////////


const std::string& sca_signal::get_trace_value() const
{
	std::ostringstream str;
	str << this->get_typed_trace_value();

	static std::string trace_value_string;
	trace_value_string=str.str().c_str();
	return trace_value_string;
}



///////////////////////////////////////////////////////////////////////////////

bool sca_signal::register_trace_callback(sca_trace_callback cb,void* cb_arg)
{
	if(trd==NULL)
	{
		trd=new sca_core::sca_implementation::sca_con_interactive_trace_data(this);
	}


	trd->register_trace_callback(cb,cb_arg);

	return true;

}


bool sca_signal::register_trace_callback(sca_util::sca_traceable_object::callback_functor_base& func)
{
	if(trd==NULL)
	{
		trd=new sca_core::sca_implementation::sca_con_interactive_trace_data(this);
	}


	trd->register_trace_callback(func);

	return true;
}


bool sca_signal::remove_trace_callback(sca_util::sca_traceable_object::callback_functor_base& func)
{
	if(trd==NULL)
	{
		return false;
	}


	return trd->remove_trace_callback(func);
}

void sca_signal::set_unit(const std::string& unit_)
{
	unit=unit_;
}

const std::string& sca_signal::get_unit() const
{
	return unit;
}

void sca_signal::set_unit_prefix(const std::string& prefix_)
{
	unit_prefix=prefix_;
}

const std::string& sca_signal::get_unit_prefix() const
{
	return unit_prefix;
}

void sca_signal::set_domain(const std::string& domain_)
{
	domain=domain_;
}

const std::string& sca_signal::get_domain() const
{
	return domain;
}

}
