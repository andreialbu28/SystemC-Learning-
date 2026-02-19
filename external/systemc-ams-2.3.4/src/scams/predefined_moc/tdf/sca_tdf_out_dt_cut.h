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

 sca_tdf_out_dt_cut.h - tdf declustering outport

 Original Author: Karsten Einwich Fraunhofer / COSEDA Technologies

 Created on: 14.11.2011

 SVN Version       :  $Revision: 2320 $
 SVN last checkin  :  $Date: 2023-06-08 12:47:38 +0000 (Thu, 08 Jun 2023) $ (UTC)
 SVN checkin by    :  $Author: pehrlich $
 SVN Id            :  $Id: sca_tdf_out_dt_cut.h 2320 2023-06-08 12:47:38Z pehrlich $

 *****************************************************************************/
/*
 * New SystemC-AMS 2.0 feature
 */
/*****************************************************************************/

#ifndef SCA_TDF_OUT_DT_CUT_H_
#define SCA_TDF_OUT_DT_CUT_H_

#include <queue>


namespace sca_tdf {


template<class T>
//class sca_out : public implementation-derived-from sca_core::sca_port<sca_tdf::sca_signal_out_if >
class sca_out<T,SCA_DT_CUT>: public sca_tdf::sca_out_base<T>
{

	typedef  sca_tdf::sca_implementation::sca_port_attributes attr_base;

public:
	sca_out();
	explicit sca_out(const char* );

	virtual ~sca_out();

	void set_delay(unsigned long );
	void set_ct_delay(const sca_core::sca_time& );
	void set_ct_delay(double ,sc_core::sc_time_unit );
	void set_rate(unsigned long );
	void set_timestep(const sca_core::sca_time& );
	void set_timestep(double , ::sc_core::sc_time_unit );
	void set_max_timestep(const sca_core::sca_time& );
	void set_max_timestep(double , ::sc_core::sc_time_unit );

	unsigned long get_delay() const;
	sca_core::sca_time get_ct_delay() const;
	unsigned long get_rate() const;
	sca_core::sca_time get_time(unsigned long sample_id = 0) const;
	sca_core::sca_time get_timestep(unsigned long sample_id = 0) const;
	sca_core::sca_time get_max_timestep() const;
	sca_core::sca_time get_last_timestep(unsigned long sample_id=0) const;

	virtual const char* kind() const;

	void initialize(const T& value, unsigned long sample_id = 0);
	void set_initial_value(const T& );
	const T& read_delayed_value(unsigned long sample_id = 0) const;

	bool is_timestep_changed(unsigned long sample_id=0) const;
	bool is_rate_changed() const;
	bool is_delay_changed() const;

	void write(const T& value, unsigned long sample_id = 0);
	void write(sca_core::sca_assign_from_proxy<sca_tdf::sca_out_base<T> >&);
	sca_tdf::sca_out<T,sca_tdf::SCA_DT_CUT>& operator=(const T& );
	sca_tdf::sca_out<T,sca_tdf::SCA_DT_CUT>& operator=(const sca_tdf::sca_in<T>& );
	sca_tdf::sca_out<T,sca_tdf::SCA_DT_CUT>& operator=(sca_tdf::sca_de::sca_in<T>& );
	sca_tdf::sca_out<T,sca_tdf::SCA_DT_CUT>& operator=(sca_core::sca_assign_from_proxy<
			sca_tdf::sca_out_base<T> >&);
	sca_core::sca_assign_to_proxy<sca_tdf::sca_out<T,sca_tdf::SCA_DT_CUT>,T >& operator[](unsigned long sample_id);


	/** method of interactive tracing interface, which returns the value
	 * at the current SystemC time (the time returned by sc_core::sc_time_stamp())
	 */
	const T& get_typed_trace_value() const;

	/** method of interactive tracing interface, which forces a value
	 */
    void force_typed_value(const T&);

