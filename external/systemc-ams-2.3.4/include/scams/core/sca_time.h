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

 sca_time.h - SystemC AMS time class

 Original Author: Karsten Einwich Fraunhofer IIS/EAS Dresden

 Created on: 03.03.2009

 SVN Version       :  $Revision: 1265 $
 SVN last checkin  :  $Date: 2011-11-19 20:43:31 +0000 (Sat, 19 Nov 2011) $ (UTC)
 SVN checkin by    :  $Author: karsten $
 SVN Id            :  $Id: sca_time.h 1265 2011-11-19 20:43:31Z karsten $

 *****************************************************************************/
/*
 LRM clause 3.2.5.
 The class sca_core::sca_time shall be used to represent simulation time
 for the AMS extensions.
 */

/*****************************************************************************/

#ifndef SCA_TIME_H_
#define SCA_TIME_H_


namespace sca_core
{
typedef sc_core::sc_time sca_time;
}

#endif /* SCA_TIME_H_ */
