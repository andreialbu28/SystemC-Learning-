/*****************************************************************************

    Copyright 2010-2014
    Fraunhofer-Gesellschaft zur Foerderung der angewandten Forschung e.V.

    Copyright 2015-2020 COSEDA Technologies GmbH


 *****************************************************************************/

/*****************************************************************************

  sca_eln_ccl.cpp - description

  Original Author: Karsten Einwich COSEDA Technologies GmbH

  Created on: 13.05.2014

   SVN Version       :  $Revision: 1703 $
   SVN last checkin  :  $Date: 2014-04-23 12:10:42 +0200 (Wed, 23 Apr 2014) $
   SVN checkin by    :  $Author: karsten $
   SVN Id            :  $Id: sca_eln_r.cpp 1703 2014-04-23 10:10:42Z karsten $

 *****************************************************************************/

/*****************************************************************************/


#include "scams/predefined_moc/eln/sca_eln_ccl.h"
#include "scams/impl/util/data_types/sca_method_object.h"
#include "scams/impl/util/data_types/sca_method_list.h"
#include "scams/impl/predefined_moc/conservative/sca_con_interactive_trace_data.h"

namespace sca_eln
{
sca_ccl::sca_ccl(sc_core::sc_module_name,
		const sca_util::sca_vector<std::pair<double,double> >& pwc_value_) :
		p("p"), n("n"), psi0("psi0",0.0), pwc_vector("pwc_value", pwc_value_)
{
    nadd1=-1;
    nadd2=-1;
    nadd3=-1;

    dc_init=true;

    unit="A";
    domain="I";
}

const char* sca_ccl::kind() const
{
	return "sca_eln::sca_ccl";
}


void sca_ccl::trace( sc_core::sc_trace_file* tf ) const
{
	std::ostringstream str;
	str << "sc_trace of sca_ccl module not supported for module: " << this->name();
	SC_REPORT_WARNING("SystemC-AMS",str.str().c_str());
}


void sca_ccl::matrix_stamps()
{

	// nadd1 - i
	// nadd2 - psi
	// nadd3 - v(cp,cn)
	nadd1  = add_equation(3);
	nadd2 = nadd1+1;
	nadd3 = nadd2+1;


	if(!sca_ac_analysis::sca_ac_is_running() && dc_init)
	{
		if (psi0.get() == sca_util::SCA_UNDEFINED)
		{
			//short cut  v(p)-v(n)=0
			B_wr(nadd1, p) = 1.0;
			B_wr(nadd1, n) = -1.0;
		}
		else //current source
		{
			// psi = psi0
			B_wr(nadd1, nadd2) = 1.0;
			q(nadd1).set_value(-psi0.get());
		}

		add_method(POST_SOLVE, SCA_VMPTR(sca_ccl::post_solve));

		dc_init=false;

	}
	else
	{
	    //dpsi = v
	    B_wr(nadd1, p) =  1.0;
	    B_wr(nadd1, n) = -1.0;

	    A(nadd1, nadd2) = -1.0;
	}

	//current contribution p, n
	B_wr(p, nadd1) = 1.0;
	B_wr(n, nadd1) = -1.0;

    // nadd3 = i(cp,cn)
    B_wr(cp,nadd3) = 1.0;
    B_wr(cn,nadd3) = -1.0;

    //short cut v(cp)-v(cn) = 0
    B_wr(nadd3,cp)    =  1.0;
    B_wr(nadd3,cn)    = -1.0;

	B_wr(nadd2, nadd2) = -1.0; //psi=L*i
	B_wr(nadd2, nadd1) = 1.0;

	add_pwl_b_stamp_to_B(nadd2,nadd1,nadd3,pwc_vector.get());

}



void sca_ccl::post_solve() //set capacitor stamps after first step
{
	//reset dc stamps
	B_wr(nadd1, nadd2) = 0.0;
	B_wr(nadd1, p)     = 0.0;
	B_wr(nadd1, n)     = 0.0;
	q(nadd1).set_value(0.0);

	remove_method(POST_SOLVE, SCA_VMPTR(sca_ccl::post_solve));

    //dpsi = v
    B_wr(nadd1, p) =  1.0;
    B_wr(nadd1, n) = -1.0;
    A(nadd1, nadd2) = -1.0;

    request_reinit(1);

	dc_init=false;
}




bool sca_ccl::trace_init(sca_util::sca_implementation::sca_trace_object_data& data)
{
    //trace will be activated after every complete cluster calculation
    //by the synchronization layer
    return this->add_solver_trace(data);
}

void sca_ccl::trace(long id,sca_util::sca_implementation::sca_trace_file_base& tf)
{
    sca_core::sca_time ctime = sca_eln::sca_module::get_time();

    tf.store_time_stamp(id,ctime,x(nadd1));
}

void sca_ccl::trace_interactive()
{
	if(this->trd==NULL) return;

	this->trd->store_value(x(nadd1));
	return;
}

sca_util::sca_complex sca_ccl::calculate_ac_result(sca_util::sca_complex* res_vec)
{
    return res_vec[nadd1];
}


/**
   * experimental physical domain interface
*/
void sca_ccl::set_unit(const std::string& unit_)
{
	unit=unit_;
}

const std::string& sca_ccl::get_unit() const
{
	return unit;
}

void sca_ccl::set_unit_prefix(const std::string& prefix_)
{
	unit_prefix=prefix_;
}

const std::string& sca_ccl::get_unit_prefix() const
{
	return unit_prefix;
}

void sca_ccl::set_domain(const std::string& domain_)
{
	domain=domain_;
}

const std::string& sca_ccl::get_domain() const
{
	return domain;
}



} //namespace sca_eln

