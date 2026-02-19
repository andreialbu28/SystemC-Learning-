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

 MA_generate_sparse.c - description

 Original Author: Christoph Clauss Fraunhofer IIS/EAS Dresden

 Created on: 22.10.2009

 SVN Version       :  $Revision: 1928 $
 SVN last checkin  :  $Date: 2016-02-26 15:40:16 +0000 (Fri, 26 Feb 2016) $
 SVN checkin by    :  $Author: karsten $
 SVN Id            :  $Id: MA_generate_sparse.c 1928 2016-02-26 15:40:16Z karsten $

 *****************************************************************************/

/**
 * @file 	MA_generate_sparse.c
 * @author	Christiane Reuther
 * @date	November 12, 2012
 * @brief	Source-file to define methods for generating sparse matrices
 *
 * The methods <i>MA_ConvertFullToSparse</i>,
 * <i>MA_GenerateProductValueSparse</i> and <i>MA_GenerateSumMatrixWeighted</i>
 * store matrices in full presentation (the first one) and in sparse matrix
 * presentation as sparse matrix using the CRS-Format, where we additionally use
 * <i>sm->fa</i> to build links between non-zero entries of the sparse matrix
 * belonging to one mutual line. In difference to the method
 * <i>sparse_write_val</i> in the file <i>linear_direct_sparse</i> the correct
 * order of the stored non-zero elements of the sparse matrix is important. The
 * indizes of the non-zero elements of the sparse matrix are chosen line-wise
 * from left to right. The order is necessary to apply the \f$L U\f$-
 * factorization in method <i>MA_LequSparseCodegen</i>.
 *
 * Example:<br>
 * The aim is to store the full matrix
 * \f[
 * A = \left(\begin{array}{c} \\ 1.0 \\  \\ 2.0 \end{array}
 * \begin{array}{c} 6.0 \\  \\ 6.0 \\ 3.0 \end{array}
 * \begin{array}{c}  \\ 5.0 \\  \\ \end{array}
 * \begin{array}{c}  \\  \\  \\ 7.0 \end{array}\right)
 * \f]
 * in CRS-Format with additional information about successors within a line
 * (<i>sm->fa</i>).<br>
 * The only choice of storage is given in the following list:
 * \f[
 * \begin{array}{c} i \\\hline 0 \\ 1 \\ 2 \\ 3 \\ 4 \\ 5 \\ 6 \end{array}
 * \begin{array}{c} a[i] \\\hline 6.0 \\ 1.0 \\ 5.0 \\ 6.0 \\ 2.0 \\ 3.0 \\ 7.0
 * \end{array}
 * \begin{array}{c} ja[i] \\\hline 1 \\ 0 \\ 2 \\ 1 \\ 0 \\ 1 \\ 3 \end{array}
 * \begin{array}{c} fa[i] \\\hline -1 \\ 2 \\ -1 \\ -1 \\ 5 \\ 6 \\ -1
 * \end{array}
 * \f]
 * Here, \f$i\f$ denotes the storage index, <i>sm->a</i> denotes the value of
 * non-zero entry, <i>sm->ja</i> gives the column of the entry and <i>sm->fa</i>
 * denotes the successor and equals to \f$-1\f$ if the entry is the last one in
 * line. Moreover, the list
 * \f[
 * \begin{array}{c} i \\\hline 0 \\ 1 \\ 2 \\ 3 \end{array}
 * \begin{array}{c} ia[i] \\\hline 0 \\ 1 \\ 3 \\ 4 \end{array}
 * \f]
 * gives the index of the first entry in each line (<i>sm->ia</i> with line
 * index \f$i\f$).
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
 *  File    :  MA_generate_sparse.c
 *  Purpose :  procedures, which generate or use sparse matrix descriptions
 *  Notes   :
 *  Author  :  C.Clauss
 *  Version :  $Id: MA_generate_sparse.c 1928 2016-02-26 15:40:16Z karsten $
 *
 */

