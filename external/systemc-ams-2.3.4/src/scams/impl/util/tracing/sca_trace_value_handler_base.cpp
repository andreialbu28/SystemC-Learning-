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

  sca_trace_value_handler_base.cpp - description

  Original Author: Karsten Einwich Fraunhofer IIS/EAS Dresden

  Created on: 13.11.2009

   SVN Version       :  $Revision: 2320 $
   SVN last checkin  :  $Date: 2023-06-08 12:47:38 +0000 (Thu, 08 Jun 2023) $
   SVN checkin by    :  $Author: pehrlich $
   SVN Id            :  $Id: sca_trace_value_handler_base.cpp 2320 2023-06-08 12:47:38Z pehrlich $

 *****************************************************************************/

/*****************************************************************************/

#include "systemc-ams"
#include "scams/impl/util/tracing/sca_trace_value_handler_base.h"
#include "scams/impl/util/tracing/sca_trace_value_handler_mm.h"

namespace sca_util
{
namespace sca_implementation
{


sca_trace_value_handler_base::~sca_trace_value_handler_base()
{
}


sca_trace_value_handler_base::sca_trace_value_handler_base()
{
	ref_cnt=NULL;
	id=-1;
	next_value=NULL;
	memory_manager=NULL;
	localy_generated=false;
}


void sca_trace_value_handler_base::set_local()
{
	this->localy_generated=true;
}


bool sca_trace_value_handler_base::is_local()
{
	return localy_generated;
}

void sca_trace_value_handler_base::free() //free handle
{
	if( (ref_cnt!=NULL) && (*ref_cnt>1))
	{
		(*ref_cnt)--;

		ref_cnt=NULL;
		this->remove_value();

		//for all cases where ref_cnt>1 it is definitively a local generated value
		//only the last is may a main program generated value
		this->memory_manager->free_local(this);

		return;
	}


	//locally generated handler will be returned into the local queue, main program
	//generated handle returned to the main program queue
	// -> we should achieve a settled number of available handle in booth queues
	//if ref_cnt=1 we check whether this is a handle, which was with the value locally generated
	//e.g. due to interpolation
	if(this->is_local())
	{
		this->memory_manager->free_local(this);
	}
	else
	{
		this->memory_manager->free(this);  //return to main program queue
	}
}

sca_trace_value_handler_base* sca_trace_value_handler_base::get_new_local_base()
{
	return this->memory_manager->get_new_base_local();
}


}

}

