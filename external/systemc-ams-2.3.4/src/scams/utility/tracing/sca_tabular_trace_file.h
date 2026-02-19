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

 sca_tabular_trace_file.h - functions for creatin/closing sca-tabular trace files

 Original Author: Karsten Einwich Fraunhofer IIS/EAS Dresden

 Created on: 08.03.2009

 SVN Version       :  $Revision: 1911 $
 SVN last checkin  :  $Date: 2016-02-16 13:51:22 +0000 (Tue, 16 Feb 2016) $ (UTC)
 SVN checkin by    :  $Author: karsten $
 SVN Id            :  $Id: sca_tabular_trace_file.h 1911 2016-02-16 13:51:22Z karsten $

 *****************************************************************************/
/*
 * The function sca_util::sca_create_tabular_trace_file shall create new trace
 * file handle object of class sca_util::sca_trace_file. If a file name is
 * passed as a character string, it shall be opened. If a pointer to an object
 * of class std::ostream is passed, the traces shall be written to that object.
 * The functions shall return a pointer to the trace file handle.
 *
 * The function sca_util::sca_close_tabular_trace_file shall close the tabular
 * trace file and delete the file handle pointed to by the argument.
 */

/*****************************************************************************/

#ifndef SCA_TABULAR_TRACE_FILE_H_
#define SCA_TABULAR_TRACE_FILE_H_

namespace sca_util
{

sca_util::sca_trace_file* sca_create_tabular_trace_file(
		const char* name);


sca_util::sca_trace_file* sca_create_tabular_trace_file(
		std::ostream& os);

void sca_close_tabular_trace_file(sca_util::sca_trace_file* tf);

}

#endif /* SCA_TABULAR_TRACE_FILE_H_ */
