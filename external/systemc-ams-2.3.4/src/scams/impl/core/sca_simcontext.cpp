/*****************************************************************************

    Copyright 2010-2013
    Fraunhofer-Gesellschaft zur Foerderung der angewandten Forschung e.V.


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

 sca_simcontext.cpp - description

 Original Author: Karsten Einwich Fraunhofer IIS/EAS Dresden

 Created on: 13.05.2009

 SVN Version       :  $Revision: 2361 $
 SVN last checkin  :  $Date: 2023-07-06 15:00:20 +0000 (Thu, 06 Jul 2023) $
 SVN checkin by    :  $Author: pehrlich $
 SVN Id            :  $Id: sca_simcontext.cpp 2361 2023-07-06 15:00:20Z pehrlich $

 *****************************************************************************/

/*****************************************************************************/

#include <systemc-ams>
#include "scams/impl/core/sca_globals.h"

//#include "scams/impl/util/sca_trace_base.h"
#include "scams/impl/core/sca_simcontext.h"

#include "scams/impl/core/sca_object_manager.h"
#include "scams/impl/core/sca_solver_manager.h"
#include "scams/impl/core/sca_solver_base.h"


#include<string>
#include <iostream>

//STL usage
#include <string>

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#ifndef PACKAGE_VERSION
#define PACKAGE_VERSION "????"
#endif

#ifndef REVISION
#define REVISION "$Revision: 2361 $"
#endif

#define SCA_RELEASE_DATE    SCA_VERSION_RELEASE_DATE

using namespace sca_core;
using namespace sca_core::sca_implementation;

namespace sca_core
{
namespace sca_implementation
{

static const sca_core::sca_time not_valid_sca_time;

const sca_core::sca_time& NOT_VALID_SCA_TIME()
{
	//IMPROVE: find a better solution, which may not sets the resolution fixed flag
	//after we set the time, the time resolution can't be changed
	//(except the value is zero) -thus we are not able to initialize the constant

	if(not_valid_sca_time==sc_core::SC_ZERO_TIME)
	(*const_cast<sca_core::sca_time*>(&not_valid_sca_time))=
		sc_core::SC_ZERO_TIME-sc_core::sc_get_time_resolution();

	return not_valid_sca_time;
}

// Not MT-safe!
static sca_simcontext* sca_curr_simcontext = 0;
static bool            sca_simulation_finished=false;

class systemc_ams_initializer : sc_core::sc_module
{
	void end_of_elaboration();
	void start_of_simulation();
	void end_of_simulation();

	const char* kind() const
	{
		return "sca_core::sca_implementation::systemc_ams_initializer";
	}


public:

	bool construction_finished;
	bool elaboration_finished;

	systemc_ams_initializer(sc_core::sc_module_name)
	{
		sca_curr_simcontext=NULL;
		construction_finished=false;
		elaboration_finished=false;
	}

