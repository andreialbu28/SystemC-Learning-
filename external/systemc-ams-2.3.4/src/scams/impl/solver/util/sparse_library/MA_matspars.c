/*****************************************************************************

    Copyright 2010
    Fraunhofer-Gesellschaft zur Foerderung der angewandten Forschung e.V.

    Copyright 2015-2023 COSEDA Technologies GmbH

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

 MA_matspars.c - description

 Original Author: Christoph Clauss Fraunhofer IIS/EAS Dresden

 Created on: 22.10.2009

 SVN Version       :  $Revision: 2320 $
 SVN last checkin  :  $Date: 2023-06-08 12:47:38 +0000 (Thu, 08 Jun 2023) $
 SVN checkin by    :  $Author: pehrlich $
 SVN Id            :  $Id: MA_matspars.c 2320 2023-06-08 12:47:38Z pehrlich $

 *****************************************************************************/

/**
 * @file 	MA_matspars.c
 * @author	Christiane Reuther
 * @date	November 12, 2012
 * @brief	Source-file to define methods for initializing/freeing sparse
 * matrices and decomposition/solution code, computation with sparse matrices
 */

/*****************************************************************************/

/* /// C Module ///////////////////////////////////////// -*- Mode: C -*- /// */
/*
 *  Copyright (C) 1999 by Fraunhofer-Gesellschaft. All rights reserved.
 *  Fraunhofer Institute for Integrated Circuits, Design Automation Department
 *  Zeunerstrasse 38, D-01069 Dresden, Germany (http://www.eas.iis.fhg.de)
 *
 *  Permission is hereby granted, without written agreement and without
 *  license or royalty fees, to use, copy, modify, and distribute this
 *  software and its documentation for any purpose, provided that the
 *  above copyright notice and the following two paragraphs appear in all
 *  copies of this software.
 *
 *  IN NO EVENT SHALL THE FRAUNHOFER INSTITUTE FOR INTEGRATED CIRCUITS
 *  BE LIABLE TO ANY PARTY FOR DIRECT, INDIRECT, SPECIAL, INCIDENTAL, OR
 *  CONSEQUENTIAL DAMAGES ARISING OUT OF THE USE OF THIS SOFTWARE AND ITS
 *  DOCUMENTATION, EVEN IF THE FRAUNHOFER INSTITUTE HAS BEEN ADVISED OF THE
 *  POSSIBILITY OF SUCH DAMAGE.
 *
 *  THE FRAUNHOFER INSTITUTE FOR INTEGRATED CIRCUITS SPECIFICALLY DISCLAIMS
 *  ANY WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
 *  OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE. THE SOFTWARE
 *  PROVIDED HEREUNDER IS ON AN "AS IS" BASIS, AND THE FRAUNHOFER INSTITUTE
 *  HAS NO OBLIGATION TO PROVIDE MAINTENANCE, SUPPORT, UPDATES,
 *  ENHANCEMENTS, OR MODIFICATIONS.
 *
 *  Project :  KOSIM - MA
 *  File    :  MA_matspars.c
 *  Purpose :  procedures for sparse matrix technique,  lin. equation solver
 *  Notes   :
 *  Author  :  C.Clauss
 *  Version :  $Id: MA_matspars.c 2320 2023-06-08 12:47:38Z pehrlich $
 *
 */

/*#define NOT_USED*/

/* /// Headers ////////////////////////////////////////////////////////////// */

#include <stdio.h>
#include <stdlib.h>
#include "ma_sparse.h"


/* ////////////////////////////////////////////////////////////////////////// */

exportMA_Sparse void MA_InitSparse(struct sparse* sA)
{
	sA->nd     = sA->nel = sA->nmax = sA->m = sA->n = 0;
	sA->a      = NULL;
	sA->ja     = NULL;
	sA->ia     = NULL;
	sA->fa     = NULL;
	sA->sparse_list_ordered = 0;
	sA->full   = 0;
	sA->decomp = 0;
}

/* ////////////////////////////////////////////////////////////////////////// */

exportMA_Sparse void MA_FreeSparse(struct sparse* sA)
{
	if(sA==NULL) return;

	sA->nd = sA->nel = sA->nmax = sA->m = sA->n = 0;
	sA->sparse_list_ordered = 0;

	if (sA->a != NULL)
	{
		free( sA->a);
		sA->a = NULL;
	}

	if (sA->ja != NULL)
	{
		free( sA->ja);
		sA->ja = NULL;
	}

	if (sA->ia != NULL)
	{
		free( sA->ia);
		sA->ia = NULL;
	}

	if (sA->fa != NULL)
	{
		free( sA->fa);
		sA->fa = NULL;
	}

	sA->full = 0;
	sA->decomp = 0;
}

/* ////////////////////////////////////////////////////////////////////////// */

/**
 *  @return
 *  <ul><li>    0 - okay
 *  <li>        1 - reallocation of NULL pointer
 *  <li>        2 - not enough memory
 *  </ul>
 */
