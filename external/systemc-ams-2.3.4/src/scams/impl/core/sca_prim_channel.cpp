/*****************************************************************************

    Copyright 2010-2013
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

  sca_prim_channel.cpp - description

  Original Author: Karsten Einwich Fraunhofer IIS/EAS Dresden / COSEDA Technologies

  Created on: 15.05.2009

   SVN Version       :  $Revision: 2320 $
   SVN last checkin  :  $Date: 2023-06-08 12:47:38 +0000 (Thu, 08 Jun 2023) $
   SVN checkin by    :  $Author: pehrlich $
   SVN Id            :  $Id: sca_prim_channel.cpp 2320 2023-06-08 12:47:38Z pehrlich $

 *****************************************************************************/

/*****************************************************************************/

#include<systemc-ams>

#include "scams/core/sca_prim_channel.h"


#include "scams/impl/core/sca_simcontext.h"
#include "scams/impl/core/sca_solver_base.h"
#include "scams/impl/core/sca_object_manager.h"
#include "scams/impl/core/sca_port_base.h"
#include "scams/impl/util/tracing/sca_trace_file_base.h"


namespace sca_core
{

using namespace sca_implementation;

sca_prim_channel::sca_prim_channel()
{
	construct();
}

sca_prim_channel::sca_prim_channel(const char* _name) : ::sc_core::sc_object(_name)
{
	construct();
}

//reset data for rescheduling;
void sca_prim_channel::reset_for_reschedule()
{
    max_samples    = -1;
    multiple       =  1;
    remainder_flag = false;
    max_buffer_usage=-1;

}


void sca_prim_channel::construct()
{
    sca_get_curr_simcontext()->get_sca_object_manager()->insert_channel(this);

    this->reset_for_reschedule();

    cluster_id=-1;  //no id assigned

    if(sca_get_curr_simcontext()->construction_finished())
    {
    	std::ostringstream str;
    	str << " construction of sca_prim_channel not allowed after elaboration has been finished for: ";
    	str << name();
    	SC_REPORT_ERROR("SystemC-AMS",str.str().c_str());
    }
}


const char* sca_prim_channel::kind() const
{
	return "sca_core::sca_prim_channel";
}

///////////////////////////////////////////////////////////////////

sca_prim_channel::~sca_prim_channel()
{
	if(sca_get_curr_simcontext()!=NULL)
	{
		if((sc_core::sc_status() & 0x3)!=0) //during elaboration delte allowed
		{
			delete sca_get_curr_simcontext();
		}
	}
}

///////////////////////////////////////////////////////////////////

void sca_prim_channel::end_of_elaboration()
{
}

///////////////////////////////////////////////////////////////////

void sca_prim_channel::register_port(sca_port_base* port)
{
  //sets id for port identificattion
	  port->set_if_id((unsigned long)(port_list.size()));
  port_list.push_back(port);

  //get parent module of port
  sca_module* module = port->get_parent_module();
  //look whether it is yet inserted
  sca_module_list_iteratorT itm;
  for(itm=module_list.begin(); itm<module_list.end(); ++itm)
  {
    if(*itm==module)	break;
  }
  //insert connected module
  if(itm>=module_list.end())
    module_list.push_back(module);
}

///////////////////////////////////////////////////////////////////////////

unsigned long sca_prim_channel::get_number_of_connected_modules() const
{
	 return (unsigned long)(module_list.size());
}

///////////////////////////////////////////////////////////////////////////

sca_module_listT& sca_prim_channel::get_connected_module_list()
{
	return module_list;
}



const sca_module_listT& sca_prim_channel::get_connected_module_list() const
{
	return module_list;
}


///////////////////////////////////////////////////////////////////////////

unsigned long sca_prim_channel::get_number_of_connected_ports() const
{
	  return (unsigned long)(port_list.size());
}

///////////////////////////////////////////////////////////////////////////

sca_port_base** sca_prim_channel::get_connected_port_list() const
{
  return const_cast<sca_port_base**>(&(*port_list.begin()));   //!!!!!!!!!!!!!!!!!!!!!!!
}


///////////////////////////////////////////////////////////////////////////

void sca_prim_channel::register_sc_interface(::sc_core::sc_interface* channel)
{
  systemc_channel = channel;
}

///////////////////////////////////////////////////////////////////////////

void sca_prim_channel::register_connected_solver(sca_synchronization_port_data* sync_port)
{
  if( (sync_port->dir == SCA_TO_ANALOG) |
      (sync_port->dir == SCA_TO_SYSTEMC) )
  {
     from_solver.push_back(sync_port->parent_obj);
     from_solver_sync_port.push_back(sync_port);
  }
  else
  {
  	to_solver.push_back(sync_port->parent_obj);
  	to_solver_sync_port.push_back(sync_port);
  }
}

///////////////////////////////////////////////////////////////////////////

sca_prim_channel::sca_solver_listT& sca_prim_channel::get_connected_solvers(bool con_from_solver)
{
  if(con_from_solver) return from_solver;
  else                return to_solver;
}

///////////////////////////////////////////////////////////////////////////

sca_prim_channel::sca_sync_port_listT& sca_prim_channel::get_connected_sync_ports
									(bool con_from_solver)
{
  if(con_from_solver) return from_solver_sync_port;
  else                return to_solver_sync_port;
}

///////////////////////////////////////////////////////////////////////////

void sca_prim_channel::print_connected_module_names(std::ostream& ostr)
{
  for(sca_module_list_iteratorT it=module_list.begin();
      it != module_list.end(); ++it)

      ostr << (*it)->name() << std::endl;
}

///////////////////////////////////////////////////////////////////////////

sca_synchronization_obj_if* sca_prim_channel::get_synchronization_if() const
{
  if(to_solver.size())          return to_solver[0];
  else if(from_solver.size())   return from_solver[0];
  else if(module_list.size()>0) return module_list[0]->get_sync_domain();
  else return NULL;
}

///////////////////////////////////////////////////////////////////////////

bool sca_prim_channel::trace_init(sca_util::sca_implementation::sca_trace_object_data& data)
{
    std::ostringstream str;
    str << "Tracing not supported for this kind of sca_channel "
         << name() << std::endl;
    SC_REPORT_WARNING("SystemC-AMS",str.str().c_str());

    return false;
}

///////////////////////////////////////////////////////////////////////////

void sca_prim_channel::trace(long id,sca_util::sca_implementation::sca_trace_file_base& tf)
{

}


void sca_prim_channel::resize_buffer()
{

}

void sca_prim_channel::trace( sc_core::sc_trace_file* tf ) const
{
	std::ostringstream str;
	str << "sc_trace of sca_prim_channel not supported for channel: " << this->name();
	SC_REPORT_WARNING("SystemC-AMS",str.str().c_str());
}


} //namespace sca_core

///////////////////////////////////////////////////////////////////////////

