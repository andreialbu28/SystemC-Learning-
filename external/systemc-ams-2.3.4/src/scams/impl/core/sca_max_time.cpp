/*****************************************************************************

    Copyright 2010-2012
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

 sca_max_time.cpp - description

 Original Author: Karsten Einwich Fraunhofer IIS/EAS Dresden

 Created on: 05.04.2012

 SVN Version       :  $Revision: 1917 $
 SVN last checkin  :  $Date: 2016-02-24 13:09:44 +0000 (Wed, 24 Feb 2016) $
 SVN checkin by    :  $Author: karsten $
 SVN Id            :  $Id: sca_max_time.cpp 1917 2016-02-24 13:09:44Z karsten $

 *****************************************************************************/

/*****************************************************************************/

#include <systemc>
#include "scams/core/sca_time.h"

#include "scams/core/sca_max_time.h"



namespace sca_core
{
	const sca_core::sca_time& sca_max_time()
	{
#if defined(SYSTEMC_VERSION) & (SYSTEMC_VERSION <= 20120701)
		static sca_core::sca_time max_time;

		if(max_time==sc_core::SC_ZERO_TIME)
		(*const_cast<sca_core::sca_time*>(&max_time))=
			sc_core::SC_ZERO_TIME-sc_core::sc_get_time_resolution();

		return max_time;
#else
		return sc_core::sc_max_time();
#endif
	}
}
