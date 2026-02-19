/*****************************************************************************

    Copyright 2015-2022
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

 sca_lock_free_queue.h - lock free SPSC queue for thread communication

 Original Author: Karsten Einwich COSEDA Technologies GmbH

 Created on: 07.03.2022

 SVN Version       :  $Revision$
 SVN last checkin  :  $Date$
 SVN checkin by    :  $Author$
 SVN Id            :  $Id$

 *****************************************************************************/


#ifndef SRC_SCAMS_IMPL_UTIL_DATA_TYPES_SCA_LOCK_FREE_QUEUE_H_
#define SRC_SCAMS_IMPL_UTIL_DATA_TYPES_SCA_LOCK_FREE_QUEUE_H_

#include<memory>

namespace sca_util
{
namespace sca_implementation
{

class sca_lock_free_queue
{
public:

	void push(void* data);
	void* pop();

	sca_lock_free_queue();
	~sca_lock_free_queue();

private:

	struct states;

	std::unique_ptr<states> sp;
	states& s;

	void increase_buffer();

};


}
}


#endif /* SRC_SCAMS_IMPL_UTIL_DATA_TYPES_SCA_LOCK_FREE_QUEUE_H_ */
