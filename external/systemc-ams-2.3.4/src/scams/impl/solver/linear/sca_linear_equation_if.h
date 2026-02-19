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

  sca_linear_equation_if.h - description

  Original Author: Karsten Einwich Fraunhofer IIS/EAS Dresden

  Created on: 06.11.2009

   SVN Version       :  $Revision: 1706 $
   SVN last checkin  :  $Date: 2014-04-23 13:50:03 +0000 (Wed, 23 Apr 2014) $
   SVN checkin by    :  $Author: karsten $
   SVN Id            :  $Id: sca_linear_equation_if.h 1706 2014-04-23 13:50:03Z karsten $

 *****************************************************************************/

/*****************************************************************************/

#include "systemc-ams"
#include "scams/impl/util/data_types/sca_method_list.h"
#include "scams/impl/util/data_types/sca_function_vector.h"
#include "scams/impl/util/data_types/sca_change_matrix.h"
#include "scams/impl/util/data_types/sca_pwl_stamps.h"


#ifndef SCA_LINEAR_EQUATION_IF_H_
#define SCA_LINEAR_EQUATION_IF_H_

namespace sca_core
{
namespace sca_implementation
{

class sca_linear_equation_system
{
  public:

     sca_util::sca_matrix<double>   A;
     sca_util::sca_matrix<double>   B;
     sca_util::sca_implementation::sca_function_vector<double> q;

     sca_util::sca_implementation::sca_pwl_stamps pwl_stamps;

     sca_util::sca_implementation::sca_change_matrix B_change;

     sca_util::sca_implementation::sca_solve_methods lin_eq_if_methods;
     sca_util::sca_implementation::sca_method_vector reinit_methods;
};

/**
 * equation interface is implemented by sca_linnet_view
 * and used by sca_linear_solver
 */
class sca_linear_equation_if
{
 public:

  virtual sca_linear_equation_system& get_equation_system() = 0;
  virtual void reinit_equations()                = 0;

  virtual long& get_reinit_request()             = 0;
  virtual long& get_count_request_woodbury()     = 0;
  virtual bool& get_request_decomp_A()			 = 0;
  virtual int& get_request_restore_checkpoint()	 = 0;
  virtual bool& get_request_store_coefficients() = 0;

  virtual ~sca_linear_equation_if() {};
};



}
}


#endif /* SCA_LINEAR_EQUATION_IF_H_ */
