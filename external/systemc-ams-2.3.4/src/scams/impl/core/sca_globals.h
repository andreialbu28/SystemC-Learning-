/*****************************************************************************

    Copyright 2010 - 2015
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

  sca_globals.h - description

  Original Author: Karsten Einwich Fraunhofer IIS/EAS Dresden

  Created on: 14.05.2009

   SVN Version       :  $Revision: 1812 $
   SVN last checkin  :  $Date: 2015-01-06 20:47:51 +0000 (Tue, 06 Jan 2015) $
   SVN checkin by    :  $Author: karsten $
   SVN Id            :  $Id: sca_globals.h 1812 2015-01-06 20:47:51Z karsten $

 *****************************************************************************/

/*****************************************************************************/


#ifndef SCA_GLOBALS_H_
#define SCA_GLOBALS_H_

/**
  Global function for SystemC-AMS termination,
  which has to be called after the last sc_start().
*/
namespace sca_core
{

namespace sca_implementation
{

void sca_systemc_ams_init();

} //namespace sca_implementation
} //namespace sca_core


#endif /* SCA_GLOBALS_H_ */
