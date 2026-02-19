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

  sca_tdf_port_impl.h - description

  Original Author: Karsten Einwich Fraunhofer IIS/EAS Dresden

  Created on: 05.08.2009

   SVN Version       :  $Revision: 2320 $
   SVN last checkin  :  $Date: 2023-06-08 12:47:38 +0000 (Thu, 08 Jun 2023) $
   SVN checkin by    :  $Author: pehrlich $
   SVN Id            :  $Id: sca_tdf_port_impl.h 2320 2023-06-08 12:47:38Z pehrlich $

 *****************************************************************************/

/*****************************************************************************/


#ifndef SCA_TDF_PORT_IMPL_H_
#define SCA_TDF_PORT_IMPL_H_

namespace sca_ac_analysis
{
bool sca_ac_is_running();
}

namespace sca_core
{
namespace sca_implementation
{
	const sca_core::sca_time& NOT_VALID_SCA_TIME();
}
}


namespace sca_tdf
{

namespace sca_implementation
{

/**
   Port base class for static dataflow modeling
   provides interfaces for port read/write,
   the datflow scheduling configuration (sample rates,
   sample time, time offset) and  getting currently  setted
   rates and times
*/
template<class SC_IF, class T>
class sca_tdf_port_impl : public sca_core::sca_port<SC_IF>,
                          public sca_tdf::sca_implementation::sca_port_attributes
{

 protected:

  typedef ::sca_core::sca_port<SC_IF> base_type;


  //declaration due permissive problem (no arguments which are depending on
  //a template parameter)
  unsigned long get_if_id() const { return base_type::get_if_id(); }

  sca_tdf_port_impl();
  explicit sca_tdf_port_impl(const char* name_);

  virtual ~sca_tdf_port_impl(){}

  /** Port read for first sample */
  const T& read() const;

  /** Port read for an arbitrary sample (sample must be < rate) */
  const T& read(unsigned long sample) const;

  /** Read delayed value in reinitialize*/
  const T& read_delayed_value(unsigned long sample) const;

  /** Port write to the first sample */
  void write(const T& value);


  /** Port write for an an arbitrary sample (sample must be < rate) */
  void write(const T& value,unsigned long sample);

  /** Port write in the initialize phase */
  void initialize(const T& value, unsigned long sample_id);

  /** gets reference to sample sample (for implementing [] operator for
   * tdf out
   */
  T& get_ref_for_write(unsigned long sample) const;

  /** Sets sample rate of the port */
  void set_rate(long rate);

  /** Sets number of sample delay of the port */
  void set_delay(long samples);

  /** Sets time distance between two samples */
  void set_timestep(sca_core::sca_time t_period) ;
  void set_timestep(double dtime, sc_core::sc_time_unit unit) ;

  /** Sets time distance between two samples */
  void set_max_timestep(sca_core::sca_time t_period) ;
  void set_max_timestep(double dtime, sc_core::sc_time_unit unit) ;


  /** Gets the absolute number of samples (from simulation start) samples
      of the current call are not included */
  unsigned long get_sample_cnt() const { return sig->get_sample_cnt(get_if_id()); }
  /** Gets number of sample delay of the port */
  unsigned long get_delay() const
  {
	  return sca_tdf::sca_implementation::sca_port_attributes::get_delay();
  }

  /** Gets the setted sampling rate of the port */
  unsigned long get_rate() const
  {
	  return sca_tdf::sca_implementation::sca_port_attributes::get_rate();
  }


  /** Gets the calculated sample time distance -> if the time was setted it must
      be equal to the calculated one - if not the scheduler will throw an
      exception */
  sca_core::sca_time get_timestep(unsigned long sample_id) const
  {
	  return sca_tdf::sca_implementation::sca_port_attributes::get_timestep(sample_id);
  }


	sca_core::sca_time get_last_timestep(unsigned long sample_id) const
	{
		  return sca_tdf::sca_implementation::sca_port_attributes::get_last_timestep(sample_id);
	}


  /** Gets the absolute time (from simulation start) of the first sample of the
      current call */
  sca_core::sca_time get_time(unsigned long sample_id = 0) const
  {
	  return sca_tdf::sca_implementation::sca_port_attributes::get_time(sample_id);
  }



	sca_core::sca_time get_max_timestep() const
	{
		return sca_tdf::sca_implementation::sca_port_attributes::get_max_timestep();
	}



	bool is_timestep_changed(unsigned long sample_id) const
	{
		return sca_tdf::sca_implementation::sca_port_attributes::is_timestep_changed(sample_id);
	}



	unsigned long get_last_rate()
	{
		return sca_tdf::sca_implementation::sca_port_attributes::get_last_rate();
	}



	/*unsigned long get_last_last_rate()
	{
		return sca_tdf::sca_implementation::sca_port_attributes::get_last_last_rate();
	}
*/

	bool is_rate_changed() const
	{
		return sca_tdf::sca_implementation::sca_port_attributes::is_rate_changed();
	}

	bool is_delay_changed() const
	{
		return sca_tdf::sca_implementation::sca_port_attributes::is_delay_changed();
	}


  virtual const char* kind() const  override { return "sca_tdf_port_impl"; }


 protected:

  void end_of_port_elaboration() override; 
  void start_of_simulation() override;

  bool* allow_processing_access_flag;

 private:

  sca_core::sca_time t0;


  bool allow_processing_access_default;

  void construct();

  sca_tdf::sca_implementation::sca_tdf_signal_impl_if<T>* sig;

  void set_type_info(sca_util::sca_implementation::sca_trace_object_data& data) override;

  sca_core::sca_module* pmod; //parent module

  T*    quick_value;
  bool* is_quick_access;

};

//////////////////////////////////////////////////////





template<class SC_IF, class T>
inline void sca_tdf_port_impl<SC_IF,T>::
	set_type_info(sca_util::sca_implementation::sca_trace_object_data& data)
{
	data.set_type_info<T>();
}



template<class SC_IF, class T>
inline void sca_tdf_port_impl<SC_IF,T>::set_rate(long rate_)
{
	sca_tdf::sca_implementation::sca_port_attributes::set_rate((unsigned long)(rate_));
}

template<class SC_IF, class T>
inline sca_tdf_port_impl<SC_IF,T>::sca_tdf_port_impl() : base_type(), sca_tdf::sca_implementation::sca_port_attributes(this)
{
  construct();

#ifdef SCA_IMPLEMENTATION_DEBUG
  std::cout << "port " << sc_object::basename() << " initialized" << std::endl;
#endif
}

template<class SC_IF, class T>
inline sca_tdf_port_impl<SC_IF,T>::sca_tdf_port_impl(const char* name_) : base_type(name_),
                                   sca_tdf::sca_implementation::sca_port_attributes(this)
{
  construct();

#ifdef SCA_IMPLEMENTATION_DEBUG
  std::cout << "port " << name_ << " initialized" << std::endl;
#endif
}

template<class SC_IF, class T>
inline void sca_tdf_port_impl<SC_IF,T>::construct()
{
  sig=NULL;


  t0 = sca_core::sca_implementation::NOT_VALID_SCA_TIME();

  allow_processing_access_default=false;
  allow_processing_access_flag=&allow_processing_access_default;

  pmod=dynamic_cast<sca_core::sca_module*>(this->get_parent_object());
}

template<class SC_IF, class T>
inline const T& sca_tdf_port_impl<SC_IF,T>::read() const
{
	if(!(*allow_processing_access_flag))
	{
		if(!sca_ac_analysis::sca_ac_is_running())
		{
			std::ostringstream str;
			str << "Port: " << this->name() << " can't be read due it is not accessed "
		        << " from the context of the processing method of the parent sca_tdf::sca_module"
		        << " (see LRM clause 4.1.1.4.16)";
			SC_REPORT_ERROR("SystemC-AMS",str.str().c_str());
		}
	}

	if(*is_quick_access)
	{
		return *quick_value;
	}

  return sig->read(get_if_id());
}

template<class SC_IF, class T>
inline const T& sca_tdf_port_impl<SC_IF,T>::read(unsigned long sample) const
{
#ifndef SCA_DISABLE_PORT_ACCESS_CHECK
	if(!(*allow_processing_access_flag))
	{
		if(!sca_ac_analysis::sca_ac_is_running())
		{
			std::ostringstream str;
			str << "Port: " << this->name() << " can't be read due it is not accessed "
			    << " from the context of the processing method of the parent sca_tdf::sca_module"
			    << " (see LRM clause 4.1.1.4.16)";
			SC_REPORT_ERROR("SystemC-AMS",str.str().c_str());
		}
	}
#endif

	if((*is_quick_access)&&(sample==0))
	{
		return *quick_value;
	}

	return sig->read(get_if_id(),sample);

}



template<class SC_IF, class T>
inline const T& sca_tdf_port_impl<SC_IF,T>::read_delayed_value(unsigned long sample) const
{
	if(!(this->pmod->reinitialize_executes_flag))
	{
		if(!sca_ac_analysis::sca_ac_is_running())
		{
			std::ostringstream str;
			str << "Port: " << this->name() << " can't be read delayed values due it is not accessed "
			    << " from the context of the reinitialize method of the parent sca_tdf::sca_module"
			    << " (see LRM)";
			SC_REPORT_ERROR("SystemC-AMS",str.str().c_str());
		}
	}

	return sig->read_delayed_value(get_if_id(),sample);

}



template<class SC_IF, class T>
inline void sca_tdf_port_impl<SC_IF,T>::write(const T& value)
{
#ifndef SCA_DISABLE_PORT_ACCESS_CHECK
	if(!(*allow_processing_access_flag))
	{
		std::ostringstream str;
	    str << "Port: " << this->name() << " can't be written due it is not accessed "
		    << " from the context of the processing method of the parent sca_tdf::sca_module"
		    << " (see LRM clause 4.1.1.5.16)";
		SC_REPORT_ERROR("SystemC-AMS",str.str().c_str());
	}
#endif

	if(*is_quick_access)
	{
		*quick_value=value;
		return;
	}

	sig->write(get_if_id(),value);

}

template<class SC_IF, class T>
inline void sca_tdf_port_impl<SC_IF,T>::write(const T& value,unsigned long sample)
{

#ifndef SCA_DISABLE_PORT_ACCESS_CHECK
	if(!(*allow_processing_access_flag))
	{
		std::ostringstream str;
	    str << "Port: " << this->name() << " can't be written due it is not accessed "
		    << " from the context of the processing method of the parent sca_tdf::sca_module"
		    << " (see LRM clause 4.1.1.5.16)";
		SC_REPORT_ERROR("SystemC-AMS",str.str().c_str());
	}
#endif


	if((*is_quick_access)&&(sample==0))
	{
		*quick_value=value;
		return;
	}


	sig->write(get_if_id(),value,sample);

}


template<class SC_IF, class T>
void sca_tdf_port_impl<SC_IF,T>::initialize(const T& value, unsigned long sample_id)
{
	if((pmod==NULL) ||
			!( (pmod->is_initialize_executing()) || pmod->is_reinitialize_executing()  ))
	{
		std::ostringstream str;
	    str << "Port: " << this->name() << " can't be initialized due it is not accessed "
		    << " from the context of the initialize or reinitialize method of the parent sca_tdf::sca_module"
		    << " (see LRM)";
	    SC_REPORT_ERROR("SystemC-AMS",str.str().c_str());
	}

	sig->initialize(get_if_id(),value,sample_id);
}

template<class SC_IF, class T>
inline T& sca_tdf_port_impl<SC_IF,T>::get_ref_for_write(unsigned long sample) const
{
#ifndef SCA_DISABLE_PORT_ACCESS_CHECK
	if(!(*allow_processing_access_flag))
	{
		std::ostringstream str;
	    str << "Port: " << this->name() << " can't be written due it is not accessed "
		    << " from the context of the processing method of the parent sca_tdf::sca_module"
		    << " (see LRM clause 4.1.1.5.16)";
		SC_REPORT_ERROR("SystemC-AMS",str.str().c_str());
	}
#endif

	if(*is_quick_access)
	{
		return *quick_value;
	}


	return sig->get_ref_for_write(get_if_id(),sample);

}


template<class SC_IF, class T>
inline void sca_tdf_port_impl<SC_IF,T>::set_delay(long samples)
{
	sca_tdf::sca_implementation::sca_port_attributes::set_delay((unsigned long)samples);
}

template<class SC_IF, class T>
inline void sca_tdf_port_impl<SC_IF,T>::set_timestep(sca_core::sca_time t_period)
{
	sca_tdf::sca_implementation::sca_port_attributes::set_timestep(t_period);
}


template<class SC_IF, class T>
inline void sca_tdf_port_impl<SC_IF,T>::set_timestep(double dtime, sc_core::sc_time_unit unit)
{
	this->set_timestep(sca_core::sca_time(dtime,unit));
}




template<class SC_IF, class T>
inline void sca_tdf_port_impl<SC_IF,T>::set_max_timestep(sca_core::sca_time t_period)
{

	sca_tdf::sca_implementation::sca_port_attributes::set_max_timestep(t_period);
}


template<class SC_IF, class T>
inline void sca_tdf_port_impl<SC_IF,T>::set_max_timestep(double dtime, sc_core::sc_time_unit unit)
{
	this->set_max_timestep(sca_core::sca_time(dtime,unit));
}






//is called during end_of_elaboration however before set_attributes
template<class SC_IF, class T>
inline void sca_tdf_port_impl<SC_IF,T>::end_of_port_elaboration()
{
#ifdef SCA_IMPLEMENTATION_DEBUG
  std::cout << "end of port elaboration" << std::endl;
#endif

  sig=dynamic_cast<sca_tdf::sca_implementation::sca_tdf_signal_impl_if<T>*>(this->sca_get_interface());
  sig_base=dynamic_cast<sca_tdf::sca_implementation::sca_tdf_signal_impl_base*>(sig);
  port_id=get_if_id();


  if(sig_base==NULL)
  {
	  std::ostringstream str;
	  str << "No tdf signal bound to port: " << this->name();
	  SC_REPORT_ERROR("SystemC-AMS",str.str().c_str());
	  return;
  }

  this->signal_elaboration(sig_base);

  quick_value=this->sig->get_quick_value_ref();
  is_quick_access=this->sig_base->get_quick_signal_enable_ref();

}


template<class SC_IF, class T>
inline void sca_tdf_port_impl<SC_IF,T>::start_of_simulation()
{
	  if(pmod)
	  {
		  allow_processing_access_flag=pmod->get_allow_processing_access_flag_ref();
		  if(allow_processing_access_flag==NULL) allow_processing_access_flag=&allow_processing_access_default;
	  }

}

} //namespace sca_implementation
} //namespace sca_tdf


#endif /* SCA_TDF_PORT_IMPL_H_ */
