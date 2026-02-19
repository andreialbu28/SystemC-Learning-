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

 sca_tdf_module.cpp - description

 Original Author: Karsten Einwich Fraunhofer IIS Dresden / COSEDA Technologies

 Created on: 05.08.2009

 SVN Version       :  $Revision: 2082 $
 SVN last checkin  :  $Date: 2020-01-24 13:06:35 +0000 (Fri, 24 Jan 2020) $
 SVN checkin by    :  $Author: karsten $
 SVN Id            :  $Id: sca_tdf_module.cpp 2082 2020-01-24 13:06:35Z karsten $

 *****************************************************************************/

/*****************************************************************************/

#define SC_INCLUDE_DYNAMIC_PROCESSES
#include <systemc-ams>

#include "scams/impl/predefined_moc/tdf/sca_tdf_view.h"
#include "scams/impl/analysis/ac/sca_ac_domain_globals.h"
#include "scams/impl/core/sca_simcontext.h"
#include "scams/impl/core/sca_solver_base.h"


namespace sca_core
{
namespace sca_implementation
{
	const sca_core::sca_time& NOT_VALID_SCA_TIME();
}
}


namespace sca_tdf
{

void sca_module::construct()
{
	//assign module to tdf - view
	view_interface = new sca_tdf::sca_implementation::sca_tdf_view;

	attr_method
			= static_cast<sca_module_method> (&sca_tdf::sca_module::set_attributes);
	init_method
			= static_cast<sca_module_method> (&sca_tdf::sca_module::initialize);
	sig_proc_method
			= static_cast<sca_module_method> (&sca_tdf::sca_module::processing);

	ac_processing_method= static_cast<sca_module_method> (&sca_tdf::sca_module::ac_dummy);

	//obsolete
	post_method
			= static_cast<sca_module_method> (&sca_tdf::sca_module::post_proc);

	// new SystemC-AMS 2.0 dtdf extension

	reinitialize_method
			= static_cast<sca_module_method> (&sca_tdf::sca_module::reinitialize);

	change_attributes_method=
			static_cast<sca_module_method> (&sca_tdf::sca_module::change_attributes);

	 reg_attr_method=false;
	 reg_init_method=false;
	 reg_processing_method=false;
	 reg_ac_processing_method=false;
	 reg_post_method=false;

	 no_default_processing=false;
	 no_default_ac_processing=false;
	 no_change_attributes=false;
	 no_reinitialize=false;

	 check_for_processing=false;

	 does_attribute_changes_flag=false;
	 accept_attribute_changes_flag=false;

	 last_does_attribute_changes_flag=false;
	 last_accept_attribute_changes_flag=false;

	 has_executed_attribute_change_method=false;
	 port_attribute_validation_required=false;


	 //should be not be -1 (set_attributes) and not 0
	 does_attr_ch_id=-2;
	 accept_attr_ch_id=-2;

	 call_counter=NULL;
}

sca_module::sca_module(const sc_core::sc_module_name& nm) : sca_core::sca_module(nm)
{
	construct();
}

sca_module::sca_module() : sca_core::sca_module()
{
	construct();
}

sca_module::sca_module(const char* nm) : sca_core::sca_module(nm)
{
	construct();
}


sca_module::sca_module(const std::string&  nm) : sca_core::sca_module(nm)
{
	construct();
}




void sca_module::validate_port_attributes()
{
	if(!port_attribute_validation_required) return;
	port_attribute_validation_required=false;

	for (sca_port_base_list_iteratorT pit = get_port_list().begin(); pit
				!= get_port_list().end(); ++pit)
	{
			(*pit)->validate_port_attributes();
	}

}

void sca_module::ac_processing()
{
	(this->*ac_processing_method)();
}

void sca_module::elaborate()
{
	sca_core::sca_module::elaborate(); // call base class method

	//check for not allowed SystemC objects
	std::vector<sc_core::sc_object*> objl=get_child_objects();
	for(unsigned long i=0;i<objl.size();i++)
	{
		if(dynamic_cast<sc_core::sc_port_base*>(objl[i])!=NULL)
		{
			if(dynamic_cast<sca_core::sca_implementation::sca_port_base*>(objl[i])==NULL)
			{
				std::ostringstream str;
				str << " A sca_tdf::sca_module can contain sca_tdf ports only, port: " << objl[i]->name();
				str << " in module: " << name()
					<< " is not an sca_tdf::sca_in/out or sca_tdf::sc_in/out port";
				SC_REPORT_ERROR("SystemC-AMS",str.str().c_str());
			}
		}

		if(dynamic_cast<sc_core::sc_module*>(objl[i])!=NULL)
		{
			if(objl[i]->kind()!=std::string("sca_core::sca_implementation::systemc_ams_initializer"))
			{
				std::ostringstream str;
				str << " A sca_tdf::sca_module can not contain other modules. The module: "
				    << objl[i]->basename() << " kind: " << objl[i]->kind();
				str << " is instantiated in the context of module: " << name();
				SC_REPORT_ERROR("SystemC-AMS",str.str().c_str());
			}
		}
	}
}

//////////////////////////////////////////////

sca_module::~sca_module()
{
	if(view_interface!=NULL) delete view_interface;
	view_interface=NULL;
}

//////////////////////////////////////////////

void sca_module::register_attr_method(sca_module_method funcp)
{
	if(sca_core::sca_implementation::sca_get_curr_simcontext()->construction_finished())
	{
		std::ostringstream str;
		str << "register_attr_method can be only called during module construction for ";
		str << "module: " << name();
		SC_REPORT_ERROR("SystemC-AMS",str.str().c_str());
	}

	if(reg_attr_method)
	{
		std::ostringstream str;
		str << "only one register_attr_method can be registered for ";
		str << "module: " << name();
		SC_REPORT_ERROR("SystemC-AMS",str.str().c_str());
	}
	reg_attr_method=true;


	attr_method = funcp;
}

//////////////////////////////////////////////

void sca_module::register_init_method(sca_module_method funcp)
{
	if(sca_core::sca_implementation::sca_get_curr_simcontext()->construction_finished())
	{
		std::ostringstream str;
		str << "register_init_method can be only called during module construction for ";
		str << "module: " << name();
		SC_REPORT_ERROR("SystemC-AMS",str.str().c_str());
	}

	if(reg_init_method)
	{
		std::ostringstream str;
		str << "only one register_init_method can be registered for ";
		str << "module: " << name();
		SC_REPORT_ERROR("SystemC-AMS",str.str().c_str());
	}
	reg_init_method=true;


	init_method = funcp;
}

//////////////////////////////////////////////

void sca_module::register_processing(sca_module_method funcp)
{
	if(sca_core::sca_implementation::sca_get_curr_simcontext()->construction_finished())
	{
		std::ostringstream str;
		str << "register_processing can be only called during module construction for ";
		str << "module: " << name();
		SC_REPORT_ERROR("SystemC-AMS",str.str().c_str());
	}

	check_for_processing=true;
	try { processing();}
	catch(...){}
	check_for_processing=false;

	if(!no_default_processing)
	{
		std::ostringstream str;
		str << "register_processing is called, however method processing was implemented for ";
		str << "module: " << name();
		SC_REPORT_ERROR("SystemC-AMS",str.str().c_str());
	}

	if(reg_processing_method)
	{
		std::ostringstream str;
		str << "only one register_processing_method can be registered for ";
		str << "module: " << name();
		SC_REPORT_ERROR("SystemC-AMS",str.str().c_str());
	}
	reg_processing_method=true;

	sig_proc_method = funcp;
}

//////////////////////////////////////////////


void sca_module::register_ac_processing(sca_tdf::sca_module::sca_module_method funcp)
{
	if(sca_core::sca_implementation::sca_get_curr_simcontext()->construction_finished())
	{
		std::ostringstream str;
		str << "register_ac_processing can be only called during module construction for ";
		str << "module: " << name();
		SC_REPORT_ERROR("SystemC-AMS",str.str().c_str());
	}

	//ignore errors
	try  { ac_processing(); }
	catch(...){}

	if(!no_default_ac_processing)
	{
		std::ostringstream str;
		str << "register_ac_processing is called, however method ac_processing was implemented for ";
		str << "module: " << name();
		SC_REPORT_ERROR("SystemC-AMS",str.str().c_str());
	}


	if(reg_ac_processing_method)
	{
		std::ostringstream str;
		str << "only one register_ac_processing_method can be registered for ";
		str << "module: " << name();
		SC_REPORT_ERROR("SystemC-AMS",str.str().c_str());
	}
	reg_ac_processing_method=true;

	ac_processing_method=funcp;
}

void sca_module::register_post_method(sca_module_method funcp)
{
	if(sca_core::sca_implementation::sca_get_curr_simcontext()->construction_finished())
	{
		std::ostringstream str;
		str << "register_post_method can be only called during module construction for ";
		str << "module: " << name();
		SC_REPORT_ERROR("SystemC-AMS",str.str().c_str());
	}

	if(reg_post_method)
	{
		std::ostringstream str;
		str << "only one register_post_method can be registered for ";
		str << "module: " << name();
		SC_REPORT_ERROR("SystemC-AMS",str.str().c_str());
	}
	reg_post_method=true;

	post_method = funcp;
}

const char* sca_module::kind() const
{
	return "sca_tdf::sca_module";
}

void sca_module::set_attributes()
{
}

// new SystemC-AMS 2.0 dtdf extension
void sca_module::change_attributes()
{
	no_change_attributes=true;
}

void sca_module::reinitialize()
{
	no_reinitialize=true;
}

void sca_module::initialize()
{
}



void sca_module::processing()
{
	 no_default_processing=true;

	 if(!check_for_processing)
	 {
		check_for_processing=true;
		std::ostringstream str;
		str << "No processing method implemented for TDF ";
		str << "module: " << name();
		SC_REPORT_WARNING("SystemC-AMS",str.str().c_str());

	 }
}

void sca_module::ac_dummy()
{
	 no_default_ac_processing=true;
}

sca_core::sca_time sca_module::get_time() const
{
	return sca_core::sca_module::sca_get_time();
}

sca_core::sca_time sca_module::get_timestep() const
{
	return sca_core::sca_module::get_timestep();
}

sca_core::sca_time sca_module::get_max_timestep() const
{
	return sca_core::sca_module::get_max_timestep();
}

// new SystemC-AMS 2.0 dtdf extension
void sca_module::request_next_activation(const sca_core::sca_time& dt)
{
	if(is_set_attributes_executing())
	{
		request_first_activation(dt);
		return;
	}


	if(!is_change_attributes_executing())
	{
		std::ostringstream str;
		str << "request_next_activation can only be called from the context of ";
		str << "the tdf module callbacks set_attributes  or change_attributes ";
		str << "in module: " << name();
		SC_REPORT_ERROR("SystemC-AMS",str.str().c_str());
	}

	has_executed_attribute_change_method=true;

	sync_domain->request_next_activation_abs(dt+get_time());
}


void sca_module::request_next_activation(double dt,sc_core::sc_time_unit tu)
{
	request_next_activation(sca_core::sca_time(dt,tu));
}

void sca_module::request_next_activation(const sc_core::sc_event& ev)
{
	if(is_set_attributes_executing())
	{
		request_first_activation(ev);
		return;
	}

	if(!is_change_attributes_executing())
	{
		std::ostringstream str;
		str << "request_next_activation can only be called from the context of ";
		str << "the tdf module callbacks set_attributes or change_attributes ";
		str << "in module: " << name();
		SC_REPORT_ERROR("SystemC-AMS",str.str().c_str());
	}


	has_executed_attribute_change_method=true;

	sync_domain->request_next_activation(ev);
}

void sca_module::request_next_activation(const sca_core::sca_time& dt,const sc_core::sc_event& ev)
{
	request_next_activation(ev);
	request_next_activation(dt);
}


void sca_module::request_next_activation(double dt,sc_core::sc_time_unit tu,const sc_core::sc_event& ev)
{
	request_next_activation(ev);
	request_next_activation(dt,tu);
}




void sca_module::request_next_activation(const sc_core::sc_event_or_list& or_list)
{
	if(is_set_attributes_executing())
	{
		request_first_activation(or_list);
		return;
	}

	if(!is_change_attributes_executing())
	{
		std::ostringstream str;
		str << "request_next_activation can only be called from the context of ";
		str << "the tdf module callbacks set_attributes or change_attributes ";
		str << "in module: " << name();
		SC_REPORT_ERROR("SystemC-AMS",str.str().c_str());
	}


	has_executed_attribute_change_method=true;

	sync_domain->request_next_activation(or_list);
}


void sca_module::request_next_activation(const sc_core::sc_event_and_list& and_list)
{
	if(is_set_attributes_executing())
	{
		request_first_activation(and_list);
		return;
	}

	if(!is_change_attributes_executing())
	{
		std::ostringstream str;
		str << "request_next_activation can only be called from the context of ";
		str << "the tdf module callbacks set_attributes or change_attributes ";
		str << "in module: " << name();
		SC_REPORT_ERROR("SystemC-AMS",str.str().c_str());
	}


	has_executed_attribute_change_method=true;

	sync_domain->request_next_activation(and_list);

}


void sca_module::request_next_activation(const sca_core::sca_time& timeout,const sc_core::sc_event_or_list& or_list)
{
	request_next_activation(or_list);
	request_next_activation(timeout);
}


void sca_module::request_next_activation(const sca_core::sca_time& timeout,const sc_core::sc_event_and_list& and_list)
{
	request_next_activation(and_list);
	request_next_activation(timeout);
}


void sca_module::request_next_activation(double dtime,sc_core::sc_time_unit tunit,const sc_core::sc_event_or_list& or_list)
{
	request_next_activation(sca_core::sca_time(dtime,tunit),or_list);
}


void sca_module::request_next_activation(double dtime,sc_core::sc_time_unit tunit,const sc_core::sc_event_and_list& and_list)
{
	request_next_activation(sca_core::sca_time(dtime,tunit),and_list);
}



void sca_module::set_max_timestep(const sca_core::sca_time& tp)
{
	if(!is_change_attributes_executing())
	{
		sca_core::sca_module::set_max_timestep(tp);
		return;
	}

	has_executed_attribute_change_method=true;

	sync_domain->set_max_period(tp,this);

}

sca_core::sca_time sca_module::get_last_timestep() const
{
	if (sync_domain == NULL) return sca_core::sca_implementation::NOT_VALID_SCA_TIME();

	return sync_domain->get_last_period();
}

bool sca_module::is_timestep_changed() const
{
	//one resolution timestep is no change
	sca_core::sca_time ctime=this->get_timestep();
	sca_core::sca_time ltime=this->get_last_timestep();

	if((ctime-ltime)<=sc_core::sc_get_time_resolution()) return false;
	if((ltime-ctime)<=sc_core::sc_get_time_resolution()) return false;
	return true;
}


void sca_module::set_max_timestep(double dt,sc_core::sc_time_unit tu)
{
	set_max_timestep(sca_core::sca_time(dt,tu));
}


void sca_module::set_timestep(const sca_core::sca_time& tp)
{
	if(!is_change_attributes_executing())
	{
		sca_core::sca_module::set_timestep(tp);
		return;
	}

	has_executed_attribute_change_method=true;

	sync_domain->change_timestep(tp,this);
}

void sca_module::set_timestep(double dt,sc_core::sc_time_unit tu)
{
	set_timestep(sca_core::sca_time(dt,tu));
}



void sca_module::does_attribute_changes()
{
	if(!does_attribute_changes_flag)
	{
		if(get_sync_domain()!=NULL) //before init check will be always performed
			get_sync_domain()->attribute_change_flags_changed();


		if( (this->call_counter!=NULL) && (*(this->call_counter)>1))
		{
			if(this->does_attr_ch_id!=*(this->call_counter))
			{
				this->does_attr_ch_id=*(this->call_counter);
				last_does_attribute_changes_flag=does_attribute_changes_flag;
			}
		}
		else
		{
			last_does_attribute_changes_flag=true;
		}
	}

	does_attribute_changes_flag=true;
}

void sca_module::does_no_attribute_changes()
{
	if(does_attribute_changes_flag)
	{
		if(get_sync_domain()!=NULL) //before init check will be always performed
			get_sync_domain()->attribute_change_flags_changed();

		if( (this->call_counter!=NULL) && (*(this->call_counter)>1))
		{
			if(this->does_attr_ch_id!=*(this->call_counter))
			{
				this->does_attr_ch_id=*(this->call_counter);
				last_does_attribute_changes_flag=does_attribute_changes_flag;
			}
		}
		else
		{
			last_does_attribute_changes_flag=false;
		}
	}

	does_attribute_changes_flag=false;
}

void sca_module::accept_attribute_changes()
{
	if(!accept_attribute_changes_flag)
	{
		if(get_sync_domain()!=NULL) //before init check will be always performed
			get_sync_domain()->attribute_change_flags_changed();


		if( (this->call_counter!=NULL) && (*(this->call_counter)>1))
		{
			if(this->accept_attr_ch_id!=*(this->call_counter))
			{
				this->accept_attr_ch_id=*(this->call_counter);
				last_accept_attribute_changes_flag=accept_attribute_changes_flag;
			}
		}
		else
		{
			last_accept_attribute_changes_flag=true;
		}
	}

	accept_attribute_changes_flag=true;
}


void sca_module::reject_attribute_changes()
{
	if(accept_attribute_changes_flag)
	{
		if(get_sync_domain()!=NULL) //before init check will be always performed
			get_sync_domain()->attribute_change_flags_changed();

		if( (this->call_counter!=NULL) && (*(this->call_counter)>1))
		{
			if(this->accept_attr_ch_id!=*(this->call_counter))
			{
				this->accept_attr_ch_id=*(this->call_counter);
				last_accept_attribute_changes_flag=accept_attribute_changes_flag;
			}
		}
		else
		{
			last_accept_attribute_changes_flag=false;
		}
	}

	accept_attribute_changes_flag=false;
}


bool sca_module::is_dynamic() const
{
	return get_sync_domain()->is_dynamic_tdf();
}

bool sca_module::are_attribute_changes_allowed() const
{
	//during change attributes the previous state is used
	if(is_change_attributes_executing())
	{
		//no update in the current call
		if(*(this->call_counter)!=this->does_attr_ch_id)
		{
			this->does_attr_ch_id=*(this->call_counter);
			last_does_attribute_changes_flag=does_attribute_changes_flag;
		}

		if(*(this->call_counter)!=this->accept_attr_ch_id)
		{
			this->accept_attr_ch_id=*(this->call_counter);
			last_accept_attribute_changes_flag=accept_attribute_changes_flag;
		}


		//if the module does not allow changes - changes are not allowed
		if(!last_does_attribute_changes_flag) return false;

		if(get_sync_domain()->are_attribute_changes_allowed())  return true;
		else if(get_sync_domain()->are_attribute_changes_may_allowed())
		{
			//if this module is the only one which does not accepts changes
			//this module will able to change attributes
			if(!last_accept_attribute_changes_flag) return true;
		}

	}
	else
	{
		//if the module does not allow changes - changes are not allowed
		if(!does_attribute_changes_flag) return false;

		if(get_sync_domain()->are_attribute_changes_allowed())  return true;
		else if(get_sync_domain()->are_attribute_changes_may_allowed())
		{
			//if this module is the only one which does not accepts changes
			//this module will able to change attributes
			if(!accept_attribute_changes_flag) return true;
		}
	}

	return false;
}



bool sca_module::is_port_attribute_changed() const
{

	for(sca_port_base_listT::const_iterator pit=this->get_port_list().begin();
			pit!=this->get_port_list().end();
			++pit)
	{
		sca_tdf::sca_implementation::sca_port_attributes* attr;
		attr=dynamic_cast<sca_tdf::sca_implementation::sca_port_attributes* >(*pit);
		if(attr!=NULL)
		{
			if(attr->is_rate_changed_internal()) return true;
			if(attr->is_delay_changed_internal()) return true;
			if(attr->is_timestep_changed_internal(0)) return true;
		}
	}

	return false;
}



bool sca_module::are_attributes_changed() const
{
	return is_timestep_changed() || is_port_attribute_changed();
}


}
//////////////////////////////////////////////


