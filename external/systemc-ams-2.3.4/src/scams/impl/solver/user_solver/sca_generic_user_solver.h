/*****************************************************************************

    Copyright 2010-2014
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

  sca_generalized_solver.h - description

  Original Author: Karsten Einwich Fraunhofer IIS/EAS Dresden

  Created on: 8.01.2014

   SVN Version       :  $Revision: 1620 $
   SVN last checkin  :  $Date: 2013-08-30 14:48:25 +0200 (Fri, 30 Aug 2013) $
   SVN checkin by    :  $Author: karsten $
   SVN Id            :  $Id: sca_solver_base.h 1620 2013-08-30 12:48:25Z karsten $

 *****************************************************************************/




/*****************************************************************************/

#ifndef SCA_GENERIC_USER_SOLVER_H_
#define SCA_GENERIC_USER_SOLVER_H_


#include "scams/impl/core/sca_solver_base.h"


namespace sca_core
{
namespace sca_implementation
{

/**
  Solver class for a generalized generic (user defined) solver
*/
class sca_generic_user_solver : public sca_core::sca_implementation::sca_solver_base
{

public:

	sca_generic_user_solver(
			std::vector<sca_module*>& mods,
			std::vector<sca_interface*>& chans,
			sca_core::sca_user_solver_base* solver);

	virtual ~sca_generic_user_solver();

    /** Implements the pure virtual initialize method of @ref #sca_solver_base. */
    void initialize();

    /** Implements the pure virtual print_post_solve_statisitcs method of @ref #sca_solver_base. */
    void print_post_solve_statisitcs();


    virtual const char* kind() const;

    sca_util::sca_implementation::sca_trace_object_data& get_trace_obj_data(int id);


    //removes compiler warning due virtual function in sc_object
    void trace( sc_core::sc_trace_file* tf ) const;

    void send_trace_to_file();

    const std::vector<sca_core::sca_module*>&    get_associated_modules();
    const std::vector<sca_core::sca_interface*>& get_associated_channels();

private:

    /** set implementation defined solver parameter */
    void set_solver_parameter(
    		  sca_core::sca_module* mod,
    		  const std::string& par,
    		  const std::string& val);

    sca_core::sca_user_solver_base* user_solver;


    /**
     * called by the sca_traceable object trace_init callback to assign a trace to the solver
     * returns true if successful
     */
    bool add_solver_trace(sca_util::sca_implementation::sca_trace_object_data& tr_obj);



    std::vector<sca_util::sca_implementation::sca_trace_object_data*> trace_objects;
};


} // namespace sca_implementation
} // namespace sca_core





#endif /* SCA_GENERALIZED_SOLVER_H_ */
