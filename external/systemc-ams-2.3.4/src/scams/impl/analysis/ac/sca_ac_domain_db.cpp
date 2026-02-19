/*****************************************************************************

    Copyright 2010-2013
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

  sca_ac_domain_db.cpp - description

  Original Author: Karsten Einwich Fraunhofer IIS/EAS Dresden

  Created on: 02.01.2010

   SVN Version       :  $Revision: 2046 $
   SVN last checkin  :  $Date: 2017-09-20 12:41:42 +0000 (Wed, 20 Sep 2017) $
   SVN checkin by    :  $Author: karsten $
   SVN Id            :  $Id: sca_ac_domain_db.cpp 2046 2017-09-20 12:41:42Z karsten $

 *****************************************************************************/

/*****************************************************************************/

#include "systemc-ams"
#include "scams/impl/analysis/ac/sca_ac_domain_entity.h"
#include "scams/impl/analysis/ac/sca_ac_domain_db.h"
#include "scams/impl/core/sca_solver_base.h"

namespace sca_ac_analysis
{
namespace sca_implementation
{

///////////////////////////////////////////////////////////////////////////////

sca_ac_domain_db::sca_ac_domain_db()
{
	active_object=NULL;
	equations=NULL;
    active_object=NULL;
    ac_domain=false;
    noise_domain=false;
    bflag=false;
    current_x=NULL;
    current_y=NULL;
    current_w=0.0;
    initialized=false;
    elaborated=false;
}

///////////////////////////////////////////////////////////////////////////////

sca_ac_domain_db::~sca_ac_domain_db()
{
    for(std::vector<sca_ac_domain_entity*>::iterator it=entities.begin();
        it!=entities.end(); ++it)
    {
        delete *it;
    }
}

///////////////////////////////////////////////////////////////////////////////

void sca_ac_domain_db::register_entity(sca_ac_domain_entity* entity)
{
    entities.push_back(entity);
}

///////////////////////////////////////////////////////////////////////////////

void sca_ac_domain_db::register_arc(sc_core::sc_interface* arc)
{
	//if not yet inserted -> insert
	arcs.insert(arc);
}

///////////////////////////////////////////////////////////////////////////////

void sca_ac_domain_db::set_current_node_data(
                      sca_util::sca_vector<sca_util::sca_complex >* node_x,
                      sca_util::sca_vector<sca_util::sca_complex >* node_y,
                      double                        w,
                      bool                          calc_bflag
                               )
{
    current_w = w;

    current_x = node_x;
    current_y = node_y;
    bflag     = calc_bflag;
}


sc_core::sc_object* sca_ac_domain_db::find_object_for_id(long id)
{
	for(std::map<sc_core::sc_interface*,long>::iterator it=arc_map.begin();
			it!=arc_map.end();it++)
	{
		if(it->second==id) return dynamic_cast<sc_core::sc_object*>(it->first);
	}

	return NULL;
}

///////////////////////////////////////////////////////////////////////////////

long sca_ac_domain_db::get_arc_id(const sc_core::sc_interface* arc) const
{
    std::map<sc_core::sc_interface*,long>::const_iterator it=
    		arc_map.find(const_cast<sc_core::sc_interface*>(arc));
    if(it==arc_map.end()) return -1;
    else                  return it->second;
}


///////////////////////////////////////////////////////////////////////////////

long sca_ac_domain_db::get_inport_id(const sc_core::sc_port_base* port)
{
    std::map<const sc_core::sc_port_base*,long>::iterator it=inport_map.find(port);
    if(it!=inport_map.end()) return it->second;
    return -1;
}

///////////////////////////////////////////////////////////////////////////////

long sca_ac_domain_db::get_outport_id(const sc_core::sc_port_base* port)
{
    std::map<const sc_core::sc_port_base*,long>::iterator it=outport_map.find(port);
    if(it!=outport_map.end()) return it->second;
    return -1;
}

///////////////////////////////////////////////////////////////////////////////

long sca_ac_domain_db::get_port_id(const sc_core::sc_port_base* port)
{
    std::map<const sc_core::sc_port_base*,long>::iterator it=inport_map.find(port);
    if(it!=inport_map.end())
    {
    	return it->second;
    }
    else
    {
    	it=outport_map.find(port);
    	if(it!=outport_map.end()) return it->second;
    }
    return -1;
}

///////////////////////////////////////////////////////////////////////////////

void sca_ac_domain_db::register_add_eq_arc(sc_core::sc_interface* arc,
                                           long* rel_pos,
                                           bool* ignore_flag)
{
  long id = (long)add_eq_arcs.size();
  add_eq_arcs.resize(id+1);

  add_eq_arcs[id].arc         = arc;
  add_eq_arcs[id].rel_pos     = rel_pos;
  add_eq_arcs[id].ignore_flag = ignore_flag;
}

///////////////////////////////////////////////////////////////////////////////

//its required for tracing
void sca_ac_domain_db::init_arc_map_for_add_eqs()
{
  for(unsigned long i=0; i<add_eq_arcs.size(); ++i)
  {
    if(add_eq_arcs[i].ignore_flag != NULL)
                             if(*(add_eq_arcs[i].ignore_flag)) continue;

    long rel_pos = *(add_eq_arcs[i].rel_pos);
    if(rel_pos<0) continue;

    //some complicated casting to get the associated solver
    // 1. a interface must be implemented by a sca_channel
    sc_core::sc_interface* sca_if = add_eq_arcs[i].arc;
    sca_core::sca_prim_channel* ch = dynamic_cast<sca_core::sca_prim_channel*>(sca_if);
    if(ch==NULL) continue;
    //the channel of a conservative signal (a channel with represents an arc
    //of an equation system) is associated with one synchronization domain
    sca_core::sca_implementation::sca_synchronization_obj_if* sync_if = ch->get_synchronization_if();
    //a synchronization object must be implemented by a solver
    sca_core::sca_implementation::sca_solver_base* solver =
    	dynamic_cast<sca_core::sca_implementation::sca_solver_base*>(sync_if);
    if(sync_if==NULL) continue;

    for(unsigned long j=0; j<entities.size(); ++j)
    {
      if(entities[j]->solver==solver)
      {
        arc_map[sca_if] = rel_pos + entities[j]->start_of_add_eqs;
        break;
      }
    }
  }
}

 ///////////////////////////////////////////////////////////////////////////////



} // namespace sca_implementation
} // namespace sca_ac_analysis


