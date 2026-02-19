/*****************************************************************************

    Copyright 2010
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

 sca_object_manager.cpp - description

 Original Author: Karsten Einwich Fraunhofer IIS/EAS Dresden

 Created on: 14.05.2009

 SVN Version       :  $Revision: 2361 $
 SVN last checkin  :  $Date: 2023-07-06 15:00:20 +0000 (Thu, 06 Jul 2023) $
 SVN checkin by    :  $Author: pehrlich $
 SVN Id            :  $Id: sca_object_manager.cpp 2361 2023-07-06 15:00:20Z pehrlich $

 *****************************************************************************/

/*****************************************************************************/

#include "scams/impl/core/sca_object_manager.h"


#include "systemc-ams"


#include "scams/impl/core/sca_view_manager.h"
#include "scams/impl/core/sca_solver_manager.h"
#include "scams/impl/core/sca_view_object.h"
#include "scams/impl/core/sca_globals.h"
#include "scams/impl/synchronization/sca_synchronization_layer.h"
#include "scams/impl/core/sca_simcontext.h"



#include "sca_solver_base.h"

#include<systemc>

//STL usage
#include <string>
#include <algorithm>
#include <functional>

//RTI usage
#include <typeinfo>
using namespace std;

namespace sca_core
{
namespace sca_implementation
{

//////////////////////////////////////////////////////////////////


sca_object_manager::sca_object_manager()
{
	sc_kernel_object_manager = sc_core::sc_get_curr_simcontext()->get_object_manager();

	views = new sca_view_manager();
	solvers = new sca_solver_manager();
	synchronization = new sca_synchronization_layer();

	init_done = false;

	object_deleted=false;
}

//////////////////////////////////////////////////////////////////

sca_object_manager::~sca_object_manager()
{
	sc_kernel_object_manager = NULL;

	delete synchronization;
	synchronization=NULL;

	delete solvers;
	solvers=NULL;

	delete views;
	views=NULL;

	init_done = false;
}

sc_core::sc_object* sca_object_manager::get_current_context()
{
	return synchronization==NULL?NULL:synchronization->get_current_context();
}

//////////////////////////////////////////////////////////////////

void sca_object_manager::set_default_solver_parameter(
		std::string solver,
		std::string par,
		std::string value
		)
{
	solver_default_parameter[solver].push_back(std::make_pair(par,value));
}


//////////////////////////////////////////////////////////////////

std::string sca_object_manager::get_default_solver_parameter(
		const std::string& solver_name, const std::string& parameter_name) const
{
	solver_default_parameter_listT::const_iterator it;
	it=solver_default_parameter.find(solver_name);
	if(it!=solver_default_parameter.end())
	{
		for(std::size_t i=0;i<it->second.size();++i)
		{
			if(it->second[i].first==parameter_name)
			{
				return it->second[i].second;
			}
		}
	}

	return "";
}



std::vector<std::string> sca_object_manager::get_solver_parameter_names(
		const std::string& solver_name) const
{
	std::vector<std::string> paras;

	solver_default_parameter_listT::const_iterator it;
	it=solver_default_parameter.find(solver_name);
	if(it!=solver_default_parameter.end())
	{
		for(std::size_t i=0;i<it->second.size();++i)
		{
			paras.push_back(it->second[i].first);
		}
	}

	return paras;
}



std::vector<std::string> sca_object_manager::get_solver_names() const
{
	std::vector<std::string> solv;

	for(sca_core::sca_implementation::sca_solver_manager::sca_solver_listT::const_iterator
			it=solvers->get_solvers().begin();it!=solvers->get_solvers().end();++it)
	{
		solv.push_back((*it)->kind());
	}

	return solv;
}


//////////////////////////////////////////////////////////////////

void sca_object_manager::insert_module(sca_module* module)
{
	//store the module in a global list
	sca_module_list.push_back(module);
}

//////////////////////////////////////////////////////////////////

bool sca_object_manager::remove_module(
		sca_module_list_iteratorT instance_number)
{
	if ((instance_number < sca_module_list.begin()) || (instance_number
			>= sca_module_list.end()))
		return false;
	sca_module_list.erase(instance_number);

	object_deleted=true;

	return true;
}

//////////////////////////////////////////////////////////////////


class comp_module_names
{
public:

	using result_type = bool;
	using first_argument_type = sca_module*;
	using second_argument_type = const char*;


