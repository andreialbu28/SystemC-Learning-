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

  sca_sync_value_handle.cpp - description

  Original Author: Karsten Einwich Fraunhofer IIS/EAS Dresden

  Created on: 27.08.2009

   SVN Version       :  $Revision: 1265 $
   SVN last checkin  :  $Date: 2011-11-19 20:43:31 +0000 (Sat, 19 Nov 2011) $
   SVN checkin by    :  $Author: karsten $
   SVN Id            :  $Id: sca_sync_value_handle.cpp 1265 2011-11-19 20:43:31Z karsten $

 *****************************************************************************/

/*****************************************************************************/

#include <systemc>
#include "scams/impl/synchronization/sca_sync_value_handle.h"

namespace sca_core
{
namespace sca_implementation
{

void sca_sync_value_handle_base::call_method()
{
	(object->*method)();
}


void sca_sync_value_handle_base::set_method(::sc_core::sc_object* obj, value_method method_)
{
	object=obj;
	method=method_;
}


long sca_sync_value_handle_base::get_index()
{
	return index;
}

void sca_sync_value_handle_base::set_index(long index_)
{
	index = index_;
}


void sca_sync_value_handle_base::set_id(long id_)
{
	id=id_;
}


::sc_core::sc_object* sca_sync_value_handle_base::get_object()
{
	return object;
}

long sca_sync_value_handle_base::get_id()
{
	return id;
}

sca_sync_value_handle_base::~sca_sync_value_handle_base()
{

}




}
}
