/*****************************************************************************

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

 sca_lock_free_queue.h - lock free queue for thread communication

 Original Author: Karsten Einwich COSEDA Technologies GmbH

 Created on: 07.03.2022

 SVN Version       :  $Revision$
 SVN last checkin  :  $Date$
 SVN checkin by    :  $Author$
 SVN Id            :  $Id$

 *****************************************************************************/

#include "sca_lock_free_queue.h"
#include <systemc>

#include <atomic>
#include <mutex>

namespace sca_util
{
namespace sca_implementation
{

struct sca_lock_free_queue::states
{
	std::atomic<std::size_t> read_cnt;
	std::atomic<std::size_t> write_cnt;


	std::size_t size;

	void** buffer;

	std::mutex resize_mutex;

	states()
	{
		read_cnt=0;
		write_cnt=0;

		size=256;

		buffer=new void*[size];
		for(std::size_t idx=0;idx<size;++idx) buffer[idx]=NULL;

	}

};

sca_lock_free_queue::sca_lock_free_queue() : sp(new states), s(*sp)
{

}


sca_lock_free_queue::~sca_lock_free_queue()
{

}

void sca_lock_free_queue::increase_buffer()
{
	std::unique_lock<std::mutex> lock(s.resize_mutex);

	std::size_t new_size=s.size+256;
	void** new_buffer=new void*[new_size];

	for(std::size_t idx=0;idx<new_size;++idx) new_buffer[idx]=NULL;

	for(std::size_t idx=s.read_cnt;idx<s.write_cnt;idx++)
	{
		new_buffer[idx%new_size]=s.buffer[idx%s.size];
	}


	delete[] s.buffer;
	s.buffer=new_buffer;

	s.size=new_size;

}


void sca_lock_free_queue::push(void* data)
{
	if((s.write_cnt-s.read_cnt) >= s.size)
	{
		this->increase_buffer();
	}

	std::size_t pos=s.write_cnt%s.size;

	s.buffer[pos]=data;
	s.write_cnt++;
}

void* sca_lock_free_queue::pop()
{

	if(s.read_cnt>=s.write_cnt) //buffer empty
	{
		return NULL;
	}

	void* val=NULL;


	//if(!s.resize_flag.test_and_set())
	{
		std::unique_lock<std::mutex> lock(s.resize_mutex);
		std::size_t pos=s.read_cnt%s.size;
		val=s.buffer[pos];
	}


		s.read_cnt++;

	//else
	{
	//	val=s.buffer[s.read_cnt++%s.size];
	}


	return val;
}


}

}

