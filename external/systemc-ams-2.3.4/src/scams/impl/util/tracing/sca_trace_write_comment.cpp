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

  sca_trace_write_comment.cpp - description

  Original Author: Karsten Einwich Fraunhofer IIS/EAS Dresden

  Created on: Jan 19, 2010

   SVN Version       :  $Revision: 2320 $
   SVN last checkin  :  $Date: 2023-06-08 12:47:38 +0000 (Thu, 08 Jun 2023) $
   SVN checkin by    :  $Author: pehrlich $
   SVN Id            :  $Id: sca_trace_write_comment.cpp 2320 2023-06-08 12:47:38Z pehrlich $

 *****************************************************************************/

/*****************************************************************************/

#include "systemc-ams"
#include "scams/utility/tracing/sca_trace_write_comment.h"
#include "scams/impl/util/tracing/sca_tabular_trace.h"
#include "scams/impl/util/tracing/sca_vcd_trace.h"


namespace sca_util
{

void sca_write_comment(sca_util::sca_trace_file* tf,
                const std::string& comment)
{
  if(tf->outstr)
  {
    if(dynamic_cast<sca_util::sca_implementation::sca_tabular_trace*>(tf)!=NULL)
    {
    	tf->synchronize();
      (*(tf->outstr)) << '%' << comment << std::endl;
    }

    if(dynamic_cast<sca_util::sca_implementation::sca_vcd_trace*>(tf)!=NULL)
    {
    	tf->synchronize();
      (*(tf->outstr)) << "$comment"<< std::endl << comment
                      << std::endl << "$end"<< std::endl;
    }
  }

}

} // namespace sca_util