    /**
     * releases forced value
     */
    virtual void release_value();

    /**
     * registers trace callback
     */
    virtual bool register_trace_callback(
    		sca_util::sca_traceable_object::sca_trace_callback,void*);

    /**
     * gets current value as string
     */
    virtual const std::string& get_trace_value() const;

private:
	// Disabled
	sca_out(const sca_tdf::sca_out<T,sca_tdf::SCA_DT_CUT>&);

	sca_tdf::sca_out<T,sca_tdf::SCA_DT_CUT>& operator=
			(const sca_tdf::sca_out<T,sca_tdf::SCA_DT_CUT>&);

	const unsigned long& get_delay_internal() const override;

	sca_core::sca_assign_to_proxy<sca_tdf::sca_out<T,sca_tdf::SCA_DT_CUT>,T > proxy;


	//method called by synchronization layer at each sampling point
	//of the declustered inport
	void read_from_sc() override;

	void port_processing() override;

	//method called by the synchronization layer at each sampling point of
	//the cluster
	void write_to_sc()  override;

	void write_buffer_out(const T& val,unsigned long n);


	sca_core::sca_time ct_delay;


	T* buffer_out;
	std::uint64_t buffer_out_size;

	std::queue<T> decluster_queue;


	unsigned long read_cnt;

	T last_value;
	T next_value;
	sca_core::sca_time next_value_time;
	sca_core::sca_time remaining_delay;

	bool value_forced;

	//samples delay of the outport - overall delay: sample_delay*timestep + interpolation_delay
	unsigned long sample_delay;
	unsigned long sample_ct_delay;

	void initialize_port() override;
	bool reinitialize_port_timing() override;
	void reinitialize_port() override;


	typedef sca_tdf::sca_out_base<T>            base_type;
	typedef typename sca_tdf::sca_out_base<T>::base_type base_base_type;

	void construct();

	//method to pass attributes to module after change attributes
	void validate_port_attributes();

	sca_core::sca_module* pmod;

	class sca_hierarchical_decluster_module : public sca_tdf::sca_module
	{
	public:
		sca_tdf::sca_in<T> inp;

		void processing();

		sca_hierarchical_decluster_module(
				sc_core::sc_module_name,
				sca_tdf::sca_out<T,SCA_DT_CUT>& dec_port,
				sca_tdf::sca_signal<T>& ch);

		sca_tdf::sca_out<T,SCA_DT_CUT>& dport;
		sca_tdf::sca_signal<T>& channel;

	};

	sca_hierarchical_decluster_module* hdmodule;

public:

