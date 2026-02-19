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

 sca_trace_value_handler_mm.cpp - memory handler for trace value wrapper class

 Original Author: Karsten Einwich COSEDA Technologies GmbH

 Created on: 07.03.2022

 SVN Version       :  $Revision$
 SVN last checkin  :  $Date$
 SVN checkin by    :  $Author$
 SVN Id            :  $Id$

 *****************************************************************************/
#include "systemc-ams"
#include "sca_trace_value_handler_mm.h"
#include "../data_types/sca_lock_free_queue.h"
#include <queue>

namespace sca_util
{
namespace sca_implementation
{


struct sca_trace_value_handler_mm_typed::states
{
	sca_lock_free_queue queue;

	std::queue<sca_trace_value_handler_base*> local_queue;
};

sca_trace_value_handler_mm_typed::sca_trace_value_handler_mm_typed() : sp(new states),s(*sp)
{

}

sca_trace_value_handler_mm_typed::~sca_trace_value_handler_mm_typed()
{
}

sca_trace_value_handler_base* sca_trace_value_handler_mm_typed::get_new_base()
{
	return static_cast<sca_trace_value_handler_base*>(s.queue.pop());
}

void sca_trace_value_handler_mm_typed::free(sca_trace_value_handler_base* vh)
{
	s.queue.push(static_cast<void*>(vh));
}


sca_trace_value_handler_base* sca_trace_value_handler_mm_typed::get_new_base_local()
{
	if(s.local_queue.empty())
	{
		return NULL;
	}

	sca_trace_value_handler_base* ret=s.local_queue.front();
	s.local_queue.pop();

	return ret;
}

void sca_trace_value_handler_mm_typed::free_local(sca_trace_value_handler_base* vh)
{
	s.local_queue.push(vh);
}

///////////////////////////////////////////////////////

struct sca_trace_value_handler_mm::states
{
	//we need for each type an separate handler - to ensure this we create for each id an handler
	std::vector<sca_trace_value_handler_mm_typed*> typed_handler;
};

sca_trace_value_handler_mm::sca_trace_value_handler_mm() : sp(new states),s(*sp)
{

}

sca_trace_value_handler_mm::~sca_trace_value_handler_mm()
{
	for(auto& hd : s.typed_handler)
	{
		if(hd!=NULL)
		{
			delete hd;
			hd=NULL;
		}
	}
}

void sca_trace_value_handler_mm::set_handler(sca_trace_value_handler_base* val,std::int64_t id)
{
	if(id<0) //sync messages handles
	{
		val->memory_manager=s.typed_handler[0];
	}
	else
	{
		val->memory_manager=s.typed_handler[id+1];
	}
}

sca_trace_value_handler_base* sca_trace_value_handler_mm::get_new_base(std::int64_t  id,
		std::function<sca_trace_value_handler_base*()> allocator)
{
	sca_trace_value_handler_mm_typed* handler=NULL;

	std::uint64_t idx=0;
	if(id>=0)
	{
		idx=id+1;
	}

	if(s.typed_handler.size()<=idx)
	{
		s.typed_handler.resize(idx+1,NULL);
	}

	handler=s.typed_handler[idx];

	if(handler==NULL)
	{
		handler=new sca_trace_value_handler_mm_typed;
		s.typed_handler[idx]=handler;
	}

	auto ret=handler->get_new_base();

	if(ret==NULL)
	{
		ret=allocator();
		this->set_handler(ret,id);
	}

	ret->id=id;

	return ret;
}




}

}




