/*****************************************************************************

    Copyright 2010-2013
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

 sca_set_default_solver_parameter.h

 Original Author: Karsten Einwich Fraunhofer IIS/EAS Dresden

 Created on: 25.05.2013

 SVN Version       :  $Revision: 1265 $
 SVN last checkin  :  $Date: 2011-11-19 21:43:31 +0100 (Sat, 19 Nov 2011) $ (UTC)
 SVN checkin by    :  $Author: karsten $
 SVN Id            :  $Id: sca_time.h 1265 2011-11-19 20:43:31Z karsten $

 *****************************************************************************/
/*
 Global function for setting default implementation depended solver parameter
 */

/*****************************************************************************/

#ifndef SCA_SET_DEFAULT_SOLVER_PARAMETER_H_
#define SCA_SET_DEFAULT_SOLVER_PARAMETER_H_


namespace sca_core
{
	/** function for setting implementation depended default solver parameter*/
	void sca_set_default_solver_parameter(
			std::string solver,
			std::string parameter,
			std::string value);

	std::string sca_get_default_solver_parameter(
			const std::string& solver_name,
			const std::string& parameter_name);

	std::vector<std::string> sca_get_solver_names();

	std::vector<std::string> sca_get_solver_parameter_names(const std::string& solver_name);
}

#endif /* SCA_TIME_H_ */
