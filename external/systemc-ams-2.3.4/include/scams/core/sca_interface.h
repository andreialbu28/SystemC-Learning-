/*****************************************************************************

    Copyright 2010
    Fraunhofer-Gesellschaft zur Foerderung der angewandten Forschung e.V.

    Copyright 2015-2020
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

  sca_interface.h - base class for all SystemC AMS interfaces

  Original Author: Karsten Einwich COSEDA Technologies GmbH

  Created on: 03.03.2009

   SVN Version       :  $Revision: 2102 $
   SVN last checkin  :  $Date: 2020-02-21 14:58:34 +0000 (Fri, 21 Feb 2020) $
   SVN checkin by    :  $Author: karsten $
   SVN Id            :  $Id: sca_interface.h 2102 2020-02-21 14:58:34Z karsten $

 *****************************************************************************/
   /*
     LRM clause 3.2.2.
     The class sca_core::sca_interface shall define the base class
     to derive interfaces for the predefined models of computation.
   */

/*****************************************************************************/


#ifndef SCA_INTERFACE_H_
#define SCA_INTERFACE_H_


//begin LRM copy

namespace sca_core
{

  class sca_interface : public sc_core::sc_interface,
                        public sca_core::sca_physical_domain_interface
  {

   protected:
    sca_interface();

   private:
    // Disabled
    sca_interface( const sca_core::sca_interface& );
    sca_core::sca_interface& operator= ( const sca_core::sca_interface& );
  };

} // namespace sca_core

//end LRM copy

#endif /* SCA_INTERFACE_H_ */
