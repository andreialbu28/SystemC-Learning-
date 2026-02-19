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

  sca_sc_trace.cpp - description

  Original Author: Karsten Einwich Fraunhofer IIS/EAS Dresden

  Created on: 09.12.2009

   SVN Version       :  $Revision: 2106 $
   SVN last checkin  :  $Date: 2020-02-26 15:58:39 +0000 (Wed, 26 Feb 2020) $
   SVN checkin by    :  $Author: karsten $
   SVN Id            :  $Id: sca_sc_trace.cpp 2106 2020-02-26 15:58:39Z karsten $

 *****************************************************************************/

/*****************************************************************************/
#define SC_INCLUDE_DYNAMIC_PROCESSES
#include "systemc-ams"
#include "scams/impl/util/tracing/sca_sc_trace.h"
#include "scams/impl/core/sca_simcontext.h"

namespace sca_util
{
namespace sca_implementation
{

class sca_sc_trace_base;

class sc_trace_functor
{
public:
	void operator()();
	sc_trace_functor(sca_sc_trace_base* object,bool* terminated);

	sca_sc_trace_base* obj;
	bool* terminated;
};

void sc_trace_functor::operator()()
{
	if((terminated==NULL) || (*terminated))
	{
		if(terminated!=NULL) delete terminated;
		terminated=NULL;
		obj=NULL;
		return;
	}


	//for the case we have digital traces only, we must be able to initialize, also
	//without any ams cluster
	sca_core::sca_implementation::sca_get_curr_simcontext()->initialize_all_traces();

	if(obj->data!=NULL)
	{
        if(!sca_ac_analysis::sca_ac_is_running()) obj->data->trace();
	}

	//we use dynamic sensitivity -> thus if we access to the event of a port, the port
	//binding has been finished
	sc_core::next_trigger(obj->get_trigger_event());
}


sc_trace_functor::sc_trace_functor(sca_sc_trace_base* object, bool* tf):
	obj(object),terminated(tf)
{
}


////////////////

sca_sc_trace_base::sca_sc_trace_base()
{
	data=NULL;

	terminated=new bool(false);
	sc_core::sc_spawn_options opt;
	opt.spawn_method();
    sc_core::sc_spawn(sc_trace_functor(this,terminated),0,&opt);
}


bool sca_sc_trace_base::trace_init(sca_trace_object_data& data_)
{
	data=&data_;

	set_type_info(data_);

    data->event_driven=true;
    data->dont_interpolate=true;

    return true;

}

sca_sc_trace_base::~sca_sc_trace_base()
{
	if(terminated!=NULL) *terminated=true;  //terminate process
	terminated=NULL;
}

}
}
