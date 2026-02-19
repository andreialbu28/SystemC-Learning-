/*****************************************************************************

    Copyright 2010-2014
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

 sca_tdf_sc_out.h - Converter port tdf -> SystemC

 Original Author: Karsten Einwich Fraunhofer / COSEDA Technologies

 Created on: 04.03.2009

 SVN Version       :  $Revision: 2320 $
 SVN last checkin  :  $Date: 2023-06-08 12:47:38 +0000 (Thu, 08 Jun 2023) $ (UTC)
 SVN checkin by    :  $Author: pehrlich $
 SVN Id            :  $Id: sca_tdf_sc_out.h 2320 2023-06-08 12:47:38Z pehrlich $

 *****************************************************************************/
/*
 LRM clause 4.1.1.8.
 The class sca_tdf::sca_de::sca_out shall define a specialized port class
 for the TDF MoC. It provides functions for defining or getting attribute
 values (e.g. sampling rate or timestep), for initialization and for writing
 output values. The port shall be bound to a channel derived from an interface
 proper of class sc_core::sc_signal_inout_if or to a port of
 class sc_core::sc_out of the corresponding type. The port shall perform
 the synchronization between the TDF MoC and the SystemC kernel.
 A port of class sca_tdf::sca_de::sca_out can be only a member of the class
 sca_tdf::sca_module, otherwise it shall be an error.
 */

/*****************************************************************************/

#ifndef SCA_TDF_SC_OUT_H_
#define SCA_TDF_SC_OUT_H_