/* /// Headers ////////////////////////////////////////////////////////////// */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "ma_sparse.h"
#include "assert.h"

/* ////////////////////////////////////////////////////////////////////////// */

/**
 *  @return
 *  <ul><li>    0 - okay
 *  <li>        2 - not enough memory
 *  <li>		3 - dimension erroneous
 *  </ul>
 */
exportMA_Sparse err_code MA_ConvertFullToSparse(value* A, count_near n,
		struct sparse* sA, count_far maxlen)
{
	count_far i, j, anz, nn;
	size_t  nmax;

	/*----------------------- exceptions  -----------------------------*/
	if (n < 1)
		return 3;

	/*----------------------- conversion  -----------------------------*/
	MA_FreeSparse(sA);
	sA->nd = sA->m = sA->n = (count_far) n;
	nn = sA->nd * sA->nd;

	/* number of non-zero fill ins */
	anz = 0;
	for (i = 0; i < nn; i++)
	{
		if (*(A + i) != 0.0)
			anz++;
	}
	sA->nel = anz;
	if (maxlen > 0)
	{
		if (maxlen < anz) return 1;
		sA->nmax = maxlen + 2;
	}
	else
	{
		sA->nmax = anz + (count_far) anz/10 + 20; /* prophylactical */
	}

	nmax=(size_t)(sA->nmax);

	sA->a = (value *) calloc(nmax, (size_t) sizeof(value));
	sA->ja = (count_near *)calloc(nmax,(size_t) sizeof(count_near));
	sA->fa = (count_far *) calloc(nmax,(size_t) sizeof(count_far));
	sA->ia = (count_far *) calloc((size_t) (n + 2),(size_t) sizeof(count_far));

	if (sA->a == NULL || sA->ja == NULL || sA->fa == NULL || sA->ia == NULL)
		return 2;

	/*******************************/

	sA->ia[0] = anz = 0;
	sA->fa[0] = -1;

	for (i = 0; i < n; i++) /* line */
	{
		sA->ia[i + 1] = sA->ia[i];
		for (j = 0; j < n; j++) /* column */
		{
			if (*(A + j * n + i) != 0.0)
			{
				assert((size_t)anz<nmax);
				/*
				 * should never happen due nmax must be > as zero values
				 */
				sA->a[anz] = *(A + j * n + i);
				sA->ja[anz] = j;
				sA->fa[anz] = anz+1;

				anz++;
			}
		}

		sA->ia[i+1] = anz;
		if (anz != 0)
			sA->fa[anz-1] = -1; /* last element within line has no successor */
	}

	/* sparse list is ordered line by line and column by column */
	sA->sparse_list_ordered = 1;

	return 0;
}

/* ////////////////////////////////////////////////////////////////////////// */

/**
 * The functin <i>MA_GenerateProductValueSparse</i> computes the product
 * \f$A = val\;B\f$ in sparse matrix presentation.
 *
 *  @return
 *  <ul><li>    0 - okay
 *  <li>        2 - not enough memory
 *  <li>		3 - dimension erroneous
 *  </ul>
 */
exportMA_Sparse err_code   MA_GenerateProductValueSparse(struct sparse* sA,
		struct sparse* sB, value val)
{
	count_near i;
	count_far k, k_new, anz;
	size_t nmax;

	/*******************************/

	MA_FreeSparse(sA);

	if (sB->m < 1 || sB->n < 1 || sB->nd < 1)
		return 3;

	sA->m = sB->m;
	sA->n = sB->n;
	sA->nd = sB->nd;

	anz = 0;

	for (k = 0; k < sB->nel; ++k)
	{
		if (*(sB->a + k) != 0.0)
			anz++;
	}

	sA->nel = anz;
	sA->nmax = anz + (count_far) anz/10 + 20; /* prophylactical */
    nmax=(size_t)(sA->nmax);

