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

  sca_ct_delay_buffer.h - description

  Original Author: Karsten Einwich Fraunhofer IIS/EAS Dresden

  Created on: Feb 20, 2010

   SVN Version       :  $Revision: 1886 $
   SVN last checkin  :  $Date: 2016-01-08 14:25:12 +0000 (Fri, 08 Jan 2016) $
   SVN checkin by    :  $Author: karsten $
   SVN Id            :  $Id: sca_ct_delay_buffer.h 1886 2016-01-08 14:25:12Z karsten $

 *****************************************************************************/

/*****************************************************************************/


#ifndef SCA_CT_DELAY_BUFFER_H_
#define SCA_CT_DELAY_BUFFER_H_

#include "systemc-ams"
#include "sca_delay_buffer_base.h"

namespace sca_tdf
{
namespace sca_implementation
{

template<class T>
class sca_ct_delay_buffer : public sca_delay_buffer_base
{
public:
	void store_value(sca_core::sca_implementation::sca_signed_time vtime,const T& value);
	bool get_value(sca_core::sca_implementation::sca_signed_time vtime, T& value);

	bool get_next_value_after(sca_core::sca_implementation::sca_signed_time& atime, T& value,sca_core::sca_implementation::sca_signed_time ntime);

	sca_ct_delay_buffer();
	virtual ~sca_ct_delay_buffer();

private:

	T* value_buffer;

	void resize_value_buffer(unsigned long size);

};


//////////////////////////

template<>
class sca_ct_delay_buffer<sca_util::sca_vector<double> > : public sca_delay_buffer_base
{
public:
	void store_value(sca_core::sca_implementation::sca_signed_time vtime,const sca_util::sca_vector<double>& values);
	bool get_value(sca_core::sca_implementation::sca_signed_time vtime, sca_util::sca_vector<double>& values);

	bool get_next_value_after(sca_core::sca_implementation::sca_signed_time& atime, sca_util::sca_vector<double>& value,sca_core::sca_implementation::sca_signed_time ntime);

	sca_ct_delay_buffer();
	virtual ~sca_ct_delay_buffer();

private:

	sca_util::sca_vector<double>* value_buffer;

	void resize_value_buffer(unsigned long size);

};


/////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////

template<class T>
sca_ct_delay_buffer<T>::sca_ct_delay_buffer() : value_buffer(NULL)
{
	resize(2);
}

template<class T>
sca_ct_delay_buffer<T>::~sca_ct_delay_buffer()
{
	if(value_buffer!=NULL) delete[] value_buffer;
	value_buffer=NULL;
}

template<class T>
void  sca_ct_delay_buffer<T>::resize_value_buffer(unsigned long size)
{
	T* tmp_buf=new T[size];

	for(unsigned long i=0;(i<buffer_used)&&(i<size);i++)
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
void sca_ct_delay_buffer<T>::store_value(sca_core::sca_implementation::sca_signed_time vtime,const T& value)
{
	sca_delay_buffer_base::store_value(vtime);
	value_buffer[(buffer_write-1)%buffer_size]=value;
}

template<class T>
bool sca_ct_delay_buffer<T>::get_value(sca_core::sca_implementation::sca_signed_time vtime, T& value)
{
	int res=get_time_interval(vtime);

	if(res<=0) return true; //no time interval found

	if(res==2) //value at left side of interval
	{
		value = value_buffer[first_index%buffer_size];
		return false;
	}

	unsigned long first=first_index%buffer_size;
	unsigned long second=second_index%buffer_size;

	value=(value_buffer[second]-value_buffer[first]) /
	       (time_buffer[second]-time_buffer[first]).to_seconds() *
	       (vtime-time_buffer[first]).to_seconds() + value_buffer[first];

	return false;
}


template<class T>
bool sca_ct_delay_buffer<T>::get_next_value_after
		(sca_core::sca_implementation::sca_signed_time& atime, T& value,sca_core::sca_implementation::sca_signed_time ntime)
{
	if(get_next_time_after(ntime))
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
