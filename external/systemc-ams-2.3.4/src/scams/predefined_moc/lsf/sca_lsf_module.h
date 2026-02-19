/*****************************************************************************

    Copyright 2010
    Fraunhofer-Gesellschaft zur Foerderung der angewandten Forschung e.V.


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

  sca_lsf_module.h - description

  Original Author: Karsten Einwich Fraunhofer IIS/EAS Dresden

  Created on: 05.01.2010

   SVN Version       :  $Revision: 2048 $
   SVN last checkin  :  $Date: 2017-09-20 15:24:12 +0000 (Wed, 20 Sep 2017) $
   SVN checkin by    :  $Author: karsten $
   SVN Id            :  $Id: sca_lsf_module.h 2048 2017-09-20 15:24:12Z karsten $

 *****************************************************************************/


/*****************************************************************************/


#ifndef SCA_LSF_MODULE_H_
#define SCA_LSF_MODULE_H_

namespace sca_util
{

namespace sca_implementation
{

template<class T>
class sca_method_object;

template<class T>
class sca_method_list;

template<class T>
class sca_function_vector;

class sca_method_vector;
class sca_change_matrix;
class sca_pwl_stamps;

}

}

namespace sca_core
{
namespace sca_implementation
{
class sca_linear_equation_system;
}
}



namespace sca_lsf
{

class sca_signal;

namespace sca_implementation
{
class lin_eqs_cluster;
class sca_lsf_view;
}

class sca_signal_if;

/**
 * Base class for modules for a electrical linear net.
 * The equation system to be solved is:
 *                  A dx/dt  + B x = q(t)
 */
class sca_module: public sca_core::sca_implementation::sca_conservative_module
{
public:

	virtual const char* kind() const;


private:
	//used by the sca_eln_view to set references to eq-system and results
	void set_equations(
			sca_core::sca_implementation::sca_linear_equation_system* equation_system,
			sca_util::sca_vector<double>& xs,
			double& ctime,
			sca_core::sca_implementation::request_parameters& params,
			sc_dt::uint64& call_id);

protected:

	//constructor
	sca_module();

	//destructor
	virtual ~sca_module();

	virtual void matrix_stamps() = 0;

	/**
	 * access to the equation system
	 */
	double& A(long a, long b);
	double& B(long a, long b);
	const double& A(long a, long b) const;
	const double& B(long a, long b) const;
	sca_util::sca_implementation::sca_method_list<double>& q(long a);


	void add_pwl_stamp_to_B(
			unsigned long idy,
			unsigned long idx,
			const sca_util::sca_vector<std::pair<double,double> >& pwl_vector);

	void add_pwl_b_stamp_to_B(
			unsigned long idy,
			unsigned long idx,
			unsigned long arg_idx,
			const sca_util::sca_vector<std::pair<double,double> >& pwl_vector);

	/**
	 * result/state vector
	 */
	double& x(long a);

	/**
	 * returns current time
	 */
	double sca_get_time();

	/**
	 * overall number of equations
	 */
	unsigned long nequations();

	/**
	 * add new equations ;
	 * returns number (index starting by 0) of first new equation
	 */
	unsigned long add_equation(unsigned long num = 1);


	//redirect to base class to prevent friend declaration  ... see above
	sca_core::sca_time get_time()
	{
		return sca_core::sca_module::sca_get_time();
	}

	/* kinds of solve_methods:
	 *PRE_SOLVE: pre_solve_method - update matrix stamps or set parameters for Woodbury cur_algorithm before
	 *PRE_SOLVE_REINIT: pre_solve_reinit_method - update matrix stamps before
	 *POST_SOLVE: post_solve_method - update matrix stamps after solve
	 *TEST: test_method - update matrix stamps after solve after checking intervals of pwl controlled sources
	 *LOCAL_TEST: local_test_method - update matrix stamps at intermediate time steps after checking intervals of pwl controlled sources
	 */
	enum method_kinds {PRE_SOLVE, PRE_SOLVE_REINIT, POST_SOLVE, TEST, LOCAL_TEST};

	/**
	 * add  and remove method which will be called before or after the solver for each time step
	 * can be used to schedule reinitialization and Woodbury cur_algorithm
	 */
	void add_method(method_kinds kind, const sca_util::sca_implementation::sca_method_object<void>&);
	void remove_method(method_kinds kind, const sca_util::sca_implementation::sca_method_object<void>&);

	bool enable_b_change;
	bool continous;


private:

	sca_util::sca_matrix<double>* Ai;
	sca_util::sca_matrix<double>* Bi;
	sca_util::sca_implementation::sca_function_vector<double>* qi;

	sca_util::sca_implementation::sca_pwl_stamps* pwl_stamps;

    sca_util::sca_implementation::sca_change_matrix* B_change;

	sca_util::sca_vector<double>* xi;

	sca_util::sca_implementation::sca_solve_methods* mod_methods; /* solve_methods */

	double* current_time;

	friend class sca_lsf::sca_implementation::lin_eqs_cluster;
	friend class sca_lsf::sca_implementation::sca_lsf_view;
	friend class sca_lsf::sca_signal;
};

}



#endif /* SCA_LSF_MODULE_H_ */
