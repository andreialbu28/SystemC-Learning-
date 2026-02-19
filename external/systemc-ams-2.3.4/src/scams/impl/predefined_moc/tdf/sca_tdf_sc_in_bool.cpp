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

 sca_tdf_sc_in_bool.cpp - converter port SystemC -> tdf

 Original Author: Karsten Einwich Fraunhofer / COSEDA Technologies

 Created on: 10.04.2012

 SVN Version       :  $Revision: 2320 $
 SVN last checkin  :  $Date: 2023-06-08 12:47:38 +0000 (Thu, 08 Jun 2023) $ (UTC)
 SVN checkin by    :  $Author: pehrlich $
 SVN Id            :  $Id: sca_tdf_sc_in_bool.cpp 2320 2023-06-08 12:47:38Z pehrlich $


 *****************************************************************************/
/*
 LRM clause 4.1.1.11.
 */

/*****************************************************************************/

#include <systemc-ams>

namespace sca_tdf
{

namespace sca_de
{

sc_core::sc_interface* sca_in<bool>::get_interface()
{
	return this->sc_get_interface();
}


const sc_core::sc_interface* sca_in<bool>::get_interface() const
{
	return this->sc_get_interface();
}


/** method of interactive tracing interface, which returns the value
 * at the current SystemC time (the time returned by sc_core::sc_time_stamp())
 */
const bool& sca_in<bool>::get_typed_trace_value() const
{
	if(this->value_forced)
	{
		return this->forced_value;
	}

	//return last written value
	std::uint64_t scnt=this->pmod->get_call_counter() * this->get_rate_internal() + this->get_delay_internal() + this->write_cnt - 1;

	return this->buffer[scnt%buffer_size];
}

const std::string& sca_in<bool>::get_trace_value() const
{
	std::ostringstream str;
	str << get_typed_trace_value();
	current_trace_value_string=str.str();

	return current_trace_value_string;
}

/**
 * method for register a trace callback
 */
inline bool sca_in<bool>::register_trace_callback(sca_util::sca_traceable_object::callback_functor_base& func)
{
	this->callbacks.push_back(&func);
	this->callback_registered=true;
    return true;
}

/**
 * method for removing a trace callback
 */
inline bool sca_in<bool>::remove_trace_callback(sca_util::sca_traceable_object::callback_functor_base& func)
{
	for(std::vector<sca_util::sca_traceable_object::callback_functor_base*>::iterator
			it=this->callbacks.begin();it!=this->callbacks.end();++it)
	{
		if(*it == &func)
		{
			this->callbacks.erase(it);
			if(this->callbacks.size()<=0) this->callback_registered=false;
			return true;
		}
	}

	return false;
}


/** method of interactive tracing interface, which forces a value
 */
inline void sca_in<bool>::force_typed_value(const bool& val)
{
	this->forced_value=val;
	this->value_forced=true;
}



void sca_in<bool>::set_force_value(const std::string& stri)
{
	if(!sca_util::sca_implementation::convert_from_string(forced_value,stri))
	{
		std::ostringstream str;
		str << "Cannot convert string: " << stri << " to port type of port: ";
		str << this->name() << " in method force_value - use force_typed_value instead";
		SC_REPORT_WARNING("SystemC-AMS",str.str().c_str());
		return;
	}

	this->value_forced=true;
}


/** method of interactive tracing interface, which releases a forced value
*/
void sca_in<bool>::release_value()
{
	this->value_forced=false;
}

const sc_core::sc_event& sca_in<bool>::default_event() const
{
	return (*this)->default_event();
}



const sc_core::sc_event& sca_in<bool>::value_changed_event() const
{
	return (*this)->value_changed_event();
}


const sc_core::sc_event& sca_in<bool>::posedge_event() const
{
	return (*this)->posedge_event();
}


const sc_core::sc_event& sca_in<bool>::negedge_event() const
{
	return (*this)->negedge_event();
}



bool sca_in<bool>::event() const
{
	return (*this)->event();
}



bool sca_in<bool>::posedge() const
{
	return (*this)->posedge();
}


bool sca_in<bool>::negedge() const
{
	return (*this)->negedge();
}





//IMPROVE: use own method called after solver creation (tdf-view eq-setup)

void sca_in<bool>::initialize_port()
{
	if(this->buffer!=NULL)
	{
		this->reinitialize_port();
		return;
	}

	this->buffer_size=this->get_delay_internal() + this->get_rate_internal() *
			this->sca_core::sca_implementation::sca_port_base::get_parent_module()->get_calls_per_period();

	this->buffer=new bool[this->buffer_size];
	this->write_cnt=this->get_delay_internal();

	//initialize buffer with initial value of signal
	//thus if AC started without time domain before, the initial value can be accessed
	const bool& init_val=(*this)->read();
	for(std::uint64_t  i=0;i<this->buffer_size;++i)
	{
		this->buffer[i]=init_val;
	}

	if(pmod)
	{
		  allow_processing_access_flag=pmod->get_allow_processing_access_flag_ref();
		  if(allow_processing_access_flag==NULL) allow_processing_access_flag=&allow_processing_access_default;
	}


	if(this->get_timeoffset_internal() >= this->get_propagated_timestep())
	{
		std::ostringstream str;
		str << "timeoffset of port: " << this->name() << " : ";
	    str << this->get_timeoffset_internal() << " must be smaller than timestep: ";
	    str << this->get_propagated_timestep();
	    SC_REPORT_ERROR("SystemC-AMS",str.str().c_str());
	}
}

void sca_in<bool>::reinitialize_port()
{
	std::uint64_t new_buffer_size=this->get_delay_internal() + this->get_rate_internal() *
			this->sca_core::sca_implementation::sca_port_base::get_parent_module()->get_calls_per_period();


	bool* new_buffer=NULL;
	if(this->buffer_size!=new_buffer_size)
	{
		new_buffer=new bool[new_buffer_size];
	}
	else
	{
		new_buffer=this->buffer;
	}


	for(long idx=this->get_delay_internal()<=this->write_cnt?this->get_delay_internal():this->write_cnt+1;idx>0;--idx)
	{
		new_buffer[this->get_delay_internal()-idx]=this->buffer[(this->write_cnt-1-idx)%this->buffer_size];
	}

	if(this->buffer_size!=new_buffer_size)
	{
		delete[] this->buffer;
		this->buffer=new_buffer;

		this->buffer_size=new_buffer_size;
	}

	this->write_cnt=this->get_delay_internal();
}

inline const bool& sca_in<bool>::read_buffer(unsigned long n)
{
	std::uint64_t scnt=this->pmod->get_call_counter() * this->get_rate_internal() + n;

	return this->buffer[scnt%this->buffer_size];
}


inline void sca_in<bool>::read_from_sc()
{
	this->buffer[this->write_cnt%buffer_size]=(*this)->read();
	this->write_cnt++;


	if(this->callback_registered)
	{
		for(unsigned long i=0;i<this->callbacks.size();++i)
		{
			(*(this->callbacks[i]))();
		}
	}

	if(traces_available)
	{
		//due the tracing is not under control of the TDF scheduler, we must
		//check whether all trace files are initialized
		initialize_all_traces();
		for(unsigned long i=0;i<trace_data.size();i++) trace_data[i]->trace();
	}
}



void sca_in<bool>::construct()
{
	base_type::sca_port_type = base_type::SCA_SC_IN_PORT;

	pmod=dynamic_cast<sca_core::sca_module*>(this->get_parent_object());

	if(pmod==NULL)
	{
		std::ostringstream str;
		str << "The sca_tdf::sca_de::sca_out port " << this->name()
			<< " must be instantiated in the context of an sca_tdf::sca_module";
		SC_REPORT_ERROR("SystemC-AMS",str.str().c_str());
	}

	allow_processing_access_default=false;
	allow_processing_access_flag=NULL;

	traces_available=false;

	callback_registered=false;
	value_forced=false;

	buffer=NULL;
}


sca_in<bool>::sca_in() :
	sca_in<bool>::base_type(sc_core::sc_gen_unique_name("sca_tdf_sc_in")),
	sca_port_attributes(this)
{
	construct();
}


sca_in<bool>::sca_in(const char* name_) : sca_in<bool>::base_type(name_),
		sca_port_attributes(this)
{
	construct();
}


void sca_in<bool>::end_of_port_elaboration()
{
	base_type::m_sca_if=NULL;
}



void sca_in<bool>::set_delay(unsigned long nsamples)
{
	this->sca_tdf::sca_implementation::sca_port_attributes::set_delay(nsamples);
}


void sca_in<bool>::set_rate(unsigned long rate_)
{
	this->sca_tdf::sca_implementation::sca_port_attributes::set_rate(rate_);
}


void sca_in<bool>::set_timestep(const sca_core::sca_time& tstep)
{
	this->sca_tdf::sca_implementation::sca_port_attributes::set_timestep(tstep);

	if(pmod->is_change_attributes_executing() )
	{
	  pmod->set_timestep(tstep*this->get_rate_internal());
	}
}


void sca_in<bool>::set_timestep(double tstep, ::sc_core::sc_time_unit unit)
{
	this->sca_tdf::sca_implementation::sca_port_attributes::set_timestep(sca_core::sca_time(tstep, unit));
}

////


void sca_in<bool>::set_max_timestep(const sca_core::sca_time& tstep)
{
	this->sca_tdf::sca_implementation::sca_port_attributes::set_max_timestep(tstep);
}

////


void sca_in<bool>::set_max_timestep(double tstep, sc_core::sc_time_unit unit)
{
	this->set_max_timestep(sca_core::sca_time(tstep,unit));
}


//validate port attributes
void sca_in<bool>::validate_port_attributes()
{
	this->port_attribute_validation();
}




void sca_in<bool>::set_timeoffset(const sca_core::sca_time& toffset)
{
	this->sca_tdf::sca_implementation::sca_port_attributes::set_timeoffset(toffset);
}


void sca_in<bool>::set_timeoffset(double toffset,
		::sc_core::sc_time_unit unit)
{
	this->sca_tdf::sca_implementation::sca_port_attributes::set_timeoffset(sca_core::sca_time(toffset, unit));
}


unsigned long sca_in<bool>::get_delay() const
{
	return this->sca_tdf::sca_implementation::sca_port_attributes::get_delay();
}


unsigned long sca_in<bool>::get_rate() const
{
	return this->sca_tdf::sca_implementation::sca_port_attributes::get_rate();
}


sca_core::sca_time sca_in<bool>::get_time(unsigned long sample_id) const
{
	return this->sca_tdf::sca_implementation::sca_port_attributes::get_time(sample_id);
}


sca_core::sca_time sca_in<bool>::get_timestep(unsigned long sample_id) const
{
	return this->sca_tdf::sca_implementation::sca_port_attributes::get_timestep(sample_id);
}


/////////


sca_core::sca_time sca_in<bool>::get_max_timestep() const
{
	return this->sca_tdf::sca_implementation::sca_port_attributes::get_max_timestep();
}

/////////


sca_core::sca_time sca_in<bool>::get_last_timestep(unsigned long sample_id) const
{
	return this->sca_tdf::sca_implementation::sca_port_attributes::get_last_timestep(sample_id);
}





sca_core::sca_time sca_in<bool>::get_timeoffset() const
{
	return this->sca_tdf::sca_implementation::sca_port_attributes::get_timeoffset();
}


const char* sca_in<bool>::kind() const
{
	return "sca_tdf::sca_de::sca_in";
}


void sca_in<bool>::initialize(const bool& value, unsigned long sample_id)
{
	sca_core::sca_module* pmod;
	pmod=dynamic_cast<sca_core::sca_module*>(this->get_parent_object());

	if(!pmod || !(pmod->is_initialize_executing()))
	{
		std::ostringstream str;
	    str << "Port: " << this->name() << " can't be initialized due it is not accessed "
		    << " from the context of the processing method of the parent sca_tdf::sca_module"
		    << " (see LRM clause 4.1.1.6.15)";
	    SC_REPORT_ERROR("SystemC-AMS",str.str().c_str());
	}

	if (sample_id >= this->get_delay_internal())
	{
		std::ostringstream str;
		str << "Sample id (" << sample_id << ") is greater than delay ("
				<< this->get_delay_internal() << " while initializing port: " << this->name();
		sc_core::sc_report_handler::report(::sc_core::SC_ERROR,"SystemC-AMS",
				str.str().c_str(), __FILE__, __LINE__);
	}


	buffer[sample_id] = value;
}



/////


const bool& sca_in<bool>::read_delayed_value(unsigned long sample_id) const
{
	if(!pmod || !(pmod->is_reinitialize_executing()))
	{
		std::ostringstream str;
	    str << "Port: " << this->name() << " can't execute method read_delayed_value due it is not accessed "
		    << " from the context of the reinitialize method of the parent sca_tdf::sca_module: "
		    << pmod->name()
		    << " (see LRM)";
	    SC_REPORT_ERROR("SystemC-AMS",str.str().c_str());
	}

	if (sample_id >= this->get_delay_internal())
	{
		std::ostringstream str;
		str << "Sample id (" << sample_id << ") is greater than delay ("
				<< sca_in<bool>::get_delay() << " while read_delayed_value port: " << sca_in<bool>::name();
		::sc_core::sc_report_handler::report(::sc_core::SC_ERROR,"SystemC-AMS",
				str.str().c_str(), __FILE__, __LINE__);
	}

	std::uint64_t scnt=this->pmod->get_call_counter() * this->get_rate_internal() + sample_id;


	return buffer[scnt%this->buffer_size];
}


/////


bool sca_in<bool>::is_timestep_changed(unsigned long sample_id) const
{
	return this->sca_tdf::sca_implementation::sca_port_attributes::is_timestep_changed(sample_id);
}


/////


bool sca_in<bool>::is_rate_changed() const
{
	return this->sca_tdf::sca_implementation::sca_port_attributes::is_rate_changed();
}

/////


bool sca_in<bool>::is_delay_changed() const
{
	return this->sca_tdf::sca_implementation::sca_port_attributes::is_delay_changed();
}





const bool& sca_in<bool>::read(unsigned long sample_id)
{
	if((allow_processing_access_flag==NULL) || !(*allow_processing_access_flag))
	{
		if(!sca_ac_analysis::sca_ac_is_running())
		{
			std::ostringstream str;
			str << "Port: " << this->name() << " can't be read due it is not accessed "
		        << " from the context of the processing method of the parent sca_tdf::sca_module"
		        << " (see LRM clause 4.1.1.6.16)";
			SC_REPORT_ERROR("SystemC-AMS",str.str().c_str());
		}
	}

	if (sample_id >= rate)
	{
		std::ostringstream str;
		str << "Sample id (" << sample_id << ") is greater or equal than rate ("
				<< rate << " while reading from port: " << this->name();

		::sc_core::sc_report_handler::report(::sc_core::SC_ERROR,"SystemC-AMS",
				str.str().c_str(), __FILE__, __LINE__);
	}

	if(this->value_forced)
	{
		return this->forced_value;
	}

	return this->read_buffer(sample_id);
}


sca_in<bool>::operator const bool&()
{
	return this->read();
}


const bool& sca_in<bool>::operator[](unsigned long sample_id)
{
	return this->read(sample_id);
}





void sca_in<bool>::bind(sc_core::sc_port<sc_core::sc_signal_inout_if<bool>,1,sc_core::SC_ONE_OR_MORE_BOUND >& oport)
{
	//IMPROVE: hack - find a better solution??
	sc_core::sc_port<sc_core::sc_signal_inout_if<bool> >* base_op=&oport;
	base_type::bind(*(sc_core::sc_port<sc_core::sc_signal_in_if<bool> >*)(base_op));
}


void sca_in<bool>::operator()(sc_core::sc_port<sc_core::sc_signal_inout_if<bool>,1,sc_core::SC_ONE_OR_MORE_BOUND >& oport)
{
	bind(oport);
}


void sca_in<bool>::bind(sc_core::sc_port<sc_core::sc_signal_in_if<bool>,1,sc_core::SC_ONE_OR_MORE_BOUND >& iport)
{
	base_type::bind(iport);
}


void sca_in<bool>::operator()(sc_core::sc_port<sc_core::sc_signal_in_if<bool>,1,sc_core::SC_ONE_OR_MORE_BOUND >& iport)
{
	base_type::bind(iport);
}


void sca_in<bool>::bind(sc_core::sc_signal_in_if<bool>& intf)
{
	base_type::bind(intf);
}



void sca_in<bool>::operator()(sc_core::sc_signal_in_if<bool>& intf)
{
	base_type::bind(intf);
}

//methods private and thus disabled - should not possible to call

void sca_in<bool>::bind(sca_tdf::sca_de::sca_out<bool>&)
{
	SC_REPORT_ERROR("SystemC-AMS","Binding of converter port to converter port not permitted");
}


void sca_in<bool>::operator()(sca_tdf::sca_de::sca_out<bool>& port)
{
	bind(port);
}


void sca_in<bool>::bind(sca_tdf::sca_de::sca_in<bool>&)
{
	SC_REPORT_ERROR("SystemC-AMS","Binding of converter port to converter port not permitted");
}


void sca_in<bool>::operator()(sca_tdf::sca_de::sca_in<bool>& port)
{
	bind(port);
}


//overload sc_port_b bind method
inline void sca_in<bool>::bind( sc_core::sc_port_b<sc_core::sc_signal_in_if<bool> >& parent_ )
{
	sc_core::sc_port<sc_core::sc_signal_in_if<bool> >* port;
	port=dynamic_cast<sc_core::sc_port<sc_core::sc_signal_in_if<bool> >*>(&parent_);

	if(port==NULL)
	{
		std::ostringstream str;
		str << "Port of kind: " << parent_.kind() << " can't bound to";
		str << " port of kind: " << this->kind() << " for port: ";
		str << this->name();
		SC_REPORT_ERROR("SystemC-AMS",str.str().c_str());
		return;
	}

	this->bind(*port);
}



bool sca_in<bool>::trace_init(sca_util::sca_implementation::sca_trace_object_data& data)
{
	traces_available=true;

	trace_data.push_back(&data);
	data.set_type_info<bool>();

    data.event_driven=true;
    data.dont_interpolate=true;

    return true;

}


void sca_in<bool>::trace(long id, sca_util::sca_implementation::sca_trace_file_base& tf)
{
	tf.store_time_stamp(id, sc_core::sc_time_stamp(),(*this)->read());
}


//sc trace of sc_core::sc_object to prevent clang warning due overloaded
//sca_traceable_object function
inline void sca_in<bool>::trace( sc_core::sc_trace_file* tf ) const
{
	sc_core::sc_port_b<sc_core::sc_signal_in_if<bool> >::trace(tf);
}



} //end namespace sca_de
} //end namespace sca_tdf



