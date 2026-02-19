/*****************************************************************************

    Copyright 2010-2014
    Fraunhofer-Gesellschaft zur Foerderung der angewandten Forschung e.V.

    Copyright 2015-2020
    COSEDA Technologies GmbH


 *****************************************************************************/

/*****************************************************************************

  sca_eln_ccc.cpp - description

  Original Author: Karsten Einwich Fraunhofer IIS/EAS Dresden

  Created on: 13.05.2014

   SVN Version       :  $Revision: 1703 $
   SVN last checkin  :  $Date: 2014-04-23 12:10:42 +0200 (Wed, 23 Apr 2014) $
   SVN checkin by    :  $Author: karsten $
   SVN Id            :  $Id: sca_eln_r.cpp 1703 2014-04-23 10:10:42Z karsten $

 *****************************************************************************/

/*****************************************************************************/


#include "scams/predefined_moc/eln/sca_eln_ccc.h"
#include "scams/impl/util/data_types/sca_method_object.h"
#include "scams/impl/util/data_types/sca_method_list.h"
#include "scams/impl/predefined_moc/conservative/sca_con_interactive_trace_data.h"

namespace sca_eln
{
sca_ccc::sca_ccc(sc_core::sc_module_name,
		const sca_util::sca_vector<std::pair<double,double> >& pwc_value_) :
		p("p"), n("n"), q0("q0",0.0), pwc_vector("pwc_value", pwc_value_)
{
    nadd1=-1;
    nadd2=-1;
    nadd3=-1;
    nadd4=-1;

    dc_init=true;

	unit="A";
	domain="I";
}

const char* sca_ccc::kind() const
{
	return "sca_eln::sca_ccc";
}


void sca_ccc::trace( sc_core::sc_trace_file* tf ) const
{
	std::ostringstream str;
	str << "sc_trace of sca_ccc module not supported for module: " << this->name();
	SC_REPORT_WARNING("SystemC-AMS",str.str().c_str());
}


void sca_ccc::matrix_stamps()
{
	    ///////////////////////////////////////////////////////////////

		//nadd1 - current
		nadd1  = add_equation(4); // i
		//nadd2 - charge
		nadd2 = nadd1+1;   // q
		nadd3 = nadd2+1;   // v(p,n)
		nadd4 = nadd3+1;   // i(cp,cn)


		//first timestep
		if(!sca_ac_analysis::sca_ac_is_running() && dc_init)
		{
			if (q0.get() == sca_util::SCA_UNDEFINED) //remove capacitor
			{
				B_wr(nadd1, nadd1) = -1.0; //i=0

			}
			else  //add voltage source
			{
				B_wr(p, nadd1) = 1.0;
				B_wr(n, nadd1) = -1.0;

				// q=q0
			    B_wr(nadd1,nadd2)     =  1.0;
			    q(nadd1).set_value(-q0.get());
			}

			add_method(POST_SOLVE, SCA_VMPTR(sca_ccc::post_solve));

			dc_init=false;
		}
		else  //for restore for / after AC
		{
			//current contribution to node p and n
			B_wr(p,nadd1) =  1.0;
			B_wr(n,nadd1) = -1.0;

		    // i = dq - make current traceable
		    B_wr(nadd1,nadd1)  = -1.0;
		    A(nadd1,nadd2)    =  1.0;

		}


	    //nadd3 = v(p) - v(n)
	    B_wr(nadd3,nadd3) = -1.0;
	    B_wr(nadd3,p)     =  1.0;
	    B_wr(nadd3,n)     = -1.0;


	    // nadd4 = i(cp,cn)
	    B_wr(cp,nadd4) = 1.0;
	    B_wr(cn,nadd4) = -1.0;

	    //short cut v(cp) - v(cn) = 0
	    B_wr(nadd4,cp)    =  1.0;
	    B_wr(nadd4,cn)    = -1.0;

	    // q = C * v
	    B_wr(nadd2, nadd2) = -1.0;
	    B_wr(nadd2, nadd3) =  1.0;

	    //q=C(vc) * v
	    add_pwl_b_stamp_to_B(nadd2,nadd3,nadd4,pwc_vector.get());
}



void sca_ccc::post_solve() //set capacitor stamps after first step
{
	//reset dc stamps
	B_wr(nadd1, nadd1) = 0.0;
	B_wr(nadd1,nadd2)  = 0.0;
    q(nadd1).set_value(0.0);

    remove_method(POST_SOLVE, SCA_VMPTR(sca_ccc::post_solve));

	//current contribution to node p and n
	B_wr(p,nadd1) =  1.0;
	B_wr(n,nadd1) = -1.0;

    // i = dq - make current traceable
    B_wr(nadd1,nadd1)  = -1.0;
    A(nadd1,nadd2)     =  1.0;

    request_reinit(1);

	dc_init=false;

}




bool sca_ccc::trace_init(sca_util::sca_implementation::sca_trace_object_data& data)
{
    //trace will be activated after every complete cluster calculation
    //by the synchronization layer
    return this->add_solver_trace(data);
}

void sca_ccc::trace(long id,sca_util::sca_implementation::sca_trace_file_base& tf)
{
    sca_core::sca_time ctime = sca_eln::sca_module::get_time();

    tf.store_time_stamp(id,ctime,x(nadd1));
}

void sca_ccc::trace_interactive()
{
	if(this->trd==NULL) return;

	this->trd->store_value(x(nadd1));
	return;
}


sca_util::sca_complex sca_ccc::calculate_ac_result(sca_util::sca_complex* res_vec)
{
    return res_vec[nadd1];
}


/**
   * experimental physical domain interface
*/
void sca_ccc::set_unit(const std::string& unit_)
{
	unit=unit_;
}

const std::string& sca_ccc::get_unit() const
{
	return unit;
}

void sca_ccc::set_unit_prefix(const std::string& prefix_)
{
	unit_prefix=prefix_;
}

const std::string& sca_ccc::get_unit_prefix() const
{
	return unit_prefix;
}

void sca_ccc::set_domain(const std::string& domain_)
{
	domain=domain_;
}

const std::string& sca_ccc::get_domain() const
{
	return domain;
}

} //namespace sca_eln