	sca_tdf::sca_signal<T>*            hierarchical_decluster_channel;
};

//begin implementation specific

template<class T>
inline const unsigned long& sca_out<T,SCA_DT_CUT>::get_delay_internal() const
{
	return this->sample_delay;
}


template<class T>
inline void sca_out<T,SCA_DT_CUT>::initialize_port()
{
	if(buffer_out!=NULL)
	{
		this->reinitialize_port();
		return;
	}

	sca_core::sca_time overall_delay=this->attr_base::get_delay_internal()*this->get_timestep_internal(0) + this->ct_delay;

	this->remaining_delay=overall_delay%this->get_timestep_internal(0);
	this->sample_delay=overall_delay/this->get_timestep_internal(0);
	this->sample_ct_delay=this->sample_delay-this->attr_base::get_delay_internal();


	this->buffer_out_size=this->sample_delay + this->get_rate_internal() * this->sca_core::sca_implementation::sca_port_base::get_parent_module()->get_calls_per_period();

	this->buffer_out=new T[this->buffer_out_size];
	this->read_cnt=0;
}


template<class T>
inline bool sca_out<T,SCA_DT_CUT>::reinitialize_port_timing()
{
	sca_core::sca_time overall_delay=this->attr_base::get_delay_internal()*this->get_timestep_internal(0) + this->ct_delay;

	unsigned long old_sample_delay=this->sample_delay;

	this->remaining_delay=overall_delay%this->get_timestep_internal(0);
	this->sample_delay=overall_delay/this->get_timestep_internal(0);
	this->sample_ct_delay=this->sample_delay-this->attr_base::get_delay_internal();

	return old_sample_delay!=this->sample_delay;
}



template<class T>
inline void sca_out<T,SCA_DT_CUT>::reinitialize_port()
{
	sca_core::sca_time overall_delay=this->attr_base::get_delay_internal()*this->get_timestep_internal(0) + this->ct_delay;

	this->remaining_delay=overall_delay%this->get_timestep_internal(0);
	this->sample_delay=overall_delay/this->get_timestep_internal(0);


	std::uint64_t new_buffer_size=this->sample_delay + this->get_rate_internal() * this->sca_core::sca_implementation::sca_port_base::get_parent_module()->get_calls_per_period();

	T* new_buffer=NULL;
	if(new_buffer_size!=this->buffer_out_size)
	{
		new_buffer=new T[new_buffer_size];
		for(unsigned long idx=0;idx<new_buffer_size;++idx) new_buffer[idx]=T();
	}
	else
	{
		new_buffer=this->buffer_out;
	}


	unsigned long new_read=this->pmod->get_call_counter() * this->get_rate_internal();
	for(unsigned long idx=0;idx<this->sample_delay;++idx)
	{
		new_buffer[(new_read+idx)%new_buffer_size]=this->buffer_out[(this->read_cnt+idx)%this->buffer_out_size];
	}

	this->read_cnt=new_read;

	if(new_buffer_size!=this->buffer_out_size)
	{
		delete [] this->buffer_out;
		this->buffer_out=new_buffer;
		this->buffer_out_size=new_buffer_size;
	}
}



/** method of interactive tracing interface, which returns the value
 * at the current SystemC time (the time returned by sc_core::sc_time_stamp())
 */
template<class T>
inline const T& sca_out<T,SCA_DT_CUT>::get_typed_trace_value() const
{
	const sc_core::sc_interface* scif=this->get_interface();
	const sca_tdf::sca_signal<T>* scasig=
			dynamic_cast<const sca_tdf::sca_signal<T>*>(scif);

	if(scasig==NULL)
	{
		static const T dummy=T();
		return dummy;
	}

	return scasig->get_typed_trace_value();
}


/** method of interactive tracing interface, which forces a value
 */
template<class T>
inline void sca_out<T,SCA_DT_CUT>::force_typed_value(const T& val)
{
	sc_core::sc_interface* scif=this->get_interface();
	sca_tdf::sca_signal<T>* scasig=
			dynamic_cast<sca_tdf::sca_signal<T>*>(scif);

	if(scasig==NULL)
	{
		return;
	}

	scasig->force_typed_value(val);
}


/** method of interactive interface, which release forced a value
 */
template<class T>
inline void sca_out<T,SCA_DT_CUT>::release_value()
{
	sc_core::sc_interface* scif=this->get_interface();
	sca_tdf::sca_signal<T>* scasig=
			dynamic_cast<sca_tdf::sca_signal<T>*>(scif);

	if(scasig==NULL)
	{
		return;
	}

	scasig->release_value();
}


template<class T>
inline bool sca_out<T,SCA_DT_CUT>::register_trace_callback(
		sca_util::sca_traceable_object::sca_trace_callback cb,
		void* arg)
{
	sc_core::sc_interface* scif=this->get_interface();
	sca_tdf::sca_signal<T>* scasig=
			dynamic_cast<sca_tdf::sca_signal<T>*>(scif);

	if(scasig==NULL)
	{
		return false;
	}

	return scasig->register_trace_callback(cb,arg);
}


template<class T>
inline const std::string& sca_out<T,SCA_DT_CUT>::get_trace_value() const
{
	const sc_core::sc_interface* scif=this->get_interface();
	const sca_tdf::sca_signal<T>* scasig=
			dynamic_cast<const sca_tdf::sca_signal<T>*>(scif);

	if(scasig==NULL)
	{
		static const std::string empty_string;
		return empty_string;
	}

	return scasig->get_trace_value();
}



template<class T>
inline void sca_out<T,SCA_DT_CUT>::sca_hierarchical_decluster_module::processing()
{
	dport.write(inp.read());
}

template<class T>
inline sca_out<T,SCA_DT_CUT>::sca_hierarchical_decluster_module::sca_hierarchical_decluster_module(
		sc_core::sc_module_name,
		sca_tdf::sca_out<T,SCA_DT_CUT>& dec_port,
		sca_tdf::sca_signal<T>& ch) :
			dport(dec_port),
			channel(ch)
{
	this->register_port(&dec_port);
	inp.bind(channel);
}


template<class T>
inline void sca_out<T,SCA_DT_CUT>::validate_port_attributes()
{
	this->port_attribute_validation();
}


template<class T>
inline void sca_out<T,SCA_DT_CUT>::construct()
{
	base_base_type::sca_port_type = base_base_type::SCA_DECL_OUT_PORT;


	pmod=dynamic_cast<sca_core::sca_module*>(this->get_parent_object());

	if(pmod==NULL)
	{
		hierarchical_decluster_channel=new sca_tdf::sca_signal<T>("hierarchical_decluster_channel");

		//create dummy module, attach port to this module, create channel
		hdmodule=new sca_hierarchical_decluster_module
				(sc_core::sc_gen_unique_name("sca_hierarchical_decluster_module"),
						*this,*hierarchical_decluster_channel);

		//change parent module
		pmod=hdmodule;
		this->parent_module=hdmodule;
		this->set_parent_attribute_module(pmod);
	}

	this->decluster_port=true;
	this->value_forced=false;

	this->next_value_time=sc_core::sc_max_time();

	buffer_out=NULL;
	buffer_out_size=0;
	sample_ct_delay=0;

	sample_delay=0;

}


template<class T>
inline sca_out<T,SCA_DT_CUT>::sca_out() :
	sca_tdf::sca_out_base<T>(sc_core::sc_gen_unique_name("sca_tdf_out_dt_cut"))
{
	construct();
}

//////

template<class T>
inline sca_out<T,SCA_DT_CUT>::sca_out(const char* name_) :
	sca_tdf::sca_out_base<T>(name_)
{
	construct();
}



template<class T>
inline sca_out<T,SCA_DT_CUT>::sca_out(const sca_tdf::sca_out<T,SCA_DT_CUT>&)
{
	construct();
}


template<class T>
inline sca_out<T,SCA_DT_CUT>::~sca_out()
{

}


template<class T>
inline void sca_out<T,SCA_DT_CUT>::set_delay(unsigned long nsamples)
{
	this->sca_tdf::sca_implementation::sca_port_attributes::set_delay(nsamples);
}


template<class T>
inline void sca_out<T,SCA_DT_CUT>::set_rate(unsigned long _rate)
{
	this->sca_tdf::sca_implementation::sca_port_attributes::set_rate(_rate);
}


template<class T>
inline void sca_out<T,SCA_DT_CUT>::set_timestep(const sca_core::sca_time& tstep)
{
	this->sca_tdf::sca_implementation::sca_port_attributes::set_timestep(tstep);

}

template<class T>
inline void sca_out<T,SCA_DT_CUT>::set_timestep(double tstep, ::sc_core::sc_time_unit unit)
{
	this->sca_tdf::sca_implementation::sca_port_attributes::set_timestep(sca_core::sca_time(tstep,unit));
}



////

template<class T>
inline void sca_out<T,SCA_DT_CUT>::set_max_timestep(const sca_core::sca_time& tstep)
{
	this->sca_tdf::sca_implementation::sca_port_attributes::set_max_timestep(tstep);
}

////

template<class T>
inline void sca_out<T,SCA_DT_CUT>::set_max_timestep(double tstep, sc_core::sc_time_unit unit)
{
	this->sca_tdf::sca_implementation::sca_port_attributes::set_max_timestep(sca_core::sca_time(tstep,unit));
}




template<class T>
inline unsigned long sca_out<T,SCA_DT_CUT>::get_delay() const
{
	return sca_tdf::sca_implementation::sca_port_attributes::get_delay();
}



template<class T>
inline sca_core::sca_time sca_out<T,SCA_DT_CUT>::get_ct_delay() const
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
		 str << "can't execute get_ct_delay() "
				 "outside the context of the callbacks ac_processing, "
				 "initialize, reinitialize, processing or change_attributes"
				 "of the current module ";
		 str << "for port: " << this->sca_name();
		 SC_REPORT_ERROR("SytemC-AMS",str.str().c_str());
		 return last_value;
	}

	return ct_delay;
}


