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

  sca_ac_module.h - description

  Original Author: Karsten Einwich COSEDA Technologies GmbH Dresden

  Created on: Sep 18, 2017

   SVN Version       :  $Revision$
   SVN last checkin  :  $Date$
   SVN checkin by    :  $Author$
   SVN Id            :  $Id$


 Description:

  virtual base class for modules which implement an ac behviour

 *****************************************************************************/


#ifndef SRC_SCAMS_ANALYSIS_AC_SCA_AC_MODULE_H_
#define SRC_SCAMS_ANALYSIS_AC_SCA_AC_MODULE_H_

namespace sca_ac_analysis
{

class sca_ac_module : public sca_ac_object
{
public:

	virtual void ac_processing()=0;

protected:

	virtual ~sca_ac_module(){}
};


}

#endif /* SRC_SCAMS_ANALYSIS_AC_SCA_AC_MODULE_H_ */

