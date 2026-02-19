/*****************************************************************************

    Copyright 2010-2014
    Fraunhofer-Gesellschaft zur Foerderung der angewandten Forschung e.V.

    Copyright 2015-2022 COSEDA Technologies GmbH

 *****************************************************************************/

/*****************************************************************************

 sca_eln_vcr.h - electrical linear net piece wise constant voltage controlled capacitor

 Original Author: Karsten Einwich COSEDA Technologies GmbH

 Created on: 13.05.2015

 SVN Version       :  $Revision: 1523 $
 SVN last checkin  :  $Date: 2013-02-17 21:36:57 +0100 (Sun, 17 Feb 2013) $ (UTC)
 SVN checkin by    :  $Author: karsten $
 SVN Id            :  $Id: sca_eln_r.h 1523 2013-02-17 20:36:57Z karsten $


****************************************************************************/


#ifndef SCA_ELN_VCC_H_
#define SCA_ELN_VCC_H_

#include <systemc-ams>

namespace sca_eln
{


class sca_vcc: public    sca_eln::sca_module ,
               public sca_util::sca_traceable_object,
	           public sca_core::sca_physical_domain_interface
{
public:
	sca_eln::sca_terminal p;
	sca_eln::sca_terminal n;

	sca_eln::sca_terminal cp;
	sca_eln::sca_terminal cn;

	sca_core::sca_parameter<double>                        q0;
	sca_core::sca_parameter<sca_util::sca_vector<std::pair<double,double> > > pwc_vector;

	virtual const char* kind() const;

	explicit sca_vcc(sc_core::sc_module_name,
			const sca_util::sca_vector<std::pair<double,double> >& pwc_value_ =
					sca_util::sca_create_pair_vector(1.0,1.0));


	/**
	   * experimental physical domain interface
	*/
	virtual void set_unit(const std::string& unit);
	virtual const std::string& get_unit() const;

	virtual void set_unit_prefix(const std::string& prefix);
	virtual const std::string& get_unit_prefix() const;

	virtual void set_domain(const std::string& domain);
	virtual const std::string& get_domain() const;

private:
	 virtual void matrix_stamps();

	 bool trace_init(sca_util::sca_implementation::sca_trace_object_data& data);
	 void trace(long id,sca_util::sca_implementation::sca_trace_file_base& tf);
	 void trace_interactive();
	 sca_util::sca_complex calculate_ac_result(sca_util::sca_complex* res_vec);

	 //sc trace of sc_core::sc_object to prevent clang warning due overloaded
	 //sca_traceable_object function
	 void trace( sc_core::sc_trace_file* tf ) const;

	 void post_solve();

	 long nadd1, nadd2, nadd3, nadd4;

	 bool dc_init;

	 std::string unit;
	 std::string unit_prefix;
	 std::string domain;

};

} // namespace sca_eln


#endif /* SCA_ELN_VCC_H_ */