template<class T>
inline unsigned long sca_out<T,SCA_DT_CUT>::get_rate() const
{
	return this->sca_tdf::sca_implementation::sca_port_attributes::get_rate();
}


template<class T>
inline sca_core::sca_time sca_out<T,SCA_DT_CUT>::get_timestep(unsigned long sample_id) const
{
	return this->sca_tdf::sca_implementation::sca_port_attributes::get_timestep(sample_id);
}



/////////

template<class T>
inline sca_core::sca_time sca_out<T,SCA_DT_CUT>::get_max_timestep() const
{
	return this->sca_tdf::sca_implementation::sca_port_attributes::get_max_timestep();
}

/////////

template<class T>
inline sca_core::sca_time sca_out<T,SCA_DT_CUT>::get_last_timestep(unsigned long sample_id) const
{
	return this->sca_tdf::sca_implementation::sca_port_attributes::get_last_timestep(sample_id);
}




template<class T>
inline sca_core::sca_time sca_out<T,SCA_DT_CUT>::get_time(unsigned long sample_id) const
{
	return this->sca_tdf::sca_implementation::sca_port_attributes::get_time(sample_id);
}




template<class T>
const char* sca_out<T,SCA_DT_CUT>::kind() const
{
	return "sca_tdf::sca_out<SCA_DT_CUT>";
}

