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

  sca_synchronization_layer.cpp - description

  Original Author: Karsten Einwich Fraunhofer IIS/EAS Dresden

  Created on: 25.08.2009

   SVN Version       :  $Revision: 2320 $
   SVN last checkin  :  $Date: 2023-06-08 12:47:38 +0000 (Thu, 08 Jun 2023) $
   SVN checkin by    :  $Author: pehrlich $
   SVN Id            :  $Id: sca_synchronization_layer.cpp 2320 2023-06-08 12:47:38Z pehrlich $

 *****************************************************************************/

/*****************************************************************************/

#include "scams/impl/synchronization/sca_synchronization_alg.h"
#include "scams/impl/synchronization/sca_synchronization_layer.h"


#include <iostream>
using namespace std;

namespace sca_core
{
namespace sca_implementation
{





sca_synchronization_layer::sca_synchronization_layer():  alg(NULL)
{
	alg=new sca_core::sca_implementation::sca_synchronization_alg();
}

/////////////////////////////////////////////////////////

sca_synchronization_layer::~sca_synchronization_layer()
{
	delete alg;
	alg=NULL;
}

/////////////////////////////////////////////////////////

void sca_synchronization_layer::registrate_solver_instance(sca_synchronization_obj_if* solver)
{
  solvers.push_back(solver);
}


void sca_synchronization_layer::reanalyze_cluster_timing(unsigned long cluster_id)
{
	alg->reanalyze_cluster_timing(cluster_id);
}


void sca_synchronization_layer::reschedule_cluster(unsigned long cluster_id)
{
  alg->reschedule_cluster(cluster_id);
}

bool sca_synchronization_layer::reinitialize_cluster_timing(unsigned long cluster_id)
{
	return alg->reinitialize_custer_timing(cluster_id);
}

void sca_synchronization_layer::reinitialize_cluster_datastructures(unsigned long cluster_id)
{
	return alg->reinitialize_custer_datastructures(cluster_id);
}


sc_core::sc_object* sca_synchronization_layer::get_current_context()
{
	return alg==NULL?NULL:alg->get_current_context();
}



void sca_synchronization_layer::finish_simulation()
{
	if(alg!=NULL) alg->finish_simulation();
}


/////////////////////////////////////////////////////////

void sca_synchronization_layer::initialize()
{
  alg->initialize(solvers);

#ifdef SCA_IMPLEMENTATION_DEBUG
 std::cout << "\t" << "Sysnchronization layer initialized: " << solvers.size() << std::endl << std::endl;
#endif
}

/////////////////////////////////////////////////////////


}
}
