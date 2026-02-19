/*****************************************************************************

    Copyright 2010-2013
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

  sca_conservative_view.h - description

  Original Author: Karsten Einwich Fraunhofer IIS/EAS Dresden

  Created on: 04.11.2009

   SVN Version       :  $Revision: 2320 $
   SVN last checkin  :  $Date: 2023-06-08 12:47:38 +0000 (Thu, 08 Jun 2023) $
   SVN checkin by    :  $Author: pehrlich $
   SVN Id            :  $Id: sca_conservative_view.h 2320 2023-06-08 12:47:38Z pehrlich $

 *****************************************************************************/

/*****************************************************************************/


#ifndef SCA_CONSERVATIVE_VIEW_H_
#define SCA_CONSERVATIVE_VIEW_H_


#include "scams/impl/core/sca_view_base.h"

#include <vector>
#include <list>

namespace sca_core
{

namespace sca_implementation
{
class sca_port_base;
class sca_conservative_module;
class sca_conservative_signal;


//type definition for conservative cluster
class sca_conservative_cluster
{

public:

   typedef std::vector<sca_conservative_module*>::iterator iterator;

   void push_back(sca_conservative_module* mod) {modules.push_back(mod);}
   long size()      { return (long)(modules.size()); }
   iterator begin() { return modules.begin();}
   iterator end()   { return modules.end();  }

   std::vector<sca_conservative_module*>  modules;
   std::vector<sca_conservative_signal*>  channels;
   std::vector<sca_conservative_signal*>  ignored_channels;


   //ports to other continous time domains or dt (SystemC) domains
   //communicating via synchronization layer
   std::vector<sca_port_base*>  foreign_ports;

   virtual ~sca_conservative_cluster(){} //makes class polymorphic

};



/**
 base class for conservative views - provides some
 basic mechanism
*/
class sca_conservative_view : public sca_view_base
{

public:

  /** Overwritten (virtual) method for equation setup */
    virtual void setup_equations();

    void assign_node_numbers();
    void cluster_nets();

   sca_conservative_view();
    ~sca_conservative_view();

 protected:

   virtual sca_conservative_cluster* create_cluster() { return new sca_conservative_cluster; }

   typedef std::list<sca_conservative_module*> cmodule_listT;
   cmodule_listT                          tmp_module_list;


   typedef std::vector<sca_conservative_cluster*>            ccluster_listT;
   ccluster_listT                             conservative_clusters;

   void check_modules();
   void move_connected_module( sca_conservative_cluster* cluster,long cluster_id,
                                sca_conservative_module* cmit);

};




} // namespace sca_implementation
} // namespace sca_core


#endif /* SCA_CONSERVATIVE_VIEW_H_ */
