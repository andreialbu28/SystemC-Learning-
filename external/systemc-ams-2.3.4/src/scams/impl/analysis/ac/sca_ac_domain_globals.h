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

  sca_ac_domain_globals.h - description

  Original Author: Karsten Einwich COSEDA Technologies GmbH

  Created on: 02.01.2010

   SVN Version       :  $Revision: 2320 $
   SVN last checkin  :  $Date: 2023-06-08 12:47:38 +0000 (Thu, 08 Jun 2023) $
   SVN checkin by    :  $Author: pehrlich $
   SVN Id            :  $Id: sca_ac_domain_globals.h 2320 2023-06-08 12:47:38Z pehrlich $

 *****************************************************************************/

/*****************************************************************************/


#ifndef SCA_AC_DOMAIN_GLOBALS_H_
#define SCA_AC_DOMAIN_GLOBALS_H_


namespace sca_ac_analysis
{
namespace sca_implementation
{

typedef void (sca_ac_analysis::sca_ac_object::*sca_ac_domain_method)();
typedef void (sca_ac_analysis::sca_ac_object::*sca_add_ac_domain_eq_method)(
                                              sca_util::sca_matrix<double>*& A,
                                              sca_util::sca_matrix<double>*& B,
                                              sca_util::sca_vector<sca_util::sca_complex >*& q
                                                      );

typedef void (sca_ac_analysis::sca_ac_object::*sca_calc_add_eq_cons_method)
                                     ( sca_util::sca_matrix<sca_util::sca_complex >*& con_matr,
                                       sca_util::sca_vector<sca_util::sca_complex >& y );

//methods must be member functions of the registrated sc_module/sca_solver_base
void sca_ac_domain_register_entity(sc_core::sc_module*                    mod,
                                   sca_ac_domain_method          ac_fct,
                                   sca_add_ac_domain_eq_method   add_eq  =NULL,
                                   sca_calc_add_eq_cons_method   add_cons=NULL,
                                   bool                          noise_src=false
                                  );


void sca_ac_domain_register_entity(sca_core::sca_implementation::sca_solver_base*   mod,
                                   sca_ac_domain_method          ac_fct,
                                   sca_add_ac_domain_eq_method   add_eq  =NULL,
                                   sca_calc_add_eq_cons_method   add_cons=NULL,
                                   bool                          noise_src=false
                                  );

int sca_ac_port_direction(sc_core::sc_port_base* port);


void sca_ac_register_arc(sc_core::sc_interface* sca_if);
void sca_ac_register_add_eq_arc(sc_core::sc_interface* sca_if,
                                long* rel_pos,
                                bool* ignore_flag = NULL);

sca_ac_domain_db& get_ac_database();

sca_util::sca_vector<double>& generate_frequencies(
													sca_util::sca_vector<double>& frequencies,
													double startf,
													double endf,
													unsigned long npoints,
													sca_ac_analysis::sca_ac_scale spacet);


} // namespace sca_implementation
} // namespace sca_ac_analysis

#endif /* SCA_AC_DOMAIN_GLOBALS_H_ */
