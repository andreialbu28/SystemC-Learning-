/*****************************************************************************

    Copyright 2010-2014
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

  sca_generic_solver.cpp - description

  Original Author: Karsten Einwich Fraunhofer IIS/EAS Dresden

  Created on: 8.01.2014

   SVN Version       :  $Revision: 1620 $
   SVN last checkin  :  $Date: 2013-08-30 14:48:25 +0200 (Fri, 30 Aug 2013) $
   SVN checkin by    :  $Author: karsten $
   SVN Id            :  $Id: sca_solver_base.h 1620 2013-08-30 12:48:25Z karsten $

 *****************************************************************************/




/*****************************************************************************/

#include "systemc-ams"
#include "scams/core/sca_user_solver_base.h"
#include "scams/impl/solver/user_solver/sca_generic_user_solver.h"

namespace sca_core
{


/**
  Abstract class for deriving a  generic (user defined) solver
*/
//class sca_generic_solver : public sc_core::sc_object


sca_user_solver_base::sca_user_solver_base() :
		sc_core::sc_object(sc_core::sc_gen_unique_name("sca_user_solver_base"))
{
	solver_requesting_module=NULL;
	generic_solver=NULL;
}

sca_user_solver_base::sca_user_solver_base(const char* nm) : sc_core::sc_object(nm)
{
	solver_requesting_module=NULL;
	generic_solver=NULL;
}

sca_user_solver_base::~sca_user_solver_base()
{
}


void sca_user_solver_base::initialize()
{
}

void sca_user_solver_base::processing()
{
}

void sca_user_solver_base::change_attributes()
{
}

void sca_user_solver_base::reinitialize()
{
}

void sca_user_solver_base::end_of_simulation()
{
}

/** Implements the pure virtual print_post_solve_statisitcs method of @ref #sca_solver_base. */
void sca_user_solver_base::print_post_solve_statisitcs()
{
}


 /** set implementation defined solver parameter */
void sca_user_solver_base::set_solver_parameter(
    		  sca_core::sca_module* mod,
    		  const std::string& par,
    		  const std::string& val)
{
}

const char* sca_user_solver_base::kind() const
{
	return "sca_user_solver_base";
}



//will be overloaded by the user solver
//initializes a trace, is called by SystemC-AMS
//returns true if successful
bool sca_user_solver_base::trace_init(sca_util::sca_traceable_object*,int id)
{
	return false;
}


sca_util::sca_implementation::sca_trace_object_data& sca_user_solver_base::get_trace_obj_data(int id)
{
	sca_util::sca_implementation::sca_trace_object_data& data=generic_solver->get_trace_obj_data(id);
	return data;
}


/*
//this function will be called by the solver to send a value to the trace file
void sca_user_solver_base::trace_from_user_solver(sca_core::sca_time tt,double value,int id)
{
	generic_solver->trace(tt,value,id);
}
*/

void sca_user_solver_base::send_trace_to_file()
{
	generic_solver->send_trace_to_file();
}

const std::vector<sca_core::sca_module*>&    sca_user_solver_base::get_associated_modules()
{
	return generic_solver->get_associated_modules();
}
const std::vector<sca_core::sca_interface*>& sca_user_solver_base::get_associated_channels()
{
	return generic_solver->get_associated_channels();
}


sca_core::sca_time sca_user_solver_base::get_time()
{
	return generic_solver->get_current_time();
}


sca_core::sca_time sca_user_solver_base::get_timestep()
{
	return generic_solver->get_current_period();
}

} // namespace sca_core

