/*****************************************************************************

    Copyright 2010
    Fraunhofer-Gesellschaft zur Foerderung der angewandten Forschung e.V.

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

  sca_trace_value_handler_base.h - description

  Original Author: Karsten Einwich Fraunhofer IIS/EAS Dresden

  Created on: 13.11.2009

   SVN Version       :  $Revision: 2320 $
   SVN last checkin  :  $Date: 2023-06-08 12:47:38 +0000 (Thu, 08 Jun 2023) $
   SVN checkin by    :  $Author: pehrlich $
   SVN Id            :  $Id: sca_trace_value_handler_base.h 2320 2023-06-08 12:47:38Z pehrlich $

 *****************************************************************************/

/*****************************************************************************/


#ifndef SCA_TRACE_VALUE_HANDLER_BASE_H_
#define SCA_TRACE_VALUE_HANDLER_BASE_H_

namespace sca_util
{
namespace sca_implementation
{

class sca_trace_value_handler_mm;
class sca_trace_value_handler_mm_typed;

class sca_trace_value_handler_base
{
  public:

    virtual void print(std::ostream& ostr)=0;
    virtual sca_trace_value_handler_base& hold(const sca_core::sca_time& ctime)        = 0;
    virtual sca_trace_value_handler_base& interpolate(const sca_core::sca_time& ctime) = 0;
    virtual sca_trace_value_handler_base* duplicate()=0;

	virtual void swap_values_base(sca_trace_value_handler_base*) = 0;


    virtual const sca_type_explorer_base& get_typed_value(const void*& data) const=0;
    virtual const sca_type_explorer_base& get_type() const =0;

    void free(); //free handle


    sca_trace_value_handler_base* get_new_local_base();

    std::int64_t id;

  protected:

    virtual void remove_value() = 0;


    friend class sca_trace_buffer;

    sca_core::sca_time             systemc_time;
    sca_core::sca_time             this_time;
    sca_core::sca_time             next_time;
    sca_trace_value_handler_base*  next_value;

	std::uint64_t* ref_cnt;

	bool localy_generated;

	void set_local();
	bool is_local();


    friend class sca_trace_value_handler_mm;

    sca_trace_value_handler_mm_typed* memory_manager;

    sca_trace_value_handler_base();
    virtual ~sca_trace_value_handler_base();

};

} // namespace sca_implementation
} // namespace sca_util

#endif /* SCA_TRACE_VALUE_HANDLER_BASE_H_ */
