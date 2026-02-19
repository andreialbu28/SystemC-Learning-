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

 sca_trace.h - functions for adding traces to trace files

 Original Author: Karsten Einwich Fraunhofer IIS/EAS Dresden

 Created on: 08.03.2009

 SVN Version       :  $Revision: 1911 $
 SVN last checkin  :  $Date: 2016-02-16 13:51:22 +0000 (Tue, 16 Feb 2016) $
 SVN checkin by    :  $Author: karsten $
 SVN Id            :  $Id: sca_trace.h 1911 2016-02-16 13:51:22Z karsten $

 *****************************************************************************/
/*
 * The function sca_util::sca_trace shall trace the value passed as the second
 * argument to the trace file passed as the first argument, using the string
 * passed as the third argument to identify the value in the trace file.
 */

/*****************************************************************************/

#ifndef SCA_TRACE_H_
#define SCA_TRACE_H_

namespace sca_util
{

void sca_trace(sca_util::sca_trace_file* tf,
		const sca_util::sca_traceable_object& obj, const std::string& str);

void sca_trace(sca_util::sca_trace_file* tf,
		const sca_util::sca_traceable_object* obj, const std::string& str);

template<class T>
void sca_trace(sca_util::sca_trace_file* tf,
		const sc_core::sc_signal_in_if<T>& value, const std::string& str);

template<class T>
void sca_trace(sca_util::sca_trace_file* tf, const sc_core::sc_port<sc_core::sc_signal_in_if<T> >& value,
		const std::string& str);


template<class T>
void sca_trace(sca_util::sca_trace_file* tf, const sc_core::sc_port<sc_core::sc_signal_inout_if<T> >& value,
		const std::string& str);


//begin implementation specific

template<class T>
inline void sca_trace(sca_util::sca_trace_file* tf,
		const sc_core::sc_signal_in_if<T>& value, const std::string& str)
{
	tf->add(new sca_util::sca_implementation::sca_sc_trace<sc_core::sc_signal_in_if<T> >(value),str);
}


template<class T>
inline void sca_trace(sca_util::sca_trace_file* tf,
		const sc_core::sc_port<sc_core::sc_signal_in_if<T> >& value,
		const std::string& str)
{
	tf->add(new sca_util::sca_implementation::sca_sc_trace<sc_core::sc_port<sc_core::sc_signal_in_if<T> > >(value),str);
}


template<class T>
inline void sca_trace(sca_util::sca_trace_file* tf,
		const sc_core::sc_port<sc_core::sc_signal_inout_if<T> >& value,
		const std::string& str)
{
	tf->add(new sca_util::sca_implementation::sca_sc_trace<sc_core::sc_port<sc_core::sc_signal_inout_if<T> > >(value),str);
}


//end implementation specific

} // namespace sca_util


#endif /* SCA_TRACE_H_ */