////////////////////////////////////////////////////////////////////////

//method called by the synchronization layer at each sampling point of
//the (current) cluster
template<class T>
inline void sca_out<T,SCA_DT_CUT>::write_to_sc()
{
	T value;

	value=this->buffer_out[this->read_cnt%this->buffer_out_size];

	this->read_cnt++;

	//due to ct delay, the interpolation delay can be larger than a timestep
	//the interpolator can only manage one time interval
	//thus we must manage the may required one point more outside the interpolator

	const sc_core::sc_time& ctime=sc_core::sc_time_stamp();
	sca_core::sca_time nstore_time=ctime+this->remaining_delay;;

	if(nstore_time<ctime)
	{
		this->last_value=value;
		this->next_value_time=sc_core::sc_max_time();
	}
	else
	{
		this->last_value=this->next_value;
		this->next_value=value;
		this->next_value_time=nstore_time;
	}

}

template<class T>
inline void sca_out<T,SCA_DT_CUT>::write_buffer_out(const T& val,unsigned long n)
{
	std::uint64_t scnt=this->pmod->get_call_counter() * this->get_rate_internal() + this->sample_delay + n;
	this->buffer_out[scnt%this->buffer_out_size]=val;
}



//method called by synchronization layer at each sampling point
//of the declustered inport
template<class T>
inline void sca_out<T,SCA_DT_CUT>::read_from_sc()
{
	const sc_core::sc_time& ctime=sc_core::sc_time_stamp();

	if(ctime>this->next_value_time)
	{
		this->last_value=this->next_value;
		this->next_value_time=sc_core::sc_max_time();
	}

	this->decluster_queue.push(this->last_value);
}