namespace sca_tdf
{

namespace sca_de
{

template<class T>
//class sca_out : public implementation-derived-from sca_core::sca_port&lt; sc_core::sc_signal_inout_if >
class sca_out: public sca_core::sca_port< ::sc_core::sc_signal_inout_if<T> >,
		       public sca_tdf::sca_implementation::sca_port_attributes
{
public:
	sca_out();
	explicit sca_out(const char* );

	void set_delay(unsigned long );
	void set_rate(unsigned long );
	void set_timestep(const sca_core::sca_time& );
	void set_timestep(double ,sc_core::sc_time_unit );
	void set_max_timestep(const sca_core::sca_time& );
	void set_max_timestep(double ,sc_core::sc_time_unit );

	unsigned long get_delay() const;
	unsigned long get_rate() const;
	sca_core::sca_time get_time(unsigned long sample_id = 0) const;
	sca_core::sca_time get_timestep(unsigned long sample_id = 0) const;
	sca_core::sca_time get_max_timestep() const;
	sca_core::sca_time get_last_timestep(unsigned long sample_id=0) const;

	virtual const char* kind() const override;

	void initialize(const T& value, unsigned long sample_id = 0);
	void initialize_de_signal( const T&  );
	const T& read_delayed_value(unsigned long sample_id = 0) const;

	bool is_timestep_changed(unsigned long sample_id=0) const;
	bool is_rate_changed() const;
	bool is_delay_changed() const;

	void write(const T& value, unsigned long sample_id = 0);
	void write(sca_core::sca_assign_from_proxy<sca_tdf::sca_de::sca_out<T> >&);
	sca_tdf::sca_de::sca_out<T>& operator=(const T&);
	sca_tdf::sca_de::sca_out<T>& operator=(const sca_tdf::sca_in<T>&);
	sca_tdf::sca_de::sca_out<T>& operator=(sca_tdf::sca_de::sca_in<T>&);
	sca_tdf::sca_de::sca_out<T>& operator=(sca_core::sca_assign_from_proxy<
			sca_tdf::sca_de::sca_out<T> >&);



	sca_core::sca_assign_to_proxy<sca_tdf::sca_de::sca_out<T>,T>&
	                                       operator[](unsigned long sample_id);


//begin implementation specific

	//on this way we forbid port-to-port binding to a converter port
	virtual void bind(sc_core::sc_port<sc_core::sc_signal_inout_if<T>,1,sc_core::SC_ONE_OR_MORE_BOUND >&);
	void operator()(sc_core::sc_port<sc_core::sc_signal_inout_if<T>,1,sc_core::SC_ONE_OR_MORE_BOUND >&);

	virtual void bind(sc_core::sc_signal_inout_if<T>&) override;
	void operator()(sc_core::sc_signal_inout_if<T>&);

	/** return the sc_interface for converter ports*/
	virtual sc_core::sc_interface* get_interface() override;
	virtual const sc_core::sc_interface* get_interface() const override;

	virtual ~sca_out();

	void set_timeoffset(const sca_core::sca_time& toffset);
	void set_timeoffset(double toffset, ::sc_core::sc_time_unit unit);

	sca_core::sca_time get_timeoffset() const;


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

//end implementation specific

private:
	// Disabled
	sca_out(const sca_tdf::sca_de::sca_out<T>&);
	sca_tdf::sca_de::sca_out<T>&  operator=( const sca_tdf::sca_de::sca_out<T>& );

	sca_core::sca_assign_to_proxy<sca_tdf::sca_de::sca_out<T>,T> bracket_proxy;

	//on this way we forbid port-to-port binding to a converter port
	void bind(sca_tdf::sca_de::sca_out<T>&);
	void operator()(sca_tdf::sca_de::sca_out<T>&);

	//begin implementation specific

	 //overload sc_port_b bind method
	 void bind( sc_core::sc_port_b<sc_core::sc_signal_inout_if<T> >& parent_ ) override;


	void construct();

	void initialize_port() override;
	void reinitialize_port() override;

	typedef sca_core::sca_port< ::sc_core::sc_signal_inout_if<T> > base_type;

	void end_of_port_elaboration() override;

	const bool* allow_processing_access_flag;
	bool allow_processing_access_default;

	sca_core::sca_module* pmod;


	virtual bool trace_init(sca_util::sca_implementation::sca_trace_object_data& data) override;
	//virtual void trace(long id, sca_util::sca_implementation::sca_trace_buffer& buffer);  //not used for converter ports

	void trace_internal(const sca_core::sca_time& tp,const T& val);

	 //sc trace of sc_core::sc_object to prevent clang warning due overloaded
	 //sca_traceable_object function
	 using sca_core::sca_implementation::sca_port_base::trace;  //tell the compiler, that we want hide the method from sca_port_base
	 void trace( sc_core::sc_trace_file* tf ) const override;

	bool traces_available;
	std::vector<sca_util::sca_implementation::sca_trace_object_data*> trace_data;

	//method to pass attributes to module after change attributes
	void validate_port_attributes() override;

	bool initialization_pendig;
	T initialization_value_pending;


	//variables for interactive trace callback
	std::vector<sca_util::sca_traceable_object::callback_functor_base*> callbacks;
	bool callback_registered;

	T forced_value;
	bool value_forced;

	sca_core::sca_time last_traced_time;
	bool trace_cluster_cb_added;

	mutable std::string current_trace_value_string;


	T* buffer;
	std::uint64_t buffer_size;

	void write_buffer(const T& val,unsigned long n);
	void write_to_sc() override; //will be called by the synchronization layer
	unsigned long read_cnt;


	//end implementation specific
};

//begin implementation specific


//IMPROVE: use own method called after solver creation (tdf-view eq-setup)
template<class T>
inline void sca_out<T>::initialize_port()
{
	if(this->buffer!=NULL)
	{
		this->reinitialize_port();
		return;
	}

	this->buffer_size=this->get_delay_internal() + this->get_rate_internal() *
			this->sca_core::sca_implementation::sca_port_base::get_parent_module()->get_calls_per_period();

	this->buffer=new T[this->buffer_size];
	this->read_cnt=0;


	if(pmod)
	{
		  allow_processing_access_flag=pmod->get_allow_processing_access_flag_ref();
		  if(allow_processing_access_flag==NULL) allow_processing_access_flag=&allow_processing_access_default;
	}

	if(this->get_timeoffset_internal() >=
			sca_tdf::sca_implementation::sca_port_attributes::get_timestep_internal(0))
	{
		std::ostringstream str;
		str << "timeoffset of port: " << this->name() << " : ";
	    str << this->get_timeoffset() << " must be smaller than timestep: ";
	    str << sca_tdf::sca_implementation::sca_port_attributes::get_timestep(0);
	    SC_REPORT_ERROR("SystemC-AMS",str.str().c_str());
	}

	if(initialization_pendig)
	{
		initialization_pendig=false;
		this->initialize_de_signal(initialization_value_pending);
		if(initialization_pendig)
		{
			std::ostringstream str;
			str << "Can't initialize de signal of port: " << this->name();
			str << " due may no signal is bound";
			SC_REPORT_WARNING("SystemC-AMS",str.str().c_str());
		}
	}
}



template<class T>
void sca_out<T>::reinitialize_port()
{
	unsigned long new_buffer_size=this->get_delay_internal() + this->get_rate_internal() *
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

	for(long idx=0;idx<(long)this->get_delay_internal();++idx)
	{
		new_buffer[idx]=this->buffer[(this->read_cnt+idx)%new_buffer_size];
	}

	if(this->buffer_size!=new_buffer_size)
	{
		delete[] this->buffer;
		this->buffer=new_buffer;

		this->buffer_size=new_buffer_size;
	}

	this->read_cnt=0;
}



template<class T>
inline void sca_out<T>::write_buffer(const T& val,unsigned long n)
{
	std::uint64_t scnt=this->pmod->get_call_counter() * this->get_rate_internal() + this->get_delay_internal() + n;
	this->buffer[scnt%this->buffer_size]=val;
}


template<class T>
inline void sca_out<T>::write_to_sc()
{
	if(this->value_forced)
	{
		(*this)->write(this->forced_value);

		if(traces_available)
		{
			this->trace_internal(sc_core::sc_time_stamp(),this->forced_value);
		}
	}
	else
	{
		(*this)->write(this->buffer[this->read_cnt%this->buffer_size]);


		if(traces_available)
		{
			this->trace_internal(sc_core::sc_time_stamp(),this->buffer[this->read_cnt%buffer_size]);
		}
	}

	if(this->callback_registered)
	{
		for(unsigned long i=0;i<this->callbacks.size();++i)
		{
			(*(this->callbacks[i]))();
		}
	}

	this->read_cnt++;
}




/** method of interactive tracing interface, which returns the value
 * at the current SystemC time (the time returned by sc_core::sc_time_stamp())
 */
template<class T>
const T& sca_out<T>::get_typed_trace_value() const
{
	//the last we wrote to SystemC
	std::uint64_t scnt=this->pmod->get_call_counter() * this->get_rate_internal() + this->read_cnt -1;

	return this->buffer[scnt%buffer_size];
}

template<class T>
const std::string& sca_out<T>::get_trace_value() const
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
inline bool sca_out<T>::register_trace_callback(sca_util::sca_traceable_object::callback_functor_base& func)
{
	this->callbacks.push_back(&func);
	this->callback_registered=true;
    return true;
}

/**
 * method for removing a trace callback
 */
template<class T>
inline bool sca_out<T>::remove_trace_callback(sca_util::sca_traceable_object::callback_functor_base& func)
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
inline void sca_out<T>::force_typed_value(const T& val)
{
	this->forced_value=val;
	this->value_forced=true;
}

template<class T>
inline void sca_out<T>::set_force_value(const std::string& stri)
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
void sca_out<T>::release_value()
{
	this->value_forced=false;
}


template<class T>
inline void sca_out<T>::validate_port_attributes()
{
	this->port_attribute_validation();
}

template<class T>
inline sc_core::sc_interface* sca_out<T>::get_interface()
{
	return this->sc_get_interface();
}

template<class T>
inline const sc_core::sc_interface* sca_out<T>::get_interface() const
{
	return this->sc_get_interface();
}



template<class T>
inline void sca_out<T>::construct()
{
	this->buffer=NULL;
	this->buffer_size=0;
	this->read_cnt=0;

	base_type::sca_port_type = base_type::SCA_SC_OUT_PORT;

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
	initialization_pendig=false;

	callback_registered=false;
	value_forced=false;

	trace_cluster_cb_added=false;
}



template<class T>
inline sca_out<T>::sca_out() :
	sca_out<T>::base_type(::sc_core::sc_gen_unique_name("sca_tdf_sc_out")),
	sca_port_attributes(this)
{
	construct();
}

template<class T>
inline sca_out<T>::sca_out(const char* name_) :
	sca_out<T>::base_type(name_),
	sca_port_attributes(this)
{
	construct();
}

template<class T>
inline sca_out<T>::~sca_out()
{
	if(buffer!=NULL) delete[] buffer;
	buffer=NULL;
	buffer_size=0;
	read_cnt=0;
}

template<class T>
inline void sca_out<T>::end_of_port_elaboration()
{
	base_type::m_sca_if = NULL;
}



template<class T>
void sca_out<T>::initialize_de_signal( const T& value )
{
	sc_core::sc_signal_inout_if<T>* sc_if;
	sc_if=dynamic_cast<sc_core::sc_signal_inout_if<T>*>(this->sc_get_interface());

	if(sc_if==NULL)
	{
		initialization_pendig=true;
		initialization_value_pending=value;
		return;
	}
	else
	{
		sc_if->write(value);
	}
}

template<class T>
inline void sca_out<T>::set_delay(unsigned long nsamples)
{

	this->sca_tdf::sca_implementation::sca_port_attributes::set_delay(nsamples);
}

template<class T>
inline void sca_out<T>::set_rate(unsigned long rate_)
{
	sca_tdf::sca_implementation::sca_port_attributes::set_rate(rate_);
}

template<class T>
inline void sca_out<T>::set_timestep(const sca_core::sca_time& tstep)
{
	sca_tdf::sca_implementation::sca_port_attributes::set_timestep(tstep);
}

template<class T>
inline void sca_out<T>::set_timestep(double tstep, ::sc_core::sc_time_unit unit)
{
	sca_out<T>::set_timestep(sca_core::sca_time(tstep, unit));
}


////

template<class T>
inline void sca_out<T>::set_max_timestep(const sca_core::sca_time& tstep)
{
	this->sca_tdf::sca_implementation::sca_port_attributes::set_max_timestep(tstep);
}

////

template<class T>
inline void sca_out<T>::set_max_timestep(double tstep, sc_core::sc_time_unit unit)
{
	this->set_max_timestep(sca_core::sca_time(tstep,unit));
}


template<class T>
inline void sca_out<T>::set_timeoffset(const sca_core::sca_time& toffset)
{
	sca_tdf::sca_implementation::sca_port_attributes::set_timeoffset(toffset);
}

template<class T>
inline void sca_out<T>::set_timeoffset(double toffset,
		::sc_core::sc_time_unit unit)
{
	sca_out<T>::set_timeoffset(sca_core::sca_time(toffset, unit));
}

template<class T>
inline unsigned long sca_out<T>::get_delay() const
{
	return sca_tdf::sca_implementation::sca_port_attributes::get_delay();
}

template<class T>
inline unsigned long sca_out<T>::get_rate() const
{
	return sca_tdf::sca_implementation::sca_port_attributes::get_rate();
}

template<class T>
inline sca_core::sca_time sca_out<T>::get_time(unsigned long sample_id) const
{
	return sca_tdf::sca_implementation::sca_port_attributes::get_time(sample_id);
}

template<class T>
inline sca_core::sca_time sca_out<T>::get_timestep(unsigned long sample_id) const
{
	return sca_tdf::sca_implementation::sca_port_attributes::get_timestep(sample_id);
}



/////////

template<class T>
inline sca_core::sca_time sca_out<T>::get_max_timestep() const
{
	return sca_tdf::sca_implementation::sca_port_attributes::get_max_timestep();
}

/////////

template<class T>
inline sca_core::sca_time sca_out<T>::get_last_timestep(unsigned long sample_id) const
{
	return sca_tdf::sca_implementation::sca_port_attributes::get_last_timestep(sample_id);
}



template<class T>
inline sca_core::sca_time sca_out<T>::get_timeoffset() const
{
	return sca_tdf::sca_implementation::sca_port_attributes::get_timeoffset();
}

template<class T>
inline const char* sca_out<T>::kind() const
{
	return "sca_tdf::sca_de::sca_out";
}

template<class T>
inline void sca_out<T>::initialize(const T& value, unsigned long sample_id)
{
	if(!pmod ||
			!(pmod->is_initialize_executing() ||
			  pmod->is_reinitialize_executing() )
		)
	{
		std::ostringstream str;
	    str << "Port: " << this->name() << " can't be initialized due it is not accessed "
		    << " from the context of the processing method of the parent sca_tdf::sca_module"
		    << " (see LRM)";
	    SC_REPORT_ERROR("SystemC-AMS",str.str().c_str());
	}

	unsigned long      delay    = this->get_delay_internal();


	if (sample_id >= delay)
	{
		std::ostringstream str;
		str << "Sample id (" << sample_id << ") is greater than delay ("
				<< delay << " while initializing port: "
				<< this->name();
		::sc_core::sc_report_handler::report(::sc_core::SC_ERROR,
				"SystemC-AMS", str.str().c_str(), __FILE__, __LINE__);
	}

	std::uint64_t scnt= sample_id;

	this->buffer[scnt%this->buffer_size]=value;
}



/////

template<class T>
inline const T& sca_out<T>::read_delayed_value(unsigned long sample_id) const
{
	if(!pmod || pmod->is_reinitialize_executing())
	{
		 std::ostringstream str;
		 str << "can't execute read_delayed_value() "
				 "outside the context of the callback reinitialize, "
				 "of the current module ";
		 str << "for port: " << this->name();
		 SC_REPORT_ERROR("SytemC-AMS",str.str().c_str());
		 return buffer[0];
	}

	unsigned long delay=this->get_delay_internal();
	if (sample_id >= delay)
	{
		std::ostringstream str;
		str << "Sample id (" << sample_id << ") is greater than delay ("
				<< this->get_delay() << " while read_delayed_value port: " << this->name();
		sc_core::sc_report_handler::report(::sc_core::SC_ERROR,"SystemC-AMS",
				str.str().c_str(), __FILE__, __LINE__);
		 return buffer[0];
	}

	long csample=
			pmod->get_call_counter() %
			pmod->get_calls_per_period()
			* this->get_rate_internal() - sample_id;


	this->buffer[csample%this->buffer_size];
}


/////

template<class T>
inline bool sca_out<T>::is_timestep_changed(unsigned long sample_id) const
{
	return (this->get_timestep(sample_id)!=this->get_last_timestep(sample_id));
}


/////

template<class T>
inline bool sca_out<T>::is_rate_changed() const
{
	return this->sca_tdf::sca_implementation::sca_port_attributes::is_rate_changed();
}

/////

template<class T>
inline bool sca_out<T>::is_delay_changed() const
{
	return this->sca_tdf::sca_implementation::sca_port_attributes::is_delay_changed();
}




template<class T>
inline void sca_out<T>::write(const T& value, unsigned long sample_id)
{

	if((allow_processing_access_flag==NULL) || !(*allow_processing_access_flag))
	{
		std::ostringstream str;
	    str << "Port: " << this->name() << " can't be written due it is not accessed "
		    << " from the context of the processing method of the parent sca_tdf::sca_module"
		    << " (see LRM)";
		SC_REPORT_ERROR("SystemC-AMS",str.str().c_str());
	}


	if (sample_id >= rate)
	{
		std::ostringstream str;
		str << "Sample id (" << sample_id
				<< ") is greater or equal than rate (" << rate
				<< " while writing to port: " << this->name();

		sc_core::sc_report_handler::report(::sc_core::SC_ERROR,
				"SystemC-AMS", str.str().c_str(), __FILE__, __LINE__);
	}


	this->write_buffer(value, sample_id);

}



template<class T>
inline sca_tdf::sca_de::sca_out<T>& sca_out<T>::operator=(const T& value)
{
	this->write(value);
	return *this;
}


template<class T>
inline sca_tdf::sca_de::sca_out<T>& sca_out<T>::operator=(const sca_tdf::sca_in<T>& iport)
{
	this->write(iport.read());
	return *this;
}


template<class T>
inline sca_tdf::sca_de::sca_out<T>& sca_out<T>::operator=(sca_tdf::sca_de::sca_in<T>& iport)
{
	this->write(iport.read());
	return *this;
}



template<class T>
inline void sca_out<T>::write
		(sca_core::sca_assign_from_proxy<sca_tdf::sca_de::sca_out<T> >& proxy)
{
	if(!(*allow_processing_access_flag))
	{
		std::ostringstream str;
	    str << "Port: " << this->name() << " can't be written due it is not accessed "
		    << " from the context of the processing method of the parent sca_tdf::sca_module"
		    << " (see LRM)";
		SC_REPORT_ERROR("SystemC-AMS",str.str().c_str());
	}

	proxy.assign_to(*this);
}



template<class T>
inline sca_tdf::sca_de::sca_out<T>& sca_out<T>::operator=(sca_core::sca_assign_from_proxy<
		sca_tdf::sca_de::sca_out<T> >& proxy)
{
	this->write(proxy);
	return *this;
}



template<class T>
inline void sca_out<T>::bind(sc_core::sc_port<sc_core::sc_signal_inout_if<T>,1,sc_core::SC_ONE_OR_MORE_BOUND >& oport)
{
	base_type::bind(oport);
}

template<class T>
void sca_out<T>::operator()(sc_core::sc_port<sc_core::sc_signal_inout_if<T>,1,sc_core::SC_ONE_OR_MORE_BOUND >& oport)
{
	bind(oport);
}




template<class T>
inline void sca_out<T>::bind(sc_core::sc_signal_inout_if<T>& intf)
{
	base_type::bind(intf);
}

template<class T>
inline void sca_out<T>::operator()(sc_core::sc_signal_inout_if<T>& intf)
{
	bind(intf);
}

//methods private and thus disabled - should not possible to call
template<class T>
inline void sca_out<T>::bind(sca_tdf::sca_de::sca_out<T>&)
{
	SC_REPORT_ERROR("SystemC-AMS","Binding of converter port to converter port not permitted");
}

template<class T>
inline void sca_out<T>::operator()(sca_tdf::sca_de::sca_out<T>& port)
{
	bind(port);
}

//overload sc_port_b bind method
template<class T>
inline void sca_out<T>::bind( sc_core::sc_port_b<sc_core::sc_signal_inout_if<T> >& parent_ )
{
	sc_core::sc_port<sc_core::sc_signal_inout_if<T> >* port;
	port=dynamic_cast<sc_core::sc_port<sc_core::sc_signal_inout_if<T> >*>(&parent_);

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
inline sca_core::sca_assign_to_proxy<sca_tdf::sca_de::sca_out<T>,T>&
	                            sca_out<T>::operator[](unsigned long sample_id)
{
  bracket_proxy.index=sample_id;
  bracket_proxy.obj=this;

  return bracket_proxy;
}


template<class T>
inline bool sca_out<T>::trace_init(sca_util::sca_implementation::sca_trace_object_data& data)
{
	traces_available=true;

	trace_data.push_back(&data);
	data.set_type_info<T>();

    data.event_driven=true;
    data.dont_interpolate=true;

    if(!trace_cluster_cb_added) this->add_cluster_trace(data);
    trace_cluster_cb_added=true;

    return true;

}

template<class T>
inline void sca_out<T>::trace_internal(const sca_core::sca_time& tp,const T& val)
{
	//will be called for each write action
	for(std::size_t i=0;i<trace_data.size();++i)
	{
		trace_data[i]->trace_base->store_time_stamp(trace_data[i]->id, tp,val);
	}
}


/*
template<class T>
inline void sca_out<T>::trace(long id, sca_util::sca_implementation::sca_trace_buffer& buffer)
{
}
*/

//sc trace of sc_core::sc_object to prevent clang warning due overloaded
//sca_traceable_object function
template<class T>
inline void sca_out<T>::trace( sc_core::sc_trace_file* tf ) const
{
	sc_core::sc_port_b<sc_core::sc_signal_inout_if<T> >::trace(tf);
}



} // namespace sca_de

//template<class T>
//typedef sca_tdf::sca_de::sca_out<T> sc_out<T> ;

template<class T>
class sc_out: public sca_tdf::sca_de::sca_out<T>
{
public:
	sc_out() :
		sca_tdf::sca_de::sca_out<T>()
	{
	}
	explicit sc_out(const char* name_) :
		sca_tdf::sca_de::sca_out<T>(name_)
	{
	}
};

} // namespace sca_tdf

#endif /* SCA_TDF_SC_OUT_H_ */
