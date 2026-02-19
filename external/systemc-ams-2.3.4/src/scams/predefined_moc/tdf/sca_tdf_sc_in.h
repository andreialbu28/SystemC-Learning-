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

 sca_tdf_sc_in_bool.h - converter port SystemC -> tdf

 Original Author: Karsten Einwich Fraunhofer / COSEDA Technologies

 Created on: 04.03.2009

 SVN Version       :  $Revision: 2320 $
 SVN last checkin  :  $Date: 2023-06-08 12:47:38 +0000 (Thu, 08 Jun 2023) $ (UTC)
 SVN checkin by    :  $Author: pehrlich $
 SVN Id            :  $Id: sca_tdf_sc_in.h 2320 2023-06-08 12:47:38Z pehrlich $


 *****************************************************************************/
/*
 LRM clause 4.1.1.7.
 The class sca_tdf::sca_de::sca_in shall define a specialized port class
 for the TDF MoC. It provides functions for defining or getting attribute
 values (e.g. sampling rate or timestep), for initialization and for reading
 input values. The port shall be bound to a channel derived from an interface
 proper of class sc_core::sc_signal_in_if or to a port of class sc_core::sc_in
 of the corresponding type. The port shall perform the synchronization between
 the TDF MoC and the SystemC kernel.
 A port of class sca_tdf::sca_de::sca_in can be only a member of the class
 sca_tdf::sca_module, otherwise it shall be an error.
 */

/*****************************************************************************/

#ifndef SCA_TDF_SC_IN_H_
#define SCA_TDF_SC_IN_H_


