/*****************************************************************************

    Copyright 2010
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

 sca_tdf_trace_variable.h - variable for tracing internal tdf states

 Original Author: Karsten Einwich Fraunhofer / COSEDA Technologies

 Created on: 04.03.2009

 SVN Version       :  $Revision: 2320 $
 SVN last checkin  :  $Date: 2023-06-08 12:47:38 +0000 (Thu, 08 Jun 2023) $ (UTC)
 SVN checkin by    :  $Author: pehrlich $
 SVN Id            :  $Id: sca_tdf_trace_variable.h 2320 2023-06-08 12:47:38Z pehrlich $

 *****************************************************************************/
/*
 LRM clause 4.1.1.9.
 The class sca_tdf::sca_trace_variable shall implement a variable, which can
 be traced in a trace file of class sca_util::sca_trace_file.
 */

/*****************************************************************************/

#ifndef SCA_TDF_TRACE_VARIABLE_H_
#define SCA_TDF_TRACE_VARIABLE_H_

namespace sca_tdf
{

//template<class T>
//class sca_trace_variable: protected sca_util::sca_traceable_object,
//public ::sc_core::sc_object
template<class T>
class sca_trace_variable: public sca_tdf::sca_implementation::sca_trace_variable_base
{
public:
	sca_trace_variable();
	~sca_trace_variable();

	explicit sca_trace_variable(const char* );

	virtual const char* kind() const;

	void write(const T& );
	const T& read() const;
	operator const T&() const;

	sca_tdf::sca_trace_variable<T>& operator=(const T& value);
	sca_tdf::sca_trace_variable<T>& operator=(const sca_tdf::sca_in<T>& port);
	sca_tdf::sca_trace_variable<T>& operator=(sca_tdf::sca_de::sca_in<T>& port);





	//optional methods for handling more than one sample per module call
	void set_rate(unsigned long rate);
	void set_timeoffset(const sca_core::sca_time& toffset);
	void set_timeoffset(double toffset, ::sc_core::sc_time_unit unit);

	void write(const T& value, unsigned long sample_id);
	sca_core::sca_assign_to_proxy<sca_tdf::sca_trace_variable<T>,T>&
	                                      operator[](unsigned long sample_id);

	/** method of interactive tracing interface, which returns the value
	 * at the current SystemC time (the time returned by sc_core::sc_time_stamp())
	 */
	const T& get_typed_trace_value() const;

	/** method of interactive tracing interface, which forces a value
	 */
    void force_typed_value(const T&);

	virtual void print( std::ostream& = std::cout ) const;
	virtual void dump( std::ostream& = std::cout ) const;


//Begin implementation specific

private:

	T* buffer; //due std::vector<bool> operator [] does not return a reference

	T last_value;
	T active_value;

    void resize_buffer(long n);
    void store_to_last(long n);
    void trace_last(long id,const sca_core::sca_time& ct,sca_util::sca_implementation::sca_trace_file_base& tf);
    void set_type_info(sca_util::sca_implementation::sca_trace_object_data& data);

	sca_core::sca_assign_to_proxy<sca_tdf::sca_trace_variable<T>,T> bracket_proxy;

	const std::string& get_current_trace_value(unsigned long sample) const;

	mutable std::string current_trace_value_string;

	//overloads virtual method of base class
	void set_force_value(const std::string&);


