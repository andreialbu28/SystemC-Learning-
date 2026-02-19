/*****************************************************************************

    Copyright 2010-2015
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

  almost_equal.cpp - description

  Original Author: Karsten Einwich Fraunhofer IIS/EAS Dresden

  Created on: May 4, 2015

   SVN Version       :  $Revision$
   SVN last checkin  :  $Date$
   SVN checkin by    :  $Author$
   SVN Id            :  $Id$

 *****************************************************************************/


#include "almost_equal.h"

#include <cmath>
#include <limits>
#include <algorithm>

namespace sca_util
{
namespace sca_implementation
{

bool almost_equal(double x, double y, unsigned int ulp)
{
    return (std::abs(x-y) <= std::numeric_limits<double>::epsilon() *
    		std::max(std::abs(x), std::abs(y)) * ulp );
}

} //namespace sca_implementation
} //namespace sca_util

