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

 sca_tdf_out.h - tdf outport

 Original Author: Karsten Einwich Fraunhofer / COSEDA Technologies

 Created on: 04.03.2009

 SVN Version       :  $Revision: 2106 $
 SVN last checkin  :  $Date: 2020-02-26 15:58:39 +0000 (Wed, 26 Feb 2020) $ (UTC)
 SVN checkin by    :  $Author: karsten $
 SVN Id            :  $Id: sca_tdf_out.h 2106 2020-02-26 15:58:39Z karsten $

 *****************************************************************************/
/*
 LRM clause 4.1.1.6.
 The class sca_tdf::sca_out shall define a port class for the TDF MoC.
 It provides functions for defining or getting attribute values (e.g.
 sampling rate or timestep), for initialization and for writing output samples.
 The port shall be bound to a primitive channel of class sca_tdf::sca_signal
 of the corresponding type.
 */

/*****************************************************************************/

#ifndef SCA_TDF_OUT_H_
#define SCA_TDF_OUT_H_

namespace sca_tdf {

enum sca_cut_policy
{
	SCA_NO_CUT,
	SCA_CT_CUT,
	SCA_DT_CUT
};




template<class T,sca_cut_policy CUT_POL=sca_tdf::SCA_NO_CUT,
		class INTERP=sca_tdf::sca_default_interpolator<T> >
class sca_out
{
private:
	//should never be used - only specializations usable
	sca_out(){}
};


template<class T>
//class sca_out : public implementation-derived-from sca_core::sca_port<sca_tdf::sca_signal_out_if >
class sca_out<T,SCA_NO_CUT>: public sca_tdf::sca_out_base<T>
{
public:
	sca_out();
	explicit sca_out(const char* );

	virtual ~sca_out();

	void set_delay(unsigned long );
	void set_rate(unsigned long );
	void set_timestep(const sca_core::sca_time& );
	void set_timestep(double , sc_core::sc_time_unit );
	void set_max_timestep(const sca_core::sca_time& );
	void set_max_timestep(double , sc_core::sc_time_unit );

	unsigned long get_delay() const;
	unsigned long get_rate() const;
	sca_core::sca_time get_time(unsigned long sample_id = 0) const;
	sca_core::sca_time get_timestep(unsigned long sample_id=0) const;
	sca_core::sca_time get_max_timestep() const;
	sca_core::sca_time get_last_timestep(unsigned long sample_id=0) const;

	virtual const char* kind() const;

	void initialize(const T& value, unsigned long sample_id = 0);
	const T& read_delayed_value(unsigned long sample_id = 0) const;

	bool is_timestep_changed(unsigned long sample_id=0) const;
	bool is_rate_changed() const;
	bool is_delay_changed() const;

	void write(const T& value, unsigned long sample_id = 0);
	void write(sca_core::sca_assign_from_proxy<sca_tdf::sca_out_base<T> >&);

	sca_tdf::sca_out<T>& operator=(const T&);
	sca_tdf::sca_out<T>& operator=(const sca_tdf::sca_in<T>&);
	sca_tdf::sca_out<T>& operator=(const sca_tdf::sca_de::sca_in<T>&);

	sca_tdf::sca_out<T>& operator=(sca_core::sca_assign_from_proxy<
			sca_tdf::sca_out_base<T> >&);

	sca_core::sca_assign_to_proxy<sca_tdf::sca_out<T>,T >& operator[](unsigned long sample_id);

	void bind(sca_tdf::sca_out<T>&);
	void operator()(sca_tdf::sca_out<T>&);

	//hierarchical decluster port binding
	template<class INTERP>
	void bind(sca_tdf::sca_out<T,sca_tdf::SCA_CT_CUT,INTERP>& port);

	template<class INTERP>
	void operator()(sca_tdf::sca_out<T,sca_tdf::SCA_CT_CUT,INTERP>& port);


	void bind(sca_tdf::sca_out<T,sca_tdf::SCA_DT_CUT>& port);
	void operator()(sca_tdf::sca_out<T,sca_tdf::SCA_DT_CUT>& port);


	void bind(sca_tdf::sca_signal_if<T>&);
	void operator()(sca_tdf::sca_signal_if<T>&);


	/** method of interactive tracing interface, which returns the value
	 * at the current SystemC time (the time returned by sc_core::sc_time_stamp())
	 */
	const T& get_typed_trace_value() const;