	//using this may we are later able to provide a solution without
	//static variable
	sca_simcontext* sca_curr_simcontext;
};


void systemc_ams_initializer::end_of_elaboration()
{
	construction_finished=true;
	sca_core::sca_implementation::sca_systemc_ams_init();
	elaboration_finished=true;
}


void systemc_ams_initializer::start_of_simulation()
{
#ifndef DISABLE_PERFORMANCE_STATISTICS
	sca_curr_simcontext->elaboration_duration=std::chrono::high_resolution_clock::now() - sca_curr_simcontext->start;
#endif
}


void systemc_ams_initializer::end_of_simulation()
{

#ifndef DISABLE_PERFORMANCE_STATISTICS

	sca_curr_simcontext->duration=std::chrono::high_resolution_clock::now() - sca_curr_simcontext->start;

#endif

	if(sca_curr_simcontext!=NULL)
	{
		std::vector<sca_solver_base*>& solvers=
				sca_curr_simcontext->get_sca_solver_manager()->get_solvers();

		for(unsigned int i=0;i<solvers.size();i++)
		{
			if(solvers[i]!=NULL)
			{
				sca_synchronization_obj_if* sif;
				sif=dynamic_cast<sca_synchronization_obj_if*>(solvers[i]);

				if(sif!=NULL)
				{
					sif->terminate();
				}

				solvers[i]->print_post_solve_statisitcs();
			}
		}

		sca_curr_simcontext->get_sca_object_manager()->finish_simulation();

	}

#ifndef DISABLE_PERFORMANCE_STATISTICS

	unsigned long& info_mask(
	            sca_core::sca_implementation::sca_get_curr_simcontext()->
	            get_information_mask());

	if((info_mask & sca_util::sca_info::sca_tdf_solver.mask))
	{
		double overall_sec=sca_curr_simcontext->duration.count() ;
		double elaboration_sec=sca_curr_simcontext->elaboration_duration.count() ;

		std::ostringstream str;
		str << std::endl;
		str << "\tOverall wall clock time     : " << overall_sec     << " s" << std::endl;
		str << "\tElaboration wall clock time : "  << elaboration_sec << " s / "
			<< (int)((elaboration_sec/overall_sec)*100.0+0.5)<< "%" << std::endl;
		SC_REPORT_INFO("SystemC-AMS",str.str().c_str());
	}

#endif
}

sca_simcontext* sca_get_curr_simcontext()
{
	if (sca_curr_simcontext == 0)
	{
		//after simulation start the simcontext cannot created anymore
		if(sc_core::sc_end_of_simulation_invoked()||sca_simulation_finished)
		{
			return NULL;
		}

#ifdef PURIFY
		static sca_simcontext sca_default_global_context;
		sca_curr_simcontext = &sca_default_global_context;
#else
		sca_curr_simcontext = new sca_simcontext;
#endif
	}

	/**
	 * if the SystemC simcontext was re-created -> recreate SystemC-AMS simcontext also
	 */
	if(sc_core::sc_get_curr_simcontext()!=sca_curr_simcontext->get_sc_simcontext())
	{
		delete sca_curr_simcontext;
		sca_curr_simcontext = new sca_simcontext;
	}

	return sca_curr_simcontext;
}



//////////////////////////////////////////////////////////////////////////////////

sca_object_manager* sca_simcontext::get_sca_object_manager()
{
	return m_sca_object_manager;
}

//////////////////////////////////////////////////////////////////////////////////

sca_solver_manager* sca_simcontext::get_sca_solver_manager()
{
	return m_sca_solver_manager;
}

//////////////////////////////////////////////////////////////////////////////////

sca_view_manager* sca_simcontext::get_sca_view_manager()
{
	return m_sca_view_manager;
}

//////////////////////////////////////////////////////////////////////////////////

#if !defined(SC_DISABLE_COPYRIGHT_MESSAGE)
#  define SYSTEMC_AMS_DISABLE_COPYRIGHT_MESSAGE 0
#endif

static bool& sca_get_lnp()
{
    static bool lnp = false;
    return lnp;
}

void sca_disable_copyright_message()
{
    sca_get_lnp()=true;
}

void sca_simcontext::sca_pln()
{
	bool& lnp = sca_get_lnp();

	if ( lnp || getenv("SYSTEMC_AMS_DISABLE_COPYRIGHT_MESSAGE") != 0 )
	{
	        lnp = true;
	}


	if (!lnp)
	{
#ifdef REVISION
		std::string revision(REVISION);
#else
		std::string revision;
#endif
		std::string::size_type br;
		while ((br = revision.find("$")) != std::string::npos)
			revision.erase(br, 1);
		while ((br = revision.find("Revision:")) != std::string::npos)
			revision = revision.substr(br + sizeof("Revision:"));

		std::cerr << std::endl << std::endl;
		std::cerr << "        SystemC AMS extensions " << sca_core::sca_release()
				<< " Release date: " << SCA_RELEASE_DATE  << " " << revision << std::endl;

		std::cerr << sca_copyright_string << std::endl << std::endl << std::endl;


		lnp = true;
	}
}

//////////////////////////////////////////////////////////////////


bool sca_simcontext::initialized()
{
	bool ret_val;
	if (m_sca_object_manager == NULL)
		ret_val = false;
	else
		ret_val = m_sca_object_manager->initialized();

	return ret_val;
}

//////////////////////////////////////////////////////////////////


sca_simcontext::sca_simcontext()
{
#ifndef DISABLE_PERFORMANCE_STATISTICS

	collect_profile_data=true;

	if(this->collect_profile_data)
	{
		start=std::chrono::high_resolution_clock::now();
	}

#else
	collect_profile_data=false;
#endif

	sca_pln();

	time_domain_simulation_flag = true; //default time domain simulation

	m_sca_object_manager = new sca_object_manager; //create objectmanager

	m_sca_solver_manager = m_sca_object_manager->solvers;
	m_sca_view_manager = m_sca_object_manager->views;

	//establish reference to main simcontext
	sc_kernel_simcontext = sc_core::sc_get_curr_simcontext();

	//if no traces available, all traces are initialized
	traces_initialized=true;

	information_mask=~0;  //default all information on

	ac_db=NULL;

	scams_init=new systemc_ams_initializer(sc_core::sc_gen_unique_name("sca_implementation"));
	scams_init->sca_curr_simcontext=this;

}

//////////////////////////////////////////////////////////////////

unsigned long& sca_simcontext::get_information_mask()
{
  return information_mask;
}

bool& sca_simcontext::all_traces_initialized()
{
	return traces_initialized;
}

void sca_simcontext::initialize_all_traces()
{
	//for the case we have digital traces only, we must be able to initialize, also
	//without any ams cluster

	if (!traces_initialized)
	{
		traces_initialized = true;

		std::vector<sca_util::sca_implementation::sca_trace_file_base*>* traces;
		traces=get_trace_list();

		for (std::vector<sca_util::sca_implementation::sca_trace_file_base*>::iterator
					it = traces-> begin(); it != traces->end(); it++)
				(*it)->initialize();
	}
}


double sca_simcontext::get_consumed_wallclock_time_in_sec()
{
#ifndef DISABLE_PERFORMANCE_STATISTICS
	return this->duration.count();
#endif

	return 0.0;
}


void sca_simcontext::enable_performance_data_collection()
{
	this->collect_profile_data=true;
}

void sca_simcontext::disable_performance_data_collection()
{
	this->collect_profile_data=false;
}


bool sca_simcontext::is_performance_data_collection_enabled()
{
	return this->collect_profile_data;
}

//////////////////////////////////////////////////////////////////

sca_simcontext::~sca_simcontext()
{

	if(m_sca_object_manager!=NULL) 
	{
		delete m_sca_object_manager;
		m_sca_object_manager=NULL;
	}
	//delete scams_init; //we cant destroy a module
	                     //addtionally the end_of_simulation callback
	                     //of this modules calls the sca_simcontext
	                     //destructor

	
	sca_curr_simcontext = NULL; //will not work ifdef PURIFY
	sca_simulation_finished=true;
}

sc_core::sc_object* sca_simcontext::get_current_context()
{
	return m_sca_object_manager==NULL?NULL:m_sca_object_manager->get_current_context();
}


/**
 * returns the cluster id of the current context (see above)
 * if no context active or not yet clustered -1 is returned
 */
long sca_simcontext::get_current_context_cluster_id()
{
	sc_core::sc_object* obj=get_current_context();
	if(obj==NULL) return -1;
	sca_core::sca_module* scamod=dynamic_cast<sca_core::sca_module*>(obj);

	if(scamod!=NULL) return scamod->get_cluster_id();

	sca_core::sca_implementation::sca_solver_base* scasolv=dynamic_cast<sca_core::sca_implementation::sca_solver_base*>(obj);

	if(scasolv!=NULL) return scasolv->get_cluster_id();

	return -1;
}


//////////////////////////////////////////////////////////////////

std::vector<sca_util::sca_implementation::sca_trace_file_base*>* sca_simcontext::get_trace_list()
{
	return &(m_sca_object_manager->trace_list);
}


//////////////////////////////////////////////////////////////////

bool sca_simcontext::construction_finished()
{
	return scams_init->construction_finished;
}


//////////////////////////////////////////////////////////////////

bool sca_simcontext::elaboration_finished()
{
	return scams_init->elaboration_finished;
}


//////////////////////////////////////////////////////////////////

bool sca_simcontext::time_domain_simulation()
{
	return time_domain_simulation_flag;
}

//////////////////////////////////////////////////////////////////

void sca_simcontext::set_no_time_domain_simulation()
{
	time_domain_simulation_flag = false;
}

//////////////////////////////////////////////////////////////////

void sca_simcontext::set_time_domain_simulation()
{
	time_domain_simulation_flag = true;
}

//////////////////////////////////////////////////////////////////

}
}
