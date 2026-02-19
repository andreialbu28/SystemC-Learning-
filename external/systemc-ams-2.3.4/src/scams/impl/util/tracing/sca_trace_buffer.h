/*****************************************************************************

    Copyright 2010
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

  sca_trace_buffer.h - description

  Original Author: Karsten Einwich Fraunhofer IIS/EAS Dresden

  Created on: 13.11.2009

   SVN Version       :  $Revision: 2320 $
   SVN last checkin  :  $Date: 2023-06-08 12:47:38 +0000 (Thu, 08 Jun 2023) $
   SVN checkin by    :  $Author: pehrlich $
   SVN Id            :  $Id: sca_trace_buffer.h 2320 2023-06-08 12:47:38Z pehrlich $

 *****************************************************************************/

/*****************************************************************************/


#ifndef SCA_TRACE_BUFFER_H_
#define SCA_TRACE_BUFFER_H_

namespace sca_util
{

namespace sca_implementation
{


class sca_trace_buffer
{
 public:

  class value_buffer
  {
    public:

        void resize(long n);
        long size();

        sca_trace_value_handler_base*& operator [] (long n);

        value_buffer();
        ~value_buffer();

        value_buffer* last_values;

    private:

        sca_trace_value_handler_base** buffer;
        long size_var;
  };

  public:

    //registers trace, returns access id
    long register_trace();

    //returns reference to the most recently stamp
    //do not add missing values - return true if last stamp has been read
    bool read_next_stamp(sca_core::sca_time& ntime, value_buffer*& values);

    //returns reference to the most recently stamp, add missing values
    //by interpolation - return true if no further interpolation possible
    //due no further next_value available
    bool read_next_stamp_interpolate(sca_core::sca_time& ntime, value_buffer*& values, bool hold_sample);

    //samples at time ntime and reads if required - returns true if reading
    //has been failed due no further value is available
    bool read_sample_interpolate(const sca_core::sca_time& ntime,value_buffer*& values);

    void enable();
    void disable();
    long stored_elements();

    sca_trace_buffer();
    ~sca_trace_buffer();

    const sca_core::sca_time& get_first_time();

    void print_trace_buffer();

    bool finish;

    //hold sample of signal instead interpolate
    bool* no_interpolation;


    void store_abstract_time_stamp(sca_trace_value_handler_base& value);

  private:

    bool disabled;



    unsigned long number_of_traces;
    std::map<sca_core::sca_time,value_buffer>             trace_buffer;

    //last written timepoint
    value_buffer last_values;

    //references to last stored time points for creating datastructure
    //(add reference to next time point)
    value_buffer last_stored_values;
    //init value buffer for propagating first stored values to time zero
    //if first stored value later than zero - used if a time point required
    //before the first stored value time
    value_buffer init_values;

    //counts number of waves, which received an initial value
    unsigned long init_value_cnt;

    sca_core::sca_time current_systemc_time;

};


}  //namespace sca_implementation

} // namespace sca_util


#endif /* SCA_TRACE_BUFFER_H_ */
