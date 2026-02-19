/*****************************************************************************

    Copyright 2010-2013
    Fraunhofer-Gesellschaft zur Foerderung der angewandten Forschung e.V.

    Copyright 2015-2016
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

 sca_tdf_signal_impl.h - description

 Original Author: Karsten Einwich Fraunhofer IIS/EAS Dresden

 Created on: 06.08.2009

 SVN Version       :  $Revision: 2320 $
 SVN last checkin  :  $Date: 2023-06-08 12:47:38 +0000 (Thu, 08 Jun 2023) $
 SVN checkin by    :  $Author: pehrlich $
 SVN Id            :  $Id: sca_tdf_signal_impl.h 2320 2023-06-08 12:47:38Z pehrlich $

 *****************************************************************************/
/*
Buffer organization
            (ring buffer - size=max_indelay  + out_delay + sample_per_period):

     |   out delay sample    | samples per period | maximum indelay sample
    /|\                     /|\
     |                       |
  read position         write position
 (without delay)

  for (re-)initialization each inport has it's own indelay buffer

  port specific indelay buffer (size=delay[port]):

  idx      0             1           2            ...    delay-1
         z**-1         z**-2        z**-3               z**-delay

*/
/*****************************************************************************/

#ifndef SCA_TDF_SIGNAL_IMPL_H_
#define SCA_TDF_SIGNAL_IMPL_H_


namespace sca_tdf
{

namespace sca_implementation
{


template<class T>
class sca_tdf_signal_impl_if : public sca_tdf::sca_signal_if<T>
{
public:

	/** Port read for first sample */
	virtual const T& read(unsigned long port) const=0;

	/** Port read for an arbitrary sample (sample must be < rate) */
	virtual const T& read(unsigned long port, unsigned long sample) const=0;

	/** Port write to the first sample */
	virtual void write(unsigned long port,const T& value)=0;

	/** Port write for an an arbitrary sample (sample must be < rate) */
	virtual void write(unsigned long port,const T& value, unsigned long sample)=0;

	/** Port write for an an arbitrary sample (sample must be < rate) */
	virtual void initialize(unsigned long port,const T& value, unsigned long sample)=0;

	/** returns reference to a buffer element to write - for
	 * implementing the [] operator of an outport (sample must be < rate) */
	virtual T& get_ref_for_write(unsigned long port, unsigned long sample) const=0;

	virtual const char* kind() const=0;

	/** Gets the absolute number of samples (from simulation start) samples
	 of the current call are not included */
	virtual unsigned long get_sample_cnt(unsigned long port)=0;

	/** Gets the set sampling rate of the port */
	virtual unsigned long& get_rate(unsigned long port)=0;

	/** Gets the set sampling delay of the port */
	virtual unsigned long& get_delay(unsigned long port)=0;

	/** Gets the calculated sample time distance -> if the time was set it
	 must be equal to the calculated one - if not the scheduler will through
	 an exception */
	virtual sca_core::sca_time& get_timestep_calculated_ref(unsigned long port) const=0;

	virtual T* get_quick_value_ref()=0;