template<class T>
inline void sca_out<T,SCA_DT_CUT>::port_processing()
{
	if(this->allow_processing_access_flag!=NULL) *this->allow_processing_access_flag=true;
	this->base_base_type::write(this->decluster_queue.front());
	if(this->allow_processing_access_flag!=NULL)  *this->allow_processing_access_flag=false;

	this->decluster_queue.pop();
}




////////////////////////////////////////////////////////////////////////

template<class T>
inline void sca_out<T,SCA_DT_CUT>::write(const T& value, unsigned long sample_id)
{
	if(  (pmod==NULL) ||
		 !( pmod->is_processing_executing()
		  ))
	{
		 std::ostringstream str;
		 str << "can't execute write() "
				 "outside the context of the callback, "
				 "processing "
				 "of the current module ";
		 str << "for port: " << this->sca_name();
		 SC_REPORT_ERROR("SytemC-AMS",str.str().c_str());
		 return;
	}

	this->write_buffer_out(value, sample_id);
}



//////////////////////

template<class T>
inline void sca_out<T,SCA_DT_CUT>::set_ct_delay(double dt,sc_core::sc_time_unit tu)
{
	this->set_ct_delay(sca_core::sca_time(dt,tu));
}

template<class T>
inline void sca_out<T,SCA_DT_CUT>::set_ct_delay(const sca_core::sca_time& cdt)
{
	if(  (pmod==NULL) ||
		 !(
			pmod->is_set_attributes_executing()   ||
			pmod->is_change_attributes_executing()
		  ))
	{
		 std::ostringstream str;
		 str << "can't execute set_ct_delay() "
				 "outside the context of the callbacks "
				 "set_attributes or change_attributes"
				 "of the current module ";
		 str << "for port: " << this->sca_name();
		 SC_REPORT_ERROR("SytemC-AMS",str.str().c_str());
		 return;
	}

	ct_delay=cdt;
}

//////////////////////

template<class T>
inline void sca_out<T,SCA_DT_CUT>::set_initial_value(const T& value)
{
	if(  (pmod==NULL) ||
		 ((!( pmod->is_initialize_executing()    ||
			 pmod->is_reinitialize_executing())

		 ) && sc_core::sc_is_running())
		 )
	{
		 std::ostringstream str;
		 str << "can't execute set_initial_value "
				 "outside the context of the callback "
				 "initialize, reinitialize or before end of elaboration"
				 "of the current module ";
		 str << "for port: " << this->sca_name();
		 SC_REPORT_ERROR("SytemC-AMS",str.str().c_str());
		 return;
	}


	this->next_value=value;
	this->last_value=value;
}

//////////////////////

template<class T>
inline void sca_out<T,SCA_DT_CUT>::initialize(const T& value, unsigned long sample_id)
{
	if(  (pmod==NULL) ||
		 !( pmod->is_initialize_executing()    ||
			pmod->is_reinitialize_executing()
		  ))
	{
		 std::ostringstream str;
		 str << "can't execute initialize "
				 "outside the context of the callbacks "
				 "initialize or reinitialize"
				 "of the current module ";
		 str << "for port: " << this->sca_name();
		 SC_REPORT_ERROR("SytemC-AMS",str.str().c_str());
		 return;
	}

	if(sample_id>=this->get_delay_internal())
	{
		std::ostringstream str;
		str << "Sample number: " << sample_id << " for port initialize for port: " << this->name();
		str << " must be smaller than the port delay: " << this->get_delay_internal();
		SC_REPORT_ERROR("SystemC-AMS",str.str().c_str());
	}

	this->buffer_out[this->sample_ct_delay+sample_id]=value;
}


/////