	/** method of interactive tracing interface, which forces a value
	 */
    void force_typed_value(const T&);


private:
	// Disabled
	sca_out(const sca_tdf::sca_out<T>&);
	sca_tdf::sca_out<T>& operator= (const sca_tdf::sca_out<T>& );

	sca_core::sca_assign_to_proxy<sca_tdf::sca_out<T>,T > proxy;

	// Disabled
	void bind(sca_tdf::sca_in<T>&);
	void operator()(sca_tdf::sca_in<T>&);

	 //overload sc_port_b bind method
	 void bind( sc_core::sc_port_b<sca_tdf::sca_signal_if<T> >& parent_ );

protected:

	typedef sca_tdf::sca_out_base<T> base_type;
	typedef typename sca_tdf::sca_out_base<T>::base_type base_base_type;
};

//begin implementation specific



/** method of interactive tracing interface, which returns the value
 * at the current SystemC time (the time returned by sc_core::sc_time_stamp())
 */
template<class T>
const T& sca_out<T>::get_typed_trace_value() const
{
	const sc_core::sc_interface* scif=this->get_interface();
	const sca_tdf::sca_signal<T>* scasig=
			dynamic_cast<const sca_tdf::sca_signal<T>*>(scif);

	if(scasig==NULL)
	{
		std::ostringstream str;
		str << "get_(typed)_trace_value() called for not (yet) bound port: ";
		str << this->name() << " - the result will be undefined";
		SC_REPORT_WARNING("SystemC-AMS",str.str().c_str());
		static const T dummy=T();
		return dummy;
	}

	return scasig->get_typed_trace_value();
}

/** method of interactive tracing interface, which forces a value
 */
template<class T>
void sca_out<T>::force_typed_value(const T& val)
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



template<class T>
inline void sca_out<T>::bind(sca_tdf::sca_in<T>&)
{
	::sc_core::sc_report_handler::report(::sc_core::SC_ERROR, "SystemC-AMS",
			"Binding of sca_tdf::sca_in to "
			"sca_tdf::sca_out not permitted", __FILE__, __LINE__);
	return;
}


template<class T>
void sca_out<T>::bind( sc_core::sc_port_b<sca_tdf::sca_signal_if<T> >& parent_ )
{
	sca_tdf::sca_out<T>* port;
	port=dynamic_cast<sca_tdf::sca_out<T>*>(&parent_);
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
inline void sca_out<T>::operator()(sca_tdf::sca_in<T>& p) {
	sca_out<T>::bind(p);
}


template<class T>
inline void sca_out<T>::bind(sca_tdf::sca_out<T>& p)
{
	base_type::bind(p);
	return;
}

template<class T>
inline void sca_out<T>::operator()(sca_tdf::sca_out<T>& p)
{
	sca_out<T>::bind(p);
}

template<class T>
inline void sca_out<T>::bind(sca_tdf::sca_signal_if<T>& sig)
{
	base_type::bind(sig);
	return;
}

template<class T>
inline void sca_out<T>::operator()(sca_tdf::sca_signal_if<T>& sig)
{
	sca_out<T>::bind(sig);
}

template<class T>
template<class INTERP>
inline void sca_out<T>::bind(sca_tdf::sca_out<T,sca_tdf::SCA_CT_CUT,INTERP>& port)
{
	this->bind(*(port.hierarchical_decluster_channel));
}

template<class T>
template<class INTERP>
inline void sca_out<T>::operator()(sca_tdf::sca_out<T,sca_tdf::SCA_CT_CUT,INTERP>& port)
{
	this->bind(port);
}

template<class T>
inline void sca_out<T>::bind(sca_tdf::sca_out<T,sca_tdf::SCA_DT_CUT>& port)
{
	this->bind(*(port.hierarchical_decluster_channel));
}

template<class T>
inline void sca_out<T>::operator()(sca_tdf::sca_out<T,sca_tdf::SCA_DT_CUT>& port)
{
	this->bind(port);
}



template<class T>
inline sca_out<T>::sca_out() :
	sca_tdf::sca_out_base<T>(sc_core::sc_gen_unique_name("sca_tdf_out"))
{
}

template<class T>
inline sca_out<T>::~sca_out()
{
}

//////

template<class T>
inline sca_out<T>::sca_out(const char* name_) : sca_tdf::sca_out_base<T>(name_)
{
}

///////

template<class T>
inline void sca_out<T>::set_delay(unsigned long nsamples)
{
	base_base_type::set_delay(nsamples);
}

/////

template<class T>
inline void sca_out<T>::set_rate(unsigned long rate)
{
	base_base_type::set_rate(rate);
}

//////////

template<class T>
inline void sca_out<T>::set_timestep(const sca_core::sca_time& tstep)
{
	base_base_type::set_timestep(tstep);
}

//////

template<class T>
inline void sca_out<T>::set_timestep(double tstep, ::sc_core::sc_time_unit unit)
{
	base_base_type::set_timestep(tstep, unit);
}

////

template<class T>
inline void sca_out<T>::set_max_timestep(const sca_core::sca_time& tstep)
{
	base_base_type::set_max_timestep(tstep);
}

////

template<class T>
inline void sca_out<T>::set_max_timestep(double tstep, sc_core::sc_time_unit unit)
{
	base_base_type::set_max_timestep(tstep, unit);
}



////


template<class T>
inline unsigned long sca_out<T>::get_delay() const
{
	return base_base_type::get_delay();
}

/////

template<class T>
inline unsigned long sca_out<T>::get_rate() const
{
	return base_base_type::get_rate();
}

//////

template<class T>
inline sca_core::sca_time sca_out<T>::get_time(unsigned long sample_id) const
{
	return base_base_type::get_time(long(sample_id));
}

////////

template<class T>
inline sca_core::sca_time sca_out<T>::get_timestep(unsigned long sample_id) const
{
	return base_base_type::get_timestep(sample_id);
}

/////////

template<class T>
inline sca_core::sca_time sca_out<T>::get_max_timestep() const
{
	return base_base_type::get_max_timestep();
}

/////////

template<class T>
inline sca_core::sca_time sca_out<T>::get_last_timestep(unsigned long sample_id) const
{
	return base_base_type::get_last_timestep(sample_id);
}

////////

template<class T>
inline const char* sca_out<T>::kind() const {
	return "sca_tdf::sca_out";
}

//////////

template<class T>
inline void sca_out<T>::initialize(const T& value, unsigned long sample_id)
{
	base_base_type::initialize(value, sample_id);
}

/////

template<class T>
inline const T& sca_out<T>::read_delayed_value(unsigned long sample_id) const
{
	return base_base_type::read_delayed_value(sample_id);
}


/////

template<class T>
inline bool sca_out<T>::is_timestep_changed(unsigned long sample_id) const
{
	return base_base_type::is_timestep_changed(sample_id);
}


/////

template<class T>
inline bool sca_out<T>::is_rate_changed() const
{
	return base_base_type::is_rate_changed();

}

/////

template<class T>
inline bool sca_out<T>::is_delay_changed() const
{
	return base_base_type::is_delay_changed();
}





//////

template<class T>
inline void sca_out<T>::write(const T& value, unsigned long sample_id)
{
	base_base_type::write(value, sample_id);
}

/////

template<class T>
inline sca_tdf::sca_out<T>& sca_out<T>::operator=(const T& value)
{
	base_base_type::write(value);
	return *this;
}

//////////////

template<class T>
inline sca_tdf::sca_out<T>& sca_out<T>::operator=(
		const sca_tdf::sca_in<T>& port)
{
	base_base_type::write(port.read());
	return *this;
}

/////////

template<class T>
inline sca_tdf::sca_out<T>& sca_out<T>::operator=(
		const sca_tdf::sca_de::sca_in<T>& port)
{
	this->write(const_cast<sca_tdf::sca_de::sca_in<T>*>(&port)->read());
	return *this;
}

////////

template<class T>
inline void sca_out<T>::write(sca_core::sca_assign_from_proxy<sca_tdf::sca_out_base<T> >& proxy)
{
	proxy.assign_to(*this);
}

template<class T>
inline sca_tdf::sca_out<T>& sca_out<T>::operator=(
		sca_core::sca_assign_from_proxy<sca_tdf::sca_out_base<T> >& proxy)
{
	write(proxy);
	return *this;
}

//////

template<class T>
inline sca_core::sca_assign_to_proxy<sca_tdf::sca_out<T>,T >& sca_out<T>::operator[](unsigned long sample_id) {
	proxy.index=sample_id;
	proxy.obj=this;
	return proxy;
}

template<class T>
inline sca_out<T>::sca_out(const sca_tdf::sca_out<T>&) {

}

//end implementation specific


} // namespace sca_tdf



#endif /* SCA_TDF_OUT_H_ */
