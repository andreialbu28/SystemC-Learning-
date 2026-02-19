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

 sca_linear_solver.h - description

 Original Author: Karsten Einwich COSEDA Technologies GmbH

 Created on: 09.11.2009

 SVN Version       :  $Revision: 2320 $
 SVN last checkin  :  $Date: 2023-06-08 12:47:38 +0000 (Thu, 08 Jun 2023) $
 SVN checkin by    :  $Author: pehrlich $
 SVN Id            :  $Id: sca_linear_solver.h 2320 2023-06-08 12:47:38Z pehrlich $

 *****************************************************************************/

/*****************************************************************************/

#ifndef SCA_LINEAR_SOLVER_H_
#define SCA_LINEAR_SOLVER_H_

#include "scams/impl/core/sca_solver_base.h"
#include "scams/utility/data_types/sca_matrix.h"
#include "scams/impl/solver/linear/sca_linear_equation_if.h"
#include "scams/impl/util/data_types/sca_method_list.h"
#include "scams/impl/util/data_types/sca_function_vector.h"
#include "scams/impl/solver/util/sparse_library/linear_analog_solver.h"


#ifndef DISABLE_PERFORMANCE_STATISTICS
#include <chrono>
#endif

struct sca_solv_data;
struct sca_solv_checkpoint_data;
struct sca_coefficients;

namespace sca_core
{
namespace sca_implementation
{

/**
  Solver class for an analog linear solver.
*/
class sca_linear_solver : public sca_core::sca_implementation::sca_solver_base

{

public:

	sca_linear_solver(std::vector<sca_module*>& mods, std::vector<sca_interface*>& chans);
	virtual ~sca_linear_solver();

    /** Implements the pure virtual initialize method of @ref #sca_solver_base. */
    void initialize();

    void assign_equation_system(sca_core::sca_implementation::sca_linear_equation_if& eqs);

    sca_util::sca_vector<double>& get_state_vector() { return x; }

    sc_core::sc_object* get_object_of_equation(long eq_nr);

    void print_post_solve_statisitcs();

    double current_time;
    sc_dt::uint64 call_id;

    //sca_linear_solver_utilities.cpp

    void store_checkpoint(); //store global x and xp
    void restore_checkpoint(); //restore global x and xp

    virtual const char* kind() const;


    //Methods for supporting ac-simulation
    void ac_domain_eq_method( sca_util::sca_matrix<double>*& A_o,
                              sca_util::sca_matrix<double>*& B_o,
                              sca_util::sca_vector<sca_util::sca_complex >*& q_o );

    void ac_add_eq_cons_method( sca_util::sca_matrix<sca_util::sca_complex >*& con_matr,
                                           sca_util::sca_vector<sca_util::sca_complex >& y );

private:

    /** set implementation defined solver parameter */
    void set_solver_parameter(
    		  sca_core::sca_module* mod,
    		  const std::string& par,
    		  const std::string& val);

    //variables to permit check of inconsistent double setting
    bool                  force_implicit_euler_method;
    int                   reinitialization_steps;
    bool                  algorithm_set;
    std::string           algorithm_value;
    sca_core::sca_module* algorithm_module;


    std::string get_name_associated_names(int max_num=-1) const;

    void init_eq_system(); //initialize equation system
    void solve_eq_system(); //solve equation system at current time

    void iterate_pwl_intervalls();
    bool do_pwl_iteration(int& check_mode,double& timestep);


    void error_message(int error, int method, double n_dt); //error message for errors during (re)initialization and Woodbury formula
    void call_methods(sca_util::sca_implementation::sca_method_vector* methods); //call methods


    //reference to equation system
    sca_core::sca_implementation::sca_linear_equation_system*              equations;
    sca_core::sca_implementation::sca_linear_equation_if*                  equation_if;

    sca_util::sca_matrix<double>*   A;
    sca_util::sca_matrix<double>*   B;
    sca_util::sca_implementation::sca_function_vector<double>* q;
    sca_util::sca_implementation::sca_pwl_stamps*              pwl_stamps;

    sca_util::sca_implementation::sca_change_matrix* B_change;

    sca_util::sca_implementation::sca_solve_methods* lin_solver_methods; /* solve_methods */
    sca_util::sca_implementation::sca_method_vector* reinit_methods;

    sca_util::sca_vector<double>  x;  //state vector

    double*           x_flat;

    double dt;          // current time step
    double dt_global;   // current global timestep
    double pwl_dt;  //last pwl timestep
    double pwl_dt_last;  //last pwl timestep for interpolation
    double checkp_dt; // timestep at checkpoint
    double reinit_dt; // last time step with reinit

    bool cp_restored;  //checkpoint restored
    bool first_timestep; //first timestep

    unsigned long statistics_pwl_segment_changes;
    unsigned long statistics_pwl_timestep_changes;
    unsigned long statistics_max_pwl_segment_iterations;
    unsigned long statistics_max_pwl_timestep_iterations;
    unsigned long statistics_cur_pwl_segment_iterations;
    unsigned long statistics_cur_pwl_timestep_iterations;
    double        statistics_pwl_min_timestep;

    //for check point restore with pwl to ensure that reinit mode is the same
    unsigned long last_reinit_flag;



    sca_solv_data* internal_solver_data; //solver data to solve equation system



    sca_util::sca_vector<sca_util::sca_complex > q_ac;
    sca_util::sca_matrix<sca_util::sca_complex > con_matr_ac;

    bool ac_equation_initialized;

    unsigned long number_of_timesteps; //number of all timesteps (without intermediate time steps)
    unsigned long number_of_reinit; //number of reinitializations

    //PWL data


    struct pwl_data
    {
    	// eq_idx : offset(x[x_idx]) = B[eq_idx,x_idx](x[x_idx]) * x[x_idx] + ...
    	unsigned long x_idx;   //index of variable
    	unsigned long eq_idx;  //index of pwl equation
    	unsigned long arg_idx; //index argument pwl function

        std::vector<double> x_values;         //variable values
        std::vector<double> b_coefficients;   //corresponding b coefficient
        std::vector<double> offsets;          //corresponding offset

        unsigned long n_segments;

        unsigned long last_valid_segment;
        unsigned long current_segment;
        unsigned long new_segment;


        pwl_data() : x_idx(0), eq_idx(0), arg_idx(0), n_segments(0),
        		last_valid_segment(0), current_segment(0), new_segment(0)
        {}
    };

    //current q for pwl iteration
    double* q_current;
    //pointer to interpolated current q memory
    double* q_current_tmp;

    //nex/last q for pwl_iteration
    std::vector<double> q_last;
    std::vector<double> q_next;

    //reached time for current step
    double reached_dt;



    void init_pwl_data(bool ac);
    int check_pwl_intervals(int mode);
    void search_valid_segments(double* x);

    //used in pwl iteration
    void change_pwl_coefficients();

    //interpolates B and q coefficients from last global time to dtime
    void interpolate_B_q(double dtime);

    std::vector<pwl_data> pwl_coefficients;

    bool pwl_coeff_available;

    //prints error message for solv_eq_system
    void print_reinitialization_error();

    sca_solv_checkpoint_data* global_cp;

    sca_solv_checkpoint_data* pwl_iteration_cp;

#ifndef DISABLE_PERFORMANCE_STATISTICS
	std::uint64_t activation_cnt=0;
	std::chrono::time_point<std::chrono::high_resolution_clock> start;
	std::chrono::duration<double> duration;
#endif

	bool collect_profile_data;


};


} // namespace sca_implementation
} // namespace sca_core



#endif /* SCA_LINEAR_SOLVER_H_ */
