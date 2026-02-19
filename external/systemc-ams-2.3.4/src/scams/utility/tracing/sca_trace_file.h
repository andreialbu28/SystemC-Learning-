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

 sca_trace_file.h - base class for sca_trace files

 Original Author: Karsten Einwich Fraunhofer IIS/EAS Dresden

 Created on: 08.03.2009

 SVN Version       :  $Revision: 1265 $
 SVN last checkin  :  $Date: 2011-11-19 20:43:31 +0000 (Sat, 19 Nov 2011) $
 SVN checkin by    :  $Author: karsten $
 SVN Id            :  $Id: sca_trace_file.h 1265 2011-11-19 20:43:31Z karsten $

 *****************************************************************************/
/*
 * LRM clause 6.1.1.2.
 * The class sca_util::sca_trace_file shall define the abstract base class, from
 * which the classes that provide file handles for VCD, tabular, or other
 * implementation-defined trace file formats are derived. An application shall
 * not construct objects of class sca_util::sca_trace_file, but may define
 * pointers and references to this type.
 */

/*****************************************************************************/

#ifndef SCA_TRACE_FILE_H_
#define SCA_TRACE_FILE_H_

namespace sca_util
{

//class sca_trace_file
class sca_trace_file : protected sca_util::sca_implementation::sca_trace_file_base
{
public:
	void enable();
	void disable();

	void set_mode(const sca_util::sca_trace_mode_base&);

    void reopen(const std::string& name, std::ios_base::openmode mode =
			std::ios_base::out | std::ios_base::trunc);

	void reopen(std::ostream& outstream);

	// Other members
	/*implementation-defined*/

	friend void sca_write_comment(sca_util::sca_trace_file* tf,const std::string& comment);
	friend void sca_close_tabular_trace_file(sca_util::sca_trace_file* tf);
	friend void sca_close_vcd_trace_file(sca_util::sca_trace_file* tf);
	friend void sca_trace(sca_util::sca_trace_file* tf,
			const sca_util::sca_traceable_object& obj, const std::string& str);
	friend void sca_trace(sca_util::sca_trace_file* tf,
			const sca_util::sca_traceable_object* obj, const std::string& str);

	template<class T>
	friend void sca_trace(sca_util::sca_trace_file* tf,
			const sc_core::sc_signal_in_if<T>& value, const std::string& str);

	template<class T>
	friend void sca_trace(sca_util::sca_trace_file* tf,
			const sc_core::sc_port<sc_core::sc_signal_inout_if<T> >& value, const std::string& str);

	template<class T>
	friend void sca_trace(sca_util::sca_trace_file* tf,
			const sc_core::sc_port<sc_core::sc_signal_in_if<T> >& value, const std::string& str);
private:

	virtual void reopen_impl(const std::string& name, std::ios_base::openmode mode =
			std::ios_base::out | std::ios_base::trunc);

	virtual void set_mode_impl(const sca_util::sca_trace_mode_base&);
};

} // namespace sca_util

#endif /* SCA_TRACE_FILE_H_ */
