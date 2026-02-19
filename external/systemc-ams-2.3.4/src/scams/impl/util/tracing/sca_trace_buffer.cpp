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

 sca_trace_buffer.cpp - description

 Original Author: Karsten Einwich Fraunhofer IIS/EAS Dresden

 Created on: 13.11.2009

 SVN Version       :  $Revision: 2320 $
 SVN last checkin  :  $Date: 2023-06-08 12:47:38 +0000 (Thu, 08 Jun 2023) $
 SVN checkin by    :  $Author: pehrlich $
 SVN Id            :  $Id: sca_trace_buffer.cpp 2320 2023-06-08 12:47:38Z pehrlich $

 *****************************************************************************/

/*****************************************************************************/

#include <systemc-ams>
#include "scams/impl/util/tracing/sca_trace_value_handler.h"
#include "scams/impl/util/tracing/sca_trace_buffer.h"

namespace sca_util
{
namespace sca_implementation
{

///////////////////////////////////////////////////////////////////////////////

sca_trace_buffer::sca_trace_buffer()
{
	number_of_traces = 0;
	disabled = false;
	finish = false;
    no_interpolation=NULL;

    init_value_cnt=0;
}

sca_trace_buffer::~sca_trace_buffer()
{
  number_of_traces = 0;
  if(no_interpolation!=NULL) delete[] no_interpolation;
}

///////////////////////////////////////////////////////////////////////////////

void sca_trace_buffer::store_abstract_time_stamp(sca_trace_value_handler_base& value)
{
	const std::int64_t& id=value.id;

	if(value.systemc_time>this->current_systemc_time) this->current_systemc_time=value.systemc_time;

	//store first value as last value
	if (init_values[id] == NULL)
	{
		init_values[id] = value.duplicate();
		init_values[id]->next_value=&value;
		init_values[id]->next_time=value.this_time;
		init_values[id]->this_time=sc_core::SC_ZERO_TIME;

		init_value_cnt++;
	}



	//if not yet stored other values at this time create vector and/otherwise
	//return a reference to the value handle vector
	value_buffer* ele = &(trace_buffer[value.this_time]);
	ele->last_values = &last_values;


	//if vector created resize and initialize with NULL
	ele->resize(number_of_traces);
	//store value in vector


	//if there a yet stored time point delete
	if ((*ele)[id] != NULL)
	{
		(*ele)[id]->swap_values_base(&value); //overwrite value
		value.free();
		return;
	}
	else
	{
		(*ele)[id] = &value;
	}



	//prepare datastructure for interpolation
	if (last_stored_values[id] != NULL)
	{
		last_stored_values[id]->next_value = &value;
		last_stored_values[id]->next_time = value.this_time;
	}

	last_stored_values[id] = &value;
}

///////////////////////////////////////////////////////////////////////////////

long sca_trace_buffer::register_trace()
{
	number_of_traces++;
	last_values.resize(number_of_traces);
	last_stored_values.resize(number_of_traces);
	init_values.resize(number_of_traces);

	return number_of_traces - 1;
}

///////////////////////////////////////////////////////////////////////////////
//
// this function reads the next time stamp - if true return no further time stamp can be read
// if values==NULL no time stamp could be read
// for signals which has no value at the current time point, the value vector contains a NULL
// ctime returns the time of the current time stamp
//
bool sca_trace_buffer::read_next_stamp(sca_core::sca_time& ctime,value_buffer*& values)
{
	std::map<sca_core::sca_time, value_buffer>::iterator buf_it;
	buf_it = trace_buffer.begin();

	values = NULL;

	if (buf_it == trace_buffer.end())
		return true;

	ctime = buf_it->first;

	//limit to current SystemC time
	if (finish)
	{
		if (ctime > this->current_systemc_time)
			return true;
	}
	else
	{
		if (ctime >= this->current_systemc_time)
			return true;
	}

	//delete old value handles
	for (unsigned long i = 0; i < number_of_traces; ++i)
	{

		if (last_values[i] != NULL) //in the case there was a last value, we can delete this value
		{
			//we used an non interpolation mode -> thus it was not required
			//to wait until the next value is available
			if(last_values[i]->next_value==NULL)
			{
				last_stored_values[i]=NULL;
			}
			last_values[i]->free();
			last_values[i] = NULL;
		}

		last_values[i] = buf_it->second[i];
	}

	//erase read time point
	trace_buffer.erase(buf_it);

	values = &last_values;

	if (trace_buffer.size() <= 0)  //no further time stamp available
	{
		return true;
	}

	return false;
}

///////////////////////////////////////////////////////////////////////////////
//method used if trace is in sample mode
bool sca_trace_buffer::read_sample_interpolate(const sca_core::sca_time& ntime,value_buffer*& values)
{
	values = NULL;

	//limit to current SystemC time
	if (finish)
	{
		if (ntime > this->current_systemc_time)
			return true;
	}
	else
	{
		if (ntime >= this->current_systemc_time)
			return true;
	}

	std::map<sca_core::sca_time, value_buffer>::iterator buf_it;
	buf_it = trace_buffer.begin();

	//assert: top_ntime is still in trace_buffer
	if (buf_it == trace_buffer.end())
		SC_REPORT_ERROR("SystemC-AMS", "internal error in sca_trace_buffer");

	sca_core::sca_time next_buffer_time = buf_it->first;

	//read time stamps until sample time point
	while (next_buffer_time < ntime)
	{
		bool stop_reading = false;
		for (unsigned long i = 0; i < number_of_traces; ++i)
		{
			if (buf_it->second[i] != NULL)
			{
				if (last_values[i] != NULL)
				{
					last_values[i]->free();
					last_values[i] = NULL;
				}
				last_values[i] = buf_it->second[i];
			}

			//stop if no interpolation possible
			if ((last_values[i] == NULL)
					|| (last_values[i]->next_value == NULL))
			{
				if ((no_interpolation != NULL) && !no_interpolation[i])
				{
					stop_reading = true;
				}
			}

		}

		//erase read time point
		trace_buffer.erase(buf_it);

		buf_it = trace_buffer.begin();

		//if return true -> no timestamp available
		if (stop_reading)
			return true;
		if (buf_it == trace_buffer.end())
			return true; //stop at buffer end

		next_buffer_time = buf_it->first;
	}

	//we found the next timestamp and we can interpolate
	for (unsigned long i = 0; i < number_of_traces; ++i)
	{
		if (last_values[i] != NULL)
		{
			sca_trace_value_handler_base* val;

			//generation by interpolation or holding
			if ((no_interpolation != NULL) && no_interpolation[i])
			{
				val = &(last_values[i]->hold(ntime));
			}
			else
			{
				val = &(last_values[i]->interpolate(ntime));
			}

			last_values[i]->free();
			last_values[i] = val;
		}
		else
		{
			if (init_values[i] == NULL)
			{
				std::ostringstream str;
				str << "Internal error - every trace must write at least "
						<< "one value before writing to file " << __FILE__
						<< " line: " << __LINE__ << std::endl;
				SC_REPORT_ERROR("SystemC-AMS",str.str().c_str());
			}
			//first value is propagated backward
			last_values[i] = init_values[i];
		}
	}

	values = &last_values;
	return false;
}

///////////////////////////////////////////////////////////////////////////////
//
//  this function returns true if no further timestamp can be read
//  the values for the read time stamp returned into the values pointer
//  if the values pointer is NULL no value could be read
//  the values vector contains for all signals a value at the current time stamp
//  if there was no value available, a value is added by interpolation or hold
//  ntime returns the time of the time stamp
//
bool sca_trace_buffer::read_next_stamp_interpolate(sca_core::sca_time& ntime,
		value_buffer*& values, bool hold_sample)
{

	if(init_value_cnt<number_of_traces)
	{
		//not yet all init values available - can't create values for all traces
		values=NULL;
		return true;
	}


	//next to read sample must be always the first
	std::map<sca_core::sca_time, value_buffer>::iterator buf_it;
	buf_it = trace_buffer.begin();

	//if no time stamp than there are no traces added
	if (buf_it == trace_buffer.end())
	{
		values=NULL;
		return true;
	}

	ntime = buf_it->first;
	values = &last_values;

	//limit to current SystemC time
	if (finish)
	{
		//limit to SystemC time - thus we are aleays able to catch SystemC signals
		if (ntime > this->current_systemc_time)
		{
			values = NULL;
			return true;
		}
	}
	else
	{
		//limit to SystemC time - thus we are always able to catch SystemC signals
		if (ntime >= this->current_systemc_time)
		{
			values = NULL;
			return true;
		}
	}

	bool stop_reading = false;
	for (unsigned long i = 0; i < number_of_traces; ++i) //for all traced signals
	{
		//no value available -> we must generate a value
		if ((buf_it->second)[i] == NULL) //no value at the current time stamp
		{
			if (last_values[i] != NULL)
			{
				//generation by interpolation or holding
				if( hold_sample || ((no_interpolation != NULL) && no_interpolation[i]))
				{
					buf_it->second[i] = &(last_values[i]->hold(ntime));
				}
				else
				{
					buf_it->second[i] = &(last_values[i]->interpolate(ntime));
				}
			}
			else //no last_value available -> we propagate the first value to time zero
			{
				if (init_values[i] == NULL)
				{
					//can happen if the file is closed before the first
					//cluster execution has been finished
					values=NULL;
					return true;
				}
				//first value is propagated backward
				buf_it->second[i] = init_values[i];
			}
		}

		if (last_values[i] != NULL)
		{
			last_values[i]->free();
			last_values[i] = NULL;
		}
		last_values[i] = buf_it->second[i];

		if (last_values[i] == NULL)
		{
			SC_REPORT_ERROR("SystemC-AMS","Internal Tracing Error due a Bug");
		}

		//no further interpolation value available
		if (last_values[i]->next_value == NULL)
		{
			if ((no_interpolation != NULL) && !no_interpolation[i])
			{
				stop_reading = true;
			}
		}
	} //for (unsigned long i = 0; i < number_of_traces; ++i) //for all traced signals

	//erase read time point
	trace_buffer.erase(buf_it);

	if (trace_buffer.size() == 0) //no further time stamp
	{
		return true; //stop reading
	}

	return stop_reading;
}

///////////////////////////////////////////////////////////////////////////////

void sca_trace_buffer::enable()
{
	disabled = false;
}

///////////////////////////////////////////////////////////////////////////////

void sca_trace_buffer::disable()
{
	disabled = true;
}

void sca_trace_buffer::value_buffer::resize(long n)
{
	if (size_var == n)
		return;
	if (buffer != NULL)
	{
		if (n < size_var)
			size_var = n;
		sca_trace_value_handler_base** tmp = buffer;
		buffer = new sca_trace_value_handler_base*[n];
		for (long i = 0; i < size_var; i++)
			buffer[i] = tmp[i];
		delete[] tmp;
	}
	else
	{
		buffer = new sca_trace_value_handler_base*[n];
	}

	for (long i = size_var; i < n; i++)
		buffer[i] = NULL;
	size_var = n;
}

///////////////////////////////////////////////////////

long sca_trace_buffer::value_buffer::size()
{
	return size_var;
}

///////////////////////////////////////////////////////

sca_trace_value_handler_base*& sca_trace_buffer::value_buffer::operator [](
		long n)
{
	return buffer[n];
}

///////////////////////////////////////////////////////

sca_trace_buffer::value_buffer::value_buffer()
{
	buffer     = NULL;
	size_var   = 0;
	last_values= NULL;
}

///////////////////////////////////////////////////////


sca_trace_buffer::value_buffer::~value_buffer()
{
	if(buffer!=NULL) delete[] buffer;
	buffer=NULL;
}

///////////////////////////////////////////////////////

long sca_trace_buffer::stored_elements()
{
	return (long)(trace_buffer.size());
}

///////////////////////////////////////////////////////
/*
 const sca_core::sca_time& sca_trace_buffer::get_last_period()
 {
 return last_time_period;
 }
 */
const sca_core::sca_time& sca_trace_buffer::get_first_time()
{
	return trace_buffer.begin()->first;
}

void sca_trace_buffer::print_trace_buffer()
{
	for (std::map<sca_core::sca_time, value_buffer>::iterator buf_it =
			trace_buffer.begin(); buf_it != trace_buffer.end(); ++buf_it)
	{
		std::cout << buf_it->first;
		sca_trace_buffer::value_buffer& values(buf_it->second);

		for (int i = 0; i < values.size(); ++i)
		{
			std::cout << "  " << values[i];
			//values[i]->print(std::cout);
		}

		std::cout << std::endl;
	}

}

} // namespace sca_implementation
} // namespace sca_util