namespace sca_tdf
{

namespace sca_de
{

//begin implementation specific

template<class T>
class sca_out;

//end implementation specific


template<class T>
//class sca_in : public implementation-derived-from sca_core::sca_port&lt; sc_core::sc_signal_in_if >
class sca_in: public sca_core::sca_port< sc_core::sc_signal_in_if<T> >,
		public sca_tdf::sca_implementation::sca_port_attributes
{
public:
	sca_in();
	explicit sca_in(const char* );

	void set_delay(unsigned long );
	void set_rate(unsigned long );
	void set_timestep(const sca_core::sca_time& );
	void set_timestep(double , sc_core::sc_time_unit );
	void set_max_timestep(const sca_core::sca_time& );
	void set_max_timestep(double , sc_core::sc_time_unit );

	unsigned long get_delay() const;
	unsigned long get_rate() const;
	sca_core::sca_time get_time(unsigned long sample_id = 0) const;
	sca_core::sca_time get_timestep(unsigned long sample_id = 0) const;
	sca_core::sca_time get_max_timestep() const;
	sca_core::sca_time get_last_timestep(unsigned long sample_id=0) const;

	virtual const char* kind() const override;

	void initialize(const T& value, unsigned long sample_id = 0);
	const T& read_delayed_value(unsigned long sample_id = 0) const;

	bool is_timestep_changed(unsigned long sample_id = 0) const;
	bool is_rate_changed() const;
	bool is_delay_changed() const;

	const T& read(unsigned long sample_id = 0);
	operator const T&();
	const T& operator[](unsigned long sample_id);

	const sc_core::sc_event& default_event() const;
	const sc_core::sc_event& value_changed_event() const;
	bool event() const;

	virtual void bind(sc_core::sc_signal_in_if<T>&) override;
	void operator()(sc_core::sc_signal_in_if<T>&);

	virtual void bind(sc_core::sc_port<sc_core::sc_signal_in_if<T> >&);
	void operator()(sc_core::sc_port<sc_core::sc_signal_in_if<T> >&);

	virtual void bind(sc_core::sc_port<sc_core::sc_signal_inout_if<T> >&);
	void operator()(sc_core::sc_port<sc_core::sc_signal_inout_if<T> >&);


	/** return the sc_interface for converter ports*/
	virtual sc_core::sc_interface* get_interface() override;
	virtual const sc_core::sc_interface* get_interface() const override;

	virtual ~sca_in();


	/** method of interactive tracing interface, which returns the value
	 * at the current SystemC time (the time returned by sc_core::sc_time_stamp())
	 */
	const T& get_typed_trace_value() const;

	virtual const std::string& get_trace_value() const override;

    /**
     * registers trace callback
     */
	virtual bool register_trace_callback(sca_util::sca_traceable_object::callback_functor_base&) override;
	virtual bool remove_trace_callback(sca_util::sca_traceable_object::callback_functor_base&) override;

	/** method of interactive tracing interface, which forces a value
	 */
    void force_typed_value(const T&);

    virtual void set_force_value(const std::string& stri);

    /** method of interactive tracing interface, which releases a forced value
    */
    virtual void release_value() override;


	void set_timeoffset(const sca_core::sca_time& toffset); //obsolete in 2.0
	void set_timeoffset(double toffset, ::sc_core::sc_time_unit unit); //obsolete in 2.0

	sca_core::sca_time get_timeoffset() const; //obsolete in 2.0

private:
	// Disabled
	sca_in(const sca_tdf::sca_de::sca_in<T>&);
	sca_tdf::sca_de::sca_in<T>& operator= (const sca_tdf::sca_de::sca_in<T>&);

	void end_of_port_elaboration() override;

	//begin implementation specific

	//on this way we forbid port-to-port binding to a converter port
	void bind(sca_tdf::sca_de::sca_out<T>&);
	void operator()(sca_tdf::sca_de::sca_out<T>&);
	void bind(sca_tdf::sca_de::sca_in<T>&);
	void operator()(sca_tdf::sca_de::sca_in<T>&);

	 //overload sc_port_b bind method
	 void bind( sc_core::sc_port_b<sc_core::sc_signal_in_if<T> >& parent_ ) override;

	 void initialize_port() override;
	 void reinitialize_port() override;

	void construct();

	typedef sca_core::sca_port< ::sc_core::sc_signal_in_if<T> > base_type;


	T ret_tmp;

	const bool* allow_processing_access_flag;
	bool allow_processing_access_default;

	sca_core::sca_module* pmod;

	virtual bool trace_init(sca_util::sca_implementation::sca_trace_object_data& data) override;
	virtual void trace(long id, sca_util::sca_implementation::sca_trace_file_base& tf) override;

	 //sc trace of sc_core::sc_object to prevent clang warning due overloaded
	 //sca_traceable_object function
	 void trace( sc_core::sc_trace_file* tf ) const override;


	bool traces_available;
	std::vector<sca_util::sca_implementation::sca_trace_object_data*> trace_data;

	//method to pass attributes to module after change attributes
	void validate_port_attributes() override;

	//variables for interactive trace callback
	std::vector<sca_util::sca_traceable_object::callback_functor_base*> callbacks;
	bool callback_registered;

	T forced_value;
	bool value_forced;

	mutable std::string current_trace_value_string;


	T* buffer;
	std::uint64_t buffer_size;

	const T& read_buffer(unsigned long n);
	void read_from_sc() override; //will be called by the synchronization layer
	unsigned long write_cnt;

	//end implementation specific
};

//begin implementation specific


//IMPROVE: use own method called after solver creation (tdf-view eq-setup)
template<class T>
inline void sca_in<T>::initialize_port()
{
	if(this->buffer!=NULL)
	{
		this->reinitialize_port();
		return;
	}

	this->buffer_size=this->get_delay_internal() + this->get_rate_internal() *
			this->sca_core::sca_implementation::sca_port_base::get_parent_module()->get_calls_per_period();

	this->buffer=new T[this->buffer_size];
	this->write_cnt=this->get_delay_internal();

	//initialize buffer with initial value of signal
	//thus if AC started without time domain before, the initial value can be accessed
	const T& init_val=(*this)->read();
	for(std::uint64_t  i=0;i<this->buffer_size;++i)
	{
		this->buffer[i]=init_val;
	}


	if(pmod)
	{
		  allow_processing_access_flag=pmod->get_allow_processing_access_flag_ref();
		  if(allow_processing_access_flag==NULL) allow_processing_access_flag=&allow_processing_access_default;
	}

}

template<class T>
void sca_in<T>::reinitialize_port()
{
	std::uint64_t new_buffer_size=this->get_delay_internal() + this->get_rate_internal() *
			this->sca_core::sca_implementation::sca_port_base::get_parent_module()->get_calls_per_period();


	T* new_buffer=NULL;
	if(this->buffer_size!=new_buffer_size)
	{
		new_buffer=new T[new_buffer_size];
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

template<class T>
inline const T& sca_in<T>::read_buffer(unsigned long n)
{
	std::uint64_t scnt=this->pmod->get_call_counter() * this->get_rate_internal() + n;

	return this->buffer[scnt%this->buffer_size];
}


template<class T>
inline void sca_in<T>::read_from_sc()
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


////////////////////////////////////////////////////////////////////////////////////////



/** method of interactive tracing interface, which returns the value
 * at the current SystemC time (the time returned by sc_core::sc_time_stamp())
 */
template<class T>
const T& sca_in<T>::get_typed_trace_value() const
{
	if(this->value_forced)
	{
		return this->forced_value;
	}

	//return last written value
	std::uint64_t scnt=this->pmod->get_call_counter() * this->get_rate_internal() + this->get_delay_internal() + this->write_cnt - 1;

	return this->buffer[scnt%buffer_size];
}

template<class T>
const std::string& sca_in<T>::get_trace_value() const
{
	std::ostringstream str;
	str << get_typed_trace_value();
	current_trace_value_string=str.str();

	return current_trace_value_string;
}


/**
 * method for register a trace callback
 */
template<class T>
inline bool sca_in<T>::register_trace_callback(sca_util::sca_traceable_object::callback_functor_base& func)
{
	this->callbacks.push_back(&func);
	this->callback_registered=true;
    return true;
}

/**
 * method for removing a trace callback
 */
template<class T>
inline bool sca_in<T>::remove_trace_callback(sca_util::sca_traceable_object::callback_functor_base& func)
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
template<class T>
inline void sca_in<T>::force_typed_value(const T& val)
{
	this->forced_value=val;
	this->value_forced=true;
}


template<class T>
inline void sca_in<T>::set_force_value(const std::string& stri)
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
template<class T>
void sca_in<T>::release_value()
{
	this->value_forced=false;
}


template<class T>
inline sc_core::sc_interface* sca_in<T>::get_interface()
{
	return this->sc_get_interface();
}


template<class T>
inline const sc_core::sc_interface* sca_in<T>::get_interface() const
{
	return this->sc_get_interface();
}


template<class T>
inline void sca_in<T>::validate_port_attributes()
{
	this->port_attribute_validation();
}

template<class T>
inline const sc_core::sc_event& sca_in<T>::default_event() const
{
	return (*this)->default_event();
}


template<class T>
inline const sc_core::sc_event& sca_in<T>::value_changed_event() const
{
	return (*this)->value_changed_event();
}


template<class T>
inline bool sca_in<T>::event() const
{
	return (*this)->event();
}





template<class T>
inline void sca_in<T>::construct()
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

template<class T>
inline sca_in<T>::sca_in() :
	sca_in<T>::base_type(::sc_core::sc_gen_unique_name("sca_tdf_sc_in")),
	sca_port_attributes(this)
{
	construct();
}

template<class T>
inline sca_in<T>::sca_in(const char* name_) : sca_in<T>::base_type(name_),
											  sca_port_attributes(this)
{
	construct();
}


template<class T>
inline sca_in<T>::~sca_in()
{
	if(buffer!=NULL) delete[] buffer;
	buffer=NULL;
	buffer_size=0;
	write_cnt=0;
}

template<class T>
inline void sca_in<T>::end_of_port_elaboration()
{
	base_type::m_sca_if=NULL;
}



template<class T>
inline void sca_in<T>::set_delay(unsigned long nsamples)
{
	this->sca_tdf::sca_implementation::sca_port_attributes::set_delay(nsamples);
}

template<class T>
inline void sca_in<T>::set_rate(unsigned long rate_)
{
	this->sca_tdf::sca_implementation::sca_port_attributes::set_rate(rate_);
}

template<class T>
inline void sca_in<T>::set_timestep(const sca_core::sca_time& tstep)
{
	this->sca_tdf::sca_implementation::sca_port_attributes::set_timestep(tstep);
}

template<class T>
inline void sca_in<T>::set_timestep(double tstep, ::sc_core::sc_time_unit unit)
{
	this->sca_tdf::sca_implementation::sca_port_attributes::set_timestep(sca_core::sca_time(tstep, unit));
}

////

template<class T>
inline void sca_in<T>::set_max_timestep(const sca_core::sca_time& tstep)
{
	this->sca_tdf::sca_implementation::sca_port_attributes::set_max_timestep(tstep);
}

////

template<class T>
inline void sca_in<T>::set_max_timestep(double tstep, sc_core::sc_time_unit unit)
{
	this->sca_tdf::sca_implementation::sca_port_attributes::set_max_timestep(sca_core::sca_time(tstep,unit));
}





template<class T>
inline void sca_in<T>::set_timeoffset(const sca_core::sca_time& toffset)
{
	this->sca_tdf::sca_implementation::sca_port_attributes::set_timeoffset(toffset);
}

template<class T>
inline void sca_in<T>::set_timeoffset(double toffset,
		::sc_core::sc_time_unit unit)
{
	this->sca_tdf::sca_implementation::sca_port_attributes::set_timeoffset(sca_core::sca_time(toffset, unit));
}

template<class T>
inline unsigned long sca_in<T>::get_delay() const
{
	return this->sca_tdf::sca_implementation::sca_port_attributes::get_delay();
}

template<class T>
inline unsigned long sca_in<T>::get_rate() const
{
	return this->sca_tdf::sca_implementation::sca_port_attributes::get_rate();
}

template<class T>
inline sca_core::sca_time sca_in<T>::get_time(unsigned long sample_id) const
{
	return this->sca_tdf::sca_implementation::sca_port_attributes::get_time(sample_id);
}

template<class T>
inline sca_core::sca_time sca_in<T>::get_timestep(unsigned long sample_id) const
{
	return this->sca_tdf::sca_implementation::sca_port_attributes::get_timestep(sample_id);
}


/////////

template<class T>
inline sca_core::sca_time sca_in<T>::get_max_timestep() const
{
	return this->sca_tdf::sca_implementation::sca_port_attributes::get_max_timestep();
}

/////////

template<class T>
inline sca_core::sca_time sca_in<T>::get_last_timestep(unsigned long sample_id) const
{
	return this->sca_tdf::sca_implementation::sca_port_attributes::get_last_timestep(sample_id);
}




template<class T>
inline sca_core::sca_time sca_in<T>::get_timeoffset() const
{
	return this->sca_tdf::sca_implementation::sca_port_attributes::get_timeoffset();
}

template<class T>
inline const char* sca_in<T>::kind() const
{
	return "sca_tdf::sca_de::sca_in";
}

template<class T>
inline void sca_in<T>::initialize(const T& value, unsigned long sample_id)
{
	sca_core::sca_module* pmod;
	pmod=dynamic_cast<sca_core::sca_module*>(this->get_parent_object());

	if(!pmod || !(pmod->is_initialize_executing()))
	{
		std::ostringstream str;
	    str << "Port: " << this->name() << " can't be initialized due it is not accessed "
		    << " from the context of the processing method of the parent sca_tdf::sca_module"
		    << " (see LRM)";
	    SC_REPORT_ERROR("SystemC-AMS",str.str().c_str());
	}

	if (sample_id >= this->get_delay_internal())
	{
		std::ostringstream str;
		str << "Sample id (" << sample_id << ") is greater than delay ("
				<< sca_in<T>::get_delay() << " while initializing port: " << sca_in<T>::name();
		::sc_core::sc_report_handler::report(::sc_core::SC_ERROR,"SystemC-AMS",
				str.str().c_str(), __FILE__, __LINE__);
	}


	buffer[sample_id] = value;
}



/////

template<class T>
inline const T& sca_in<T>::read_delayed_value(unsigned long sample_id) const
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
				<< sca_in<T>::get_delay() << " while read_delayed_value port: " << sca_in<T>::name();
		::sc_core::sc_report_handler::report(::sc_core::SC_ERROR,"SystemC-AMS",
				str.str().c_str(), __FILE__, __LINE__);
	}

	std::uint64_t scnt=this->pmod->get_call_counter() * this->get_rate_internal() + sample_id;


	return buffer[scnt%this->buffer_size];
}


/////

template<class T>
inline bool sca_in<T>::is_timestep_changed(unsigned long sample_id ) const
{
	return this->sca_tdf::sca_implementation::sca_port_attributes::is_timestep_changed(sample_id);
}


/////

template<class T>
inline bool sca_in<T>::is_rate_changed() const
{
	return this->sca_tdf::sca_implementation::sca_port_attributes::is_rate_changed();
}

/////

template<class T>
inline bool sca_in<T>::is_delay_changed() const
{
	return this->sca_tdf::sca_implementation::sca_port_attributes::is_delay_changed();
}



template<class T>
inline const T& sca_in<T>::read(unsigned long sample_id)
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

template<class T>
inline sca_in<T>::operator const T&()
{
	return this->read();
}

template<class T>
inline const T& sca_in<T>::operator[](unsigned long sample_id)
{
	return this->read(sample_id);
}




template<class T>
inline void sca_in<T>::bind(sc_core::sc_port<sc_core::sc_signal_inout_if<T>,1,sc_core::SC_ONE_OR_MORE_BOUND >& oport)
{
	//IMPROVE: hack - find a better solution??
	sc_core::sc_port<sc_core::sc_signal_inout_if<T> >* base_op=&oport;
	base_type::bind(*(sc_core::sc_port<sc_core::sc_signal_in_if<T> >*)(base_op));
}

template<class T>
inline void sca_in<T>::operator()(sc_core::sc_port<sc_core::sc_signal_inout_if<T>,1,sc_core::SC_ONE_OR_MORE_BOUND >& oport)
{
	bind(oport);
}

template<class T>
inline void sca_in<T>::bind(sc_core::sc_port<sc_core::sc_signal_in_if<T>,1,sc_core::SC_ONE_OR_MORE_BOUND >& iport)
{
	base_type::bind(iport);
}

template<class T>
inline void sca_in<T>::operator()(sc_core::sc_port<sc_core::sc_signal_in_if<T>,1,sc_core::SC_ONE_OR_MORE_BOUND >& iport)
{
	base_type::bind(iport);
}

template<class T>
inline void sca_in<T>::bind(sc_core::sc_signal_in_if<T>& intf)
{
	base_type::bind(intf);
}


template<class T>
inline void sca_in<T>::operator()(sc_core::sc_signal_in_if<T>& intf)
{
	base_type::bind(intf);
}

//methods private and thus disabled - should not possible to call
template<class T>
inline void sca_in<T>::bind(sca_tdf::sca_de::sca_out<T>&)
{
	SC_REPORT_ERROR("SystemC-AMS","Binding of converter port to converter port not permitted");
}

template<class T>
inline void sca_in<T>::operator()(sca_tdf::sca_de::sca_out<T>& port)
{
	bind(port);
}

template<class T>
inline void sca_in<T>::bind(sca_tdf::sca_de::sca_in<T>&)
{
	SC_REPORT_ERROR("SystemC-AMS","Binding of converter port to converter port not permitted");
}

template<class T>
inline void sca_in<T>::operator()(sca_tdf::sca_de::sca_in<T>& port)
{
	bind(port);
}



//overload sc_port_b bind method
template<class T>
inline void sca_in<T>::bind( sc_core::sc_port_b<sc_core::sc_signal_in_if<T> >& parent_ )
{
	sc_core::sc_port<sc_core::sc_signal_in_if<T> >* port;
	port=dynamic_cast<sc_core::sc_port<sc_core::sc_signal_in_if<T> >*>(&parent_);

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



template<class T>
inline bool sca_in<T>::trace_init(sca_util::sca_implementation::sca_trace_object_data& data)
{
	traces_available=true;

	trace_data.push_back(&data);
	data.set_type_info<T>();

    data.event_driven=true;
    data.dont_interpolate=true;

    return true;

}

template<class T>
inline void sca_in<T>::trace(long id, sca_util::sca_implementation::sca_trace_file_base& tf)
{
	tf.store_time_stamp(id, sc_core::sc_time_stamp(),(*this)->read());
}


//sc trace of sc_core::sc_object to prevent clang warning due overloaded
//sca_traceable_object function
template<class T>
inline void sca_in<T>::trace( sc_core::sc_trace_file* tf ) const
{
	sc_core::sc_port_b<sc_core::sc_signal_in_if<T> >::trace(tf);
}


//end implementation specific


} // namespace sca_de

//template<class T>
//typedef sca_tdf::sca_de::sca_in<T> sc_in<T> ;

template<class T>
class sc_in: public sca_tdf::sca_de::sca_in<T>
{
public:
	sc_in() :
		sca_tdf::sca_de::sca_in<T>()
	{
	}
	explicit sc_in(const char* name_) :
		sca_tdf::sca_de::sca_in<T>(name_)
	{
	}
};


} // namespace sca_tdf


#endif /* SCA_TDF_SC_IN_H_ */
