/*****************************************************************************

    Copyright 2010-2014
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

 ana_init.c - description

 Original Author: Karsten Einwich COSEDA Technologies GmbH

 Created on: 22.10.2009

 SVN Version       :  $Revision: 2320 $
 SVN last checkin  :  $Date: 2023-06-08 12:47:38 +0000 (Thu, 08 Jun 2023) $
 SVN checkin by    :  $Author: pehrlich $
 SVN Id            :  $Id: ana_init.c 2320 2023-06-08 12:47:38Z pehrlich $

 *****************************************************************************/

/**
 * @file 	ana_init.c
 * @author	Christiane Reuther
 * @date	November 07, 2012
 * @brief	Source-file to define method <i>ana_init</i>
 */

/*****************************************************************************/

/*#define DEBUG*/


#include <stdlib.h>
#include <stdio.h>



#include "ana_solv_data.h"
#include "linear_analog_solver.h" /*contains prototype definitions*/
#include "ma_util.h"
#include "ma_sparse.h"

/**
 * The method <i>ana_init</i> generates sparse matrices \f$W_{euler}\f$,
 * \f$Z_{euler}\f$, \f$W_{trapez}\f$ and \f$Z_{trapez}\f$ in CRS-format as
 * follows:
 * <ul>
 * <li>	\f$W_{euler} = \frac1{h} A\f$
 * <li>	\f$Z_{euler} = \frac1{h} A + B\f$
 * <li>	\f$W_{trapez} = \frac2{h} A\f$
 * <li>	\f$Z_{trapez} = \frac2{h} A + B\f$
 * </ul>
 * The sparse matrices are stored within the internal solver data. Moreover, the
 * method generates code for decomposition of the matrices \f$Z_{euler}\f$ and
 * \f$Z_{trapez}\f$ as well as the solution code to solve linear systems of
 * equations \f$Z\,x = r\f$. Here, \f$Z\f$ can be replaced by one of the
 * matrices \f$Z_{euler}\f$ and \f$Z_{trapez}\f$, \f$x\f$ is the solution
 * vector and \f$r\f$ is the right-hand-side vector.
 *
 * The parameter <i>reinit</i> is chosen such that
 * <ul>
 * <li> <i>reinit</i> = 0: allocate memory for all necessary sparse matrices
 * and codes of the internal solver data
 * <li> <i>reinit</i> = 1: regenerate sparse matrices and code for Euler method
 * <li> <i>reinit</i> = 2: regenerate sparse matrices and code for trapezoidal
 * method
 * </ul>
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
int ana_init_sparse (
            sparse_matrix* sA,
            sparse_matrix* sB,
            double  h,
            sca_solv_data **sdatap,
            int reinit
  				 )
{
	double hinv;
	sca_solv_data *sdata=NULL;
	int err=0;
	unsigned long size = sA->m;

	int force_init=0;

	/*** common data ******/
	if((!reinit)||((*sdatap)==NULL))
	{
		sdata =(sca_solv_data*)malloc(sizeof(sca_solv_data));


		if (sdata == NULL)
			return 2;

		sdata->A=NULL;
		sdata->sW_euler=NULL;
		sdata->sW_trapez=NULL;
		sdata->sZ_euler=NULL;
		sdata->sZ_trapez=NULL;
		sdata->code_euler=NULL;
		sdata->code_trapez=NULL;
		sdata->algorithm=TRAPEZ;
		sdata->cur_algorithm=EULER;
		sdata->reinit_cnt=0;
		sdata->reinit_steps=0;

		*sdatap=sdata;
		sdata->size=0;
	}
	else
	{
		sdata=*sdatap;
	}

	sdata->critical_column = -1;
	sdata->critical_row    = -1;

	force_init = (reinit==0) || (size!=sdata->size);

	if(force_init && (sdata->size!=0))
	{
		MA_FreeSparse(sdata->A);
		MA_FreeCode(sdata->code_euler);
		MA_FreeSparse(sdata->sZ_euler);
		MA_FreeSparse(sdata->sW_euler);
		MA_FreeCode(sdata->code_trapez);
		MA_FreeSparse(sdata->sZ_trapez);
		MA_FreeSparse(sdata->sW_trapez);

		if(sdata->xp!=NULL)       free(sdata->xp);
		if(sdata->x_last!=NULL)   free(sdata->x_last);
		if(sdata->r1!=NULL)       free(sdata->r1);
		if(sdata->r2!=NULL)       free(sdata->r2);
		if(sdata->A!=NULL)        free(sdata->A);
		if(sdata->sZ_euler!=NULL) free(sdata->sZ_euler);
		if(sdata->sW_euler!=NULL) free(sdata->sW_euler);
		if(sdata->code_euler!=NULL) free(sdata->code_euler);


		if(sdata->sZ_trapez!=NULL)    free(sdata->sZ_trapez);
		if(sdata->sW_trapez!=NULL)    free(sdata->sW_trapez);
		if(sdata->code_trapez!=NULL)  free(sdata->code_trapez);

		sdata->xp=NULL;
		sdata->x_last=NULL;
		sdata->r1=NULL;
		sdata->r2=NULL;
		sdata->A=NULL;
		sdata->sZ_euler=NULL;
		sdata->sW_euler=NULL;
		sdata->code_euler=NULL;
		sdata->sZ_trapez=NULL;
		sdata->sW_trapez=NULL;
		sdata->code_trapez=NULL;
		sdata->size=0;
	}

	sdata->h     = h;
	sdata->size  = size;

	if(force_init)
	{
		sdata->xp    = (double *)calloc(size,(unsigned)sizeof(double));
		sdata->x_last= (double *)calloc(size,(unsigned)sizeof(double));


		sdata->r1    = (double *)calloc(size,(unsigned)sizeof(double));
		sdata->r2    = (double *)calloc(size,(unsigned)sizeof(double));

		if (sdata->xp == NULL || sdata->x_last == NULL
				|| sdata->r1 == NULL || sdata->r2 == NULL)
			return 2;
	}

	if(reinit<2)
	{
		sdata->cur_algorithm=EULER;   /* start/restart with Euler backward method */
	                              /* if only h changes keep last method*/
		sdata->reinit_cnt=sdata->reinit_steps;
	}

	/**** initialization for Euler backward method ******/

	if(force_init)
	{
		sdata->A         =
				(struct sparse*)calloc(1,(unsigned)sizeof(struct sparse));
		sdata->sZ_euler  =
				(struct sparse*)calloc(1,(unsigned)sizeof(struct sparse));
		sdata->sW_euler  =
				(struct sparse*)calloc(1,(unsigned)sizeof(struct sparse));
		sdata->code_euler =
				(struct spcode*)calloc(1,(unsigned)sizeof(struct spcode));

		if (sdata->A == NULL ||sdata->sZ_euler == NULL
				|| sdata->sW_euler == NULL || sdata->code_euler == NULL)
			return 2;

		MA_InitSparse(sdata->sZ_euler);
		MA_InitSparse(sdata->sW_euler);
		MA_InitSparse(sdata->A);
		MA_InitCode(sdata->code_euler);
	}


	/*copy A matrix, due its required for solver*/
	MA_CopySparse(sdata->A,sA);

	if(((sdata->cur_algorithm)==EULER) || force_init)
	{
		hinv = 1.0/h;

		err = MA_GenerateProductValueSparse(sdata->sW_euler, sA, hinv);
		if (err)
			return err;

		err = MA_GenerateSumMatrixWeighted(sdata->sZ_euler, 1.0,
				sdata->sW_euler, 1.0, sB);
		if (err)
			return err;

		err = MA_LequSparseCodegen(sdata->sZ_euler,sdata->code_euler);
		if(err)
		{
			if(sdata->code_euler!=NULL)
			{
				sdata->critical_column=sdata->code_euler->critical_column;
				sdata->critical_row=sdata->code_euler->critical_line;
			}

			return(err);
		}
	}

	/*** initialization for trapezoidal method *******/

	if(sdata->algorithm==TRAPEZ)
	{

		if(force_init)
		{
			sdata->sZ_trapez  =
				(struct sparse*)calloc(1,(unsigned)sizeof(struct sparse));
			sdata->sW_trapez  =
				(struct sparse*)calloc(1,(unsigned)sizeof(struct sparse));
			sdata->code_trapez =
				(struct spcode*)calloc(1,(unsigned)sizeof(struct spcode));

			if (sdata->sZ_trapez == NULL || sdata->sW_trapez == NULL
					|| sdata->code_trapez == NULL)
				return 2;

			MA_InitSparse(sdata->sZ_trapez);
			MA_InitSparse(sdata->sW_trapez);
			MA_InitCode(sdata->code_trapez);
		}

		hinv = 2.0/h;

		err = MA_GenerateProductValueSparse(sdata->sW_trapez, sA, hinv);
		if (err)
			return err;

		err = MA_GenerateSumMatrixWeighted(sdata->sZ_trapez, 1.0,
				sdata->sW_trapez, 1.0, sB);
		if (err)
			return err;

		err = MA_LequSparseCodegen(sdata->sZ_trapez,sdata->code_trapez);
		if(err)
		{
			if(sdata->code_trapez!=NULL)
			{
				sdata->critical_column=sdata->code_trapez->critical_column;
				sdata->critical_row=sdata->code_trapez->critical_line;
			}

			return(err);
		}

	} /*if(sdata->algorithm==TRAPEZ)*/

	return(0);
}

/*****************************************************************************/

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
int ana_init (
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

   err = ana_init_sparse(&sparse_A,&sparse_B,h,sdatap,reinit);

   MA_FreeSparse(&sparse_A);
   MA_FreeSparse(&sparse_B);

   return err;
}