exportMA_Sparse err_code MA_ReallocSparse(struct sparse* sA)
{
	value* a;
	count_near* ja;
	count_far* fa;

	sA->nmax += sA->nd * 2 + 10;

	if (sA->a != NULL)
 	{
		a = (value *)realloc((char *)sA->a,
                             (unsigned)(sA->nmax * sizeof(value)) );

		if (a == NULL) return 2;

		sA->a=a;
 	}
	else
		return 1;

	if (sA->ja != NULL)
	{
		ja = (count_near *)realloc((char *)sA->ja,
                                   (unsigned)(sA->nmax * sizeof(count_near)) );
		if (ja == NULL) return 2;
		sA->ja=ja;
	}
	else
		return 1;

	if (sA->fa != NULL)
	{
		fa = (count_far *)realloc((char *)sA->fa,
                                  (unsigned)(sA->nmax * sizeof(count_far)) );
		if (fa == NULL) return 2;
		sA->fa=fa;
	}
	else
		return 1;

	return 0;
}

/* ////////////////////////////////////////////////////////////////////////// */

/**
 *  The function <i>MA_ProductSparseVector</i> computes \f$pc = A\,pb\f$.
 *
 * @return
 *  <ul><li>    0 - okay
 *  <li>		3 - dimension erroneous
 *  </ul>
 */
exportMA_Sparse err_code MA_ProductSparseVector(struct sparse* sA,value* pb,
		value* pc)
{
	count_far li,k;

	/*----------------------- exceptions  -----------------------------*/

	if (sA->nd < 1 || sA->nmax < 1)
		return 3;

	/*----------------------- multiplication --------------------------*/

	for (li = 0; li < sA->m; li++)
	{
		*(pc + li) = 0.0;                 /* initialization c */

		k = sA->ia[li];
		if (!(k == sA->ia[li+1] || k == -1))
		{
			while (k != -1)
			{
				*(pc + li) += sA->a[k] * *(pb + sA->ja[k]);
				k = sA->fa[k];
			}
		}
	}

	return 0;
}

/* ////////////////////////////////////////////////////////////////////////// */

void MA_SortSparseColumms(struct sparse *sA)
{
	count_near i, j_min;
	count_far k, k0, ind_min=0;
	value a_min=0;

	for (i = 0; i < sA->m; ++i)
	{
		k = sA->ia[i];
		while (k > -1)
		{
			j_min = sA->n;
			k0 = k;
			while (k0 > -1)
			{
				if (sA->ja[k0] < j_min)
				{
					j_min = sA->ja[k0];
					ind_min = k0;
					a_min = sA->a[k0];
				}
				k0 = sA->fa[k0];
			}

			if (j_min != sA->ja[k])
			{
				sA->ja[ind_min] = sA->ja[k];/* permute column indizes */
				sA->ja[k] = j_min;

				sA->a[ind_min] = sA->a[k];	/* permute values */
				sA->a[k] = a_min;
			}

			k = sA->fa[k];
		}
	}
}

/* ////////////////////////////////////////////////////////////////////////// */

void MA_SortSparseList(struct sparse *sA)
{

/*	count_far k;

	for (k = 0; k < sA->nel; ++k)
		printf("k = %ld\ta = %e\tja = %d\tfa = %ld\n", k, *(sA->a + k), *(sA->ja + k), *(sA->fa + k));
	printf("sparse list ordered = %d\n\n", sA->sparse_list_ordered);
*/
	struct sparse *sA_ordered;

	if (sA->sparse_list_ordered == 0)
	{
		/* sort sparse list */
	    sA_ordered = (struct sparse*)calloc(1,(unsigned)sizeof(struct sparse));

		MA_InitSparse(sA_ordered);
		MA_GenerateProductValueSparse(sA_ordered, sA, 1.0);

		MA_CopySparse(sA,sA_ordered);

		MA_FreeSparse(sA_ordered);
		free(sA_ordered);

		sA->sparse_list_ordered = 1; /* sparse list ordered */
	}
/*
	for (k = 0; k < sA->nel; ++k)
		printf("k = %ld\ta = %e\tja = %d\tfa = %ld\n", k, *(sA->a + k), *(sA->ja + k), *(sA->fa + k));
	printf("sparse list ordered = %d\n\n", sA->sparse_list_ordered);
	*/
}

/* ////////////////////////////////////////////////////////////////////////// */

exportMA_Sparse void MA_InitCode(struct spcode* code)
{
	if(code==NULL) return;

	code->ndec  = code->nsol = code->nsoldec = code->rank = 0;
	code->dec                                   = NULL;
	code->sol                                   = NULL;
	code->isort                                 = NULL;
	code->zerodemand                            = NULL;
	code->critical_line = code->critical_column = -1; /* in regular case */

	/* default values */
	code->piv_abs_tol      = 1.e-18;
	code->piv_rel_tol      = 1.e-10;
	code->gener_piv_scope  = 0.1;
	code->SparseDebug      = 0;
	code->name_of_variable = NULL;
	code->name_of_equation = NULL;

	code->fill_ins = 0;
	code->fill_ins_after_dec = 0;
}

/* ////////////////////////////////////////////////////////////////////////// */

exportMA_Sparse void MA_FreeCode(struct spcode  *code)
{
	if(code==NULL) return;

	code->ndec = code->nsol = code->nsoldec = code->rank = 0;

	if (code->dec != NULL)
	{
		free( code->dec);
		code->dec = NULL;
	}

	if (code->sol != NULL)
 	{
		free( code->sol);
		code->sol = NULL;
 	}

	if (code->isort != NULL)
	{
		free( code->isort);
		code->isort = NULL;
	}

	if (code->zerodemand != NULL)
	{
		free( code->zerodemand);
		code->zerodemand = NULL;
	}
}



/* /// end of file ////////////////////////////////////////////////////////// */
