/*****************************************************************************

    Copyright 2010-2013
    Fraunhofer-Gesellschaft zur Foerderung der angewandten Forschung e.V.

    Copyright 2015-2020
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

 sca_eln_r.h - electrical linear net resistor

 Original Author: Karsten Einwich Fraunhofer IIS/EAS Dresden / COSEDA Technologies

 Created on: 07.03.2009

 SVN Version       :  $Revision: 2320 $
 SVN last checkin  :  $Date: 2023-06-08 12:47:38 +0000 (Thu, 08 Jun 2023) $ (UTC)
 SVN checkin by    :  $Author: pehrlich $
 SVN Id            :  $Id: sca_eln_r.h 2320 2023-06-08 12:47:38Z pehrlich $

 *****************************************************************************/
/*
 * LRM clause 4.3.1.5.
 * The class sca_eln::sca_r shall implement a primitive module for the ELN MoC
 * that represents a resistor. The primitive shall contribute the following
 * equation to the equation system:
 *
 *        i(p,n)(t) = v(p,n)(t) / value
 *
 * where value is the resistor value in ohm, v(p,n)(t) is the voltage across the
 * resistor between terminals p and n, and i(p,n)(t) is the current through the
 * resistor flowing from terminal p to terminal n.
 */

/*****************************************************************************/

#ifndef SCA_ELN_R_H_
#define SCA_ELN_R_H_

namespace sca_eln
{

//  class sca_r : public implementation-derived-from sca_core::sca_module,
//                protected sca_util::sca_traceable_object
class sca_r: public    sca_eln::sca_module ,
             public sca_util::sca_traceable_object,
             public sca_core::sca_physical_domain_interface
{
public:
	sca_eln::sca_terminal p;
	sca_eln::sca_terminal n;

	sca_core::sca_parameter<double> value;

	virtual const char* kind() const;

	explicit sca_r(sc_core::sc_module_name, double value_ = 1.0);

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
	   * experimental physical domain interface
	*/
	virtual void set_unit(const std::string& unit);
	virtual const std::string& get_unit() const;

	virtual void set_unit_prefix(const std::string& prefix);
	virtual const std::string& get_unit_prefix() const;

	virtual void set_domain(const std::string& domain);
	virtual const std::string& get_domain() const;

	//begin implementation specific

private:
	 virtual void matrix_stamps();

	 bool trace_init(sca_util::sca_implementation::sca_trace_object_data& data);
	 void trace(long id,sca_util::sca_implementation::sca_trace_file_base& tf);
	 void trace_interactive();
	 sca_util::sca_complex calculate_ac_result(sca_util::sca_complex* res_vec);

	 //sc trace of sc_core::sc_object to prevent clang warning due overloaded
	 //sca_traceable_object function
	 void trace( sc_core::sc_trace_file* tf ) const;

	 long nadd;

	 std::string unit;
	 std::string unit_prefix;
	 std::string domain;

	//end implementation specific
};

} // namespace sca_eln


#endif /* SCA_ELN_R_H_ */
