/*****************************************************************************

    Copyright 2010-2014
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

  sca_trace_object_data.h - description

  Original Author: Karsten Einwich Fraunhofer IIS/EAS Dresden

  Created on: 13.11.2009

   SVN Version       :  $Revision: 2320 $
   SVN last checkin  :  $Date: 2023-06-08 12:47:38 +0000 (Thu, 08 Jun 2023) $
   SVN checkin by    :  $Author: pehrlich $
   SVN Id            :  $Id: sca_trace_object_data.h 2320 2023-06-08 12:47:38Z pehrlich $

 *****************************************************************************/

/*
 * class stores data for a value to be traced
 */

/*****************************************************************************/


#ifndef SCA_TRACE_OBJECT_DATA_H_
#define SCA_TRACE_OBJECT_DATA_H_

namespace sca_util
{

class sca_traceable_object;

namespace sca_implementation
{

class sca_trace_file_base;

class sca_trace_object_data
{
    long    not_used_value;
    sca_type_explorer_base* type_info;

public:

    long id;
    sca_trace_file_base* trace_base;

    std::string original_name;
    std::string name;
    std::string type;
    std::string unit;
    bool dont_interpolate;
    bool event_driven;

    //pointer to object, which implements trace_init and trace
    sca_util::sca_traceable_object*   trace_object;

    template<typename T>
    void set_type_info()
    {
    	if(type_info!=NULL) delete type_info;
    	type_info=new sca_type_explorer<T>();
    	type_info->manipulate_trace_data(this);
    }

    sca_type_explorer_base& get_type_info();


    //mask for checking data availability
    long           word;
    long           nwords;
    unsigned long  bit_mask;


    bool trace_disabled;

    void trace();


    sca_trace_object_data();

    void trace_init();

};



} // namespace sca_implementation
} // namespace sca_util


#endif /* SCA_TRACE_OBJECT_DATA_H_ */
