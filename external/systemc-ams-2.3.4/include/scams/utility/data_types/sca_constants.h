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

 sca_constants.h - data type for handling matrices

 Original Author: Karsten Einwich Fraunhofer IIS/EAS Dresden

 Created on: 16.01.2010

 SVN Version       :  $Revision: 1265 $
 SVN last checkin  :  $Date: 2011-11-19 20:43:31 +0000 (Sat, 19 Nov 2011) $
 SVN checkin by    :  $Author: karsten $
 SVN Id            :  $Id: sca_constants.h 1265 2011-11-19 20:43:31Z karsten $

 *****************************************************************************/

/*****************************************************************************/


#ifndef SCA_CONSTANTS_H_
#define SCA_CONSTANTS_H_

#ifdef max
#define SCA_MICROSOFT_HAS_DEFINED_A_MAX_MACRO max
#undef max
#endif


namespace sca_util
{

static const double SCA_INFINITY=std::numeric_limits<double>::infinity();
static const double SCA_UNDEFINED=std::numeric_limits<double>::max();

static const sca_util::sca_complex SCA_COMPLEX_J=sca_util::sca_complex(0.0,1.0);

}

#ifdef SCA_MICROSOFT_HAS_DEFINED_A_MAX_MACRO
#define max SCA_MICROSOFT_HAS_DEFINED_A_MAX_MACRO
#undef SCA_MICROSOFT_HAS_DEFINED_A_MAX_MACRO
#endif

#endif /* SCA_CONSTANTS_H_ */
