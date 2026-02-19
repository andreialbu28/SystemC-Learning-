/*****************************************************************************

    Copyright 2010-2011
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

  sca_delay_buffer_base.h - description

  Original Author: Karsten Einwich Fraunhofer IIS/EAS Dresden

  Created on: Nov 14, 2011

   SVN Version       :  $Revision: 1563 $
   SVN last checkin  :  $Date: 2013-05-03 15:06:50 +0000 (Fri, 03 May 2013) $
   SVN checkin by    :  $Author: karsten $
   SVN Id            :  $Id: sca_delay_buffer_base.h 1563 2013-05-03 15:06:50Z karsten $

 *****************************************************************************/

/*****************************************************************************/


#ifndef SCA_DELAY_BUFFER_BASE_H_
#define SCA_DELAY_BUFFER_BASE_H_


namespace sca_tdf
{
namespace sca_implementation
{

class sca_delay_buffer_base
{
public:

	void set_max_delay(const sca_core::sca_time& mtime);
	const sca_core::sca_time& get_max_delay() const;
	void set_time_reached(sca_core::sca_implementation::sca_signed_time rtime);


	void remove_timestamps_after(sca_core::sca_implementation::sca_signed_time rtime);


protected:

	void store_value(sca_core::sca_implementation::sca_signed_time vtime);
	int get_time_interval(sca_core::sca_implementation::sca_signed_time vtime);
	//sets first_index to time point after ntime (if not available return true)
	bool get_next_time_after(sca_core::sca_implementation::sca_signed_time ntime);
	bool get_timepoint(sca_core::sca_implementation::sca_signed_time ntime);

	sca_delay_buffer_base();
	virtual ~sca_delay_buffer_base();

	virtual void resize_value_buffer(unsigned long size)=0;


	void resize(unsigned long size);

	sca_core::sca_time max_time;

	sca_core::sca_implementation::sca_signed_time* time_buffer;
	unsigned long buffer_size;  //maximum buffer size (memory)
	unsigned long buffer_used;  //number of pairs in buffer
	unsigned long buffer_write; //next write sample number (-1 -> last written sample)
	unsigned long buffer_first; //position of first pair in buffer

	unsigned long first_index;
	unsigned long second_index;

	sca_core::sca_implementation::sca_signed_time last_read_time;

	sca_core::sca_implementation::sca_signed_time time_reached;
};

}
}


#endif /* SCA_DELAY_BUFFER_BASE_H_ */