	sA->a = (value *) calloc(nmax, (size_t) sizeof(value));
	sA->ja = (count_near *) calloc(nmax,(size_t) sizeof(count_near));
	sA->fa = (count_far *) calloc(nmax,(size_t) sizeof(count_far));
	sA->ia = (count_far *) calloc((size_t) (sA->m + 1),(size_t) sizeof(count_far));

	if (sA->a == NULL || sA->ja == NULL || sA->fa == NULL || sA->ia == NULL)
		return 2;

	/*******************************/

	k_new = 0;
	sA->ia[0] = k_new;
	sA->fa[0] = -1;

	for (i = 0; i < sB->m; ++i)
	{
		k = sB->ia[i]; /* first element in line i */
		if (!(k == sB->ia[i+1] || k == -1)) /* line not empty */
		{
			while (k > -1)
			{
				/* if a[k] != 0.0 generate new entry in sparse matrix sA */
				if (sB->a[k] != 0.0)
				{
					assert((size_t)k_new < nmax);
					/*
					 * should not happen due nmax is > number of non-zeros
					 */
					sA->ja[k_new] = sB->ja[k];	/* column of element k_new */
					sA->a[k_new] = sB->a[k]; /* value of element k_new */
					/* possible successor of element k_new */
					sA->fa[k_new] = k_new + 1;

					k_new++;
				}

				k = sB->fa[k];
			}
		}

		sA->ia[i+1] = k_new;
		if (k_new != 0)
			sA->fa[k_new-1] = -1; /* last element within line has no successor*/
	}

	if (val != 1.0)
	{
		for (k = 0; k < sA->nel; ++k)
			sA->a[k] *= val;
	}

	/* sort entries in lines with respect to column indices */
	MA_SortSparseColumms(sA);

	/* sparse list is ordered line by line and column by column */
	sA->sparse_list_ordered = 1;

	return 0;
}

/* ////////////////////////////////////////////////////////////////////////// */

/**
 * The functin <i>MA_GenerateSumMatrixWeighted</i> computes the sum
 * \f$A = vb\;B + \;vc\;C\f$ in sparse matrix presentation.
 *
 *  @return
 *  <ul><li>    0 - okay
 *  <li>        2 - not enough memory
 *  <li>		3 - dimension erroneous
 *  </ul>
 */
