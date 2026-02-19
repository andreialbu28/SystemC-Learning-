/*****************************************************************************

    Copyright 2010
    Fraunhofer-Gesellschaft zur Foerderung der angewandten Forschung e.V.

    Copyright 2015-2016
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

  sca_conservative_module.cpp - description

  Original Author: Karsten Einwich Fraunhofer IIS/EAS Dresden

  Created on: 04.11.2009

   SVN Version       :  $Revision: 1892 $
   SVN last checkin  :  $Date: 2016-01-10 11:59:12 +0000 (Sun, 10 Jan 2016) $
   SVN checkin by    :  $Author: karsten $
   SVN Id            :  $Id: sca_conservative_module.cpp 1892 2016-01-10 11:59:12Z karsten $

 *****************************************************************************/

/*****************************************************************************/

#include "systemc-ams"
#include "scams/impl/predefined_moc/conservative/sca_conservative_module.h"


namespace sca_core
{
namespace sca_implementation
{

const char* sca_conservative_module::kind() const
{
	return "sca_core::sca_implementation::sca_conservative_module";
}


void sca_conservative_module::construct()
{
	  //only base class - has to be assigned by derived class
	  view_interface = NULL;
	  cluster_id = -1;
	  reinit_request_cnt=0;

	  call_id_p=NULL;
	  call_id_request_init=0;
	  value_set=false;

	  mode3 = 0;
	  algorithm = 0;
	  each_interval = false;

	  n_add = -1;

	  pos_c1=0;
	  pos_c2=0;
	  pwl_xt1=0.0;
	  new_value=0.0;
	  diff=0.0;
	  pos_l=0;
	  p_is=0;
	  pwl_xt2=0.0;
	  pwl_x1=0.0;
	  pwl_x2=0.0;
	  n_is=0;
	  mod_params=NULL;

	  trd=NULL;
}

sca_conservative_module::sca_conservative_module()
{
  construct();
}


sca_conservative_module::sca_conservative_module(sc_core::sc_module_name)
{
	  construct();
}


//////////////////////////////////////////////

sca_conservative_module::~sca_conservative_module()
{
}

/**
 * reinitialization of equation system is forced
 */


//Remark: 	mode = 0: no reinit
//			mode = 1: reinit with Euler
//			mode = 2: reinit with Trapez
//			mode = 3: woodbury with one perturbed entry on diagonal
//			mode = 4: woodbury with one perturbed entry not on diagonal
//			mode = 5: woodbury with two perturbed entries in one line (+,-)
//			mode = 6: woodbury with four perturbed entries in (p,p),(p,n),(n,p),(n,n)
//			mode = 7: woodbury with two perturbed entries in one column (+,-)

///////////////////////////////////////////

void sca_conservative_module::request_reinit(int mode)
{
	long flag = 0;

	//flag = 1: reinit with Euler method
	//flag = 2: reinit with trapezoidal method
	//flag = 3: Woodbury with Euler method
	//flag = 4: Woodbury with trapezoidal method
	//flag = 5: no reinit or Woodbury, but next time step with Euler method

	if (mode == 0 && algorithm == 1) 	flag = 5; //no reinit, request Euler method
	if (mode == 1)	flag = 1;	//reinit with Euler method
	else if (mode == 2)	flag = 2;	//reinit with trapezoidal method
	else if (mode > 2)		//Woodbury Algorithm
	{
		mod_params->count_request_woodbury++;


		if (algorithm == 1) 	flag = 3;	//Woodbury with Euler method
		else	flag = 4;	//Woodbury with trapezoidal method
	}

	//priority: flag = 1, flag = 2, flag = 3, flag = 4, flag = 5
	if (mod_params->request_reinit != 1)
	{
		if (mod_params->request_reinit != 2)
		{
			if (mod_params->request_reinit != 3)
			{
				if (mod_params->request_reinit != 4)
				{
					mod_params->request_reinit = flag;
				}
				else if (flag < 4)
					mod_params->request_reinit = flag;
			}
			else if (flag < 3)
				mod_params->request_reinit = flag;
		}
		else if (flag < 2)
			mod_params->request_reinit = flag;
	}

	if (mode > 2) mode3 = mode;
	call_id_request_init=*call_id_p;

	reinit_request_cnt++;
	algorithm = 0;
}

///////////////////////////////////////////

void sca_conservative_module::request_reinit(int mode,double new_val)
{
	value_set=true;
	new_value=new_val;
	request_reinit(mode);
}

///////////////////////////////////////////

void sca_conservative_module::request_reinit(double new_val)
{
	request_reinit(1,new_val);
}

///////////////////////////////////////////

void sca_conservative_module::request_woodbury(int mode, int alg, double new_val, long n1, long n2, long n3, double new_diff)
{
	switch(mode) //check for negative indices
	{
	case 3:
		if (n1 < 0) return;
		break;

	case 4:
		if ((n1 < 0)  || (n2 < 0)) return;
		break;

	case 5:
		if (n1 < 0) return;
		else if (n2 < 0)
		{
			request_woodbury(4, alg, new_val, n1, n3, -1, -new_diff);
			return;
		}
		else if (n3 < 0)
		{
			request_woodbury(4, alg, new_val, n1, n2, -1, new_diff);
			return;
		}
		break;

	case 6:
		if (n1 < 0)
		{
			request_woodbury(3, alg, new_val, n2, n2, -1, new_diff);
			return;
		}
		else if (n2 < 0)
		{
			request_woodbury(3, alg, new_val, n1, n1, -1, new_diff);
			return;
		}
		break;

	case 7:
		if (n3 < 0) return;
		else if (n1 < 0)
		{
			request_woodbury(4, alg, new_val, n2, n3, -1, -new_diff);
			return;
		}
		else if (n2 < 0)
		{
			request_woodbury(4, alg, new_val, n1, n3, -1, new_diff);
			return;
		}
		break;
	}

	pos_l = n1;
	pos_c1 = n2;
	pos_c2 = n3;

	algorithm = alg;
	diff = new_diff;

	request_reinit(mode,new_val);
}

///////////////////////////////////////////

unsigned int sca_conservative_module::get_new_solver_state_idx()
{
	this->mod_params->number_of_solver_states++;

	return this->mod_params->number_of_solver_states-1;
}


void sca_conservative_module::set_solver_state(unsigned int idx,bool value)
{
	if(idx<64)
	{
		sc_dt::uint64 mask=(sc_dt::uint64(1)<<idx);
		bool current_state=((mask & mod_params->solver_state_idx) != 0);

		if(current_state!=value)
		{
			//negate bit
			mod_params->solver_state_idx ^= mask;
		}
	}
}



void sca_conservative_module::request_euler()
{
	algorithm = 1;
	request_reinit(0); //no wb and no reinit, but Euler cur_algorithm
}

///////////////////////////////////////////

void sca_conservative_module::request_reinit_and_decomposition_A()
{
	mod_params->request_decomp_A = true;


	request_reinit();
}

///////////////////////////////////////////

void sca_conservative_module::request_restore_checkpoint()
{
	if (mod_params->request_restore_checkpoint != 1)
		mod_params->request_restore_checkpoint = 2;	//neighbored intervals
}

///////////////////////////////////////////

void sca_conservative_module::request_restore_checkpoint(double lim1, double lim2, double limt1, double limt2, int p, int n)
{
	mod_params->request_restore_checkpoint = 1;		//dont skip intervals

	pwl_x1 = lim1;
	pwl_x2 = lim2;
	pwl_xt1 = limt1;
	pwl_xt2 = limt2;
	p_is = p;
	n_is = n;
	each_interval = true;
}

///////////////////////////////////////////

void sca_conservative_module::request_store_coefficients(int pos) //only for pwl controlled sources to store matrix B and vector q
{
	mod_params->request_store_coefficients = true;

	n_add = pos; //position of entry in vector q
}

///////////////////////////////////////////

bool sca_conservative_module::set_pre_solve_methods(double last_slope, double curr_slope, int mode, long n1, long n2, long n3, bool jump)
{
	bool get_jump = jump;

	static const double relerr=6.0*std::numeric_limits<double>::epsilon();
	bool test = diff_test(last_slope, curr_slope, relerr);

	if (!test || (fabs(curr_slope) < relerr)){
		request_reinit(1,curr_slope);
	}
	else
	{
		double diff_slope = last_slope - curr_slope;

		if (jump)	request_woodbury(mode,1,curr_slope, n1, n2, n3, diff_slope); //Euler method
		else 	request_woodbury(mode,2,curr_slope, n1, n2, n3, diff_slope); //trapezoidal method

		get_jump = false;
	}
	return get_jump;
}

///////////////////////////////////////////

bool sca_conservative_module::diff_test(double val1, double val2, double relerr)
{
	double diff = val1 - val2;
	diff += val2;

	if ((val1 == 0.0) && (fabs(diff) > relerr)) return false; //calculated difference not okay

	if (fabs(diff/val1 - 1.0) > relerr) return false; //calculated difference not okay
	else return true; //calculated difference okay
}

///////////////////////////////////////////

int sca_conservative_module::find_interval(double curr_val, sca_util::sca_vector<double> vector)
{
	int n_interval = -1;
	unsigned int length = vector.length();

	if (!(curr_val < vector(length - 2)))
		n_interval = length - 4;
	else
	{
		unsigned int i = 2;

		while (i < length)
		{
			if (curr_val < vector(i))
			{
				n_interval = i-2;
				break;
			}
			i += 2;
		}
	}

	return n_interval;
}

///////////////////////////////////////////

bool sca_conservative_module::check_interval(double curr_val, unsigned int curr_ind, sca_util::sca_vector<double> vector)
{
	bool check = true;

	if (curr_ind == 0)
	{
		if (curr_val < vector(2))
			check = true;
		else check = false;
	}
	else if (curr_ind == vector.length() - 4)
	{
		if (curr_val >= vector(curr_ind))
			check = true;
		else check = false;
	}
	else
	{
		if ((curr_val >= vector(curr_ind)) && (curr_val < vector(curr_ind+2)))
			check = true;
		else check = false;
	}

	return check;
}

///////////////////////////////////////////
//calc new index for pwl controlled source
int sca_conservative_module::calc_new_index(double curr_val, double calc_val, int curr_ind, sca_util::sca_vector<double> vector, int p, int n,  int last_working=-1)
{
	int new_index = find_interval(calc_val, vector);

	if (abs(curr_ind - new_index) == 2){ //neighbored intervals
		request_restore_checkpoint(); //carry out calculation for current timestep again
	}
	else if(new_index== curr_ind) //same intervall
	{
		int index_is= curr_ind;
		request_restore_checkpoint(vector(index_is), vector(index_is + 2), curr_val, calc_val, p, n);
	}
	else //dont skip any interval
	{

		int index_is;
		if(last_working == -1)
			last_working= curr_ind;
		if (new_index > last_working)
			index_is = last_working + 2;
		else
			index_is = last_working - 2;

		//divide current time step into smaller time steps in order to reach each interval
		//the following method commits the parameters needed for determining the new step size
		request_restore_checkpoint(vector(index_is), vector(index_is + 2), curr_val, calc_val, p, n);
	}

	return new_index;
}


}
}

