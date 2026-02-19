/*****************************************************************************

    Copyright 2010-2013
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

  sca_eln_tdf_vsink.cpp - description

  Original Author: Karsten Einwich Fraunhofer IIS/EAS Dresden

  Created on: 10.11.2009

   SVN Version       :  $Revision: 2320 $
   SVN last checkin  :  $Date: 2023-06-08 12:47:38 +0000 (Thu, 08 Jun 2023) $
   SVN checkin by    :  $Author: pehrlich $
   SVN Id            :  $Id: sca_eln_tdf_vsink.cpp 2320 2023-06-08 12:47:38Z pehrlich $

 *****************************************************************************/

/*****************************************************************************/


#include "systemc-ams"
#include "scams/predefined_moc/eln/sca_eln_tdf_vsink.h"
#include "scams/impl/util/data_types/sca_method_object.h"
#include "scams/impl/core/sca_solver_base.h"
#include "scams/impl/predefined_moc/conservative/sca_con_interactive_trace_data.h"

namespace sca_eln
{

namespace sca_tdf
{


const char* sca_vsink::kind() const
{
	return "sca_eln::sca_tdf::sca_vsink";
}


void sca_vsink::trace( sc_core::sc_trace_file* tf ) const
{
	std::ostringstream str;
	str << "sc_trace of sca_ln::sca_tdf::sca_vsink module not supported for module: " << this->name();
	SC_REPORT_WARNING("SystemC-AMS",str.str().c_str());
}


sca_vsink::sca_vsink(sc_core::sc_module_name, double scale_) :
	p("p"), n("n"), outp("outp"), scale("scale", scale_)
{
	unit="A";
	domain="I";
}


void sca_vsink::assign_result()
{
    if(!sca_ac_analysis::sca_ac_is_running())
    {
    	outp.write((x(p)-x(n))*scale);
    }
    else
    {
        sca_ac_analysis::sca_ac(outp) = scale * (x(p) - x(n));
    }
}

void sca_vsink::matrix_stamps()
{
	add_method(POST_SOLVE, SCA_VMPTR(sca_vsink::assign_result));
}

bool sca_vsink::trace_init(sca_util::sca_implementation::sca_trace_object_data& data)
{
    //trace will be activated after every complete cluster calculation
    //by teh synchronization layer
    return get_sync_domain()->add_solver_trace(data);
}
void sca_vsink::trace(long id,sca_util::sca_implementation::sca_trace_file_base& tf)
{
    sca_core::sca_time ctime = sca_eln::sca_module::get_time();

    double through_value = 0.0;
    tf.store_time_stamp(id,ctime,through_value);
}

void sca_vsink::trace_interactive()
{
	if(this->trd==NULL) return;

    double through_value = 0.0;

	this->trd->store_value(through_value);
	return;
}

sca_util::sca_complex sca_vsink::calculate_ac_result(sca_util::sca_complex* res_vec)
{
	return sca_util::sca_complex(0.0,0.0);
}

/**
   * experimental physical domain interface
*/
void sca_vsink::set_unit(const std::string& unit_)
{
	unit=unit_;
}

const std::string& sca_vsink::get_unit() const
{
	return unit;
}

void sca_vsink::set_unit_prefix(const std::string& prefix_)
{
	unit_prefix=prefix_;
}

const std::string& sca_vsink::get_unit_prefix() const
{
	return unit_prefix;
}

void sca_vsink::set_domain(const std::string& domain_)
{
	domain=domain_;
}

const std::string& sca_vsink::get_domain() const
{
	return domain;
}



} //namespace sca_tdf
} //namespace sca_eln
