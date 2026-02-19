/*****************************************************************************

    Copyright 2010
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

  sca_ac_domain_entity.h - description

  Original Author: Karsten Einwich COSEDA Technologies GmbH

  Created on: 02.01.2010

   SVN Version       :  $Revision: 2320 $
   SVN last checkin  :  $Date: 2023-06-08 12:47:38 +0000 (Thu, 08 Jun 2023) $
   SVN checkin by    :  $Author: pehrlich $
   SVN Id            :  $Id: sca_ac_domain_entity.h 2320 2023-06-08 12:47:38Z pehrlich $

 *****************************************************************************/

/*****************************************************************************/


#ifndef SCA_AC_DOMAIN_ENTITY_H_
#define SCA_AC_DOMAIN_ENTITY_H_

#include "scams/impl/analysis/ac/sca_ac_domain_globals.h"


namespace sca_core
{
namespace sca_implementation
{
	class sca_solver_base;
}
}


namespace sca_ac_analysis
{
namespace sca_implementation
{
class sca_ac_domain_db;

class sca_ac_domain_entity
{
    // wrapper for callback methods
    bool call_ac_domain_method();
    bool call_add_eq_method();

    bool call_add_eq_outp_con_method
      (sca_util::sca_matrix<sca_util::sca_complex >*& con_matr,
       sca_util::sca_vector<sca_util::sca_complex >& y);

public:

    // compute matrices A and B
    void computeAB(double w);

    // compute matrices A and B and insert them to the global ones
    void computeAB_insert(sca_util::sca_matrix<sca_util::sca_complex >& Ag,
    		              sca_util::sca_vector<sca_util::sca_complex >& Bg,
                          double w);

    // setup additional equations
    void setup_add_eqs(sca_util::sca_matrix<sca_util::sca_complex >& Ag,
    		           sca_util::sca_vector<sca_util::sca_complex >& Bg,
                       double w);

    // initializes all ports of the entity (solver / module)
    void initialize();

    // initializes start eq-num. of the global eq-system for
    // additional (e.g. network) equations;
    // returns number of additional equations
    unsigned long init_additional_equations(unsigned long eq_start);

    // constructors
    //an ac-node can be a sc_module or a solver only
    sca_ac_domain_entity(sc_core::sc_module* mod);
    sca_ac_domain_entity(sca_core::sca_implementation::sca_solver_base* solv);

    // destructor
    ~sca_ac_domain_entity();

    sca_ac_domain_method         ac_domain_method;
    sca_add_ac_domain_eq_method  add_eq_method;
    sca_calc_add_eq_cons_method  calc_add_eq_cons_method;

    bool                         noise_src;

    sca_ac_domain_db* ac_db;

private:

    friend class sca_ac_domain_db;

    void determine_ac_module_ports();
    void determine_ac_solver_ports();

    // stores arc id's of connected in-/outports
    std::vector<long> inport_arcs;
    std::vector<long> outport_arcs;

    // matrices for y = Ax + B
    sca_util::sca_matrix<sca_util::sca_complex > A;
    sca_util::sca_vector<sca_util::sca_complex > B;


    unsigned long number_of_add_eqs;
    unsigned long start_of_add_eqs;

    // references to additional eq-system (Ae*jw + Be)*x + qe = 0
    sca_util::sca_matrix<double>* Ae;
    sca_util::sca_matrix<double>* Be;
    sca_util::sca_vector<sca_util::sca_complex >* qe;

    sc_core::sc_module*        module;
    sca_core::sca_implementation::sca_solver_base*  solver;

    sca_ac_analysis::sca_ac_object* obj;
};




} // namespace sca_implementation
} // namespace sca_ac_analysis

#endif /* SCA_AC_DOMAIN_ENTITY_H_ */
