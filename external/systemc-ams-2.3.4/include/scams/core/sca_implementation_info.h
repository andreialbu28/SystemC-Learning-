/*****************************************************************************

    Copyright 2010-2014
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

 sca_implementation_info.h - functions for accessing implementation info

 Original Author: Karsten Einwich COSEDA Technologies GmbH

 Created on: 08.03.2009

 SVN Version       :  $Revision: 2320 $
 SVN last checkin  :  $Date: 2023-06-08 12:47:38 +0000 (Thu, 08 Jun 2023) $
 SVN checkin by    :  $Author: pehrlich $
 SVN Id            :  $Id: sca_implementation_info.h 2320 2023-06-08 12:47:38Z pehrlich $

 *****************************************************************************/
/*
 * LRM clause 6.4.
 * Implementation information
 */

/*****************************************************************************/

#ifndef SCA_IMPLEMENTATION_INFO_H_
#define SCA_IMPLEMENTATION_INFO_H_


namespace sca_core
{
#define IEEE_16661_SYSTEMC_AMS 201601L

#define SCA_VERSION_MAJOR        2
#define SCA_VERSION_MINOR        4
#define SCA_VERSION_PATCH        0
#define SCA_VERSION_ORGINATOR    "COSEDA"
#define SCA_VERSION_RELEASE_DATE "20220406"
#define SCA_VERSION_PRERELEASE   "2.4.0-COSEDA BETA"
#define SCA_IS_PRERELEASE        0
#define SCA_VERSION              "2.4.0-COSEDA"
#define SCA_COPYRIGHT            "        Copyright (c) 2010-2014  by Fraunhofer-Gesellschaft IIS/EAS\n" \
	                             "        Copyright (c) 2015-2022  by COSEDA Technologies GmbH\n" \
                                 "        Licensed under the Apache License, Version 2.0"



extern const unsigned int sca_version_major;
extern const unsigned int sca_version_minor;
extern const unsigned int sca_version_patch;
extern const std::string  sca_version_orginator;
extern const std::string  sca_version_release_date;
extern const std::string  sca_version_prerelease;
extern const bool         sca_is_prerelease;
extern const std::string  sca_version_string;
extern const std::string  sca_copyright_string;

const char* sca_copyright();
const char* sca_version();
const char* sca_release();

} // namespace sca_core


#endif /* SCA_IMPLEMENTATION_INFO_H_ */
