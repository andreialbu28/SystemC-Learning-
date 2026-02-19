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

  sca_conservative_module.h - description

  Original Author: Karsten Einwich Fraunhofer IIS/EAS Dresden

  Created on: 04.11.2009

   SVN Version       :  $Revision: 1892 $
   SVN last checkin  :  $Date: 2016-01-10 11:59:12 +0000 (Sun, 10 Jan 2016) $
   SVN checkin by    :  $Author: karsten $
   SVN Id            :  $Id: sca_conservative_module.h 1892 2016-01-10 11:59:12Z karsten $

 *****************************************************************************/

/*****************************************************************************/


#ifndef SCA_CONSERVATIVE_MODULE_H_
#define SCA_CONSERVATIVE_MODULE_H_

namespace sca_core
{
namespace sca_implementation
{

//parameters needed in linear solver
class request_parameters
{
public:
	long request_reinit;
	long count_request_woodbury;
	bool request_decomp_A;
	int request_restore_checkpoint;
	bool request_store_coefficients;

	//curent idx for stored solver states
	sc_dt::uint64 solver_state_idx;

	unsigned int number_of_solver_states;

	void reset_params()
	{
		request_reinit = 0;
		count_request_woodbury = 0;
		request_decomp_A = 0;
		request_restore_checkpoint = 0;
		request_store_coefficients = 0;

		solver_state_idx=0;
		number_of_solver_states=0;
	}
};

class sca_conservative_view;
class sca_con_interactive_trace_data;
/**
   Module base class for primitive modules
   of conservative networks.
*/
class sca_conservative_module : public sca_core::sca_module
{
	friend class sca_conservative_view;

	void construct();

  protected:
    sca_conservative_module();
    explicit sca_conservative_module(sc_core::sc_module_name);
    ~sca_conservative_module();

    virtual const char* kind() const;

    long cluster_id;

	void request_reinit(int mode=1); //reinitialization of equation system is forced
	void request_reinit(int mode,double new_val);
	void request_reinit(double new_val);
	void request_woodbury(int mode, int alg, double new_val, long n1, long n2, long n3, double new_diff); //Woodbury formula is forced
	void request_euler(); //a time step using the Euler method is forced
	void request_reinit_and_decomposition_A(); //decomposition of matrix A is forced (only once)
	void request_restore_checkpoint(); //repeat of the current time step is forced
	void request_restore_checkpoint(double lim1, double lim2, double limt1, double limt2, int p, int n); //repeat of the current time step is forced with dividing the current step size into smaller ones
	void request_store_coefficients(int pos); //storage of matrix B and vector is forced at every time step

	bool set_pre_solve_methods(double last_slope, double curr_slop, int mode, long n1, long n2, long n3, bool jump); //set request_woodbury or request_reinit
	bool diff_test(double val1, double val2, double relerr); //test for correctness of difference needed for Woodbury cur_algorithm

	int find_interval(double curr_val, sca_util::sca_vector<double> vector); //find interval in vector for pwl controlled sources
	bool check_interval(double curr_val, unsigned int curr_ind, sca_util::sca_vector<double> vector); //check index of interval for pwl controlled sources
	int calc_new_index(double curr_val, double calc_val, int curr_ind, sca_util::sca_vector<double> vector, int p, int n, int last_working); //calculate new index for jumps for pwl controlled sources


	/** gets new solver state index */
	unsigned int get_new_solver_state_idx();

	/**sets solver state*/
	void set_solver_state(unsigned int idx,bool value);

	sc_dt::uint64* call_id_p;

	sca_core::sca_implementation::request_parameters* mod_params;


  public:
    std::vector<long> add_equations;
	sc_dt::uint64 call_id_request_init;
	double        new_value;
	bool          value_set;

	//parameters needed for Woodbury formula
	int 		  mode3;
	int 		  algorithm;
	long 		  pos_l;
	long 		  pos_c1;
	long		  pos_c2;
	double 		  diff;

	//parameters needed for pwl controlled sources to prevent skipping intervals
	double		  pwl_x1, pwl_x2, pwl_xt1, pwl_xt2;
	int 		  p_is, n_is, n_add;
	bool 		  each_interval;

	unsigned long reinit_request_cnt;

	mutable sca_con_interactive_trace_data* trd;

};



} // namespace sca_core
} // namespace sca_implementation

#endif /* SCA_CONSERVATIVE_MODULE_H_ */
