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

  sca_conservative_view.cpp - description

  Original Author: Karsten Einwich Fraunhofer IIS/EAS Dresden

  Created on: 04.11.2009

   SVN Version       :  $Revision: 2320 $
   SVN last checkin  :  $Date: 2023-06-08 12:47:38 +0000 (Thu, 08 Jun 2023) $
   SVN checkin by    :  $Author: pehrlich $
   SVN Id            :  $Id: sca_conservative_view.cpp 2320 2023-06-08 12:47:38Z pehrlich $

 *****************************************************************************/

/*****************************************************************************/

#include "systemc-ams"
#include "scams/impl/core/sca_view_object.h"
#include "scams/impl/predefined_moc/conservative/sca_conservative_view.h"
#include "scams/impl/predefined_moc/conservative/sca_conservative_module.h"
#include "scams/impl/predefined_moc/conservative/sca_conservative_interface.h"
#include "scams/impl/predefined_moc/conservative/sca_conservative_signal.h"


namespace sca_core
{
namespace sca_implementation
{

////////////////////////////////////////////////

sca_conservative_view::sca_conservative_view()
{
}

////////////////////////////////////////////////

sca_conservative_view::~sca_conservative_view()
{
    for(ccluster_listT::iterator cit=conservative_clusters.begin();
        cit!=conservative_clusters.end(); ++cit)
    {
        delete *cit;
    }
}

////////////////////////////////////////////////

void sca_conservative_view::check_modules()
{

    for(sca_view_object::sca_module_list_iteratorT mit=
                view_datas->get_module_list().begin();
            mit!=view_datas->get_module_list().end();
            ++mit
       )
    {
        sca_conservative_module* conm;
        conm=dynamic_cast<sca_conservative_module*>(*mit);

        if(conm==NULL)
        {
            /**
              this should not be occur due the sca_conservative-view is only
              used by sca_conservative_modules
            */

            std::ostringstream str;
            str << "Error: Module: " << (*mit)->name()
            << " has instantiats a conservative-view "
            << " however it is no sca_conservative_module" << std::endl;
            SC_REPORT_ERROR("SystemC-AMS",str.str().c_str());
        }

        //copy to temprarly list
        tmp_module_list.push_back(conm);

        //for each port - check all connected interfaces
        for(sca_core::sca_module::sca_port_base_list_iteratorT pit=
                    (*mit)->get_port_list().begin();
                pit!=(*mit)->get_port_list().end();
                ++pit
           )
        {

        if( ((*pit)->get_port_type()==
        		 sca_core::sca_implementation::sca_port_base::SCA_SC_IN_PORT) ||
            ((*pit)->get_port_type()==
        		 sca_core::sca_implementation::sca_port_base::SCA_SC_OUT_PORT) ||
            ((*pit)->get_port_type()==
        		 sca_core::sca_implementation::sca_port_base::SCA_SC_INOUT_PORT))
        {
        	continue;
        }

            sca_core::sca_interface* sca_if = (*pit)->sca_get_interface();

            if(sca_if==NULL)
            {
            	std::ostringstream str;
                str << "Error module: " << (*mit)->name() << " Port: "
                << (*pit)->get_port_number() << " : " << (*pit)->sca_name()
                << " not bound (with a sca_interface)"
                << std::endl;
                SC_REPORT_ERROR("SystemC-AMS",str.str().c_str());
            }

            sca_conservative_interface* con_if=
                dynamic_cast<sca_conservative_interface*>(sca_if);


            if(con_if==NULL)
                continue;  //ignore non conservative signals

            sca_conservative_signal* con_ch=
                dynamic_cast<sca_conservative_signal*>(sca_if);

            //every conservative interface must be implemented by a conservative
            //channel, due the channel provides some basic functionality for
            //conservative signals
            if(con_ch==NULL)
            {
                std::ostringstream str;
                str << "Error module: " << (*mit)->name() << " Port: "
                << (*pit)->get_port_number() << " the bound conservative interface "
                << "(sca_conservative_interface) is not implemented by a "
                << " conservative channel (sca_conservative_channel)"
                << std::endl;
                SC_REPORT_ERROR("SystemC-AMS",str.str().c_str());
            }


        } //for each port
    } //for each module
}



////////////////////////////////////////////////

void sca_conservative_view::cluster_nets()
{
    check_modules();

    cmodule_listT::iterator cmit = tmp_module_list.begin();

    while(cmit!=tmp_module_list.end())
    {
        //create new cluster
        long cluster_id;
        sca_conservative_cluster* cluster= create_cluster();
        cluster_id = (long)(conservative_clusters.size());
        conservative_clusters.push_back(cluster);


        //rekursive method which moves all connected objs
        //to the cluster
        move_connected_module(cluster,cluster_id,*cmit);

        cmit = tmp_module_list.begin();
    }

}

///////////////////////////////////////////////////////////////////////////////

void sca_conservative_view::move_connected_module(
		sca_conservative_cluster* cluster,
    long cluster_id,
    sca_conservative_module* cmod_
)
{
	std::vector<sca_conservative_module*> mmods;
	mmods.reserve(tmp_module_list.size());
	cmod_->cluster_id=cluster_id; //assign module to current cluster
    cluster->push_back(cmod_);

    //add to list for following ports
	mmods.push_back(cmod_);

	while(mmods.size()>0)
	{
		//next module to follow ports
		sca_conservative_module* cmod=mmods.back();
	    mmods.pop_back();

	    //remove module from tmp list
	    cmodule_listT::iterator cmit ;

	    cmit = find(tmp_module_list.begin(),tmp_module_list.end(),cmod);

	    if(cmit == tmp_module_list.end())
	    {
	    	/**
	    	 * error should not be possible due all consevative modules
	    	 * must be inside the list
	    	 * may all can be substituted by tmp_module_list.remove(*cmit);
	    	 */

	    	std::ostringstream str;
	    	str << "Internal Error: " << __FILE__ << " line: " << __LINE__
	    		<< " clustering algorithm for conservative modules failed "
                << " cant find: " << cmod->name() << " in tmp list "
                << std::endl;
	    	SC_REPORT_ERROR("SystemC-AMS",str.str().c_str());
	    }

	    tmp_module_list.erase(cmit);


	    //find other modules connected to the cluster
	    //go in the direction of all ports
	    long pnr=0;
	    for(sca_module::sca_port_base_list_iteratorT pit=cmod->get_port_list().begin();
            pit!=cmod->get_port_list().end();
            ++pit, ++pnr
	    	)
	    {
	    	//get current port
	    	sca_port_base *cport = *pit;

	    	sca_interface* scaif = cport->sca_get_interface();

	    	if(scaif == NULL)
	    	{
	    		cluster->foreign_ports.push_back(cport);
	    		continue;  //ignore -> converter port
	    	}

	    	sca_conservative_interface* conif;

	    	conif = dynamic_cast<sca_conservative_interface*>(scaif);

	    	//store if to other domains
	    	if(conif==NULL)
	    	{
	    		cluster->foreign_ports.push_back(cport);
	    		continue;
	    	}

	    	sca_conservative_signal* conch;
	    	conch = dynamic_cast<sca_conservative_signal*>(conif);

	    	if(conch==NULL)
	    	{
	    		/**
	    		 * cant occur due it checked in check_modules
	    		 */

	    		std::ostringstream str;
	    		str << "Error a conservative interface must be implemented by a "
	    				<< "conservative signal in signal connected to module: "
						<< cmod->name() << " port: " << pnr << std::endl;
	    		SC_REPORT_ERROR("SystemC-AMS",str.str().c_str());
	    		return;
	    	}

	    	//node (e.g. reference node) will not be considered for clustering
	    	if(conch->is_reference_node())
	    	{
	    		if(conch->cluster_id!=cluster_id)
	    		{
	    			conch->cluster_id  = cluster_id;
	    			conch->node_number = (long)(cluster->ignored_channels.size());
	    			cluster->ignored_channels.push_back(conch);
	    		}

	    		//ignore node for clustering
	    		if(conch->ignore_node_for_clustering()) continue;
	    	}
	    	else
	    	{
	    		if(conch->cluster_id!=cluster_id)
	    		{
	    			//if cluster yet assigned the channel connects
	    			//different conservative views
	    			if(conch->cluster_id>=0)
	    				conch->connects_cviews = true;
	    			conch->cluster_id  = cluster_id;
	    			conch->node_number = (long)(cluster->channels.size());
	    			cluster->channels.push_back(conch);
	    		}
	    	}

	    	unsigned long  ncmod = conch -> get_number_of_connected_modules();
	    	std::vector<sca_core::sca_module*>& modules = conch -> get_connected_module_list();

	    	//for all connected modules
	    	for(unsigned long i=0; i<ncmod; ++i)
	    	{
	    		sca_conservative_module * ncmodp;

	    		ncmodp=dynamic_cast<sca_conservative_module*>(modules[i]);

	    		//ignore not conservative modules
	    		if(ncmodp==NULL)
	    			continue;

	    		//ignore yet clustered modules
	    		if(ncmodp->cluster_id>=0)
	    		{
	    			if(ncmodp->cluster_id!=cluster_id)
	    			{
	    				/**
	    				 * error should not be possible due if the module
	    				 * not in the same cluster the clustering cur_algorithm
	    				 * must be wrong
	    				 */

	    				std::ostringstream str;
	    				str << "Internal Error: " << __FILE__ << " line: " << __LINE__
	    					<< " clustering algorithm for conservative modules failed "
							<< " for modules: " << cmod->name() << " and " << ncmodp->name()
							<< std::endl;
	    				SC_REPORT_ERROR("SystemC-AMS",str.str().c_str());
	    			}
	    			continue;
	    		}

	    		//recursive call -> changed to while loop, due stack size limitation
	    		//move_connected_module(cluster,cluster_id,ncmodp);

	    	    //assign module to current cluster
	    	    cluster->push_back(ncmodp);
	    		ncmodp->cluster_id = cluster_id;
	    		mmods.push_back(ncmodp); //store to further follow ports

	    	} //for all connected modules
	    } //for all ports of current module
	} //while(mmods.size()>0)
}

//////////////////////////////////////////////////////////////////////////////

void sca_conservative_view::assign_node_numbers()
{
}



//////////////////////////////////////////////////////////////////////////////

void sca_conservative_view::setup_equations()
{
}




} // namespace sca_implementation
} // namespace sca_core
