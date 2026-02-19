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

 sca_trace_value_handler_mm.h - memory handler for trace value wrapper class

 Original Author: Karsten Einwich COSEDA Technologies GmbH

 Created on: 07.03.2022

 SVN Version       :  $Revision$
 SVN last checkin  :  $Date$
 SVN checkin by    :  $Author$
 SVN Id            :  $Id$

 *****************************************************************************/

#ifndef SRC_SCAMS_IMPL_UTIL_TRACING_SCA_TRACE_VALUE_HANDLER_MM_H_
#define SRC_SCAMS_IMPL_UTIL_TRACING_SCA_TRACE_VALUE_HANDLER_MM_H_

#include "sca_trace_value_handler.h"
#include <memory>
#include <functional>

namespace sca_util
{
namespace sca_implementation
{

class sca_trace_value_handler_base;

class sca_trace_value_handler_mm_typed
{
public:

	void free(sca_trace_value_handler_base*);
	sca_trace_value_handler_base* get_new_base();

	sca_trace_value_handler_base* get_new_base_local();
	void free_local(sca_trace_value_handler_base*);


	sca_trace_value_handler_mm_typed();
	~sca_trace_value_handler_mm_typed();

private:

	struct states;

	std::unique_ptr<states> sp;
	states& s;

};



class sca_trace_value_handler_mm
{
public:

	void free(sca_trace_value_handler_base*);

	template<class T>
	sca_trace_value_handler<T>* get_new(const sca_core::sca_time& ctime, std::int64_t id, const T& val)
	{
		auto ret=dynamic_cast<sca_trace_value_handler<T>*>(this->get_new_base(id,
				[&]()
				{
					auto ret=new sca_trace_value_handler<T>(ctime,id,val);
					return ret;
				}));

		ret->reset(ctime, id, val);
		return ret;
	}

	sca_trace_value_handler_mm();
	~sca_trace_value_handler_mm();

private:

	sca_trace_value_handler_base* get_new_base(std::int64_t id,
			std::function<sca_trace_value_handler_base*()> allocator);

	void set_handler(sca_trace_value_handler_base*,std::int64_t id);

	struct states;

	std::unique_ptr<states> sp;
	states& s;

};


}

}



#endif /* SRC_SCAMS_IMPL_UTIL_TRACING_SCA_TRACE_VALUE_HANDLER_MM_H_ */
