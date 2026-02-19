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

  sca_dt_delay_buffer.h - description

  Original Author: Karsten Einwich Fraunhofer IIS/EAS Dresden

  Created on: Nov 14, 2011

   SVN Version       :  $Revision: 1659 $
   SVN last checkin  :  $Date: 2013-12-01 22:28:30 +0000 (Sun, 01 Dec 2013) $
   SVN checkin by    :  $Author: karsten $
   SVN Id            :  $Id: sca_dt_delay_buffer.h 1659 2013-12-01 22:28:30Z karsten $

 *****************************************************************************/

/*****************************************************************************/


#ifndef SCA_DT_DELAY_BUFFER_H_
#define SCA_DT_DELAY_BUFFER_H_


namespace sca_tdf
{
namespace sca_implementation
{

template<class T>
class sca_dt_delay_buffer : public sca_delay_buffer_base
{
public:
	void store_value(sca_core::sca_implementation::sca_signed_time vtime,const T& value);
	bool get_value(sca_core::sca_implementation::sca_signed_time vtime, T& value);

	bool get_next_value_after(sca_core::sca_implementation::sca_signed_time& atime,
			T& value,sca_core::sca_implementation::sca_signed_time ntime);


	sca_dt_delay_buffer();
	virtual ~sca_dt_delay_buffer();

private:

	T* value_buffer;

	void resize_value_buffer(unsigned long size);

};


//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

template<class T>
sca_dt_delay_buffer<T>::sca_dt_delay_buffer() : value_buffer(NULL)
{
	resize(2);
	store_value(sc_core::SC_ZERO_TIME,T()); //store initial value
}

template<class T>
sca_dt_delay_buffer<T>::~sca_dt_delay_buffer()
{
	if(value_buffer!=NULL) delete[] value_buffer;
	value_buffer=NULL;
}

template<class T>
void  sca_dt_delay_buffer<T>::resize_value_buffer(unsigned long size)
{
	T* tmp_buf=new T[size];

	for(unsigned long i=0;i<buffer_used;i++)
	{
		tmp_buf[i]=value_buffer[(buffer_first+i)%buffer_size];
	}

	for(unsigned long i=buffer_used;i<size;i++)
	{
		tmp_buf[i]=T();
	}

	if(value_buffer!=NULL) delete[] value_buffer;
	value_buffer=tmp_buf;
}


template<class T>
void sca_dt_delay_buffer<T>::store_value(sca_core::sca_implementation::sca_signed_time vtime,const T& value)
{
	sca_delay_buffer_base::store_value(vtime);
	value_buffer[(buffer_write-1)%buffer_size]=value;
}


template<class T>
bool sca_dt_delay_buffer<T>::get_value(sca_core::sca_implementation::sca_signed_time vtime, T& value)
{
	int res=get_time_interval(vtime);

	if(res<0) return true;

	value = value_buffer[first_index%buffer_size];

	return false;
}

template<class T>
bool sca_dt_delay_buffer<T>::get_next_value_after
		(sca_core::sca_implementation::sca_signed_time& atime, T& value,sca_core::sca_implementation::sca_signed_time ntime)
{
	if(this->get_next_time_after(ntime))
	{
		if(buffer_used>0)
		{
			//set to last time point in buffer
			atime=time_buffer[(buffer_write-1)%buffer_size];
			value=value_buffer[(buffer_write-1)%buffer_size];
		}
		else
		{
			atime=sca_core::sca_max_time();
		}

		return true;
	}

	atime=time_buffer[first_index%buffer_size];
	value=value_buffer[first_index%buffer_size];

	return false;
}


}
}


#endif /* SCA_CT_DELAY_BUFFER_H_ */