	virtual ~sca_tdf_signal_impl_if(){};
};

/**
 Timed dataflow signal. This channel can be used
 to connect SCA_TDF_MODULE's via sca_sca_tdf::sca_in/out ports
 (@ref #class sca_tdf::sca_in<T>). The channel implements
 the @ref #class sca_tdf::sca_interface<T> for communication
 and synchronization.
 */
template<class T>
class sca_tdf_signal_impl: public sca_tdf::sca_implementation::sca_tdf_signal_impl_if<T>,
	                       public sca_tdf::sca_implementation::sca_tdf_signal_impl_base
{
	typedef sca_tdf::sca_implementation::sca_tdf_signal_impl_base base_class;

public:

	/** Port read for first sample */
	const T& read(unsigned long port) const;

	/** Port read for an arbitrary sample (sample must be < rate) */
	const T& read(unsigned long port, unsigned long sample) const;

	/** read delayed value in reinitialize */
	T& read_delayed_value(unsigned long port,unsigned long sample) const;

	/** Port write to the first sample */
	void write(unsigned long port,const T& value);

	/** Port write for an an arbitrary sample (sample must be < rate) */
	void write(unsigned long port,const T& value, unsigned long sample);

	/** Port initialize for an an arbitrary sample (sample must be < delay) */
	void initialize(unsigned long port,const T& value, unsigned long sample);

	/** returns reference to a buffer element to write - for
	 * implementing the [] operator of an outport (sample must be < rate) */
	T& get_ref_for_write(unsigned long port, unsigned long sample) const;

	const char* kind() const;


	/** Gets the absolute number of samples (from simulation start) samples
	 of the current call are not included */
	unsigned long get_sample_cnt(unsigned long port)
	{ return base_class::get_sample_cnt(port);}

	/** Gets the set sampling rate of the port */
	unsigned long& get_rate(unsigned long port)
	{ return base_class::get_rate(port);}


	/** Gets the set sampling delay of the port */
	unsigned long& get_delay(unsigned long port)
	{ return base_class::get_delay(port);}

	/** Gets the calculated sample time distance -> if the time was setted it
	 must be equal to the calculated one - if not the scheduler will through
	 an exception */
	sca_core::sca_time& get_timestep_calculated_ref(unsigned long port) const
	{ return base_class::get_timestep_calculated_ref(port);}


	/** method of interactive tracing interface, which returns the value
	 * at the current SystemC time (the time returned by sc_core::sc_time_stamp())
	 */
	const T& get_typed_trace_value() const;

	/** method of interactive tracing interface, which forces a value
	 */
    void force_typed_value(const T&);


	sca_tdf_signal_impl();
	sca_tdf_signal_impl(const char* name_);

	virtual ~sca_tdf_signal_impl();

	/**
	 * experimental physical domain mehtods
	 */
	virtual void set_unit(const std::string& unit);
	virtual const std::string& get_unit() const;

	virtual void set_unit_prefix(const std::string& prefix);
	virtual const std::string& get_unit_prefix() const;

	virtual void set_domain(const std::string& domain);
	virtual const std::string& get_domain() const;

	T* get_quick_value_ref() {return &quick_value;}

private:

	T* buffer;
	std::vector<T*> in_delay_buffer;

	void create_buffer(unsigned long size);
    void store_trace_buffer_time_stamp(sca_util::sca_implementation::sca_trace_file_base& tf,unsigned long id,sca_core::sca_time ctime, unsigned long bpos);

	void nr_in_delay_buffer_resize(unsigned long n);
	void in_delay_buffer_create(unsigned long b_nr, unsigned long size);

	void set_type_info(sca_util::sca_implementation::sca_trace_object_data& data);

	/*resizes buffer after rate or delay change*/
	void resize_buffer();

	//re-creates local indelay buffer, copies current values to this buffers
	void reinitialize_indelays();

	const std::string& get_current_trace_value(unsigned long sample) const;

	mutable T current_trace_value;
	mutable std::string current_trace_value_string;

	//overloads virtual method of base class
	void set_force_value(const std::string&);


	T forced_value;

	mutable T quick_value;

	std::string unit;
	std::string unit_prefix;
	std::string domain;

};

/////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////



template<class T>
void sca_tdf_signal_impl<T>::set_force_value(const std::string& stri)
{
	if(!sca_util::sca_implementation::convert_from_string(forced_value,stri))
	{
		std::ostringstream str;
		str << "Cannot convert string: " << stri << " to signal type of signal: ";
		str << this->name() << " in method force_value - use force_typed_value instead";
		SC_REPORT_WARNING("SystemC-AMS",str.str().c_str());
	}

	this->force_forced_value();
}

template<class T>
void sca_tdf_signal_impl<T>::force_typed_value(const T& val)
{
	forced_value=val;

	this->force_forced_value();

}

/////////////////////////////////////////////////////////////

template<class T>
const std::string& sca_tdf_signal_impl<T>::get_current_trace_value(unsigned long sample) const
{

	if(this->force_value_flag)
	{
		current_trace_value=this->forced_value;
	}
	else
	{
		current_trace_value=buffer[sample];
	}

	std::ostringstream str;
	str << current_trace_value;
	current_trace_value_string=str.str();

	return current_trace_value_string;
}


template<class T>
const T& sca_tdf_signal_impl<T>::get_typed_trace_value() const
{
	if(this->force_value_flag)
	{
		return this->forced_value;
	}


	sc_dt::int64 nsample=this->get_number_of_trace_value_sample();

	if(nsample<0)
	{
		current_trace_value=T();
		return this->current_trace_value;
	}

	current_trace_value=buffer[nsample];

	return current_trace_value;
}


template<class T>
void sca_tdf_signal_impl<T>::set_type_info(sca_util::sca_implementation::sca_trace_object_data& data)
{
	data.set_type_info<T>();
}


template<class T>
inline void sca_tdf_signal_impl<T>::create_buffer(unsigned long size)
{
	if(size==1)
	{
		buffer=&quick_value;
	}
	else
	{
		buffer = new T[size];
	}

	for (unsigned long i = 0; i < size; ++i)
		buffer[i] = T();
}



template<class T>
inline void sca_tdf_signal_impl<T>::reinitialize_indelays()
{
	long nports = get_number_of_connected_ports();

	for(long cport=0;cport<nports;++cport)
	{
		if(cport!=driver_port) //inports only considered
		{
			if(*delays[cport]!=0)
			{
				T* new_in_delay_buffer=new T[*delays[cport]];
				for(unsigned long i=0;i<*delays[cport];i++) new_in_delay_buffer[i]=T();

				//copy old delay values to in_buffer, to be able to
				//change them in reinitialize
				unsigned long snidx, soidx, dnum;
				if(*delays[cport] > *delays_old[cport])
				{
					//buffer enlarged -> first sample initial samples
					snidx=*delays[cport] - *delays_old[cport];
					soidx=0;
					dnum=*delays_old[cport];
				}
				else
				{
					//buffer reduced -> first sample removed
					snidx=0;
					soidx=*delays_old[cport] - *delays[cport];
					dnum=*delays[cport];
				}

				for(unsigned long j=0;j<dnum;++j,++snidx,++soidx)
				{
					sc_dt::int64 call_cnt=*(call_counter_refs[cport]);

					//read old value
					//next position + future values
					sc_dt::int64 old_pos = (sc_dt::int64) (call_cnt * *rates_old[cport]
					                           +  buffer_offsets[cport]+ soidx);

					T old_value;
					if (old_pos < indelay_start_sample)
					{
						old_value=(in_delay_buffer[cport])[old_pos -indelay_start_sample -buffer_offsets[cport] ];
					}
					else
					{
						old_value=(buffer[((sc_dt::uint64) old_pos) % buffer_size]);
					}

					//we start a new start phase
					new_in_delay_buffer[snidx]=old_value;
				}


				//write old value to new buffer
				buffer_offsets[cport]=-*delays[cport];

				if(in_delay_buffer[cport]!=NULL) delete [] in_delay_buffer[cport];
			    in_delay_buffer[cport]=new_in_delay_buffer;
			}
			else //new in delay is 0
			{
				if( in_delay_buffer[cport]!=NULL)
				{
					delete [] in_delay_buffer[cport];
					in_delay_buffer[cport]=NULL;
				}
				buffer_offsets[cport]=0;
			}
		}
	}
}


template<class T>
inline void sca_tdf_signal_impl<T>::resize_buffer()
{
	unsigned long nports = get_number_of_connected_ports();
	unsigned long old_out_delay=*delays_old[driver_port];
	unsigned long new_out_delay=*delays[driver_port];
	unsigned long new_max_in_delay=0;
	bool delay_change=false;

	sca_core::sca_implementation::sca_port_base**  plist  = get_connected_port_list();


	//check for delay change
	for(unsigned long i=0;i<nports;++i)
	{
		sca_tdf::sca_implementation::sca_port_attributes* attr;
		attr=dynamic_cast<sca_tdf::sca_implementation::sca_port_attributes*>(plist[i]);
		if(attr!=NULL)
		{
			attr->update_last_values();
		}

		if(*delays[i]!= *delays_old[i])
		{
			delay_change=true;

		}

		if(long(i)!=driver_port)
		{
			if(*delays[i]>new_max_in_delay) new_max_in_delay=*delays[i];
		}
	}


	//calculate new buffer size
	unsigned long new_buffer_size=get_samples_per_period()+
			new_max_in_delay+new_out_delay;


	//nothing to do
	if( (new_buffer_size==buffer_size) && !delay_change)
	{
		indelay_start_sample=get_samples_per_period();
		return;
	}


	//if no delay available simply resize the buffer
	if( (new_max_in_delay==0) && (new_out_delay==0) )
	{
		indelay_start_sample=get_samples_per_period();

		if(delay_change)
		{
			for(unsigned long cport=0;cport<nports;cport++)
			{
				if(in_delay_buffer[cport]!=NULL)
				{
					delete [] in_delay_buffer[cport];
					in_delay_buffer[cport]=NULL;
				}
				buffer_offsets[cport]=0;
			}
		}

		//nothing to do
		if(new_buffer_size==buffer_size) return;

		if(buffer!=&quick_value)
		{
			delete [] buffer;
		}
		buffer=NULL;

		if(new_buffer_size==1)
		{
			buffer=&quick_value;
		}
		else
		{
			buffer = new T[new_buffer_size];
		}

		buffer_size=new_buffer_size;

		return;
	}



	//create and initialize new buffer

	T* new_buffer;
	if(buffer_size==1)
	{
		new_buffer=&quick_value;
	}
	else
	{
		new_buffer=new T[new_buffer_size];
	}

	for (unsigned long i = 0; i < new_buffer_size; ++i)  new_buffer[i] = T();


	//copy out delay values to the new buffer at the new position
	if(*delays[driver_port]!=0)
	{

		unsigned long snidx, soidx, dnum;
		if(new_out_delay > old_out_delay)
		{
			//buffer enlarged -> first sample initial samples
			snidx=new_out_delay - old_out_delay;
			soidx=0;
			dnum=old_out_delay;
		}
		else
		{
			//buffer reduced -> first sample removed
			snidx=0;
			soidx=old_out_delay - new_out_delay;
			dnum=new_out_delay;
		}


		//copy output delay values to new buffer to the correct position
		for(unsigned long i=0;i<dnum;++i,++soidx,++snidx)
		{
			//call counter points to next call
			sc_dt::int64 ccnt = *(call_counter_refs[driver_port]);


			sc_dt::int64 old_pos = (sc_dt::int64) (ccnt * *rates_old[driver_port]
					) + soidx;


			//we set the call counter after resize to the end of the first
			//cluster execution (see sca_synchronization_alg.cpp)
			sc_dt::int64 new_pos = (sc_dt::int64) (get_samples_per_period()
					) + snidx;

			new_buffer[new_pos%new_buffer_size] = buffer[old_pos%buffer_size];
		}

		buffer_offsets[driver_port]=*delays[driver_port];
	}
	else
	{
		buffer_offsets[driver_port]=0;
	}


	//copy all inport delays to the new buffer
	reinitialize_indelays();
	indelay_start_sample=get_samples_per_period();

	if((buffer!=NULL)&&(buffer!=&quick_value)) delete [] buffer;

	buffer=new_buffer;
	buffer_size=new_buffer_size;

}



template<class T>
inline void sca_tdf_signal_impl<T>::store_trace_buffer_time_stamp(
		sca_util::sca_implementation::sca_trace_file_base& tf,unsigned long id,sca_core::sca_time ctime, unsigned long bpos)
{
	if(this->force_value_flag)
	{
		tf.store_time_stamp(id,ctime,this->forced_value);
	}
	else
	{
		tf.store_time_stamp(id,ctime,buffer[bpos]);
	}
}



template<class T>
inline void sca_tdf_signal_impl<T>::nr_in_delay_buffer_resize(unsigned long n)
{
	in_delay_buffer.resize(n, NULL);
}

template<class T>
inline void sca_tdf_signal_impl<T>::in_delay_buffer_create(unsigned long b_nr,
		unsigned long size)
{
	in_delay_buffer[b_nr] = new T[size];
}

/** Port read for first sample */
template<class T>
inline const T& sca_tdf_signal_impl<T>::read(unsigned long port) const
{
	//optimization for buffer_size=1 -> usual case especially for single rate
	if(this->quick_access_enabled)
	{
		return quick_value;
	}

	if(this->force_value_flag) return forced_value;


	sc_dt::int64 read_pos = (sc_dt::int64) (*(call_counter_refs[port]) * *rates[port])
			+  buffer_offsets[port];  // - delay[port]

	//start phase with separate indelay buffer
	if(read_pos<indelay_start_sample)
	{
		return (in_delay_buffer[port])[read_pos-indelay_start_sample -buffer_offsets[port] ];
	}
	else
	{
		if(this->quick_access_possible)  //after delay values may switch to quick access
		{
			if(!this->signal_traced)
			{
				//in the case of a delay we switch after the first cluster period to be consistent
				if(read_pos>=sc_dt::int64(buffer_size))
				{
					quick_value=buffer[read_pos% buffer_size];
					this->quick_access_enabled=true;
				}
			}
		}
	}

	return buffer[read_pos% buffer_size];
}

/** Port read for an arbitrary sample (sample must be < rate) */
template<class T>
inline const T& sca_tdf_signal_impl<T>::read(unsigned long port,
		unsigned long sample) const
{

	//optimization for buffer_size=1 -> usual case especially for single rate
	if((this->quick_access_enabled)&&(sample==0))
	{
		return quick_value;
	}
	else
	{
		if(this->force_value_flag) return forced_value;

		if (sample >= *rates[port])
		{
			std::ostringstream str;
			str << "Access to sample >= rate not allowed for port: "
				<< get_connected_port_list()[port]->sca_name() << std::endl;
			SC_REPORT_ERROR("SystemC-AMS", str.str().c_str());
		}


		sc_dt::int64 read_pos = (sc_dt::int64) (*(call_counter_refs[port]) * *rates[port]
	                                   + sample +  buffer_offsets[port] );

		//start phase with separate indelay buffer
		if(read_pos<indelay_start_sample)
		{
			if(in_delay_buffer[port]!=NULL)
			{
				return (in_delay_buffer[port])[read_pos-indelay_start_sample -buffer_offsets[port] ];
			}
		}
		else
		{
			if(this->quick_access_possible)  //after delay values may switch to quick access
			{
				if(!this->signal_traced)
				{
					//in the case of a delay we switch after the first cluster period to be consistent
					if(read_pos>=sc_dt::int64(buffer_size))
					{
						quick_value=buffer[read_pos% buffer_size];
						this->quick_access_enabled=true;
					}
				}
			}
		}

		return buffer[read_pos% buffer_size];
	}
}



/** Port read for an arbitrary sample (sample must be < rate) */
template<class T>
inline T& sca_tdf_signal_impl<T>::read_delayed_value(unsigned long port,
		unsigned long sample) const
{
	if (sample > *delays[port])
	{
		std::ostringstream str;
		str << "read delayed value to sample > delay not allowed for port: "
				<< get_connected_port_list()[port]->sca_name() << std::endl;
		SC_REPORT_ERROR("SystemC-AMS", str.str().c_str());
	}


	sc_dt::int64 call_cnt=*(call_counter_refs[port]);
	if(call_cnt<0) //in initialize
	{
		if (in_delay_buffer[port] == NULL)
		{
			return buffer[sample];
		}
		else
		{
			return (in_delay_buffer[port])[sample];
		}
	}


	if(long(port)==driver_port)
	{
		//a sample_id of zero shall refer to the first delayed sample in time (LRM)
		// -> next read position + sample
		sc_dt::int64 read_pos = (sc_dt::int64) (call_cnt * *rates[port]
		                            +  sample);

		return buffer[read_pos%buffer_size];

	}
	else
	{
		//a sample_id of zero shall refer to the first delayed sample in time (LRM)
		//next read position -sample
		sc_dt::int64 read_pos = (sc_dt::int64) (*(call_counter_refs[port]) * *rates[port]
			                                   +  buffer_offsets[port] -sample);

		//start phase with separate indelay buffer
		if(read_pos<indelay_start_sample)
		{
			//the in delay buffer is organized opposite idx: 0 z**-1, idx:1 z**-2
			return (in_delay_buffer[port])[read_pos-indelay_start_sample -buffer_offsets[port]
			                                 + 2*sample];
		}

		return buffer[read_pos% buffer_size];

	}

	return buffer[0];  //never reached
}

/** Port write to the first sample */
template<class T>
inline void sca_tdf_signal_impl<T>::write(unsigned long port,const T& value)
{
	//		cout << "Write at pos.: " <<  (unsigned long long) ( *(call_counter_refs[port]) * rates[port]
	//		                + buffer_offsets[port] ) % buffer_size << " value: " << value << endl;

	//optimization for buffer_size=1 -> usual case especially for single rate
	if(this->quick_access_enabled)
	{
		quick_value=value;
	}
	else
	{

		sc_dt::int64 call_cnt = *(call_counter_refs[port]);

		buffer[(sc_dt::uint64) (call_cnt * *rates[port] + buffer_offsets[port])
			   	   	   	   	   	   	   % buffer_size] = value;
	}

}



/** returns reference to a buffer element to write - for
 * implementing the [] operator of an outport (sample must be < rate) */
template<class T>
inline T& sca_tdf_signal_impl<T>::get_ref_for_write(unsigned long port,
		unsigned long sample) const
{
	//optimization for buffer_size=1 -> usual case especially for single rate
	if((this->quick_access_enabled)&&(sample==0))
	{
		return quick_value;
	}
	else
	{

		sc_dt::int64 call_cnt = *(call_counter_refs[port]);


		if (sample >= *rates[port])
		{
			std::ostringstream str;
			str << "access to sample >= rate not allowed for port: "
				<< get_connected_port_list()[port]->sca_name() << std::endl;
			SC_REPORT_ERROR("SystemC-AMS", str.str().c_str());
		}

		return buffer[(sc_dt::uint64) (call_cnt * *rates[port] + buffer_offsets[port]
													+ sample) % buffer_size];
	}
}




/** Port write for an an arbitrary sample (sample must be < rate) */
template<class T>
inline void sca_tdf_signal_impl<T>::write(unsigned long port,const T& value,
		unsigned long sample)
{

	//optimization for buffer_size=1 -> usual case especially for single rate
	if((this->quick_access_enabled)&&(sample==0))
	{
		quick_value=value;
		//*buffer=value;
	}
	else
	{
		get_ref_for_write(port,sample)=value;
	}
}


/** Port write for an an arbitrary sample (sample must be < rate) */
template<class T>
inline void sca_tdf_signal_impl<T>::initialize(unsigned long port,const T& value,
		unsigned long sample)
{
	if (sample >= *delays[port])
	{
		std::ostringstream str;
		str << " Sample number(" << sample << ")"
				<< " for port initialize must be smaller "
				<< "than the specified delay" << " value (" << *delays[port]
				<< ") for port: ";
		str << get_connected_port_list()[port]->sca_name();
		str << std::endl << std::endl;
		SC_REPORT_ERROR("SystemC-AMS", str.str().c_str());
	}

	if(long(port)==driver_port)
	{
		read_delayed_value(port,sample)=value;
		return;
	}

	sc_dt::int64 call_cnt=*(call_counter_refs[port]);
	if(call_cnt<0) //in initialize
	{
		if (in_delay_buffer[port] == NULL)
		{
			std::ostringstream str;
			str << " Internal not possible Error -";
			str << " initialize of a not available indelay buffer in initialize for port: ";
			str << get_connected_port_list()[port]->sca_name();
			str << std::endl << std::endl;
			SC_REPORT_ERROR("SystemC-AMS", str.str().c_str());
		}
		else
		{
			(in_delay_buffer[port])[sample]=value;
		}
	}
	else
	{
		sc_dt::int64 idstart=call_cnt/(*calls_per_period[port])*get_samples_per_period();

		if(indelay_start_sample<idstart)
		{
			unsigned long nports=get_number_of_connected_ports();
			sca_core::sca_implementation::sca_port_base**  plist  = get_connected_port_list();


			//check for delay change
			for(unsigned long i=0;i<nports;++i)
			{
				sca_tdf::sca_implementation::sca_port_attributes* attr;
				attr=dynamic_cast<sca_tdf::sca_implementation::sca_port_attributes*>(plist[i]);
				if(attr!=NULL)
				{
					attr->update_last_values();
				}

			}

			//reinitialize indelays to recreate local buffers
			reinitialize_indelays();
			indelay_start_sample=idstart;
		}
	}

	read_delayed_value(port,sample)=value;

	return;
}



template<class T>
inline const char* sca_tdf_signal_impl<T>::kind() const
{
	return "sca_sdf_signal";
}

template<class T>
inline sca_tdf_signal_impl<T>::sca_tdf_signal_impl()
{
	buffer = NULL;
}


template<class T>
inline sca_tdf_signal_impl<T>::sca_tdf_signal_impl(const char* name_):
	sca_tdf::sca_implementation::sca_tdf_signal_impl_base(name_)
{
	buffer = NULL;
}


template<class T>
inline sca_tdf_signal_impl<T>::~sca_tdf_signal_impl()
{

	if(buffer!=&quick_value)
	{
		delete[] buffer;
	}

	for (typename std::vector<T*>::iterator it = in_delay_buffer.begin(); it
			!= in_delay_buffer.end(); ++it)
	{
		if( *it != NULL) delete[] *it;
	}
}

template<class T>
inline void sca_tdf_signal_impl<T>::set_unit(const std::string& unit_)
{
	unit=unit_;
}

template<class T>
inline const std::string& sca_tdf_signal_impl<T>::get_unit() const
{
	return unit;
}

template<class T>
inline void sca_tdf_signal_impl<T>::set_unit_prefix(const std::string& prefix_)
{
	unit_prefix=prefix_;
}

template<class T>
inline const std::string& sca_tdf_signal_impl<T>::get_unit_prefix() const
{
	return unit_prefix;
}

template<class T>
inline void sca_tdf_signal_impl<T>::set_domain(const std::string& domain_)
{
	domain=domain_;
}

template<class T>
inline const std::string& sca_tdf_signal_impl<T>::get_domain() const
{
	return domain;
}


} //namespace sca_implentation
} //namespace sca_tdf


#endif /* SCA_TDF_SIGNAL_IMPL_H_ */
