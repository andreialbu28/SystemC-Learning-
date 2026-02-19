/*****************************************************************************

    Copyright 2010
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

 sca_eln_node.cpp - description

 Original Author: Karsten Einwich Fraunhofer IIS/EAS Dresden / COSEDA Technologies

 Created on: 10.11.2009

 SVN Version       :  $Revision: 2320 $
 SVN last checkin  :  $Date: 2023-06-08 12:47:38 +0000 (Thu, 08 Jun 2023) $
 SVN checkin by    :  $Author: pehrlich $
 SVN Id            :  $Id: sca_eln_node.cpp 2320 2023-06-08 12:47:38Z pehrlich $

 *****************************************************************************/

/*****************************************************************************/


#include <systemc-ams>
#include "scams/predefined_moc/eln/sca_eln_node.h"
#include "scams/impl/core/sca_solver_base.h"
#include "scams/impl/analysis/ac/sca_ac_domain_globals.h"
#include "scams/impl/predefined_moc/conservative/sca_con_interactive_trace_data.h"

namespace sca_eln
{

static const double ZERO_VALUE=0.0;

void sca_node::print( std::ostream& str) const
{
	str << this->get_typed_trace_value();
}

void sca_node::dump( std::ostream&  str) const
{
	str << this->kind() << " : " << this->name() << " value: "<< this->get_typed_trace_value();
}


sca_node::sca_node() :
	sca_conservative_signal(sc_core::sc_gen_unique_name("sca_eln_node"))
{
	unit="V";
	unit_prefix="";
	domain="V";

	connected_eln_module=NULL;

	reference_node = false;
	trd=NULL;
	sca_ac_analysis::sca_implementation::sca_ac_register_add_eq_arc(this, &node_number, &reference_node);
}

sca_node::sca_node(const char* name_) :
	sca_conservative_signal(name_)
{
	unit="V";
	unit_prefix="";
	domain="V";

	connected_eln_module=NULL;

	reference_node = false;
	trd=NULL;

	sca_ac_analysis::sca_implementation::sca_ac_register_add_eq_arc(this, &node_number, &reference_node);
}

const char* sca_node::kind() const
{
	return "sca_eln::sca_node";
}


long sca_node::get_node_number() const
{
	return sca_core::sca_implementation::sca_conservative_signal::get_node_number();
}


void sca_node::get_connected_eln_module()
{
	unsigned long nmodules = get_number_of_connected_modules();
	std::vector<sca_core::sca_module*>& modules = get_connected_module_list();

	for (unsigned long i = 0; i < nmodules; ++i)
	{
		connected_eln_module = dynamic_cast<sca_eln::sca_module*> (modules[i]);
		if(connected_eln_module!=NULL) return;
	}
}


/** overloaded sca_conservative_module method */
double* sca_node::get_result_ref()
{
	if(reference_node) return NULL;

	if(connected_eln_module==NULL)
	{
		this->get_connected_eln_module();
	}

	if(connected_eln_module==NULL) return NULL;

	return &(connected_eln_module->x(node_number));
}

//Disabled not used
sca_node::sca_node(const sca_eln::sca_node&)
{
	connected_eln_module=NULL;
	trd=NULL;
}

///////////////

bool sca_node::trace_init(
		sca_util::sca_implementation::sca_trace_object_data& data)
{
	//initialize connected eln module pointer
	this->get_connected_eln_module();


	if (connected_eln_module != NULL)
	{
		return connected_eln_module->get_sync_domain()->add_solver_trace(data);
	}

	return false;
}



//////////////////////////////////////////////////////////////////////////////


//////////////////////////////////////////////////////////////////////////////


//is called after cluster was calculated
void sca_node::trace(long id,
		sca_util::sca_implementation::sca_trace_file_base& tf)
{
	sca_core::sca_time ctime = connected_eln_module->get_time();

	if (reference_node) //cumbersome reference node tracing
	{
		double gnd = 0.0;
		tf.store_time_stamp(id, ctime, gnd);
	}
	else
	{
		tf.store_time_stamp(id, ctime, connected_eln_module->x(node_number));
	}
}


//is called after cluster was calculated
void sca_node::trace_interactive()
{
	if(this->trd==NULL) return;

	if(connected_eln_module==NULL)
	{
		this->get_connected_eln_module();
	}

	if(connected_eln_module!=NULL)
	{
		trd->store_value(connected_eln_module->x(node_number));
	}

	return;
}

const double& sca_node::get_typed_trace_value() const
{
	if (reference_node) //cumbersome reference node tracing
	{
		return ZERO_VALUE;
	}

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


const std::string& sca_node::get_trace_value() const
{
	std::ostringstream str;
	str << this->get_typed_trace_value();

	static std::string trace_value_string;
	trace_value_string=str.str().c_str();
	return trace_value_string;
}





bool sca_node::register_trace_callback(sca_trace_callback cb,void* cb_arg)
{
	if(trd==NULL)
	{
		trd=new sca_core::sca_implementation::sca_con_interactive_trace_data(this);
	}


	trd->register_trace_callback(cb,cb_arg);

	return true;

}


bool sca_node::register_trace_callback(sca_util::sca_traceable_object::callback_functor_base& func)
{
	if(trd==NULL)
	{
		trd=new sca_core::sca_implementation::sca_con_interactive_trace_data(this);
	}


	trd->register_trace_callback(func);

	return true;
}


bool sca_node::remove_trace_callback(sca_util::sca_traceable_object::callback_functor_base& func)
{
	if(trd==NULL)
	{
		return false;
	}


	return trd->remove_trace_callback(func);
}


void sca_node::set_unit(const std::string& unit_)
{
	unit=unit_;
}

const std::string& sca_node::get_unit() const
{
	return unit;
}

void sca_node::set_unit_prefix(const std::string& prefix_)
{
	unit_prefix=prefix_;
}

const std::string& sca_node::get_unit_prefix() const
{
	return unit_prefix;
}

void sca_node::set_domain(const std::string& domain_)
{
	domain=domain_;
}

const std::string& sca_node::get_domain() const
{
	return domain;
}


}
