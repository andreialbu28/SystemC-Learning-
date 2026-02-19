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

 sca_lsf_signal.h - linear signal flow signal

 Original Author: Karsten Einwich Fraunhofer IIS/EAS Dresden / COSEDA Technologies

 Created on: 05.03.2009

 SVN Version       :  $Revision: 2320 $
 SVN last checkin  :  $Date: 2023-06-08 12:47:38 +0000 (Thu, 08 Jun 2023) $
 SVN checkin by    :  $Author: pehrlich $
 SVN Id            :  $Id: sca_lsf_signal.h 2320 2023-06-08 12:47:38Z pehrlich $

 *****************************************************************************/
/*
 LRM clause 4.2.1.2.
 The class sca_lsf::sca_signal shall define a primitive channel for the
 LSF MoC. It shall be used for connecting modules of class sca_lsf::sca_module
 using ports of class sca_lsf::sca_in and sca_lsf::sca_out.
 There shall not be any application-specific access to the associated interface.
 */

/*****************************************************************************/

#ifndef SCA_LSF_SIGNAL_H_
#define SCA_LSF_SIGNAL_H_


namespace sca_core
{
namespace sca_implementation
{
	class sca_con_interactive_trace_data;
}
}


namespace sca_lsf
{

//class sca_signal : public implementation-derived-from sca_core::sca_signal_if,
//                                                         sca_core::sca_prim_channel
class sca_signal: virtual public sca_core::sca_implementation::sca_conservative_signal,
                  virtual public sca_lsf::sca_signal_if
{
public:
	sca_signal();
	explicit sca_signal(const char*);

	virtual const char* kind() const;

	/** method of interactive tracing interface, which returns the value
	 * at the current SystemC time (the time returned by sc_core::sc_time_stamp())
	 */
	const double& get_typed_trace_value() const;

	/** method of interactive tracing interface, which returns the value as string
	 * at the current SystemC time (the time returned by sc_core::sc_time_stamp())
	 */
	const std::string& get_trace_value() const;

	/** method of interactive tracing interface, which registers
	 * a trace callback function, which is called at each SystemC timepoint
	 * is a new eln result is available
	 */
	bool register_trace_callback(sca_util::sca_traceable_object::sca_trace_callback,void*);
	bool register_trace_callback(sca_util::sca_traceable_object::callback_functor_base&);
	bool remove_trace_callback(sca_util::sca_traceable_object::callback_functor_base&);


	/**
	 * experimental physical domain mehtods
	 */
	virtual void set_unit(const std::string& unit);
	virtual const std::string& get_unit() const;

	virtual void set_unit_prefix(const std::string& prefix);
	virtual const std::string& get_unit_prefix() const;

	virtual void set_domain(const std::string& domain);
	virtual const std::string& get_domain() const;

	virtual void print( std::ostream& = std::cout ) const;
	virtual void dump( std::ostream& = std::cout ) const;

private:
	// Disabled
	sca_signal(const sca_lsf::sca_signal&);
	//begin implementation specific

	//overloaded sca_conservative_module method, which returns the current result*/
	double* get_result_ref();

	long get_node_number() const;

	virtual bool trace_init(
				sca_util::sca_implementation::sca_trace_object_data& data);

	//is called after cluster was calculated
	virtual void trace(long id,
				sca_util::sca_implementation::sca_trace_file_base& tf);
	virtual void trace_interactive();

	void get_connected_lsf_module();
	sca_lsf::sca_module* lsf_module;

	mutable sca_core::sca_implementation::sca_con_interactive_trace_data* trd;


	std::string unit;
	std::string unit_prefix;
	std::string domain;

	// end implementation specific

};

} // namespace sca_lsf


#endif /* SCA_LSF_SIGNAL_H_ */
