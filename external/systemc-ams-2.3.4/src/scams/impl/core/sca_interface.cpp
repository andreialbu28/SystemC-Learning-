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

 sca_interface.cpp - description

 Original Author: Karsten Einwich Fraunhofer IIS/EAS Dresden

 Created on: 13.05.2009

 SVN Version       :  $Revision: 2160 $
 SVN last checkin  :  $Date: 2021-06-21 11:17:32 +0000 (Mon, 21 Jun 2021) $
 SVN checkin by    :  $Author: karsten $
 SVN Id            :  $Id: sca_interface.cpp 2160 2021-06-21 11:17:32Z karsten $

 *****************************************************************************/

/*****************************************************************************/


#include <systemc>
#include "scams/core/sca_physical_domain_interface.h"
#include "scams/core/sca_interface.h"
#include "scams/core/sca_time.h"
#include "scams/impl/core/sca_simcontext.h"
#include "scams/impl/core/sca_object_manager.h"

#include <sstream>

using namespace sca_core;
using namespace sca_core::sca_implementation;

namespace sca_core
{

//maybe not required
sca_interface::sca_interface()
{
	sca_core::sca_implementation::sca_simcontext* simc;
	simc=sca_core::sca_implementation::sca_get_curr_simcontext();

	if(simc==NULL)
	{
		std::ostringstream str;
		str << "Cannot create a new interface after the simulation has been finished";
		str << " or a module or sca_interface has been deleted";

		sc_core::sc_object* obj=dynamic_cast<sc_core::sc_object*>(this);
		if(obj!=NULL)
		{
			str << " for: " << obj->name();
		}
		SC_REPORT_ERROR("SystemC-AMS",str.str().c_str());
		return;
	}

	simc->get_sca_object_manager()->insert_interface(this);
}

//disabled copy constructor
sca_interface::sca_interface(const sca_core::sca_interface&)
{
}

//disabled operator
sca_interface& sca_interface::operator=(const sca_core::sca_interface&)
{
	return *this;
}

}

