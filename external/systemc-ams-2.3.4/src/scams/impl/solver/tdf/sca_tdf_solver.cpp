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

  sca_tdf_solver.cpp - description

  Original Author: Karsten Einwich Fraunhofer IIS/EAS Dresden

  Created on: 25.08.2009

   SVN Version       :  $Revision: 2320 $
   SVN last checkin  :  $Date: 2023-06-08 12:47:38 +0000 (Thu, 08 Jun 2023) $
   SVN checkin by    :  $Author: pehrlich $
   SVN Id            :  $Id: sca_tdf_solver.cpp 2320 2023-06-08 12:47:38Z pehrlich $

 *****************************************************************************/

/*****************************************************************************/

#include <systemc-ams>
#include "scams/impl/solver/tdf/sca_tdf_solver.h"
#include "scams/impl/core/sca_simcontext.h"
#include "scams/impl/core/sca_object_manager.h"
#include "scams/impl/synchronization/sca_synchronization_obj_if.h"
#include "scams/impl/synchronization/sca_synchronization_layer.h"

namespace sca_tdf
{

namespace sca_implementation
{

//////////////////////////////////////////////////////////////////////

sca_tdf_solver::sca_tdf_solver(std::vector<sca_core::sca_module*>& mods,
		                       std::vector<sca_core::sca_interface*>& chans) :
	sca_solver_base( ::sc_core::sc_gen_unique_name("sca_tdf_solver"), mods,chans )
{
}

//////////////////////////////////////////////////////////////////////

sca_tdf_solver::~sca_tdf_solver()
{

}


//////////////////////////////////////////////////////////////////////

void sca_tdf_solver::initialize()
{

 ::sca_core::sca_implementation::sca_get_curr_simcontext()->get_sca_object_manager()->
    get_synchronization_if()->registrate_solver_instance(this);
#ifdef SCA_IMPLEMENTATION_DEBUG
  std::cout << "\t\t" << " timed dataflow solver instance initialized" << std::endl;
#endif
}

//////////////////////////////////////////

void sca_tdf_solver::register_methods(sc_core::sc_object* mod,
                                      sca_tdf::sca_module::sca_module_method init_fct,
                                      sca_tdf::sca_module::sca_module_method reinit_fct,
                                      sca_tdf::sca_module::sca_module_method change_attributes_fct,
                                      sca_tdf::sca_module::sca_module_method proc_fct,
                                      sca_tdf::sca_module::sca_module_method post_fct)
{
  this->get_synchronization_object_data()->sync_obj_name = mod->name();

  init_method        = static_cast<sca_core::sca_implementation::sc_object_method>(init_fct);
  init_method_object = mod;

  reinit_method        = static_cast<sca_core::sca_implementation::sc_object_method>(reinit_fct);
  reinit_method_object = mod;

  change_attributes_method= static_cast<sca_core::sca_implementation::sc_object_method>(change_attributes_fct);
  change_attributes_method_object=mod;

  processing_method        = static_cast<sca_core::sca_implementation::sc_object_method>(proc_fct);
  processing_method_object = mod;

  post_method        = static_cast<sca_core::sca_implementation::sc_object_method>(post_fct);
  post_method_object = mod;
}


void sca_tdf_solver::exectute_lambda_processing()
{
	if(this->processing_fct!=NULL)
	{
		this->processing_fct();
	}
}


void sca_tdf_solver::register_processing_method(std::function<void()> fct)
{

	processing_fct=fct;

	processing_method        = static_cast<sca_core::sca_implementation::sc_object_method>(&sca_tdf_solver::exectute_lambda_processing);
	processing_method_object = this;
}


}
}
