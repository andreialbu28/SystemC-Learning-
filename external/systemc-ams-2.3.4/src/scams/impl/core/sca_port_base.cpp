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

 sca_port_base.cpp - description

 Original Author: Karsten Einwich Fraunhofer IIS/EAS Dresden

 Created on: 15.05.2009

 SVN Version       :  $Revision: 2320 $
 SVN last checkin  :  $Date: 2023-06-08 12:47:38 +0000 (Thu, 08 Jun 2023) $
 SVN checkin by    :  $Author: pehrlich $
 SVN Id            :  $Id: sca_port_base.cpp 2320 2023-06-08 12:47:38Z pehrlich $

 *****************************************************************************/

/*****************************************************************************/

#include <systemc-ams>
#include "scams/impl/core/sca_port_base.h"
#include "scams/impl/core/sca_simcontext.h"
#include "scams/impl/synchronization/sca_sync_value_handle.h"
#include "scams/impl/core/sca_solver_base.h"
#include "scams/impl/analysis/ac/sca_ac_domain_globals.h"

#include <sstream>

using namespace sc_core;
using namespace std;

namespace sca_core
{
namespace sca_implementation
{

void sca_port_base::construct_parent(sc_core::sc_object* pobj)
{
    //ignore hierarchical ports
    //will be already registered in base sc_module
    //ports outside modules should yield an error in the base classes
    if(pobj!=NULL)
    {
        parent_module=dynamic_cast<sca_core::sca_module*>(pobj->get_parent_object());
    }

    //parent_module = sca_get_curr_simcontext()->get_current_sca_module();
    if (parent_module!=NULL)
    {
        port_number = parent_module->register_port(this);
    }

}

sca_port_base::sca_port_base()
{
	port_elaborated = false;
	elaboration_finished=false;

	sca_port_type = SCA_NOT_DEFINED_PORT;

	m_if = NULL;
	m_sca_if = NULL;

	port_if_id = 0;
	port_number = 0;

	parent_module = NULL;
	sync_domain=NULL;
}

/////////////////////////////////////////

//destructor
sca_port_base::~sca_port_base()
{
}

/////////////////////////////////////////


sca_module* sca_port_base::get_parent_module() const
{
	return parent_module;
}

sca_solver_base* sca_port_base::get_sync_domain()
{
	if(this->sync_domain!=NULL) return this->sync_domain;

	if(parent_module==NULL) return NULL;

	this->sync_domain=parent_module->get_sync_domain();

	return this->sync_domain;
}


void sca_port_base::set_sync_domain(sca_solver_base* solv)
{
	this->sync_domain=solv;
}


/////////////////////////////////////////

bool sca_port_base::add_cluster_trace(sca_util::sca_implementation::sca_trace_object_data& tr_obj)
{
	if(this->get_parent_module()==NULL) return false;

	if(this->get_sync_domain()==NULL) return false;

	this->get_sync_domain()->add_cluster_trace(tr_obj);

	return true;
}

///////////////////////////////////////////

const char* sca_port_base::sca_name() const
{
	const sc_object* obj = dynamic_cast<const sc_object*> (this);
	if (obj == NULL)
		return "NO valid object";
	else
		return obj->name();
}

/////////////////////////////////////////

const char* sca_port_base::sca_basename() const
{
	const sc_object* obj = dynamic_cast<const sc_object*> (this);
	if (obj == NULL)
		return "NO valid object";
	else
		return obj->basename();
}

/////////////////////////////////////////

void sca_port_base::end_of_port_elaboration()
{
}


void sca_port_base::register_sc_value_trace(const ::sc_core::sc_event& ev,
			sca_core::sca_implementation::sca_sync_value_handle_base& handle)
{
	parent_module->get_sync_domain()->register_sc_value_trace(ev,handle);
}


void sca_port_base::register_sca_schedule(::sc_core::sc_time next_time,
			sca_core::sca_implementation::sca_sync_value_handle_base& handle)
{
	parent_module->get_sync_domain()->register_sca_schedule(next_time,handle);
}


void sca_port_base::get_sc_value_on_time(::sc_core::sc_time time,
		                    sca_sync_value_handle_base& handle)
{
	parent_module->get_sync_domain()->get_sc_value_on_time(time,handle);
}



//writes sc value from handle at a given time
void sca_port_base::write_sc_value_on_time( ::sc_core::sc_time current_time,
		                       ::sc_core::sc_time next_time,
		                      sca_sync_value_handle_base& handle)
{
	parent_module->get_sync_domain()->write_sc_value(current_time,next_time,handle);
}


void sca_port_base::read_from_sc()
{

}


void sca_port_base::write_to_sc()
{
}

bool sca_port_base::reinitialize_port_timing()
{
	return false;
}


void sca_port_base::reinitialize_port()
{
}



void sca_port_base::initialize_port()
{

}

sc_core::sc_time sca_port_base::get_parent_module_time() const
{
	return get_parent_module()->sca_get_time();
}

void sca_port_base::elaborate_port()
{
	m_if = sc_get_interface();

	if (m_if == NULL)
	{
		ostringstream str;
		str << "Error module: " << parent_module->name() << " port: "
				<< port_number << " not bound" << endl;
		SC_REPORT_ERROR("SystemC-AMS",str.str().c_str());
		return;
	}

	m_sca_if = dynamic_cast<sca_interface*> (m_if);

	if (sca_port_type == SCA_NOT_DEFINED_PORT)
	{
		ostringstream str;
		str << "Cannot determine type of sca_port #" << port_number
				<< " in sca_module: " << parent_module->name()
				<< " --> port_type is not set." << endl;
		SC_REPORT_WARNING("SystemC-AMS",str.str().c_str());
	}

	if (m_sca_if != NULL)
	{
		sca_prim_channel* m_sca_ch = dynamic_cast<sca_prim_channel*> (m_sca_if);

		if (m_sca_ch == NULL)
		{
			ostringstream str;
			str << "Interface bound to sca_port #" << port_number << " in "
					<< parent_module->name()
					<< ": is not implemented by a sca_channel." << endl;
			SC_REPORT_ERROR("SystemC-AMS",str.str().c_str());
			return;
		}

		//  sets id for port identificattion
		set_if_id(m_sca_ch->get_number_of_connected_ports());
		m_sca_ch->register_port(this);
	}

	port_elaborated = true;

	end_of_port_elaboration();
}

///////////////////////////////////////////////////////////////////////////////
// implementation of traceable object interface

//virtual method may overloaded
void sca_port_base::set_type_info(sca_util::sca_implementation::sca_trace_object_data& data)
{
	data.set_type_info<double>(); //default double
}


