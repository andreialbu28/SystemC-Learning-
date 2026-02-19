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

  sca_information.cpp - description

  Original Author: Karsten Einwich Fraunhofer IIS/EAS Dresden

  Created on: Jan 19, 2010

   SVN Version       :  $Revision: 2320 $
   SVN last checkin  :  $Date: 2023-06-08 12:47:38 +0000 (Thu, 08 Jun 2023) $
   SVN checkin by    :  $Author: pehrlich $
   SVN Id            :  $Id: sca_information.cpp 2320 2023-06-08 12:47:38Z pehrlich $

 *****************************************************************************/

/*****************************************************************************/
#include "systemc-ams"
#include "scams/utility/reporting/sca_information.h"
#include "scams/impl/core/sca_simcontext.h"

namespace sca_util
{

  namespace sca_info
  {
  extern const sca_util::sca_information_mask sca_module(1);
  extern const sca_util::sca_information_mask sca_tdf_solver(sca_module.mask<<1);
  extern const sca_util::sca_information_mask sca_lsf_solver(sca_module.mask<<2);
  extern const sca_util::sca_information_mask sca_eln_solver(sca_module.mask<<3);
  } // namespace sca_info


  sca_information_mask::sca_information_mask(unsigned long mask_)
  {
    mask=mask_;
  }


  sca_information_mask sca_information_mask::operator|(const sca_information_mask& mask_) const
  {
    return sca_information_mask(mask | mask_.mask);
  }

  void sca_information_on()
  {
    sca_core::sca_implementation::sca_get_curr_simcontext()->
                                                  get_information_mask()=~0;
  }


  void sca_information_on(sca_util::sca_information_mask mask)
  {
    sca_core::sca_implementation::sca_get_curr_simcontext()->
                                          get_information_mask()=mask.mask;
  }


  void sca_information_off()
  {
    sca_core::sca_implementation::sca_get_curr_simcontext()->
                                                      get_information_mask()=0;
  }

  void sca_information_off(sca_util::sca_information_mask mask)
  {
    sca_core::sca_implementation::sca_get_curr_simcontext()->
                                          get_information_mask()=~(mask.mask);
  }


  void sca_enable_performance_statistics()
  {
	  sca_core::sca_implementation::sca_get_curr_simcontext()->enable_performance_data_collection();
  }

  void sca_disable_performance_statistics()
  {
	  sca_core::sca_implementation::sca_get_curr_simcontext()->disable_performance_data_collection();
  }

}
