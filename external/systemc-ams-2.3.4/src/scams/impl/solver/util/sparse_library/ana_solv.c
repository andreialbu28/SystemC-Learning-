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

 ana_solv.c - description

 Original Author: Karsten Einwich Fraunhofer IIS/EAS Dresden

 Created on: 22.10.2009

 SVN Version       :  $Revision: 2320 $
 SVN last checkin  :  $Date: 2023-06-08 12:47:38 +0000 (Thu, 08 Jun 2023) $
 SVN checkin by    :  $Author: pehrlich $
 SVN Id            :  $Id: ana_solv.c 2320 2023-06-08 12:47:38Z pehrlich $

 *****************************************************************************/

/**
 * @file 	ana_solv.c
 * @author	Christiane Reuther
 * @date	November 07, 2012
 * @brief	Source-file to define method <i>ana_solv</i>
 */

/*****************************************************************************/


#include "ana_solv_data.h"
#include "ma_typedef.h"
#include "ma_util.h"
#include "ma_sparse.h"
#include <string.h>
#include <stdio.h> /* for fprintf and stderr */
#include <stdlib.h>
#include <math.h>

/****************************************/

/**
 * The method <i>ana_solv</i> computes the solution of a linear system of
 * equations by calling the function <i>MA_LequSparseSolut</i>:
 * <ul>
 * <li> In case of <i>sdata->cur_algorithm = EULER</i>: \f$x\f$ is the solution to
 * \f$Z_{euler} x = W_{euler}\, x_{last} - q\f$
 * <li> In case of <i>sdata->cur_algorithm = TRAPEZ</i>: \f$x\f$ is the solution to
 * \f$Z_{trapez} x = W_{trapez}\, x_{last} + A\,xp - q\f$
 * </ul>
 *
 *  Implementation details of the function <i>ana_solv</i>: <br>
 *	<ul>
 *	<li> The vector \f$ x \f$ deals as input as solution vector \f$x_{last}\f$
 *	of the previous time step.
 *	<li> The vector \f$ x \f$ deals as output as solution vector of the current
 *	time step.
 *	<li> The vector \f$xp\f$ is computed by \f$xp = \frac1{h}(x - x_{last})\f$
 *	after solving the linear system of equation, \f$xp\f$ is used as input at
 *	the next time step.
 *	</ul>
 */
void ana_solv (
            double* q,
            double* x,
            sca_solv_data* sdata
                )
{
   double hinv;
   double *r1, *r2;
   unsigned long size;
   unsigned long i;
   double *xp, *x_last;

   r1    = sdata->r1;
   r2    = sdata->r2;
   size  =sdata->size;
   xp    =sdata->xp;
   x_last=sdata->x_last;

   if(size>0) memcpy(x_last, x, size*sizeof(double));

   if(fabs(sdata->h) < 1e-300 ){
	   printf("%s \n", "Division by zero in ana_solv.c stepsize(1/dt) to small! Aborting...");
   	   exit(EXIT_FAILURE);
   }

   /************************************************/

   if(sdata->cur_algorithm==EULER)	/******* Euler backward ********/
   {
	   MA_ProductSparseVector(sdata->sW_euler, x, r1);

	   for(i=0;i<size;++i) r1[i] -= q[i];

	   MA_LequSparseSolut(sdata->sZ_euler, sdata->code_euler, r1, x);

	   hinv  = 1/sdata->h;

       for(i=0;i<size;++i) xp[i]=(x[i]-x_last[i])*hinv; /*derivation*/

       if(sdata->reinit_cnt<=0)
       {
    	   sdata->cur_algorithm=sdata->algorithm;
       }
       else
       {
    	   sdata->reinit_cnt--;
       }
   }
   else	/*** trapezoidal method ****/
   {
	   MA_ProductSparseVector(sdata->sW_trapez, x, r1);     /* W*x(i-1) */
	   MA_ProductSparseVector(sdata->A, xp, r2);            /* A*xp(i-1) */

	   for(i=0;i<size;++i)
		   r1[i] += r2[i] - q[i];     /* W*x(i-1) + A*xp(i-1) - q(i) */

	   MA_LequSparseSolut(sdata->sZ_trapez, sdata->code_trapez, r1, x);

	   hinv=2.0/sdata->h;

	   for(i=0;i<size;++i) xp[i]=hinv*(x[i]-x_last[i])-xp[i]; /*new derivation*/
   }
}

/****************************************/
