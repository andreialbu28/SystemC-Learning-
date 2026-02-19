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

 sca_trace_write_comment.h - function for writing comments into the trace file

 Original Author: Karsten Einwich Fraunhofer IIS/EAS Dresden

 Created on: 08.03.2009

 SVN Version       :  $Revision: 1265 $
 SVN last checkin  :  $Date: 2011-11-19 20:43:31 +0000 (Sat, 19 Nov 2011) $ (UTC)
 SVN checkin by    :  $Author: karsten $
 SVN Id            :  $Id: sca_trace_write_comment.h 1265 2011-11-19 20:43:31Z karsten $

 *****************************************************************************/
/*
 * LRM clause 6.1.1.7.
 * The function sca_util::sca_write_comment shall write the string given as the
 * second argument to the trace file given by the first argument, as a comment,
 * at the simulation time, at which the function is called.
 */

/*****************************************************************************/

#ifndef SCA_TRACE_WRITE_COMMENT_H_
#define SCA_TRACE_WRITE_COMMENT_H_

namespace sca_util
{

void sca_write_comment(sca_util::sca_trace_file* tf,
		const std::string& comment);

}
#endif /* SCA_TRACE_WRITE_COMMENT_H_ */