exportMA_Sparse err_code   MA_GenerateSumMatrixWeighted(struct sparse* sA,
		value vb, struct sparse* sB, value vc, struct sparse* sC)
{
	count_near i;
	count_far k, k_new, k_mid, l, anz;

	if (sB->m < 1 || sB->n < 1 || sB->nd < 1
			|| sC->m < 1 || sC->n < 1 || sC->nd < 1)
		return 3;

	/* check if number of lines/columns of sB and sC coincide */
	if (sB->m != sC->m || sB->n != sC->n)
		return 3;

	MA_FreeSparse(sA);

	sA->m = sB->m;
	sA->n = sB->n;
	sA->nd = sB->nd;

	anz = 0;
	for (k = 0; k < sB->nel; ++k)
	{
		if (*(sB->a + k) != 0.0)
			anz++;
	}

	for (k = 0; k < sC->nel; ++k)
	{
		if (*(sC->a + k) != 0.0)
			anz++;
	}

	sA->nel = anz;
	sA->nmax = anz + (count_far) anz * 0.1 + 20; /* prophylactical */

	sA->a = (value *) calloc((unsigned) sA->nmax, (unsigned) sizeof(value));
	sA->ja = (count_near *) calloc((unsigned) sA->nmax,
			(unsigned) sizeof(count_near));
	sA->fa = (count_far *) calloc((unsigned) sA->nmax,
			(unsigned) sizeof(count_far));
	sA->ia = (count_far *) calloc((unsigned) (sB->m + 1),
			(unsigned) sizeof(count_far));

	if (sA->a == NULL || sA->ja == NULL || sA->fa == NULL || sA->ia == NULL)
		return 2;

	/*******************************/

	k_new = 0;
	sA->ia[0] = k_new;
	sA->fa[0] = -1;

	for (i = 0; i < sA->m; ++i)
	{
		/* stepping through line i in matrix sB */
		k = sB->ia[i];	/* first element in line i of matrix sB */
		if (!(k == sB->ia[i+1] || k == -1)) /* line not empty */
		{
			while (k > -1)
			{
				if (sB->a[k] != 0.0)
				{
					sA->ja[k_new] = sB->ja[k];
					sA->fa[k_new] = k_new + 1;

					if (vb == 1.0)
						sA->a[k_new] = sB->a[k];
					else
						sA->a[k_new] = vb * sB->a[k];

					k_new++;
				}

				k = sB->fa[k];
			}
		}
		k_mid = k_new;

		/* stepping through line i in matrix sC */
		k = sC->ia[i];	/* first element in line i of matrix sC */
		if (!(k == sC->ia[i+1] || k == -1)) /* line not empty */
		{
			while (k > -1)
			{
				if (sC->a[k] != 0.0)
				{
					/* check if position in sA is already occupied */
					l = sA->ia[i];
					while (l < k_mid)
					{
						if (sC->ja[k] == sA->ja[l])
						{	/* there is already an entry in this column
								in line i */
							if (vc == 1.0)
								sA->a[l] += sC->a[k];
							else
								sA->a[l] += vc * sC->a[k];

							break;
						}

						l++;
					}

					if (l == k_mid) /* no entry in this column in line i */
					{
						sA->ja[k_new] = sC->ja[k];
						sA->fa[k_new] = k_new + 1;

						if (vc == 1.0)
							sA->a[k_new] = sC->a[k];
						else
							sA->a[k_new] = vc * sC->a[k];

						k_new++;
					}
				}

				k = sC->fa[k];
			}
		}

		sA->ia[i+1] = k_new;
		if (k_new != 0)
			sA->fa[k_new-1] = -1; /* last element within line has no successor*/
	}

	/*********************************************/

	/* sort entries in lines with respect to column indices */
	MA_SortSparseColumms(sA);

	/* sparse list is ordered line by line and column by column */
	sA->sparse_list_ordered = 1;

	sA->nel = k_new;

	return 0;
}

/* ////////////////////////////////////////////////////////////////////////// */

/**
 *  @return
 *  <ul><li>    0 - okay
 *  <li>        2 - not enough memory
 *  </ul>
 */
exportMA_Sparse err_code MA_CopySparse(struct sparse* sA, struct sparse* sB)
{
	sA->nel = sB->nel;
	sA->m = sB->m;
	sA->n = sB->n;
	sA->nd = sB->nd;
	sA->nmax = sB->nmax;
	sA->sparse_list_ordered = sB->sparse_list_ordered;

	if(sA->ja!=NULL)
		free(sA->ja);
	sA->ja=(count_near*)malloc((sB->nmax)*sizeof(count_near));

	if(sA->fa!=NULL)
		free(sA->fa);
	sA->fa=(count_far*)malloc((sB->nmax)*sizeof(count_far));

	if(sA->a!=NULL)
		free(sA->a);
	sA->a=(value*)malloc((sB->nmax)*sizeof(value));

	if(sA->ia!=NULL)
		free(sA->ia);
	sA->ia=(count_far*)malloc((sB->m+1)*sizeof(count_far));

	if (sA->ja == NULL || sA->fa == NULL || sA->a == NULL || sA->ia == NULL)
		return 2;

	memcpy(sA->ja, sB->ja, (sB->nel)*sizeof(count_near));
	memcpy(sA->fa, sB->fa, (sB->nel)*sizeof(count_far));
	memcpy(sA->a, sB->a, (sB->nel)*sizeof(value));
	memcpy(sA->ia, sB->ia, (sB->m+1)*sizeof(count_far));

	return 0;
}

/* /// end of file ////////////////////////////////////////////////////////// */

