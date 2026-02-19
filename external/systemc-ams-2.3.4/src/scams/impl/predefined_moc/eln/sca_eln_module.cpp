/*****************************************************************************

    Copyright 2010-2014
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

  sca_eln_module.cpp - description

  Original Author: Karsten Einwich Fraunhofer IIS/EAS Dresden

  Created on: 06.11.2009

   SVN Version       :  $Revision: 2127 $
   SVN last checkin  :  $Date: 2020-03-23 13:09:32 +0000 (Mon, 23 Mar 2020) $
   SVN checkin by    :  $Author: karsten $
   SVN Id            :  $Id: sca_eln_module.cpp 2127 2020-03-23 13:09:32Z karsten $

 *****************************************************************************/

/*****************************************************************************/

#include "systemc-ams"
#include "scams/predefined_moc/eln/sca_eln_module.h"
#include "scams/impl/util/data_types/sca_method_list.h"
#include "scams/impl/util/data_types/sca_function_vector.h"
#include "scams/impl/core/sca_view_base.h"
#include "scams/impl/solver/linear/sca_linear_equation_if.h"
#include "scams/impl/predefined_moc/eln/sca_eln_view.h"
#include "scams/impl/core/sca_solver_base.h"
#include <algorithm>
#include <iostream>

namespace sca_eln
{

void sca_module::print( std::ostream& str ) const
{
	if(dynamic_cast<const sca_util::sca_traceable_object*>(this)!=NULL)
	{
		str << get_typed_trace_value();
	}
	else
	{
		str << this->name();  //default behavior
	}
}

void sca_module::dump( std::ostream& str ) const
{
	if(dynamic_cast<const sca_util::sca_traceable_object*>(this)!=NULL)
	{
		str << this->kind() << " : " << this->name() << " current through module: "<< this->get_typed_trace_value();
	}
	else
	{
		str << this->kind() << " : " << this->name();  //default behavior
	}
}

//used by the sca_linnet_view to set references to eq-system and results
void sca_module::set_equations(
		    sca_core::sca_implementation::sca_linear_equation_system* equation_system,
			sca_util::sca_vector<double>& xs,
			double& ctime,
			sca_core::sca_implementation::request_parameters& params,
			sc_dt::uint64& call_id)
{
	Ai = &(equation_system->A);
	Bi = &(equation_system->B);
	qi = &(equation_system->q);

	pwl_stamps = &(equation_system->pwl_stamps);

	B_change =   &(equation_system->B_change);

	xi = &xs;
	current_time = &ctime;

	mod_methods = &(equation_system->lin_eq_if_methods);
	mod_params = &params;

	call_id_p=&call_id;
}

const char* sca_module::kind() const
{
	return "sca_eln::sca_module";
}

/**
 * access to the equation system
 */
const double& sca_module::A(long a, long b) const
{
	return (*Ai)(a, b);
}



const double& sca_module::B(long a, long b) const
{
	return (*Bi)(a, b);
}


double& sca_module::A(long a, long b)
{
	return (*Ai)(a, b);
}

double& sca_module::B_wr(long a, long b)
{
	if(enable_b_change)
	{
		if((a>=0) && (b>=0))
		{
			B_change->add_change(a,b,&((*Bi)(a, b)),continous);

			if(continous)
			{
				request_reinit(2);
			}
			else
			{
				request_reinit(1);
			}
		}
	}

	return (*Bi)(a, b);
}



void sca_module::add_pwl_stamp_to_B(
		unsigned long idy,
		unsigned long idx,
		const sca_util::sca_vector<std::pair<double,double> >& pwl_vector)
{
	//check for increasing x-value
	for(std::size_t i=1;i<pwl_vector.length();++i)
	{
		if(pwl_vector(i).first<=pwl_vector(i-1).first)
		{
			std::ostringstream str;
			str << "Piece wise linear (pwl) values must have an increasing x-value";
			str << " the " << i << "th. value: " << pwl_vector(i-1).first << " is larger or equal";
			str << " to the " << i+1 << "th. value: " << pwl_vector(i).first;
			str << " in: " << this->kind() << " of instance: " << this->name();
			SC_REPORT_ERROR("SystemC-AMS",str.str().c_str());
			return;
		}
	}

	if(pwl_stamps==NULL)
	{
		//pwl_stamps is assigned in sca_eln::sca_module::set_equations
		//and should be never used before
		SC_REPORT_ERROR("SystemC-AMS","Internal error - should not be possible");
		return;
	}

	std::vector<double> pwl_tmp(2*pwl_vector.length());

	for(unsigned long i=0;i<pwl_vector.length();++i)
	{
		pwl_tmp[2*i]   = pwl_vector(i).first;
		pwl_tmp[2*i+1] = pwl_vector(i).second;
	}

	pwl_stamps->add_pwl_stamp(idy,idx,pwl_tmp);
}

/**
 * B(idy,idx)=pwl(arg_idx)
 */
void sca_module::add_pwl_b_stamp_to_B(
		unsigned long idy,
		unsigned long idx,
		unsigned long arg_idx,
		const sca_util::sca_vector<std::pair<double,double> >& pwl_vector)
{
	if(pwl_stamps==NULL)
	{
		//pwl_stamps is assigned in sca_eln::sca_module::set_equations
		//and should be never used before
		SC_REPORT_ERROR("SystemC-AMS","Internal error - should not be possible");
		return;
	}

	std::vector<double> pwl_tmp(2*pwl_vector.length());

	for(unsigned long i=0;i<pwl_vector.length();++i)
	{
		pwl_tmp[2*i]   = pwl_vector(i).first;
		pwl_tmp[2*i+1] = pwl_vector(i).second;
	}

	pwl_stamps->add_pwl_b_stamp(idy,idx,arg_idx,pwl_tmp);
}


sca_util::sca_implementation::sca_method_list<double>& sca_module::q(long a)
{
	return (*qi)(a);
}

/**
 * result/state vector
 */
double& sca_module::x(long a)
{
	return (*xi)(a);
}

/**
 * returns current time
 */
double sca_module::sca_get_time()
{
	return *current_time;
}

/**
 * overall number of equations
 */
unsigned long sca_module::nequations()
{
	return (*Ai).n_cols();
}

bool sca_module::add_solver_trace(
		sca_util::sca_implementation::sca_trace_object_data& data)
{
	 return get_sync_domain()->add_solver_trace(data);
}

/**
 * add new equations ;
 * returns number (index starting by 0) of first new equation
 */
unsigned long sca_module::add_equation(unsigned long num)
{
	unsigned long n = nequations();
	unsigned long new_n = n + num;
	(*Ai).resize(new_n,new_n);
	(*Bi).resize(new_n,new_n);
	(*qi).resize(new_n);

	add_equations.push_back(n);

	return n;
}

/* add solve_method of type kind */
void sca_module::add_method(
		method_kinds kind, const sca_util::sca_implementation::sca_method_object<void>& method_obj)
{
	sca_util::sca_implementation::sca_method_vector* methods = NULL;

	switch (kind)
	{
	case PRE_SOLVE:
		methods = &mod_methods->pre_solve_methods;
		break;
	case PRE_SOLVE_REINIT:
		methods = &mod_methods->pre_solve_reinit_methods;
		break;
	case POST_SOLVE:
		methods = &mod_methods->post_solve_methods;
		break;
	case TEST:
		methods = &mod_methods->test_methods;
		break;
	case LOCAL_TEST:
		methods = &mod_methods->local_test_methods;
		break;
	}

	if (methods != NULL)
		methods->push_back(method_obj);

}

/* remove solve_method of type kind */
void sca_module::remove_method(
		method_kinds kind, const sca_util::sca_implementation::sca_method_object<void>& method_obj)
{
	sca_util::sca_implementation::sca_method_vector* methods = NULL;

	switch (kind)
	{
	case PRE_SOLVE:
		methods = &mod_methods->pre_solve_methods;
		break;
	case PRE_SOLVE_REINIT:
		methods = &mod_methods->pre_solve_reinit_methods;
		break;
	case POST_SOLVE:
		methods = &mod_methods->post_solve_methods;
		break;
	case TEST:
		methods = &mod_methods->test_methods;
		break;
	case LOCAL_TEST:
		methods = &mod_methods->local_test_methods;
		break;
	}

	if (methods != NULL)
	{
		sca_util::sca_implementation::sca_method_vector::iterator it;
		it=std::find(methods->begin(),methods->end(),method_obj);

		//schedule for removal -> request_reinit required for final remove
		if(it!=(mod_methods->post_solve_methods).end()) it->clear();
	}
}

sca_module::sca_module()
{
    //assign module to linnet - view
    view_interface = new sca_eln::sca_implementation::sca_eln_view;

    continous=false;
    enable_b_change=false;

    pwl_stamps=NULL;

    mod_methods=NULL;
    q_change=NULL;
    qi=NULL;
    B_change=NULL;
    Ai=NULL;
    xi=NULL;
    Bi=NULL;
    current_time=NULL;
}


///////////////////////////////////////////////////////////////////////////////

sca_module::~sca_module()
{
    delete view_interface;
}


const double& sca_module::get_typed_trace_value() const
{
	static const double dummy=0.0;
	return dummy;
}

///////////////////////////////////////////////////////////////////////////////


} // namespace sca_eln
