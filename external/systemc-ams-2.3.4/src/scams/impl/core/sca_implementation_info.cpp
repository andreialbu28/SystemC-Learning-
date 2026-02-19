/*****************************************************************************

    Copyright 2010-2012
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

  sca_information_info.h - description

  Original Author: Karsten Einwich Fraunhofer IIS/EAS Dresden

  Created on: Jan 19, 2010

   SVN Version       :  $Revision: 1988 $
   SVN last checkin  :  $Date: 2016-04-05 08:10:19 +0000 (Tue, 05 Apr 2016) $
   SVN checkin by    :  $Author: karsten $
   SVN Id            :  $Id: sca_implementation_info.cpp 1988 2016-04-05 08:10:19Z karsten $

 *****************************************************************************/

/*****************************************************************************/

#include <string>
#include "scams/core/sca_implementation_info.h"
#include <sstream>

namespace sca_core
{

const unsigned int sca_version_major         = SCA_VERSION_MAJOR;
const unsigned int sca_version_minor         = SCA_VERSION_MINOR;
const unsigned int sca_version_patch         = SCA_VERSION_PATCH;
const std::string  sca_version_orginator     = SCA_VERSION_ORGINATOR;
const std::string  sca_version_release_date  = SCA_VERSION_RELEASE_DATE;
const std::string  sca_version_prerelease    = SCA_VERSION_PRERELEASE;
const bool         sca_is_prerelease         = SCA_IS_PRERELEASE;
const std::string  sca_version_string        = SCA_VERSION;
const std::string  sca_copyright_string      = SCA_COPYRIGHT;


  const char* sca_copyright()
  {
      return sca_copyright_string.c_str();
  }


  const char* sca_release()
  {
	  if(sca_is_prerelease)
	  {
		  return sca_version_prerelease.c_str();
	  }

	  return sca_version_string.c_str();
  }


  inline static std::string generate_version_string()
  {
	  std::ostringstream str;
	  str << sca_version_major << "." << sca_version_minor;
	  if(sca_version_patch>0) str << "." << sca_version_patch;
	  if(sca_is_prerelease) str << "." << sca_version_prerelease;

	  return str.str();
  }


  const char* sca_version()
  {
	  static const std::string str(generate_version_string());
      return str.c_str();
  }



} // namespace sca_core

