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

  sca_delay_buffer_base.cpp - description

  Original Author: Karsten Einwich Fraunhofer IIS/EAS Dresden

  Created on: Nov 14, 2011

   SVN Version       :  $Revision: 1700 $
   SVN last checkin  :  $Date: 2014-04-22 09:19:48 +0000 (Tue, 22 Apr 2014) $
   SVN checkin by    :  $Author: karsten $
   SVN Id            :  $Id: sca_delay_buffer_base.cpp 1700 2014-04-22 09:19:48Z karsten $

 *****************************************************************************/

/*****************************************************************************/

#include "systemc-ams"
#include"sca_delay_buffer_base.h"

namespace sca_tdf
{
namespace sca_implementation
{


sca_delay_buffer_base::sca_delay_buffer_base() : time_buffer(NULL)
{
	buffer_size=0;
	buffer_used=0;
	buffer_write=0;
	buffer_first=0;
	first_index=0;
	second_index=1;
}

sca_delay_buffer_base::~sca_delay_buffer_base()
{
	if(time_buffer!=NULL) delete[] time_buffer;
	time_buffer=NULL;
}

void sca_delay_buffer_base::set_max_delay(const sca_core::sca_time& mtime)
{
	max_time=mtime;
}

const sca_core::sca_time& sca_delay_buffer_base::get_max_delay() const
{
	return max_time;
}

///////////////////

void sca_delay_buffer_base::set_time_reached(sca_core::sca_implementation::sca_signed_time rtime)
{
	time_reached=rtime;
}

///////////////////

void  sca_delay_buffer_base::resize(unsigned long size)
{
	sca_core::sca_implementation::sca_signed_time* tmp_buf=new sca_core::sca_implementation::sca_signed_time[size];

	for(unsigned long i=0;i<buffer_used;i++)
	{
		tmp_buf[i]=time_buffer[(buffer_first+i)%buffer_size];
	}

	resize_value_buffer(size);

	buffer_first=0;
	buffer_write=buffer_used;
	buffer_size=size;

	if(time_buffer!=NULL) delete[] time_buffer;

	time_buffer=tmp_buf;
}

/////////////////////////

void sca_delay_buffer_base::store_value(sca_core::sca_implementation::sca_signed_time vtime)
{
	if(buffer_size<=buffer_used)
	{
		//we remove samples older than max_time and older
		//than last_read_time - we assume, that the buffer is read continuously
		if(time_reached>max_time)
		{
			sca_core::sca_implementation::sca_signed_time delayed_time;
			if(last_read_time>time_reached-max_time)
			{
				delayed_time=time_reached-max_time;
			}
			else
			{
				delayed_time=last_read_time;
			}

			//the second sample must be less than delayed_time
			while(time_buffer[(buffer_first+1)%buffer_size]<delayed_time)
			{
				if(buffer_used<=2) break; //at least one sample must be left
				buffer_first++;           //remove obsolete sample
				buffer_used--;
			}
		}

		//if required resize buffer
		if(buffer_size<=buffer_used) resize(buffer_size+10);
	}

	//overwrite previous value at same time
	if((buffer_write<1) || (time_buffer[(buffer_write-1)%buffer_size]!=vtime))
	{
		//store value
		time_buffer[buffer_write%buffer_size]=vtime;
		buffer_write++;
		buffer_used++;
	}
}

////////////////////////////////


int sca_delay_buffer_base::get_time_interval(sca_core::sca_implementation::sca_signed_time vtime)
{
	//usually the next requested value is after the previous one
	// - if not reset index values
	if( (first_index<(buffer_write-buffer_used))  ||  //outside of avail. buffer
	    (time_buffer[first_index%buffer_size]>=vtime))
	{
		first_index=buffer_write-buffer_used; //index to first avail. value
		second_index=first_index+1;

		//error buffer has to short history - should not occur
		if(time_buffer[first_index%buffer_size]>vtime)
		{
			//for negative time use first available value
			if(vtime<sc_core::SC_ZERO_TIME)
			{
				second_index=first_index;
				return 2;  //no time intervall, however first value
			}

			return -1;
		}
	}

	//search time interval
	for(unsigned long i=second_index;i<buffer_write;i++)
	{
		second_index=i;
		first_index=i-1;

		if(time_buffer[i%buffer_size]>=vtime)
		{
			return 1;   //time interval found
		}
	}


	if(time_buffer[first_index%buffer_size]==vtime)
	{
		second_index=first_index;
		return 2;  //no time intervall, however first value
	}

	//in this case no second value available - second index is start
	//of the interval (thus second index is invalid)
	first_index=second_index;

	return 0; //error - time interval not found
}

bool  sca_delay_buffer_base::get_next_time_after(sca_core::sca_implementation::sca_signed_time ntime)
{

	last_read_time=ntime;

	//usually the next requested value is after the previous one
	// - if not reset index values
	if( (first_index<(buffer_write-buffer_used))  ||  //outside of avail. buffer
	    (time_buffer[first_index%buffer_size]>=ntime))
	{
		first_index=buffer_write-buffer_used; //index to first avail. value
		second_index=first_index+1;
	}

	//search time point
	for(unsigned long i=first_index;i<buffer_write;i++)
	{
		if(time_buffer[i%buffer_size]>ntime)
		{
			first_index=i;
			second_index=i+1;
			return false;
		}
	}

	//set to last element
	first_index=buffer_write-1;
	second_index=buffer_write-1;

	return true; //error - time interval not found
}


bool sca_delay_buffer_base::get_timepoint(sca_core::sca_implementation::sca_signed_time ntime)
{
	last_read_time=ntime;

	//usually the next requested value is after the previous one
	// - if not reset index values
	if( (first_index<(buffer_write-buffer_used))  ||  //outside of avail. buffer
	    (time_buffer[first_index%buffer_size]>=ntime))
	{
		first_index=buffer_write-buffer_used; //index to first avail. value
		second_index=first_index+1;
	}

	//search time point
	for(unsigned long i=first_index;i<buffer_write;i++)
	{
		if(time_buffer[i%buffer_size]==ntime)
		{
			first_index=i;
			second_index=i+1;
			return false;
		}
	}
	return true; //error - time interval not found
}

void sca_delay_buffer_base::remove_timestamps_after(sca_core::sca_implementation::sca_signed_time rtime)
{
	while(buffer_used>0)
	{
		unsigned long last_value=buffer_write-1;
		if(time_buffer[last_value%buffer_size]<=rtime) break;
		buffer_used--;
		buffer_write--;
	}
}

//////////////////////////////////////////
/////////////////////////////////////////


} //namespace sca_implementation
} //namespace sca_tdf
