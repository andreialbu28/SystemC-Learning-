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

 sca_eln_sc_vsink.h - electrical linear net converter from voltage to sc_signal<double>

 Original Author: Karsten Einwich Fraunhofer IIS/EAS Dresden

 Created on: 08.03.2009

 SVN Version       :  $Revision: 2320 $
 SVN last checkin  :  $Date: 2023-06-08 12:47:38 +0000 (Thu, 08 Jun 2023) $ (UTC)
 SVN checkin by    :  $Author: pehrlich $
 SVN Id            :  $Id: sca_eln_sc_vsink.h 2320 2023-06-08 12:47:38Z pehrlich $

 *****************************************************************************/
/*
 * LRM clause 4.3.1.32.
 * The class sca_eln::sc_core::sca_vsink shall implement a primitive module for
 * the ELN MoC that realizes a scaled conversion from an ELN voltage to a
 * discrete-event output signal. The value of the voltage across terminals p
 * and n shall be scaled with coefficient scale and written to a discrete-event
 * output port outp. The primitive shall not contribute any equation to the
 * equation system.
 */

/*****************************************************************************/

#ifndef SCA_ELN_SC_VSINK_H_
#define SCA_ELN_SC_VSINK_H_

namespace sca_eln
{

namespace sca_de
{

//    class sca_vsink : public implementation-derived-from sca_core::sca_module,
//                      protected sca_util::sca_traceable_object
class sca_vsink: public sca_eln::sca_module,
		         public sca_util::sca_traceable_object,
		         public sca_core::sca_physical_domain_interface
{
public:
	sca_eln::sca_terminal p;
	sca_eln::sca_terminal n;

	::sc_core::sc_out<double> outp;

	sca_core::sca_parameter<double> scale;

	virtual const char* kind() const;

	explicit sca_vsink(sc_core::sc_module_name, double scale_ = 1.0);

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
		void assign_result();

		 ::sca_tdf::sca_de::sca_out<double>* conv_port;

		 bool trace_init(sca_util::sca_implementation::sca_trace_object_data& data);
		 void trace(long id,sca_util::sca_implementation::sca_trace_file_base& tf);
		 void trace_interactive();
		 sca_util::sca_complex calculate_ac_result(sca_util::sca_complex* res_vec);

		 //sc trace of sc_core::sc_object to prevent clang warning due overloaded
		 //sca_traceable_object function
		 void trace( sc_core::sc_trace_file* tf ) const;

		 std::string unit;
		 std::string unit_prefix;
		 std::string domain;

		//end implementation specific

};

} // namespace sca_tdf

typedef sca_eln::sca_de::sca_vsink sca_de_vsink;

} // namespace sca_eln

#endif /* SCA_ELN_SC_VSINK_H_ */
