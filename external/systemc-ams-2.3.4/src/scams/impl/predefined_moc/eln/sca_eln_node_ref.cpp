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

  sca_eln_node_ref.cpp - description

  Original Author: Karsten Einwich Fraunhofer IIS/EAS Dresden

  Created on: 10.11.2009

   SVN Version       :  $Revision: 2361 $
   SVN last checkin  :  $Date: 2023-07-06 15:00:20 +0000 (Thu, 06 Jul 2023) $
   SVN checkin by    :  $Author: pehrlich $
   SVN Id            :  $Id: sca_eln_node_ref.cpp 2361 2023-07-06 15:00:20Z pehrlich $

 *****************************************************************************/

/*****************************************************************************/

#include <systemc-ams>
#include "scams/predefined_moc/eln/sca_eln_node_ref.h"
#include <cctype>

namespace sca_eln
{

#ifdef DISABLE_REFERENCE_NODE_CLUSTERING
static bool default_ignore_node_ref_for_clustering=true;
#else
static bool default_ignore_node_ref_for_clustering=false;
#endif

void sca_set_default_ignore_node_ref_for_clustering(bool ignore_for_clustering)
{
	default_ignore_node_ref_for_clustering=ignore_for_clustering;
}


bool sca_get_default_ignore_node_ref_for_clustering()
{
	std::string par=sca_core::sca_get_default_solver_parameter("sca_eln", "ignore_node_ref_default");

    using namespace std;
	std::transform(par.begin(), par.end(), par.begin(),
	    [](unsigned char c){ return tolower(c); });

	if(!par.empty())
	{
		if((par=="true")||(par=="1"))
		{
			return true;
		}

		if((par=="false")||((par=="0")))
		{
			return false;
		}

		std::ostringstream str;
		str << "solver parameter for solver sca_eln and parameter ignore_node_ref_default set to unknown value: " << par;
		SC_REPORT_WARNING("sca_node_ref",str.str().c_str());
	}

	return default_ignore_node_ref_for_clustering;
}

sca_node_ref::sca_node_ref():
	sca_node(sc_core::sc_gen_unique_name("sca_eln_node_ref"))
{
	  reference_node = true;
	  node_number    = -1;
	  ignore_for_clustering=sca_get_default_ignore_node_ref_for_clustering();
}

sca_node_ref::sca_node_ref(const char* name_): sca_node(name_)
{
	  reference_node = true;
	  node_number    = -1;
	  ignore_for_clustering=sca_get_default_ignore_node_ref_for_clustering();
}


sca_node_ref::sca_node_ref(bool ignore_for_clustering_):
	sca_node(sc_core::sc_gen_unique_name("sca_eln_node_ref"))
{
	  reference_node = true;
	  node_number    = -1;
	  ignore_for_clustering=ignore_for_clustering_;
}

sca_node_ref::sca_node_ref(const char* name_,bool ignore_for_clustering_): sca_node(name_)
{
	  reference_node = true;
	  node_number    = -1;
	  ignore_for_clustering=ignore_for_clustering_;
}

const char* sca_node_ref::kind() const
{
	return "sca_eln::sca_node_ref";
}

//Disabled not used
sca_node_ref::sca_node_ref(const sca_eln::sca_node_ref&)
{
}

}
