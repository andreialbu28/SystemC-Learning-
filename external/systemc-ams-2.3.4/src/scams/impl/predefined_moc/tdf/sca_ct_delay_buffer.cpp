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

  sca_ct_delay_buffer.cpp - description

  Original Author: Karsten Einwich Fraunhofer IIS/EAS Dresden

  Created on: Feb 20, 2010

   SVN Version       :  $Revision: 1659 $
   SVN last checkin  :  $Date: 2013-12-01 22:28:30 +0000 (Sun, 01 Dec 2013) $
   SVN checkin by    :  $Author: karsten $
   SVN Id            :  $Id: sca_ct_delay_buffer.cpp 1659 2013-12-01 22:28:30Z karsten $

 *****************************************************************************/

/*****************************************************************************/

#include"sca_ct_delay_buffer.h"

namespace sca_tdf
{
namespace sca_implementation
{



sca_ct_delay_buffer<sca_util::sca_vector<double> >::sca_ct_delay_buffer() : value_buffer(NULL)
{
	resize(2);
}


sca_ct_delay_buffer<sca_util::sca_vector<double> >::~sca_ct_delay_buffer()
{
	if(value_buffer!=NULL) delete[] value_buffer;
	value_buffer=NULL;
}


void  sca_ct_delay_buffer<sca_util::sca_vector<double> >::resize_value_buffer(unsigned long size)
{
	sca_util::sca_vector<double>* tmp_buf=new sca_util::sca_vector<double>[size];

	for(unsigned long i=0;i<buffer_used;i++)
	{
		tmp_buf[i]=value_buffer[(buffer_first+i)%buffer_size];
	}


	if(value_buffer!=NULL) delete[] value_buffer;
	value_buffer=tmp_buf;
}



void sca_ct_delay_buffer<sca_util::sca_vector<double> >::store_value(sca_core::sca_implementation::sca_signed_time vtime,
		const sca_util::sca_vector<double>& values)
{
	sca_delay_buffer_base::store_value(vtime);
	//buffer_write was increased in base class store_value
	value_buffer[(buffer_write-1)%buffer_size]=values;
}


bool sca_ct_delay_buffer<sca_util::sca_vector<double> >::get_value(sca_core::sca_implementation::sca_signed_time vtime,
		sca_util::sca_vector<double>& values)
{
	int res=get_time_interval(vtime);

	if(res<=0) return true;

	if(res==2)
	{
		values = value_buffer[first_index%buffer_size];
		return false;
	}

	unsigned long first=first_index%buffer_size;
	unsigned long second=second_index%buffer_size;

	sca_util::sca_vector<double>& fv(value_buffer[first]);
	sca_util::sca_vector<double>& sv(value_buffer[second]);

	if(fv.length()!=sv.length())
	{
		SC_REPORT_ERROR("SystemC-AMS","Internal Error due a Bug");
	}

	values.resize(fv.length());
	for(unsigned long i=0;i<fv.length();i++)
	{
		values(i) = (sv(i)-fv(i)) /
		            (time_buffer[second]-time_buffer[first]).to_seconds() *
		            (vtime-time_buffer[first]).to_seconds() + fv(i);
	}

	return false;
}


bool sca_ct_delay_buffer<sca_util::sca_vector<double> >::get_next_value_after
		(sca_core::sca_implementation::sca_signed_time& atime, sca_util::sca_vector<double>& values,sca_core::sca_implementation::sca_signed_time ntime)
{
	if(get_next_time_after(ntime))
	{	//set to last element
		atime=time_buffer[first_index%buffer_size];
		values=value_buffer[first_index%buffer_size];

		return true;
	}

	atime=time_buffer[first_index%buffer_size];
	values=value_buffer[first_index%buffer_size];

	return false;
}



} //namespace sca_implementation
} //namespace sca_tdf