 bool sca_port_base::trace_init(sca_util::sca_implementation::sca_trace_object_data& data)
 {
	 //trace init is called after end_of_elaboration -> thus port binding has
	 //been always finished
	 if(m_sca_if==NULL)
	 {
		 m_sca_if      = dynamic_cast<sca_interface*>(sc_get_interface());
	 }

	 sca_prim_channel* m_sca_ch = dynamic_cast<sca_prim_channel*>(sca_get_interface());
	 if(m_sca_ch==NULL) return false;
	 else               return m_sca_ch->trace_init(data);
 }


 void sca_port_base::trace(long id,sca_util::sca_implementation::sca_trace_file_base& tf)
 {
 sca_prim_channel* m_sca_ch = dynamic_cast<sca_prim_channel*>(sca_get_interface());
 if(m_sca_ch==NULL) return;
 else               return m_sca_ch->trace(id,tf);
 }

 void sca_port_base::trace_interactive()
 {
	 sca_prim_channel* m_sca_ch = dynamic_cast<sca_prim_channel*>(sca_get_interface());
	 if(m_sca_ch==NULL) return;
	 else               return m_sca_ch->trace_interactive();
 }


 //empty by default
void sca_port_base::port_processing_method()
{
}


/** redirect interactive debug methods to connected signal */
const std::string& sca_port_base::get_trace_value() const
{
	const sc_core::sc_interface* scif=this->sc_get_interface();
	const sca_util::sca_traceable_object* to=dynamic_cast<const sca_util::sca_traceable_object*>(scif);
	if(to==NULL) return empty_string;

	return to->get_trace_value();
}

/** redirect interactive debug methods to connected signal */
bool sca_port_base::force_value(const std::string& val)
{
	sc_core::sc_interface* scif=this->sc_get_interface();
	sca_util::sca_traceable_object* to=dynamic_cast<sca_util::sca_traceable_object*>(scif);
	if(to==NULL) return false;

	return to->force_value(val);
}

/** redirect interactive debug methods to connected signal */
void sca_port_base::release_value()
{
	sc_core::sc_interface* scif=this->sc_get_interface();
	sca_util::sca_traceable_object* to=dynamic_cast<sca_util::sca_traceable_object*>(scif);
	if(to==NULL) return;

	return to->release_value();
}

/** redirect interactive debug methods to connected signal */
bool sca_port_base::register_trace_callback(sca_trace_callback cb,void* arg)
{
	sc_core::sc_interface* scif=this->sc_get_interface();
	sca_util::sca_traceable_object* to=dynamic_cast<sca_util::sca_traceable_object*>(scif);
	if(to==NULL) return false;

	return to->register_trace_callback(cb,arg);
}

/** redirect interactive debug methods to connected signal */
bool sca_port_base::register_trace_callback(callback_functor_base& func)
{
	sc_core::sc_interface* scif=this->sc_get_interface();
	sca_util::sca_traceable_object* to=dynamic_cast<sca_util::sca_traceable_object*>(scif);
	if(to==NULL) return false;

	return to->register_trace_callback(func);
}

/** redirect interactive debug methods to connected signal */
bool sca_port_base::remove_trace_callback(callback_functor_base& func)
{
	sc_core::sc_interface* scif=this->sc_get_interface();
	sca_util::sca_traceable_object* to=dynamic_cast<sca_util::sca_traceable_object*>(scif);
	if(to==NULL) return false;

	return to->remove_trace_callback(func);
}



}//namespace sca_implementation
}//namespace sca_core