	T forced_value;

//End implementation specific

};

//Begin implementation specific

template<class T>
inline void sca_trace_variable<T>::print( std::ostream& str) const
{
	str << this->get_typed_trace_value();
}

template<class T>
inline void sca_trace_variable<T>::dump( std::ostream&  str) const
{
	str << this->kind() << " : " << this->name() << " value: "<< this->get_typed_trace_value();
}


template<class T>
inline void sca_trace_variable<T>::set_force_value(const std::string& stri)
{
	if(!sca_util::sca_implementation::convert_from_string(forced_value,stri))
	{
		std::ostringstream str;
		str << "Cannot convert string: " << stri << " to signal type of signal: ";
		str << this->name() << " in method force_value - use force_typed_value instead";
		SC_REPORT_WARNING("SystemC-AMS",str.str().c_str());
	}
}


template<class T>
inline void sca_trace_variable<T>::force_typed_value(const T& val)
{
	forced_value=val;

	this->force_forced_value();

}


template<class T>
inline const std::string& sca_trace_variable<T>::get_current_trace_value(unsigned long sample) const
{
	std::ostringstream str;

	if(this->force_value_flag)
	{
		str << this->forced_value;
	}
	else
	{
		if(!this->initialized)
		{
			str << this->active_value;
		}
		else
		{
			str << buffer[sample];
		}
	}
	current_trace_value_string=str.str();

	return current_trace_value_string;
}

template<class T>
inline const T& sca_trace_variable<T>::get_typed_trace_value() const
{
	if(this->force_value_flag)
	{
		return forced_value;
	}

	if(!this->initialized)
	{
		return this->active_value;
	}

	long idx=this->get_trace_value_index();

	return buffer[idx];
}


template<class T>
inline void sca_trace_variable<T>::set_type_info(sca_util::sca_implementation::sca_trace_object_data& data)
{
	data.set_type_info<T>();
}


template<class T>
inline void sca_trace_variable<T>::resize_buffer(long n)
{
	if(buffer==NULL)
	{
		buffer=new T[n];
		for(long i=0;i<n;++i) buffer[i]=active_value;

		time_buffer.resize(n);
		buffer_size=n;
		return;
	}

	if(buffer_size!=n)
	{
		T* nb=new T[n];
		for(long i=0;i<buffer_size;++i) nb[i]=buffer[i];
		for(long i=buffer_size;i<n;++i) nb[i]=active_value;
		delete[] buffer;
		buffer=nb;

		time_buffer.resize(n);
		buffer_size=n;
	}

}


template<class T>
inline void sca_trace_variable<T>::store_to_last(long n)
{
	last_value = buffer[n];
}


template<class T>
inline void sca_trace_variable<T>::trace_last(
		long id,const sca_core::sca_time& ct,sca_util::sca_implementation::sca_trace_file_base& tf)
{
	tf.store_time_stamp(id, ct, last_value);
}

template<class T>
inline sca_trace_variable<T>::sca_trace_variable():
	sca_tdf::sca_implementation::sca_trace_variable_base(
			sc_core::sc_gen_unique_name("sca_tdf_trace_variable"))
{
	this->buffer=NULL;
}

template<class T>
inline sca_trace_variable<T>::~sca_trace_variable()
{
	if(buffer!=NULL) delete[] buffer;
}

template<class T>
inline sca_trace_variable<T>::sca_trace_variable(const char* name_):
	sca_tdf::sca_implementation::sca_trace_variable_base(name_)
{
	this->buffer=NULL;
}

template<class T>
inline const char* sca_trace_variable<T>::kind() const
{
	return "sca_tdf::sca_trace_variable";
}

template<class T>
inline void sca_trace_variable<T>::set_rate(unsigned long rate_)
{
	rate=rate_;
}

template<class T>
inline void sca_trace_variable<T>::set_timeoffset(const sca_core::sca_time& toffset)
{
	timeoffset=toffset;
}

template<class T>
inline void sca_trace_variable<T>::set_timeoffset(double toffset, ::sc_core::sc_time_unit unit)
{
	timeoffset=sca_core::sca_time(toffset,unit);
}



template<class T>
inline sca_core::sca_assign_to_proxy<sca_tdf::sca_trace_variable<T>,T>&
                     sca_trace_variable<T>::operator[](unsigned long sample_id)
{
	  bracket_proxy.index=sample_id;
	  bracket_proxy.obj=this;

	  return bracket_proxy;
}


template<class T>
inline const T& sca_trace_variable<T>::read() const
{
	if(this->force_value_flag)
	{
		return forced_value;
	}

	return active_value;
}

template<class T>
inline sca_trace_variable<T>::operator const T&() const
{
	return this->read();
}

template<class T>
inline void sca_trace_variable<T>::write(const T& value, unsigned long sample_id)
{
	active_value=value;

	long index=this->get_write_index(sample_id);
	if(index<0) return;

	buffer[index]=value;
	active_value=value;
}

template<class T>
inline void sca_trace_variable<T>::write(const T& value)
{
	this->write(value,0);
}


template<class T>
inline sca_tdf::sca_trace_variable<T>& sca_trace_variable<T>::operator=(const T& value)
{
	write(value);
	return *this;
}

template<class T>
inline sca_tdf::sca_trace_variable<T>& sca_trace_variable<T>::operator=(const sca_tdf::sca_in<T>& port)
{
	write(port.read());
	return *this;
}


template<class T>
inline sca_tdf::sca_trace_variable<T>& sca_trace_variable<T>::operator=(
		sca_tdf::sca_de::sca_in<T>& port)
{
	write(port.read());
	return *this;
}

//End implementation specific


} // namespace sca_tdf

#endif /* SCA_TDF_TRACE_VARIABLE_H_ */
