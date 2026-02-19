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

 sca_tdf_port_attributes.cpp - description

 Original Author: Karsten Einwich Fraunhofer IIS/EAS Dresden

 Created on: 27.08.2009

 SVN Version       :  $Revision: 2320 $
 SVN last checkin  :  $Date: 2023-06-08 12:47:38 +0000 (Thu, 08 Jun 2023) $
 SVN checkin by    :  $Author: pehrlich $
 SVN Id            :  $Id: sca_tdf_port_attributes.cpp 2320 2023-06-08 12:47:38Z pehrlich $

 *****************************************************************************/

/*****************************************************************************/

#include <systemc-ams>
#include "scams/impl/predefined_moc/tdf/sca_tdf_port_attributes.h"
#include "scams/impl/core/sca_simcontext.h"
#include "scams/impl/core/sca_solver_base.h"

namespace sca_tdf
{

namespace sca_implementation
{


///////////////////////////////////////////////////////////////////////////////

//called after change_attributes if required - by sca_tdf::sca_module
void sca_port_attributes::port_attribute_validation()
{
	if(pmod==NULL)
	{
		SC_REPORT_ERROR("SystemC-AMS","Internal error due a bug should not possible");
		return;
	}

	if(max_timestep_set)
	{
		max_timestep_set=false;
		pmod->set_max_timestep(rate*max_timestep);
	}

	if(timestep_set)
	{
		timestep_set=false;
		pmod->set_timestep(rate*timestep);
	}
}

void sca_port_attributes::update_last_values()
{
	sc_dt::int64 idcnt=pmod->get_sync_domain()->get_synchronization_object_data()->id_counter;




	if(rate_change_id!=idcnt)
	{
		if(last_rate_change_id==idcnt-1)
		{
			last_rate_change_id=idcnt-1;
			last_last_rate=last_rate;
		}
		else
		{
			last_last_rate=this->rate;
		}

		last_rate=this->rate;
	}




	if(delay_change_id!=idcnt)
	{
		if(last_delay_change_id==idcnt-1)
		{
			last_delay_change_id=idcnt-1;
			last_last_delay=last_delay;
		}
		else
		{
			last_last_delay=this->delay;
		}

		last_delay=delay;
	}

}


///////////////////////////////////////////////////////////////////////////////



const unsigned long& sca_port_attributes::get_rate_internal() const
{
	if(pmod->is_change_attributes_executing())
	{
		if(rate_change_id==
		pmod->get_sync_domain()->get_synchronization_object_data()->id_counter)
		{
			return last_rate;
		}
	}

	return rate;
}




const unsigned long& sca_port_attributes::get_rate() const
{
	if(  (pmod==NULL) ||
		 !( pmod->is_ac_processing_executing() ||
			pmod->is_initialize_executing()    ||
			pmod->is_reinitialize_executing()  ||
			pmod->is_processing_executing()    ||
			pmod->is_change_attributes_executing()
		  ))
	{
		 std::ostringstream str;
		 str << "can't execute get_rate() "
				 "outside the context of the callbacks ac_processing, "
				 "initialize, reinitialize, processing or change_attributes "
				 "of the current module ";
		 str << "for port: " << pobj->sca_name();
		 SC_REPORT_ERROR("SytemC-AMS",str.str().c_str());
		 return rate;
	}

	return get_rate_internal();
}


///////////////////////////////////////////////////////////////////////////////


unsigned long sca_port_attributes::get_last_rate_internal() const
{
	//for change_attributes we must return the state before change_attributes
	if(pmod->is_change_attributes_executing())
	{
		sc_dt::int64 idcnt=
				pmod->get_sync_domain()->get_synchronization_object_data()->id_counter;

		if( rate_change_id == idcnt ) //there was a change in the current call
		{
			//there was a change in the current and the last call
			if(last_rate_change_id==idcnt-1) return last_last_rate;
			else                             return last_rate;
		}
		else
		{
			//there was no change in the current call, however in the last call
			if(last_rate_change_id==idcnt-1) return last_rate;
			else                             return this->rate;
		}


	}
	else
	{
		//if there was a change in the current call return the last_rate
		if( rate_change_id ==
		    pmod->get_sync_domain()->get_synchronization_object_data()->id_counter)
		{
			return last_rate;
		}
		else
		{
			return this->rate;
		}
	}


	return this->rate;
}




unsigned long sca_port_attributes::get_last_rate() const
{
	if(  (pmod==NULL) ||
		 !( pmod->is_ac_processing_executing() ||
			pmod->is_initialize_executing()    ||
			pmod->is_reinitialize_executing()  ||
			pmod->is_processing_executing()    ||
			pmod->is_change_attributes_executing()
		  ))
	{
		 std::ostringstream str;
		 str << "can't execute get_last_rate() "
				 "outside the context of the callbacks ac_processing, "
				 "initialize, reinitialize, processing or change_attributes "
				 "of the current module ";
		 str << "for port: " << pobj->sca_name();
		 SC_REPORT_ERROR("SytemC-AMS",str.str().c_str());
		 return this->rate;
	}



	return get_last_rate_internal();
}

///////////////////////////////////////////////////////////////////////////////

const unsigned long& sca_port_attributes::get_delay_internal() const
{
	if( (pmod!=NULL) &&
		(pmod->is_change_attributes_executing()) )
	{
		if(delay_change_id==
		pmod->get_sync_domain()->get_synchronization_object_data()->id_counter)
		{
			return last_delay;
		}
	}

	return delay;
}


const unsigned long& sca_port_attributes::get_delay() const
{
	if(  (pmod==NULL) ||
		 !( pmod->is_ac_processing_executing() ||
			pmod->is_initialize_executing()    ||
			pmod->is_reinitialize_executing()  ||
			pmod->is_processing_executing()    ||
			pmod->is_change_attributes_executing()
		  ))
	{
		 std::ostringstream str;
		 str << "can't execute get_delay() "
				 "outside the context of the callbacks ac_processing, "
				 "initialize, reinitialize, processing or change_attributes "
				 "of the current module ";
		 str << "for port: " << pobj->sca_name();
		 SC_REPORT_ERROR("SytemC-AMS",str.str().c_str());
		 return delay;
	}


	return sca_port_attributes::get_delay_internal();
}



///////////////////////////////////////////////////////////////////////////////


unsigned long sca_port_attributes::get_last_delay_internal() const
{
	sc_dt::int64 idcnt=pmod->get_sync_domain()->get_synchronization_object_data()->id_counter;

	//for change_attributes we must return the state before change_attributes
	if(pmod->is_change_attributes_executing())
	{
		if( delay_change_id == idcnt ) //there was a change in the current call
		{
			//there was a change in the current and the last call
			if(last_delay_change_id==idcnt-1) return last_last_delay;
			else                              return last_delay;
		}
		else
		{
			//there was no change in the current call, however in the last call
			if(last_delay_change_id==idcnt-1) return last_delay;
			else                             return this->delay;
		}


	}
	else
	{
		//if there was a change in the current call return the last_rate
		if( delay_change_id == idcnt)
		{
			return last_delay;
		}
		else
		{
			return this->delay;
		}
	}


	return this->delay;
}




unsigned long sca_port_attributes::get_last_delay() const
{
	if(  (pmod==NULL) ||
		 !( pmod->is_ac_processing_executing() ||
			pmod->is_initialize_executing()    ||
			pmod->is_reinitialize_executing()  ||
			pmod->is_processing_executing()    ||
			pmod->is_change_attributes_executing()
		  ))
	{
		 std::ostringstream str;
		 str << "can't execute get_last_delay() "
				 "outside the context of the callbacks ac_processing, "
				 "initialize, reinitialize, processing or change_attributes "
				 "of the current module ";
		 str << "for port: " << pobj->sca_name();
		 SC_REPORT_ERROR("SytemC-AMS",str.str().c_str());
		 return this->delay;
	}

	return get_last_delay_internal();
}



///////////////////////////////////////////////////////////////////////////////




sca_core::sca_time sca_port_attributes::get_timestep_internal(unsigned long sample_id) const
{
	if(sig_base!=NULL) //normal tdf port
	{
		const_cast<sca_port_attributes*>(this)->timestep_calculated=sig_base->get_timestep_calculated_ref(port_id);
	  //return sig_base->get_timestep(port_id,sample_id);
	}

	//gettimestep used in elaboration for eln/lsf converter ->
    //in this case pmod is NULL

	sca_core::sca_implementation::sca_solver_base* sdomain;
	sdomain=pmod->get_sync_domain();

	const_cast<sca_port_attributes*>(this)->timestep_calculated=sdomain->get_cluster_period()/(get_rate_internal()* sdomain->get_calls_per_period());

	if(pmod->get_sync_domain()->get_cluster_synchronization_data()->attributes_changed)
	{


		sc_dt::int64 call_counter=pmod->get_sync_domain()->get_synchronization_object_data()->call_counter;
		long calls_per_period=sdomain->get_calls_per_period();
		long last_calls_per_period=sdomain->get_last_calls_per_period();

		sca_core::sca_time last_cluster_period=sdomain->get_last_cluster_period();
		sca_core::sca_time current_cluster_timestep=sdomain->get_current_cluster_timestep();



		//the first sample timestep after a change is maybe different
		if((call_counter%calls_per_period==0	) && (sample_id==0))
		{
			unsigned long sample_per_period=get_last_rate_internal()*last_calls_per_period;
			sca_core::sca_time tstep;

			//current_cluster_timestep - current_cluster_start_time - last_cluster_start_time
			//last_cluster_period/samples_per_period -> last_timestep

//			std::cout << " current_cluster_timestep: " << current_cluster_timestep
//					<< " last_cluster_period: " << last_cluster_period << std::endl;

			tstep = current_cluster_timestep-
				(last_cluster_period * (sample_per_period-1))/(sample_per_period);

			if(tstep!=sc_core::SC_ZERO_TIME) return tstep;

		}
	}

	return timestep_calculated;
}





sca_core::sca_time sca_port_attributes::get_timestep(unsigned long sample_id) const
{
	if(  (pmod==NULL) ||
		 !( pmod->is_ac_processing_executing() ||
			pmod->is_initialize_executing()    ||
			pmod->is_reinitialize_executing()  ||
			pmod->is_processing_executing()    ||
			pmod->is_change_attributes_executing()
		  ))
	{
		 std::ostringstream str;
		 str << "can't execute get_timestep() "
				 "outside the context of the callbacks ac_processing, "
				 "initialize, reinitialize, processing or change_attributes "
				 "of the current module ";
		 str << "for port: " << pobj->sca_name();
		 SC_REPORT_ERROR("SytemC-AMS",str.str().c_str());
		 return sc_core::SC_ZERO_TIME;
	}


	if(sample_id>=this->get_rate_internal())
	{
		 std::ostringstream str;
		 str << "can't access to a sample: " << sample_id;
		 str << " due the sample id is equal or greater than the port rate: ";
		 str << this->get_rate() << " in get_timestep for port: ";
		 str << pobj->sca_name();
		 SC_REPORT_ERROR("SytemC-AMS",str.str().c_str());
		 return sc_core::SC_ZERO_TIME;
	}




	return get_timestep_internal(sample_id);
}

///////////////////////////////////////////////////////////////////////////////

sca_core::sca_time sca_port_attributes::get_propagated_timestep() const
{
	return timestep_calculated;
}

//returns the time of the declustered "virtual" source
sca_core::sca_time sca_port_attributes::get_declustered_time() const
{
	return this->declustered_solver->get_current_time();
}

sca_core::sca_time sca_port_attributes::get_timeoffset_internal() const
{
	return timeoffset_calculated;
}




sca_core::sca_time sca_port_attributes::get_timeoffset() const
{
	if(  (pmod==NULL) ||
		 !( pmod->is_ac_processing_executing() ||
			pmod->is_initialize_executing()    ||
			pmod->is_reinitialize_executing()  ||
			pmod->is_processing_executing()    ||
			pmod->is_change_attributes_executing()
		  ))
	{
		 std::ostringstream str;
		 str << "can't execute get_timeoffset() "
				 "outside the context of the callbacks ac_processing, "
				 "initialize, reinitialize, processing or change_attributes "
				 "of the current module ";
		 str << "for port: " << pobj->sca_name();
		 SC_REPORT_ERROR("SytemC-AMS",str.str().c_str());
		 return sc_core::SC_ZERO_TIME;
	}


	return get_timeoffset_internal();
}



///////////////////////////////////////////////////////////////////////////////


sca_core::sca_time sca_port_attributes::get_max_timestep_internal() const
{
	if(pmod->get_max_timestep()==sca_core::sca_max_time())
		                                     return sca_core::sca_max_time();

	return pmod->get_max_timestep()/this->get_rate();
}




sca_core::sca_time sca_port_attributes::get_max_timestep() const
{
	if(  (pmod==NULL) ||
		 !( pmod->is_ac_processing_executing() ||
			pmod->is_initialize_executing()    ||
			pmod->is_reinitialize_executing()  ||
			pmod->is_processing_executing()    ||
			pmod->is_change_attributes_executing()
		  ))
	{
		 std::ostringstream str;
		 str << "can't execute get_max_timestep() "
				 "outside the context of the callbacks ac_processing, "
				 "initialize, reinitialize, processing or change_attributes "
				 "of the current module ";
		 str << "for port: " << pobj->sca_name();
		 SC_REPORT_ERROR("SytemC-AMS",str.str().c_str());
		 return sc_core::SC_ZERO_TIME;
	}


	return get_max_timestep_internal();
}


///////////////////////////////////////////////////////////////////////////////


sca_core::sca_time sca_port_attributes::get_last_timestep_internal(unsigned long sample_id) const
{
	sca_core::sca_implementation::sca_solver_base* sdomain;
	sdomain=pmod->get_sync_domain();


	sc_dt::int64 call_counter=pmod->get_sync_domain()->get_synchronization_object_data()->call_counter;


	long calls_per_period=sdomain->get_calls_per_period();
	long last_calls_per_period=sdomain->get_last_calls_per_period();
	sca_core::sca_time cluster_period=sdomain->get_cluster_period();



	if(sample_id>0)
	{
		return this->get_timestep_internal(sample_id-1);
	}

	//if no sample available and for the first timestep
	//return the propagated timestep
	if(call_counter<1)
	{
			return this->get_timestep_internal(0);
	}


	//first module activation during cluster execution
	if((call_counter%calls_per_period)==0)
	{

		sca_core::sca_implementation::sca_synchronization_obj_if* syncd;
		syncd=pmod->get_sync_domain();

		sca_core::sca_implementation::sca_cluster_synchronization_data* cld;
		cld=syncd->get_cluster_synchronization_data();

		sca_core::sca_time last_cluster_period=cld->last_cluster_period;
		unsigned long last_rate=get_last_rate_internal();

		if((last_calls_per_period>1)||(last_rate>1))
		{
			return last_cluster_period / (last_calls_per_period * last_rate);
		}

		//std::cout << " last_cluster_start_time: "

		return sdomain->get_last_cluster_period();

		//return cld->last_cluster_start_time-cld->last_last_cluster_end_time;
	}
	else
		if((call_counter%calls_per_period)==1)
		{
			if(this->get_rate_internal()==1)
			{
				if(call_counter==1)
				{
					//propagated timestep
					return this->get_timestep_internal(0);
				}

				sca_core::sca_implementation::sca_synchronization_obj_if* syncd;
				syncd=pmod->get_sync_domain();
				sca_core::sca_implementation::sca_cluster_synchronization_data* cld;
				cld=syncd->get_cluster_synchronization_data();

				return cld->cluster_start_time-cld->last_cluster_end_time;
			}
		}

	return cluster_period / (this->get_rate_internal() * calls_per_period);
}




sca_core::sca_time sca_port_attributes::get_last_timestep(unsigned long sample_id) const
{
	if(  (pmodtdf==NULL) ||
		 !( pmod->is_ac_processing_executing() ||
			pmod->is_initialize_executing()    ||
			pmod->is_reinitialize_executing()  ||
			pmod->is_processing_executing()    ||
			pmod->is_change_attributes_executing()
		  ))
	{
		 std::ostringstream str;
		 str << "can't execute get_last_timestep() "
				 "outside the context of the callbacks ac_processing, "
				 "initialize, reinitialize, processing or change_attributes "
				 "of the current module ";
		 str << "for port: " << pobj->sca_name();
		 SC_REPORT_ERROR("SytemC-AMS",str.str().c_str());
		 return sc_core::SC_ZERO_TIME;
	}


	if(sample_id>=this->get_rate_internal())
	{
		 std::ostringstream str;
		 str << "can't access to a sample: " << sample_id;
		 str << " due the sample id is equal or greater than the port rate: ";
		 str << this->get_rate() << " in get_last_timestep for port: ";
		 str << pobj->sca_name();
		 SC_REPORT_ERROR("SytemC-AMS",str.str().c_str());
		 return sc_core::SC_ZERO_TIME;
	}

	return get_last_timestep_internal(sample_id);
}


///////////////////////////////////////////////////////////////////////////////



bool sca_port_attributes::is_timestep_changed_internal(unsigned long sample_id) const
{

	if(sample_id>1) return false;

	sca_core::sca_time ctime=this->get_timestep_internal(sample_id);
	sca_core::sca_time ltime=this->get_last_timestep_internal(sample_id);

	if(ctime>ltime)
	{
		return (ctime-ltime)>sc_core::sc_get_time_resolution();
	}

	return (ltime-ctime)>sc_core::sc_get_time_resolution();
}




bool sca_port_attributes::is_timestep_changed(unsigned long sample_id) const
{
	if(  (pmod==NULL) ||
		 !( pmod->is_ac_processing_executing() ||
			pmod->is_initialize_executing()    ||
			pmod->is_reinitialize_executing()  ||
			pmod->is_processing_executing()    ||
			pmod->is_change_attributes_executing()
		  ))
	{
		 std::ostringstream str;
		 str << "can't execute is_timestep_changed() "
				 "outside the context of the callbacks ac_processing, "
				 "initialize, reinitialize, processing or change_attributes "
				 "of the current module ";
		 str << "for port: " << pobj->sca_name();
		 SC_REPORT_ERROR("SytemC-AMS",str.str().c_str());
		 return false;
	}


	if(sample_id>=this->get_rate_internal())
	{
		 std::ostringstream str;
		 str << "can't access to a sample: " << sample_id;
		 str << " due the sample id is equal or greater than the port rate: ";
		 str << this->get_rate() << " in is_timestep_changed for port: ";
		 str << pobj->sca_name();
		 SC_REPORT_ERROR("SytemC-AMS",str.str().c_str());
		 return false;
	}

	return is_timestep_changed_internal(sample_id);
}


///////////////////////////////////////////////////////////////////////////////


bool sca_port_attributes::is_rate_changed_internal() const
{
	return(get_rate_internal()!=get_last_rate_internal());
}


bool sca_port_attributes::is_rate_changed()     const
{
	if(  (pmod==NULL) ||
		 !( pmod->is_ac_processing_executing() ||
			pmod->is_initialize_executing()    ||
			pmod->is_reinitialize_executing()  ||
			pmod->is_processing_executing()    ||
			pmod->is_change_attributes_executing()
		  ))
	{
		 std::ostringstream str;
		 str << "can't execute is_rate_changed() "
				 "outside the context of the callbacks ac_processing, "
				 "initialize, reinitialize, processing or change_attributes "
				 "of the current module ";
		 str << "for port: " << pobj->sca_name();
		 SC_REPORT_ERROR("SytemC-AMS",str.str().c_str());
		 return false;
	}

	return is_rate_changed_internal();
}

///////////////////////////////////////////////////////////////////////////////


bool sca_port_attributes::is_delay_changed_internal()    const
{
	return (sca_port_attributes::get_delay_internal() != get_last_delay_internal());
}



bool sca_port_attributes::is_delay_changed()    const
{
	if(  (pmod==NULL) ||
		 !( pmod->is_ac_processing_executing() ||
			pmod->is_initialize_executing()    ||
			pmod->is_reinitialize_executing()  ||
			pmod->is_processing_executing()    ||
			pmod->is_change_attributes_executing()
		  ))
	{
		 std::ostringstream str;
		 str << "can't execute is_delay_changed() "
				 "outside the context of the callbacks ac_processing, "
				 "initialize, reinitialize, processing or change_attributes "
				 "of the current module (see LRM) ";
		 str << "for port: " << pobj->sca_name();
		 SC_REPORT_ERROR("SytemC-AMS",str.str().c_str());
		 return false;
	}

	return is_delay_changed_internal();
}



///////////////////////////////////////////////////////////////////////////////


void sca_port_attributes::set_rate(unsigned long rate_)
{
	if(  (pmod==NULL) ||
		 !( pmod->is_set_attributes_executing() ||
			pmod->is_change_attributes_executing()
		  ))
	{
		 std::ostringstream str;
		 str << "can't execute set_rate() "
				 "outside the context of the callbacks set_attributes, "
				 "or change_attributes of the current module (see LRM) ";
		 str << "for port: " << pobj->sca_name();
		 SC_REPORT_ERROR("SytemC-AMS",str.str().c_str());
		 return;
	}

	if(rate_==0)
	{
		std::ostringstream str;
		str << "Attribute rate must be greater 0 for port: ";
		str <<  pobj->sca_name();
		SC_REPORT_ERROR("SystemC-AMS",str.str().c_str());
		return;
	}

	if(long(rate_)<0)
	{
		std::ostringstream str;
		str << "Attribute rate is set to a very large value, maybe a negative"
		    << "value was assigned, cannot allocate the required memory for port: ";
		str <<  pobj->sca_name();
		SC_REPORT_ERROR("SystemC-AMS",str.str().c_str());
		return;
	}



	if(pmod->is_change_attributes_executing())
	{
		if(this->rate != rate_)
		{
			sc_dt::int64 idcnt=pmod->get_sync_domain()->
					get_synchronization_object_data()->id_counter;


			if(rate_change_id!=idcnt) //only if first time called in change_attributes
			{
				last_last_rate=last_rate;
				last_rate=this->rate;


				last_rate_change_id=rate_change_id;
				rate_change_id=idcnt;
			}

			pmod->get_sync_domain()->get_cluster_synchronization_data()->rescheduling_requested=true;

			rate=rate_;
		}
	}
	else //set_attributes()
	{
		last_rate=rate_;
		last_last_rate=rate_;

		rate=rate_;
	}
}


///////////////////////////////////////////////////////////////////////////////

void sca_port_attributes::set_delay_direct(unsigned long delay_)
{
	delay=delay_;
	last_delay=delay_;
	last_last_delay=delay_;
}

void sca_port_attributes::set_delay(unsigned long delay_)
{
	if( (pmod==NULL) ||
		!(pmod->is_set_attributes_executing()    ||
		  pmod->is_change_attributes_executing()     ))
	{
		std::ostringstream str;
		str << "Method set_delay of port: " << pobj->sca_name()
			<< " can only be called in the context of set_attributes "
			<< "or change_attributes of the parent module"
			<< " (see LRM for details)";
		SC_REPORT_ERROR("SystemC-AMS",str.str().c_str());
		return;
	}

	if(long(delay_) < 0)
	{
	    std::ostringstream str;
	    str << "For TDF-Port: " << pobj->sca_name()
	        << " the Delay set by sca_sdf_port::set_delay() is a very large value"
	        << " maybe a negative value was assigned"
	        << " cannot allocate required memory";
	    SC_REPORT_ERROR("SystemC-AMS", str.str().c_str());
	    return;
	}

	if(pmod->is_change_attributes_executing())
	{
		if(this->delay != delay_)
		{
			sc_dt::int64 idcnt=pmod->get_sync_domain()->
					get_synchronization_object_data()->id_counter;

			if(delay_change_id!=idcnt) //only if first time called in change_attributes
			{
				last_last_delay=last_delay;
				last_delay=this->delay;


				last_delay_change_id=delay_change_id;
				delay_change_id=idcnt;
			}

			pmod->get_sync_domain()->get_cluster_synchronization_data()->rescheduling_requested=true;

			delay=delay_;
		}
	}
	else //set_attributes()
	{
		delay=delay_;
		last_delay=delay_;
		last_last_delay=delay_;
	}
}


///////////////////////////////////////////////////////////////////////////////


void sca_port_attributes::set_timestep(const sca_core::sca_time& timestep_)
{
	if(pmod==NULL)
	{
		SC_REPORT_ERROR("SystemC-AMS","Internal error due a bug should not possible");
		return;
	}


	if( !( pmod->is_set_attributes_executing() ||
		   pmod->is_change_attributes_executing()  ))
	{
		std::ostringstream str;
		str << "Method set_timestep of port: " << pobj->sca_name()
			<< " can only be called in the context of set_attributes or change_attributes"
			<< " of the parent module (see LRM)";
		SC_REPORT_ERROR("SystemC-AMS",str.str().c_str());
	}


	timestep=timestep_;
	timestep_calculated=timestep_;
	timestep_set=true;

	//also used in lsf/eln converter ports (before elaboration only)
	if(pmodtdf!=NULL) pmodtdf->port_attribute_validation_required=true;


	if(pmod->is_set_attributes_executing())
	{
		timestep_change_id=-1;
	}
	else
	{
		timestep_change_id=pmod->get_sync_domain()->
					get_cluster_synchronization_data()->cluster_execution_cnt;
	}

	if(pmod->is_change_attributes_executing() )
	{
		  pmod->set_timestep(timestep*get_rate());
	}

}



///////////////////////////////////////////////////////////////////////////////


void sca_port_attributes::set_max_timestep(const sca_core::sca_time& timestep_)
{

	if(  (pmod==NULL) ||
	    !(pmod->is_set_attributes_executing() ||
		  pmod->is_change_attributes_executing()  ))
	{
		std::ostringstream str;
		str << "Method set_max_timestep of port: " << pobj->sca_name()
			<< " can only be called in the context of set_attributes or change_attributes"
			<< " of the parent module (see LRM)";
		SC_REPORT_ERROR("SystemC-AMS",str.str().c_str());
		return;
	}


	if(pmod->is_change_attributes_executing() )
	{
		pmod->set_max_timestep(timestep_*get_rate());
	}


	max_timestep=timestep_;
	max_timestep_set=true;
	pmodtdf->port_attribute_validation_required=true;
}


///////////////////////////////////////////////////////////////////////////////


void sca_port_attributes::set_timeoffset(const sca_core::sca_time& timeoffset_)
{
	std::ostringstream str;
	str << "Method set_timeoffset of port: " << pobj->sca_name()
		<< " is obsolete and not anymore implemented in SystemC-AMS 2.0";
	SC_REPORT_ERROR("SystemC-AMS",str.str().c_str());


	timeoffset=timeoffset_;
	timeoffset_calculated=timeoffset_;
}


///////////////////////////////////////////////////////////////////////////////

sca_core::sca_time sca_port_attributes::get_time_internal(unsigned long sample) const
{
    sca_core::sca_time cp=pmod->get_sync_domain()->get_cluster_synchronization_data()->cluster_period;
    long calls_per_period=pmod->get_sync_domain()->get_calls_per_period();
    sc_dt::int64 call_counter=pmod->get_sync_domain()->get_synchronization_object_data()->call_counter;
    unsigned long rate=get_rate_internal();
    sca_core::sca_time cluster_start_time=pmod->get_sync_domain()->get_cluster_synchronization_data()->cluster_start_time;


    //calculate number of samples per cluster period (number of module calls * port rate)
    long sample_period = calls_per_period * rate;
    //calculate sample number since cluster start (module call since start * port rate)
    long sample_call   =(call_counter%calls_per_period) * rate;

	if(call_counter<0)
	{
		return cluster_start_time + cp*sample/sample_period;
	}

    sca_core::sca_time ct=cluster_start_time + (cp* (sample_call+sample))/sample_period;
    return ct;
}



///////////////////////////////////////////////////////////////////////////////

sca_core::sca_time sca_port_attributes::get_time(unsigned long sample) const
{
	if(  (pmod==NULL) ||
		 !( pmod->is_ac_processing_executing() ||
			pmod->is_initialize_executing()    ||
			pmod->is_reinitialize_executing()  ||
			pmod->is_processing_executing()    ||
			pmod->is_change_attributes_executing()
		  ))
	{
		 std::ostringstream str;
		 str << "can't execute get_time() "
				 "outside the context of the callbacks ac_processing, "
				 "initialize, reinitialize, processing or change_attributes "
				 "of the current module (see LRM) ";
		 str << "for port: " << pobj->sca_name();
		 SC_REPORT_ERROR("SytemC-AMS",str.str().c_str());
		 return sc_core::SC_ZERO_TIME;
	}

	return get_time_internal(sample);
}


///////////////////////////////////////////////////////////////////////////////

sca_port_attributes::sca_port_attributes(sca_core::sca_implementation::sca_port_base* port):
		pobj(port)
{
	 delay_change_id=0;
	 last_rate_change_id=0;
     last_delay_change_id=0;
     rate_change_id=0;

      rate = 1;
      delay = 0;

      last_rate = 1;
      last_last_rate = 1;

      last_delay = 0;
      last_last_delay = 0;

      timeoffset = sc_core::SC_ZERO_TIME;
      timestep = sca_core::sca_implementation::NOT_VALID_SCA_TIME();

      timeoffset_calculated = sc_core::SC_ZERO_TIME;
      timestep_calculated = sca_core::sca_implementation::NOT_VALID_SCA_TIME();

      max_timestep_set = false;
      timestep_set = false;

      pmod = pobj->get_parent_module();
      pmodtdf = dynamic_cast<sca_tdf::sca_module*> (pmod);

      timestep_change_id = -2;

      sig_base = NULL;
      port_id = 0;

      decluster_port = false;
      declustered_solver = NULL;

      decluster_timestep_calculated
          = sca_core::sca_implementation::NOT_VALID_SCA_TIME();
      decluster_timestep_change_id = -2;
      decluster_rate = 1;
      decluster_rate_old = 1;
      decluster_delay = 0;
      decluster_delay_old = 0;

}


void sca_port_attributes::set_parent_attribute_module(sca_core::sca_module* mod)
{
	   pmod = mod;
	   pmodtdf = dynamic_cast<sca_tdf::sca_module*> (pmod);
}


///////////////////////////////////////////////////////////////////////////////


void sca_port_attributes::initialize_all_traces()
{
	sca_core::sca_implementation::sca_get_curr_simcontext()->initialize_all_traces();
}

void sca_port_attributes::signal_elaboration(sca_tdf::sca_implementation::sca_tdf_signal_impl_base* signal)
{

	//signal interface has to be initialized for decluster ports also
	signal->resize_port_data(port_id);


	//the connected signal belongs to the other cluster - thus we disconnect
	//all variables of this cluster and "virtually" split the port
	if(decluster_port)
	{
		signal->timestep_calculated[port_id]=&decluster_timestep_calculated;
		signal->port_timestep_change_id[port_id]=&decluster_timestep_change_id;
		signal->rates[port_id]=&decluster_rate;
		signal->rates_old[port_id]=&decluster_rate_old;
		signal->delays[port_id]=&decluster_delay;
		signal->delays_old[port_id]=&decluster_delay_old;
	}
	else
	{
		sig_base=signal;
		sig_base->timestep_calculated[port_id]=&this->timestep_calculated;
		sig_base->port_timestep_change_id[port_id]=&this->timestep_change_id;
		sig_base->rates[port_id]=&this->rate;
		sig_base->rates_old[port_id]=&this->last_rate;
		sig_base->delays[port_id]=&this->delay;
		sig_base->delays_old[port_id]=&this->last_delay;
	}

}


}
}

