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

  sca_solve_ac_linear.h - description

  Original Author: Karsten Einwich Fraunhofer IIS/EAS Dresden

  Created on: 02.01.2010

   SVN Version       :  $Revision: 1444 $
   SVN last checkin  :  $Date: 2012-12-13 17:38:40 +0000 (Thu, 13 Dec 2012) $
   SVN checkin by    :  $Author: karsten $
   SVN Id            :  $Id: sca_solve_ac_linear.h 1444 2012-12-13 17:38:40Z karsten $

 *****************************************************************************/

/*****************************************************************************/


#ifndef SCA_SOLVE_AC_LINEAR_H_
#define SCA_SOLVE_AC_LINEAR_H_

#ifdef __cplusplus
extern "C"
{
#endif

struct sparse;


struct sca_solve_ac_linear_data;
int sca_solve_ac_linear_init(struct  sparse* A,
                               struct sca_solve_ac_linear_data** data);
int sca_solve_ac_linear(double* B, double* x,struct sca_solve_ac_linear_data** data);
void sca_solve_ac_linear_free(struct sca_solve_ac_linear_data** data);
void sca_solve_ac_get_error_position(struct sca_solve_ac_linear_data* data,long* row,long* column);

#ifdef __cplusplus
}
#endif

#endif /* SCA_SOLVE_AC_LINEAR_H_ */
