/*****************************************************************************

    Copyright 2010-2013
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

 sca_module.cpp - description

 Original Author: Karsten Einwich Fraunhofer IIS/EAS / COSEDA Technologies

 Created on: 15.05.2009

 SVN Version       :  $Revision: 2115 $
 SVN last checkin  :  $Date: 2020-03-12 17:26:27 +0000 (Thu, 12 Mar 2020) $
 SVN checkin by    :  $Author: karsten $
 SVN Id            :  $Id: sca_module.cpp 2115 2020-03-12 17:26:27Z karsten $

 *****************************************************************************/

/*****************************************************************************/

#include <systemc-ams>

#include "scams/impl/core/sca_simcontext.h"
#include "scams/core/sca_module.h"
#include "scams/impl/util/tracing/sca_trace_file_base.h"
#include "scams/impl/core/sca_object_manager.h"
#include "scams/impl/core/sca_port_base.h"
#include "scams/impl/core/sca_solver_base.h"
#include "scams/impl/core/sca_globals.h"

//STL usage
#include <algorithm>
#include <sstream>


namespace sca_core
{

using namespace sca_implementation;





void sca_module::construct()
{
	sync_domain = NULL;
	view_interface = NULL;

	timestep_set=sc_core::SC_ZERO_TIME;
    timestep_is_set=false;
    timestep_max_is_set=false;
    timestep_elaborated=false;

    initialize_executes_flag=false;
    set_attributes_executes_flag=false;
    change_attributes_executes_flag      =false;
	ac_processing_executes_flag=false;

    first_activation_requested=false;
    first_activation_time_requested=false;

    reinitialize_executes_flag=false;

    user_solver_handler=NULL;

	sca_core::sca_implementation::sca_simcontext* simc;
	simc=sca_core::sca_implementation::sca_get_curr_simcontext();

	if(simc==NULL)
	{
		std::ostringstream str;
		str << "Cannot create a new module after the simulation has been finished";
		str << " or a module or sca_interface has been deleted";

		sc_core::sc_object* obj=dynamic_cast<sc_core::sc_object*>(this);
		if(obj!=NULL)
		{
			str << " for: " << obj->name();
		}
		SC_REPORT_ERROR("SystemC-AMS",str.str().c_str());
		return;
	}

	simc->get_sca_object_manager()->insert_module(this);
}


sca_module::sca_module(const sc_core::sc_module_name& nm) : sc_core::sc_module(nm)
{
	construct();
}

sca_module::sca_module() : sc_core::sc_module()
{
	construct();
}

sca_module::sca_module(const char* nm) : sc_core::sc_module(nm)
{
	construct();
}


sca_module::sca_module(const std::string&  nm) : sc_core::sc_module(nm)
{
	construct();
}


//////////////////////////////////////////////////////////////////

//destructor
sca_module::~sca_module()
{
	if(sca_get_curr_simcontext()!=NULL)
	{
	 	if((sc_core::sc_status() & 0x3)!=0) //during elaboration delte allowed
	 	{
			delete sca_get_curr_simcontext();
	  	}
	  	else
	  	{
	
			if(this->get_sync_domain()!=NULL)
			{
				this->get_sync_domain()->associated_module_deleted=true;
			}
			sca_get_curr_simcontext()->get_sca_object_manager()->remove_module(this); 
		}
	}


}

//////////////////////////////////////////////////////////////////

const char* sca_module::kind() const
{
	return "sca_core::sca_module";
}


void sca_module::set_solver_parameter(const std::string& par,const std::string& val)
{
	if(sync_domain!=NULL)
	{
		sync_domain->set_solver_parameter(this,par,val);
	}
	else
	{
		solver_parameter.push_back(par);
		solver_parameter_values.push_back(val);
	}
}




std::string sca_module::get_solver_parameter(const std::string& name) const
{
	for(std::size_t i=0;i<solver_parameter.size();++i)
	{
		if(solver_parameter[i]==name)
		{
			return solver_parameter_values[i];
		}
	}

	static const std::string empty_string="";

	return empty_string;
}


std::string sca_module::get_solver_name() const
{
	if(sync_domain!=NULL)
	{
		return sync_domain->kind();
	}

	return "";

}


void sca_module::set_timestep(const sca_core::sca_time& tstep)
{
	if(timestep_elaborated)
	{
		std::ostringstream str;
		str << "The module timestep must set before end_of_elaboration for module: "
		    << name();
		SC_REPORT_ERROR("SystemC-AMS",str.str().c_str());
	}

	timestep_set=tstep;
	timestep_is_set=true;
}

void sca_module::set_timestep(double tstep, sc_core::sc_time_unit unit)
{
	if(timestep_elaborated)
	{
		std::ostringstream str;
		str << "The module timestep must set before end_of_elaboration for module: "
		    << name();
		SC_REPORT_ERROR("SystemC-AMS",str.str().c_str());
	}

	timestep_set=sca_core::sca_time(tstep,unit);
	timestep_is_set=true;
}



void sca_module::set_max_timestep(const sca_core::sca_time& tstep)
{
	if(timestep_elaborated)
	{
		std::ostringstream str;
		str << "The module timestep must set before end_of_elaboration for module: "
		    << name();
		SC_REPORT_ERROR("SystemC-AMS",str.str().c_str());
	}

	if(timestep_max_is_set)
	{
		if(tstep<timestep_max_set) timestep_max_set=tstep;
	}
	else
	{
		timestep_max_set=tstep;
	}

	timestep_max_is_set=true;
}

void sca_module::set_max_timestep(double tstep, sc_core::sc_time_unit unit)
{
	set_max_timestep(sca_core::sca_time(tstep,unit));
}


void sca_module::request_first_activation(const sca_core::sca_time& mtime)
{
	if(first_activation_time_requested)
	{
		if(first_activation_time>mtime) first_activation_time=mtime;
	}
	else
	{
		first_activation_time=mtime;
		first_activation_requested=true;
		first_activation_time_requested=true;
	}
}

void sca_module::request_first_activation(const sc_core::sc_event& ev)
{
	first_activation_requested=true;
	first_activation_events.push_back(&ev);
}

void sca_module::request_first_activation(const sc_core::sc_event_or_list& ev)
{
	first_activation_requested=true;
	first_activation_event_or_lists.push_back(ev);
}

void sca_module::request_first_activation(const sc_core::sc_event_and_list& ev)
{
	first_activation_requested=true;
	first_activation_event_and_lists.push_back(ev);
}


unsigned long sca_module::register_port(sca_port_base* port)
{
	unsigned long n = (unsigned long)(port_list.size());

	port_list.push_back(port);
	return n;
}

//////////////////////////////////////////////////////////////////

const sca_module::sca_port_base_listT& sca_module::get_port_list() const
{
	return port_list;
}

//////////////////////////////////////////////////////////////////


sca_module::sca_port_base_listT& sca_module::get_port_list()
{
	return port_list;
}

//////////////////////////////////////////////////////////////////

void sca_module::elaborate()
{
	if (view_interface == NULL)
	{
		std::ostringstream str;
		str << "Error: " << name()
				<< " a sca_module must be associated with a concrete view!"
				<< std::endl;
		str << "It is not allowed to instantiate the sca_module base class."
				<< std::endl;
		SC_REPORT_ERROR("SystemC-AMS",str.str().c_str());
	}

	//elaborate all ports (register bound interfaces)
	for (sca_port_base_list_iteratorT pit = get_port_list().begin(); pit
			!= get_port_list().end(); ++pit)
	{
		(*pit)->elaborate_port();
	}

	const std::vector<sc_core::sc_object*>& cobj(this->get_child_objects());

	for(unsigned long i=0;i<cobj.size();i++)
	{
		if(dynamic_cast<sca_core::sca_module*>(cobj[i]) ||
		   dynamic_cast<sca_core::sca_interface*>(cobj[i]))
		{
			std::ostringstream str;
			str << "A " << this->kind() << " ( " << this->name()
			    << " ) can't have child: " <<  cobj[i]->kind()
		        << "( " << cobj[i]->name() << " )  see LRM clause 4.1";
			SC_REPORT_ERROR("SystemC-AMS",str.str().c_str());
		}
	}
}


/////////////////////////////////////////////////////////////////


void sca_module::end_of_sca_elaboration()
{
	if(sync_domain!=NULL)
	{
		//assign solver parameters set before end of elaboration
		for(unsigned long i=0;i<solver_parameter.size();i++)
		{
			sync_domain->set_solver_parameter(this,solver_parameter[i],solver_parameter_values[i]);
		}
	}
	else
	{
		//should not possible
		std::ostringstream str;
		str << "SCA-Module: " << this->name() << " of kind ";
		str << this->kind()   << " without solver";
		SC_REPORT_WARNING("SystemC-AMS",str.str().c_str());
	}
}

/////////////////////////////////////////////////////////////////

sca_solver_base* sca_module::get_sync_domain() const
{
	return sync_domain;
}


sc_dt::int64 sca_module::get_call_counter()
{
	if(get_sync_domain()==NULL) return -2;
	return sync_domain->get_call_counter();
}

long sca_module::get_calls_per_period()
{
	if(get_sync_domain()==NULL) return -2;
	return sync_domain->get_calls_per_period();
}


/**
	 * gets unique id for the current computation cluster
	 * if the module is not (yet) associated to a cluster -1 is returned
	 */
long sca_module::get_cluster_id()
{
	if(get_sync_domain()==NULL) return -1;
	return sync_domain->get_cluster_id();
}

bool* sca_module::get_allow_processing_access_flag_ref()
{
	if(get_sync_domain()==NULL) return NULL;

	return get_sync_domain()->get_allow_processing_access_flag_ref();
}

bool sca_module::is_processing_executing() const
{
	if(get_sync_domain()==NULL) return false;

	bool* fref=get_sync_domain()->get_allow_processing_access_flag_ref();

	if(fref==NULL) return false;

	return *fref;
}


bool sca_module::is_initialize_executing() const
{
	return initialize_executes_flag;
}


bool sca_module::is_reinitialize_executing() const
{
	return reinitialize_executes_flag;
}



bool sca_module::is_set_attributes_executing() const
{
	return set_attributes_executes_flag;
}

bool sca_module::is_change_attributes_executing() const
{
	return change_attributes_executes_flag;
}

bool sca_module::is_ac_processing_executing() const
{
	return ac_processing_executes_flag;
}

bool sca_module::are_attribute_changes_enabled() const
{
	return attribute_changes_allowed_flag;
}



/////////////////////////////////////////////////////////////////

sca_core::sca_time sca_module::sca_get_time() const
{
	if (sync_domain == NULL)
	{
		return NOT_VALID_SCA_TIME();
	}

	if(change_attributes_executes_flag)
	{
		sca_core::sca_time stime=sync_domain->get_cluster_start_time();
		if(stime==NOT_VALID_SCA_TIME()) return sc_core::SC_ZERO_TIME;
		return stime+sync_domain->get_cluster_duration();
	}

	return sync_domain->get_current_time();
}

/////////////////////////////////////////////////////////////////

sca_core::sca_time sca_module::get_timestep() const
{
	sca_core::sca_time rett;

	if (sync_domain == NULL) rett=NOT_VALID_SCA_TIME();
	else
	{

		rett=sync_domain->get_current_period();
	}

	if(rett==NOT_VALID_SCA_TIME())
	{
		std::ostringstream str;
		str << "it is not allowed to call get_timestep before the elaboration has been finished ";
		str << "in module: " << name();

		SC_REPORT_ERROR("SysemC-AMS",str.str().c_str());
	}

	return rett;
}

/////////////////////////////////////////////////////////////////

sca_core::sca_time sca_module::get_max_timestep() const
{

	if(sync_domain == NULL)
	{
		std::ostringstream str;
		str << "it is not allowed to call get_max_timestep before the elaboration has been finished ";
		str << "in module: " << name();

		SC_REPORT_ERROR("SysemC-AMS",str.str().c_str());
		return sc_core::SC_ZERO_TIME;
	}

	return sync_domain->get_max_timestep();
}


/////////////////////////////////////////////////////////////////

long sca_module::sca_next_max_time(::sc_core::sc_time mtime)
{
	if (sync_domain == NULL)
		return -1;
	return sync_domain->set_max_start_time(mtime);
}

/////////////////////////////////////////////////////////////////

long sca_module::sca_next_max_time_step(::sc_core::sc_time ptime)
{
	if (sync_domain == NULL)
		return -1;
	return sync_domain->set_max_period(ptime,this);
}

/////////////////////////////////////////////////////////////////

::sc_core::sc_time sca_module::sca_synchronize_on_event(
		const ::sc_core::sc_event& ev)
{
	if (sync_domain == NULL)
		return ::sc_core::SC_ZERO_TIME;
	return sync_domain->register_activation_event(ev);
}

::sc_core::sc_time sca_module::sca_synchronize_on_event(
		const sca_port_base& iport)
{
	const ::sc_core::sc_interface* intf = iport.sc_get_interface();
	if ((intf == NULL) || (iport.get_port_type()
			!= sca_port_base::SCA_SC_IN_PORT))
	{
		std::ostringstream str;
		str << "Port " << iport.get_port_number() << " in module: "
				<< iport.get_parent_module()->name() << std::endl
				<< "is no converter inport. Setting reactivity is only possible for "
				<< "SystemC to SystemC-AMS ports (e.g. sca_scsdf_in<>)."
				<< std::endl;
		SC_REPORT_ERROR("SystemC-AMS", str.str().c_str());
		return sc_core::SC_ZERO_TIME;
	}

	if (sync_domain == NULL)
		return sc_core::SC_ZERO_TIME;
	return sync_domain->register_activation_event(intf->default_event());
}


/////////////////////////////////////////////////////////////////

} //namespace sca_core
