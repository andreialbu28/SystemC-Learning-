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

  sca_ac_object.h - description

  Original Author: Karsten Einwich COSEDA Technologies GmbH Dresden

  Created on: Sep 19, 2017

   SVN Version       :  $Revision$
   SVN last checkin  :  $Date$
   SVN checkin by    :  $Author$
   SVN Id            :  $Id$

 *****************************************************************************/


#ifndef SRC_SCAMS_ANALYSIS_AC_SCA_AC_OBJECT_H_
#define SRC_SCAMS_ANALYSIS_AC_SCA_AC_OBJECT_H_

namespace sca_ac_analysis
{

class sca_ac_object
{
public:

	void ac_enable();
	void ac_disable();
	bool is_ac_enabled();

	sca_ac_object();

	//make sca_ac_object polymorphic
	virtual ~sca_ac_object(){}

private:

	bool enabled;

};

}


#endif /* SRC_SCAMS_ANALYSIS_AC_SCA_AC_OBJECT_H_ */