template<class T>
inline const T& sca_out<T,SCA_DT_CUT>::read_delayed_value(unsigned long sample_id) const
{
	if(  (pmod==NULL) ||
		 !( pmod->is_ac_processing_executing() ||
			pmod->is_initialize_executing()    ||
			pmod->is_reinitialize_executing()
		  ))
	{
		 std::ostringstream str;
		 str << "can't execute read_delayed_value(unsigned long sample_id) "
				 "outside the context of the callbacks ac_processing, "
				 "initialize or reinitialize"
				 "of the current module ";
		 str << "for port: " << this->sca_name();
		 SC_REPORT_ERROR("SytemC-AMS",str.str().c_str());
		 return last_value;
	}

	long csample=
			pmod->get_call_counter() %
			pmod->get_calls_per_period()
			* this->get_rate_internal() - sample_id;

	return this->buffer[csample%this->buffer_size];
}


/////

template<class T>
inline bool sca_out<T,SCA_DT_CUT>::is_timestep_changed(unsigned long sample_id) const
{
	return this->sca_tdf::sca_implementation::sca_port_attributes::is_timestep_changed(sample_id);
}


/////

template<class T>
inline bool sca_out<T,SCA_DT_CUT>::is_rate_changed() const
{
	return this->sca_tdf::sca_implementation::sca_port_attributes::is_rate_changed();
}

/////

template<class T>
inline bool sca_out<T,SCA_DT_CUT>::is_delay_changed() const
{
	return this->sca_tdf::sca_implementation::sca_port_attributes::is_delay_changed();
}




//////////////////


template<class T>
inline void sca_out<T,SCA_DT_CUT>::write(sca_core::sca_assign_from_proxy<sca_tdf::sca_out_base<T> >& proxy)
{
	if(  (pmod==NULL) ||
		 !(
			pmod->is_processing_executing()
		  ))
	{
		 std::ostringstream str;
		 str << "can't execute write "
				 "outside the context of the callback "
				 "processing() "
				 "of the current module ";
		 str << "for port: " << this->sca_name();
		 SC_REPORT_ERROR("SytemC-AMS",str.str().c_str());
		 return;
	}

	proxy.assign_to(*this);
}

template<class T>
inline sca_tdf::sca_out<T,SCA_DT_CUT>& sca_out<T,SCA_DT_CUT>::operator=(const T& value)
{
	this->write(value);
	return *this;
}



template<class T>
inline sca_tdf::sca_out<T,SCA_DT_CUT>& sca_out<T,SCA_DT_CUT>::operator=(const sca_tdf::sca_in<T>& port)
{
	this->write(port.read());
	return *this;
}

template<class T>
inline sca_tdf::sca_out<T,SCA_DT_CUT>& sca_out<T,SCA_DT_CUT>::operator=(sca_tdf::sca_de::sca_in<T>& port)
{
	this->write(port.read());
	return *this;
}

template<class T>
inline sca_tdf::sca_out<T,SCA_DT_CUT>& sca_out<T,SCA_DT_CUT>::operator=(sca_core::sca_assign_from_proxy<
		sca_tdf::sca_out_base<T> >& proxy)
{
	this->write(proxy);
	return *this;
}

template<class T>
inline sca_core::sca_assign_to_proxy<sca_tdf::sca_out<T,SCA_DT_CUT>,T >& sca_out<T,SCA_DT_CUT>::operator[](unsigned long sample_id)
{
	if(  (pmod==NULL) ||
		 !(
			pmod->is_processing_executing()
		  ))
	{
		 std::ostringstream str;
		 str << "can't execute write by operator [] "
				 "outside the context of the callback "
				 "processing() "
				 "of the current module ";
		 str << "for port: " << this->sca_name();
		 SC_REPORT_ERROR("SytemC-AMS",str.str().c_str());
		 return proxy;
	}

	proxy.index=sample_id;
	proxy.obj=this;
	return proxy;
}

//end implementation specific


} // namespace sca_tdf



#endif /* SCA_TDF_OUT_DT_CUT_H_ */
