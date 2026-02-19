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

 ana_reinit.c - description

 Original Author: Karsten Einwich Fraunhofer IIS/EAS Dresden

 Created on: 22.10.2009

 SVN Version       :  $Revision: 2101 $
 SVN last checkin  :  $Date: 2020-02-21 12:16:08 +0000 (Fri, 21 Feb 2020) $
 SVN checkin by    :  $Author: karsten $
 SVN Id            :  $Id: ana_reinit.c 2101 2020-02-21 12:16:08Z karsten $

 *****************************************************************************/

/**
 * @file 	ana_reinit.c
 * @author	Christiane Reuther
 * @date	November 07, 2012
 * @brief	Source-file to define method <i>ana_reinit</i>
 */

/*****************************************************************************/


/*#define DEBUG*/
#include <stdio.h>
#include <stdlib.h>

#include "ma_typedef.h"
#include "ma_util.h"
#include "ma_sparse.h"
#include "ana_solv_data.h"
#include "linear_analog_solver.h" /*contains prototype definitions*/

/*****************************************************************************/

/**
 * The method <i>ana_reinit</i> generates sparse matrices \f$W_{euler}\f$,
 * \f$Z_{euler}\f$, \f$W_{trapez}\f$ and \f$Z_{trapez}\f$ as follows:
 * <ul>
 * <li>	\f$W_{euler} = \frac1{h} A\f$
 * <li>	\f$Z_{euler} = \frac1{h} A + B\f$
 * <li>	\f$W_{trapez} = \frac2{h} A\f$
 * <li>	\f$Z_{trapez} = \frac2{h} A + B\f$
 * </ul>
 * The sparse matrices are stored within the internal solver data. Moreover, the
 * method generates code for factorization of the matrices \f$Z_{euler}\f$ and
 * \f$Z_{trapez}\f$.
 *
 * The parameter <i>reinit</i> is chosen such that
 * <ul>
 * <li> <i>reinit</i> = 0: call method <i>ana_init</i>
 * <li> <i>reinit</i> = 1: call method <i>ana_init</i> for Euler method
 * <li> <i>reinit</i> = 2: regenerate sparse matrices and code for trapezoidal
 * method
 * </ul>
 * In case of <i>(*sdatap)->cur_algorithm</i> = EULER the method calls
 * <i>ana_init</i>.
 *
 *  @return
 *  <ul><li>    0 - okay
 *  <li>        1 - reallocation of NULL pointer
 *  <li>        2 - not enough memory
 *  <li>		3 - dimension erroneous
 *  <li>		4 - matrix singular
 *  <li>		6 - no sparse matrix
 *  </ul>
 */
int ana_reinit_sparse (
		  sparse_matrix* sA,
          sparse_matrix* sB,
          double  h,
          sca_solv_data **sdatap,
          int reinit
               )
{
	double hinv;
	sca_solv_data *sdata;
	int err = 0;
	unsigned long size = sA->m;


	/*printf("Reinitialization with %i and dt: %e\n",reinit,h);*/

	if((reinit<2 || (*sdatap)==NULL) || size!=(*sdatap)->size)
	{
		return ana_init_sparse(sA, sB, h, sdatap, reinit);
	}
	else
	{
		/*if there was no timestep before -> we must reinit all */
		if((*sdatap)->cur_algorithm==EULER)
		{
			return ana_init_sparse(sA, sB, h, sdatap, 1);
		}

		sdata = *sdatap;
	}

	/*copy A matrix, due its required for solver*/
	MA_CopySparse(sdata->A,sA);

	hinv = 2.0/h;
	sdata->h = h;

	if((*sdatap)->algorithm==TRAPEZ)
	{

		err = MA_GenerateProductValueSparse(sdata->sW_trapez, sA, hinv);
		if (err)
			return err;

		err = MA_GenerateSumMatrixWeighted(sdata->sZ_trapez, 1.0, sdata->sW_trapez,1.0, sB);

		if (err)
		{
			return err;
		}

		err = MA_LequSparseCodegen(sdata->sZ_trapez, sdata->code_trapez);

		if(err)
		{
			sdata->critical_column=sdata->code_trapez->critical_column;
			sdata->critical_row=sdata->code_trapez->critical_line;
			return(err);
		}

	} /*if((*sdatap)->algorithm==TRAPEZ)*/

	return err;
}


/****************************************/

/**
 * @return
 *  <ul><li>    0 - okay
 *  <li>        1 - reallocation of NULL pointer
 *  <li>        2 - not enough memory
 *  <li>		3 - dimension erroneous
 *  <li>		4 - matrix singular
 *  <li>		6 - no sparse matrix
 *  </ul>
 */
int ana_reinit (
   double* A,
   double* B,
   unsigned long size,
   double  h,
   sca_solv_data **sdatap,
   int reinit
   )
{
    sparse_matrix sparse_A;
	sparse_matrix sparse_B;
	int err=0;

	MA_InitSparse(&sparse_A);
	MA_InitSparse(&sparse_B);

	MA_ConvertFullToSparse(A, size, &sparse_A, 0);
	MA_ConvertFullToSparse(B, size, &sparse_B, 0);

	err = ana_reinit_sparse(&sparse_A,&sparse_B,h,sdatap,reinit);

	MA_FreeSparse(&sparse_A);
  	MA_FreeSparse(&sparse_B);

  	return err;
}

