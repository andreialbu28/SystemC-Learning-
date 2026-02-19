/*****************************************************************************

    Copyright 2015-2017
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

  sca_ac_object.cpp - description

  Original Author: Karsten Einwich COSEDA Technologies GmbH Dresden

  Created on: Sep 20, 2017

   SVN Version       :  $Revision$
   SVN last checkin  :  $Date$
   SVN checkin by    :  $Author$
   SVN Id            :  $Id$

 *****************************************************************************/


#include "scams/analysis/ac/sca_ac_object.h"

namespace sca_ac_analysis
{

sca_ac_object::sca_ac_object()
{
	enabled=true;
}

void sca_ac_object::ac_enable()
{
	enabled=true;
}

void sca_ac_object::ac_disable()
{
	enabled=false;
}
bool sca_ac_object::is_ac_enabled()
{
	return enabled;
}




}
