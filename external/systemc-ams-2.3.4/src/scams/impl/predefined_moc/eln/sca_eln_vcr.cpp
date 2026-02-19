/*****************************************************************************

    Copyright 2010-2014
    Fraunhofer-Gesellschaft zur Foerderung der angewandten Forschung e.V.

    Copyright 2015-2022
    COSEDA Technologies GmbH


 *****************************************************************************/

/*****************************************************************************

  sca_eln_vcr.cpp - description

  Original Author: Karsten Einwich Fraunhofer IIS/EAS Dresden

  Created on: 13.05.2014

   SVN Version       :  $Revision: 1703 $
   SVN last checkin  :  $Date: 2014-04-23 12:10:42 +0200 (Wed, 23 Apr 2014) $
   SVN checkin by    :  $Author: karsten $
   SVN Id            :  $Id: sca_eln_r.cpp 1703 2014-04-23 10:10:42Z karsten $

 *****************************************************************************/

/*****************************************************************************/


#include "scams/predefined_moc/eln/sca_eln_vcr.h"
#include "scams/impl/predefined_moc/conservative/sca_con_interactive_trace_data.h"

namespace sca_eln
{
sca_vcr::sca_vcr(sc_core::sc_module_name,
		const sca_util::sca_vector<std::pair<double,double> >& pwc_value_) :
		p("p"), n("n"), pwc_vector("pwc_value", pwc_value_)
{
    nadd1=-1;
    nadd2=-1;

	unit="A";
	domain="I";
}

const char* sca_vcr::kind() const
{
	return "sca_eln::sca_vcr";
}


void sca_vcr::trace( sc_core::sc_trace_file* tf ) const
{
	std::ostringstream str;
	str << "sc_trace of sca_vcr module not supported for module: " << this->name();
	SC_REPORT_WARNING("SystemC-AMS",str.str().c_str());
}


void sca_vcr::matrix_stamps()
{
	    nadd1 = add_equation(2);
	    nadd2 = nadd1 + 1;

	    B_wr(nadd1, p)     = -1.0;
	    B_wr(nadd1, n)     +=  1.0;
	    B_wr(nadd1, nadd1) =  1.0;  // 0 = nadd1 * 1.0 - v(p) + v(n)
	                                // nadd1 = ( v(p) - v(n) ) / 1.0

	    B_wr(p, nadd1) =  1.0;
	    B_wr(n, nadd1) += -1.0;



	    B_wr(nadd2,nadd2) = -1.0;  // nadd2 = v(cp) - v(cn)
	    B_wr(nadd2,cp)    =  1.0;
	    B_wr(nadd2,cn)    += -1.0;

	    add_pwl_b_stamp_to_B(nadd1,nadd1,nadd2,pwc_vector.get());
}

bool sca_vcr::trace_init(sca_util::sca_implementation::sca_trace_object_data& data)
{
    //trace will be activated after every complete cluster calculation
    //by teh synchronization layer
    return this->add_solver_trace(data);
}

void sca_vcr::trace(long id,sca_util::sca_implementation::sca_trace_file_base& tf)
{
    sca_core::sca_time ctime = sca_eln::sca_module::get_time();

    tf.store_time_stamp(id,ctime,x(nadd1));
}

void sca_vcr::trace_interactive()
{
	if(this->trd!=NULL) return;

    double through_value=x(nadd1);

	this->trd->store_value(through_value);
	return;
}

sca_util::sca_complex sca_vcr::calculate_ac_result(sca_util::sca_complex* res_vec)
{
    return res_vec[nadd1];
}

/**
   * experimental physical domain interface
*/
void sca_vcr::set_unit(const std::string& unit_)
{
	unit=unit_;
}

const std::string& sca_vcr::get_unit() const
{
	return unit;
}

void sca_vcr::set_unit_prefix(const std::string& prefix_)
{
	unit_prefix=prefix_;
}

const std::string& sca_vcr::get_unit_prefix() const
{
	return unit_prefix;
}

void sca_vcr::set_domain(const std::string& domain_)
{
	domain=domain_;
}

const std::string& sca_vcr::get_domain() const
{
	return domain;
}


} //namespace sca_eln

