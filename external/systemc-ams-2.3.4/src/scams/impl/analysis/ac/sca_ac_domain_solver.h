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

  sca_ac_domain_solver.h - description

  Original Author: Karsten Einwich Fraunhofer IIS Dresden / COSEDA Technologies

  Created on: 02.01.2010

   SVN Version       :  $Revision: 2048 $
   SVN last checkin  :  $Date: 2017-09-20 15:24:12 +0000 (Wed, 20 Sep 2017) $
   SVN checkin by    :  $Author: karsten $
   SVN Id            :  $Id: sca_ac_domain_solver.h 2048 2017-09-20 15:24:12Z karsten $

 *****************************************************************************/

/*****************************************************************************/


#ifndef SCA_AC_DOMAIN_SOLVER_H_
#define SCA_AC_DOMAIN_SOLVER_H_

#include "scams/impl/analysis/ac/sca_ac_domain_eq.h"

namespace sca_ac_analysis
{
namespace sca_implementation
{

class solve_linear_complex_eq_system
{
public:

	int solve(sca_util::sca_matrix<sca_util::sca_complex >& Ac,
	        sca_util::sca_vector<sca_util::sca_complex >& Bc,
	        sca_util::sca_vector<sca_util::sca_complex >& Xc);

	long critical_row;
	long critical_column;
};

class sca_ac_domain_solver
{
public:

    //Constructor
    sca_ac_domain_solver(sca_ac_domain_db& ac_db,bool noise_domain);

    //calculate signals for given frequency (omegas)
    void calculate(std::vector<double> omegas);

    //calculate signals for given frequency (omegas)
    void calculate_noise(std::vector<double> omegas);

    void solve_complex_eq_system(
        sca_util::sca_matrix<sca_util::sca_complex >& Ac,
        sca_util::sca_vector<sca_util::sca_complex >& Bc,
        sca_util::sca_vector<sca_util::sca_complex >& Xc
    );


private:

    void init_systemc();


    std::vector<sca_core::sca_implementation::sca_solver_base*> to_reg_ac_solver;

    void ac_elaborate();
    void register_ac_entities(sc_core::sc_object* obj);
    void hierarchic_disable_eln_lsf_solver(sc_core::sc_object* obj);

    void trace_init();
    void trace_init_noise(sca_util::sca_vector<std::string>& src_names);
    void trace(double w, sca_util::sca_vector<sca_util::sca_complex >& result);
    void trace_noise(double w, sca_util::sca_matrix<sca_util::sca_complex >& result);

    sca_ac_domain_db& ac_data;

    sca_ac_domain_eq  equations;


    //noise analysis
    void ac_noise_solver(
        sca_util::sca_matrix<sca_util::sca_complex >& Ac,
        sca_util::sca_vector<sca_util::sca_complex >& Bc,
        std::vector<bool>&                 noise_src_flags,
        sca_util::sca_matrix<sca_util::sca_complex >& result
    );

};

} // namespace sca_implementation
} // namespace sca_ac_analysis


#endif /* SCA_AC_DOMAIN_SOLVER_H_ */
