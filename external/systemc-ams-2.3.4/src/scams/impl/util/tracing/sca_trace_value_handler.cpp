/*****************************************************************************

    Copyright 2010
    Fraunhofer-Gesellschaft zur Foerderung der angewandten Forschung e.V.

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

  sca_trace_value_handler.cpp - description

  Original Author: Karsten Einwich COSEDA Technologies GmbH

  Created on: 13.11.2009

   SVN Version       :  $Revision: 2320 $
   SVN last checkin  :  $Date: 2023-06-08 12:47:38 +0000 (Thu, 08 Jun 2023) $
   SVN checkin by    :  $Author: pehrlich $
   SVN Id            :  $Id: sca_trace_value_handler.cpp 2320 2023-06-08 12:47:38Z pehrlich $

 *****************************************************************************/

/*****************************************************************************/

#include "systemc-ams"
#include "scams/impl/util/tracing/sca_trace_value_handler.h"

namespace sca_util
{
namespace sca_implementation
{

//template specialization for double
template<>
sca_trace_value_handler_base&
sca_trace_value_handler<double>::interpolate(const sca_core::sca_time& ctime)
{
    double cvalue;
    double nvalue;
    double lt = this_time.to_seconds();
    double dt = (next_time-this_time).to_seconds();
    double ct = ctime.to_seconds();


    if(next_value!=NULL)
    {
    	nvalue = dynamic_cast<sca_trace_value_handler<double>* >(next_value)->value();
    }
    else
    {
    	nvalue=dynamic_cast<sca_trace_value_handler<double>* >(this)->value();
    }

    //for template specialization workaround
    double value_tmp=dynamic_cast<sca_trace_value_handler<double>* >(this)->value();

    cvalue=(nvalue-value_tmp)/dt * (ct-lt) + value_tmp;

    //we can create a new item due double is a side effect free data type
    sca_trace_value_handler<double>* rv=this->get_new_local(ctime, this->id,cvalue);

    rv->next_value =next_value;
    rv->next_time  =next_time;
    rv->this_time  =ctime;
    rv->memory_manager=memory_manager;

    return *rv;
}


//template specialization for complex<double>
template<>
sca_trace_value_handler_base&
sca_trace_value_handler<std::complex<double> >::interpolate(const sca_core::sca_time& ctime)
{
    std::complex<double> cvalue;
    std::complex<double> nvalue;

    double lt = this_time.to_seconds();
    double dt = (next_time-this_time).to_seconds();
    double ct = ctime.to_seconds();

    if(next_value!=NULL)
    {
    	nvalue = dynamic_cast<sca_trace_value_handler<std::complex<double> >* >(next_value)->value();
    }
    else
    {
    	nvalue=dynamic_cast<sca_trace_value_handler<std::complex<double> >* >(this)->value();
    }

    //for template specialization workaround
    std::complex<double> value;
    value=dynamic_cast<sca_trace_value_handler<std::complex<double> >* >(this)->value();


    cvalue=(nvalue-value)/dt * (ct-lt) + value;

    //we can create a new item due complex is a side effect free data type
    sca_trace_value_handler<std::complex<double> >* rv=this->get_new_local(ctime, this->id, cvalue);

    rv->next_value=next_value;
    rv->next_time =next_time;
    rv->this_time =ctime;
    rv->memory_manager=memory_manager;

    return *rv;
}

}
}
