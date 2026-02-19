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

  sca_generic_user_solver.cpp - description

  Original Author: Karsten Einwich Fraunhofer IIS/EAS Dresden

  Created on: 8.01.2014

   SVN Version       :  $Revision: 1620 $
   SVN last checkin  :  $Date: 2013-08-30 14:48:25 +0200 (Fri, 30 Aug 2013) $
   SVN checkin by    :  $Author: karsten $
   SVN Id            :  $Id: sca_solver_base.h 1620 2013-08-30 12:48:25Z karsten $

 *****************************************************************************/




/*****************************************************************************/


#include "sca_generic_user_solver.h"
#include "scams/impl/core/sca_simcontext.h"
#include "scams/impl/core/sca_object_manager.h"
#include "scams/impl/synchronization/sca_synchronization_layer.h"

namespace sca_core
{
namespace sca_implementation
{


sca_generic_user_solver::sca_generic_user_solver(
		std::vector<sca_module*>& mods,
		std::vector<sca_interface*>& chans,
		sca_core::sca_user_solver_base* solver) :
			sca_core::sca_implementation::sca_solver_base(
					sc_core::sc_gen_unique_name("sca_generic_user_solver"),mods,chans),
					user_solver(solver)
{
	init_method
			= static_cast<sca_core::sca_implementation::sc_object_method> (&sca_core::sca_user_solver_base::initialize);
	init_method_object = user_solver;

	processing_method
			= static_cast<sca_core::sca_implementation::sc_object_method> (&sca_core::sca_user_solver_base::processing);
	processing_method_object = user_solver;

	change_attributes_method=
			static_cast<sca_core::sca_implementation::sc_object_method> (&sca_core::sca_user_solver_base::change_attributes);
	change_attributes_method_object=user_solver;

	reinit_method=
			static_cast<sca_core::sca_implementation::sc_object_method> (&sca_core::sca_user_solver_base::reinitialize);
	reinit_method_object=user_solver;


	post_method =
			static_cast<sca_core::sca_implementation::sc_object_method> (&sca_core::sca_user_solver_base::end_of_simulation);
	post_method_object = user_solver;

	user_solver->generic_solver=this;
}


sca_generic_user_solver::~sca_generic_user_solver()
{
}

/** Implements the pure virtual initialize method of @ref #sca_solver_base. */
void sca_generic_user_solver::initialize()
{
	//register solver to synchronization layer
	sca_core::sca_implementation::sca_get_curr_simcontext()->get_sca_object_manager()->get_synchronization_if()-> registrate_solver_instance(
			this);

#ifdef SCA_IMPLEMENTATION_DEBUG
	std::cout << "\t\t" << " generic user solver instance initialized" << std::endl;
#endif
}


/** Implements the pure virtual print_post_solve_statisitcs method of @ref #sca_solver_base. */
void sca_generic_user_solver::print_post_solve_statisitcs()
{
	user_solver->print_post_solve_statisitcs();
}


const char* sca_generic_user_solver::kind() const
{
	return "sca_generic_user_solver";
}


    /** set implementation defined solver parameter */
void sca_generic_user_solver::set_solver_parameter(
    		  sca_core::sca_module* mod,
    		  const std::string& par,
    		  const std::string& val)
{
	user_solver->set_solver_parameter(mod,par,val);
}



bool sca_generic_user_solver::add_solver_trace(sca_util::sca_implementation::sca_trace_object_data& tr_obj)
{
	trace_objects.push_back(&tr_obj);

	if(!user_solver->trace_init(tr_obj.trace_object,static_cast<int>(trace_objects.size()-1)))
	{
		trace_objects.pop_back();
		return false;
	}

	if(!sca_core::sca_implementation::sca_solver_base::add_solver_trace(tr_obj))
	{
		trace_objects.pop_back();
		return false;
	}

	return true;
}

sca_util::sca_implementation::sca_trace_object_data& sca_generic_user_solver::get_trace_obj_data(int id)
{
	return *trace_objects[id];
}

void sca_generic_user_solver::trace( sc_core::sc_trace_file* tf ) const
{
	SC_REPORT_WARNING("SystemC-AMS","sc tracing not supported for generic solver");
}

void sca_generic_user_solver::send_trace_to_file()
{
  //not any more required -> synchronization is done automatically via bit map
}

const std::vector<sca_core::sca_module*>& sca_generic_user_solver::get_associated_modules()
{
	return this->associated_module_list;
}


const std::vector<sca_core::sca_interface*>& sca_generic_user_solver::get_associated_channels()
{
	return this->associated_channel_list;
}

} // namespace sca_implementation
} // namespace sca_core

