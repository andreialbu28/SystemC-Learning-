/*****************************************************************************

    Copyright 2015-2016
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

  sca_con_interactive_trace_data.h - helper class for interactive tracing of eln/lsf

  Original Author: Karsten Einwich COSEDA Technologies GmbH Dresden

  Created on: Aug 31, 2015

   SVN Version       :  $Revision$
   SVN last checkin  :  $Date$
   SVN checkin by    :  $Author$
   SVN Id            :  $Id$

 *****************************************************************************/


#ifndef SRC_SCAMS_IMPL_PREDEFINED_MOC_CONSERVATIVE_SCA_CON_INTERACTIVE_TRACE_DATA_H_
#define SRC_SCAMS_IMPL_PREDEFINED_MOC_CONSERVATIVE_SCA_CON_INTERACTIVE_TRACE_DATA_H_


namespace sca_core
{

namespace sca_implementation
{

class sca_con_interactive_trace_data
{
public:


	void store_value(double val);
	const double& get_value();

	void register_trace_callback(sca_util::sca_traceable_object::sca_trace_callback cb,void* cb_arg);
	void register_trace_callback(sca_util::sca_traceable_object::callback_functor_base&);
	bool remove_trace_callback(sca_util::sca_traceable_object::callback_functor_base&);

	sca_con_interactive_trace_data(const sca_core::sca_implementation::sca_conservative_signal* cnode);
	sca_con_interactive_trace_data(const sca_core::sca_implementation::sca_conservative_module* mod);


private:

	void construct();

	sca_core::sca_implementation::sca_conservative_signal* node;

	void trace_callback();


	sca_util::sca_implementation::sca_trace_object_data data;

	std::vector<double> buffer;

	long last_pos;

	bool multirate;

	bool initialize_delta_waited;

	long trace_callback_cnt;

	sca_util::sca_traceable_object::sca_trace_callback  trace_cb_ptr;
	void*               trace_cb_arg;


	std::vector<sca_util::sca_traceable_object::callback_functor_base*> callbacks;
	bool callback_registered;

	sca_core::sca_implementation::sca_conservative_module* module;

	void enable_multirate();

};

} //namespace sca_implementation

} //namespace sca_core

#endif /* SRC_SCAMS_IMPL_PREDEFINED_MOC_CONSERVATIVE_SCA_CON_INTERACTIVE_TRACE_DATA_H_ */