	bool operator()(sca_module* module, const char* name) const
	{
		if (string(module->name()) == string(name))
			return true;
		else
			return false;
	}
};

sca_module_list_iteratorT sca_object_manager::sca_find_module(const char* name)
{
	sca_module_list_iteratorT instance;

	instance = find_if(sca_module_list.begin(), sca_module_list.end(),
	        std::bind(comp_module_names(),std::placeholders::_1,name)
	        );

	//if(instance==sca_module_list.end()) return NULL;      //module not found
	//else
	return instance;

}

//////////////////////////////////////////////////////////////////


bool sca_object_manager::remove_module(const char* name)
{
	sca_module_list_iteratorT instance;

	instance = sca_find_module(name);

	if (instance != sca_module_list.end())
	{
		remove_module(instance);
		return true;
	}

	return false; //module not found
}

//////////////////////////////////////////////////////////////////


bool sca_object_manager::remove_module(sca_module* module)
{
	sca_module_list_iteratorT instance;

	instance = find(sca_module_list.begin(), sca_module_list.end(), module);

	if (instance != sca_module_list.end())
	{
		remove_module(instance);
		return true;
	}
	return false; //module not found
}

//////////////////////////////////////////////////////////////////

void sca_object_manager::insert_interface(sca_interface* interf)
{
	sca_interface_list.push_back(interf);
}

//////////////////////////////////////////////////////////////////

bool sca_object_manager::remove_interface(
		sca_interface_list_iteratorT interface_number)
{
	//no valid module
	if ((interface_number < sca_interface_list.begin()) || (interface_number
			>= sca_interface_list.end()))
		return false;

	sca_interface_list.erase(interface_number);
	object_deleted=true;

	return true;
}

//////////////////////////////////////////////////////////////////

void sca_object_manager::insert_channel(sca_prim_channel* channel)
{
	sca_channel_list.push_back(channel);
}

//////////////////////////////////////////////////////////////////

bool sca_object_manager::remove_channel(
		sca_channel_list_iteratorT channel_number)
{
	//no valid module
	if ((channel_number < sca_channel_list.begin()) || (channel_number
			>= sca_channel_list.end()))
		return false;

	sca_channel_list.erase(channel_number);

	object_deleted=true;
	return true;
}

//////////////////////////////////////////////////////////////////

sca_module_list_iteratorT sca_object_manager::get_module_list()
{
	return sca_module_list.begin();
}

//////////////////////////////////////////////////////////////////

sca_interface_list_iteratorT sca_object_manager::get_interface_list()
{
	return sca_interface_list.begin();
}

//////////////////////////////////////////////////////////////////

sca_channel_list_iteratorT sca_object_manager::get_channel_list()
{
	return sca_channel_list.begin();
}

//////////////////////////////////////////////////////////////////

void sca_object_manager::systemc_ams_init()
{
	if (init_done)
		return;
	init_done = true;

#ifdef SCA_IMPLEMENTATION_DEBUG
	std::cout << endl << "systemc_ams_init for " << sca_module_list.size()
	<< " modules" << std::endl;
#endif

	//register all modules to the view manager
	//sort and create view instances
	for (sca_module_list_iteratorT mit = sca_module_list.begin(); mit
			!= sca_module_list.end(); ++mit)
	{
		(*mit)->elaborate(); //elaborate the module
		views->push_back_module(*mit);
	}

	//partitionate and setup equation systems by initializing for all view instances
	for (sca_view_manager::sca_view_listItT vIt = views->view_list.begin(); vIt
			!= views->view_list.end(); ++vIt)
	{
		(*vIt)->setup_equations();
	}


	unsigned long& info_mask(
	                    sca_core::sca_implementation::sca_get_curr_simcontext()->
	                    get_information_mask());
	if (info_mask & sca_util::sca_info::sca_module.mask)
	{
		std::ostringstream str;
		str << std::endl;
		str << "\t" << sca_module_list.size()
				<< " SystemC-AMS modules instantiated" << std::endl;
		str << "\t" << views->view_list.size() << " SystemC-AMS views created"
				<< std::endl;
		str << "\t" << solvers->solver_list.size()
				<< " SystemC-AMS synchronization objects/solvers instantiated"
				<< std::endl;
		SC_REPORT_INFO("SystemC-AMS",str.str().c_str());
	}

#ifdef SCA_IMPLEMENTATION_DEBUG
	std::cout << "\t" << solvers.solver_list.size() << " solver instances created" << std::endl;
#endif

	//initialize all solver instances and register them to
	//the synchronization manager
	for (sca_solver_manager::sca_solver_listItT sIt =
			solvers->solver_list.begin(); sIt != solvers->solver_list.end(); ++sIt)
	{
		(*sIt)->initialize();

		//set default solver parameter
		solver_default_parameter_listT::iterator spit;
		spit=solver_default_parameter.find((*sIt)->kind());
		if(spit!=solver_default_parameter.end())
		{
			solver_parameterT* spars=&spit->second;
			for(unsigned long i=0;i<spars->size();i++)
			{
				(*sIt)->set_solver_parameter(NULL,(*spars)[i].first,(*spars)[i].second);
			}
		}
	}

#ifdef SCA_IMPLEMENTATION_DEBUG
	std::cout << "\t" << "Solver instances initialized" << std::endl;
#endif

	synchronization->initialize();

	//finish elaboration
	for (sca_channel_list_iteratorT cIt = sca_channel_list.begin(); cIt
			!= sca_channel_list.end(); ++cIt)
	{
		(*cIt)->end_of_elaboration();
	}

	for (sca_module_list_iteratorT mit = sca_module_list.begin(); mit
			!= sca_module_list.end(); ++mit)
	{
		(*mit)->end_of_sca_elaboration(); //elaborate the module
	}
}

//called during end_of_simulation kernel callback
void sca_object_manager::finish_simulation()
{
	this->synchronization->finish_simulation();
}

} //namespace sca_implementation
} //namespace sca_core

//////////////////////////////////////////////////////////////////
