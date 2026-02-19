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

 MA_lequspar.c - description

 Original Author: Christoph Clauss Fraunhofer IIS/EAS Dresden

 Created on: 22.10.2009

 SVN Version       :  $Revision: 2023 $
 SVN last checkin  :  $Date: 2017-01-17 09:33:22 +0000 (Tue, 17 Jan 2017) $
 SVN checkin by    :  $Author: karsten $
 SVN Id            :  $Id: MA_lequspar.c 2023 2017-01-17 09:33:22Z karsten $

 *****************************************************************************/

/**
 * @file 	MA_lequspar.c
 * @author	Christiane Reuther
 * @date	November 12, 2012
 * @brief	Source-file to decompose sparse matrix and generate decomposition
 * and solution code by Gaussian elimination with Markowitz pivotal search
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
 *  File    :  MA_lequspar.c
 *  Purpose :  sparse matrix lin. equation solver
 *  Notes   :
 *  Author  :  C.Clauss
 *  Version :  $Id: MA_lequspar.c 2023 2017-01-17 09:33:22Z karsten $
 *
 */

/* /// Headers ////////////////////////////////////////////////////////////// */

#include <stdio.h>
#include <stdlib.h>
#include "ma_sparse.h"

/* /// Private Defines ////////////////////////////////////////////////////// */
/* /// Private Data Structures ////////////////////////////////////////////// */
/* /// External Variables /////////////////////////////////////////////////// */
/* /// Prototypes of Static Functions /////////////////////////////////////// */
/* /// Prototypes of Imported Functions ///////////////////////////////////// */
/* /// External Functions /////////////////////////////////////////////////// */
/* /// Static Functions ///////////////////////////////////////////////////// */

static value absol(value v)
{
	if (v < 0.0)
		return -v;
	else
		return v;
}

/* /// External Functions /////////////////////////////////////////////////// */

/* ////////////////////////////////////////////////////////////////////////// */

/**
 *  The internal function <i>tocodedec</i> adds a triple to the decomposition
 *  code.
 *  
 *  @return
 *  <ul><li>    0 - okay
 *  <li>        2 - not enough memory
 *  </ul>
 */

static err_code tocodedec(struct spcode* code,count_far z1,count_far z2,count_far z3)
/*-----------------------------------------------------------------*/
{
	count_far i;
	i = code->ndec;

	code->ndec += 3;
	if (code->ndec > code->ndecmax)
	{ /* reallocation */
		code->ndecmax += 1000;
		if(code->dec!=NULL)
		{
			/*prevent static analyzer to detect this as potential memory leak*/
			void* tmp;
			tmp = (count_far *) realloc((void *) code->dec,
					(unsigned) (code->ndecmax * sizeof(count_far)));

			if(tmp!=NULL)
			{
				code->dec = (count_far *)tmp;
			}
			else
			{
				free(code->dec);
				code->dec=NULL;
			}
		}
		else
		{
			code->dec = (count_far *) calloc(
					(unsigned) (code->ndecmax) , sizeof(count_far));

		}

		if (code->dec == NULL)
			return 2;
	}
	code->dec[i] = z1;
	code->dec[i + 1] = z2;
	code->dec[i + 2] = z3;

	if (z2 > -1 && z3 > -1) code->dec_ma++;

	return 0;
}

/* ////////////////////////////////////////////////////////////////////////// */

/**
 *  The internal function <i>tocodesol</i> adds a triple to the solution code.
 *
 *  @return
 *  <ul><li>    0 - okay
 *  <li>        2 - not enough memory
 *  </ul>
 */

static err_code tocodesol(struct spcode* code,count_far z1,count_far z2,count_far z3)
{
	count_far i;
	i = code->nsol;
	code->nsol += 3;
	if (code->nsol > code->nsolmax)
	{ /* reallocation */
		code->nsolmax += 300;

		if(code->sol!=NULL)
		{
			/*prevent static analyzer to detect this as potential memory leak*/
			void* tmp;
			tmp = (count_far *) realloc((char *) code->sol,
					(unsigned) (code->nsolmax * sizeof(count_far)));

			if(tmp!=NULL)
			{
				code->sol = (count_far *) tmp;
			}
			else
			{
				free(code->sol);
				code->sol=NULL;
			}
		}
		else
		{
			code->sol = (count_far *) calloc(
					(unsigned) (code->nsolmax) , sizeof(count_far));
		}
		if (code->sol == NULL)
			return 2;
	}

	code->sol[i] = z1;
	code->sol[i + 1] = z2;
	code->sol[i + 2] = z3;

	return 0;
}

/* ////////////////////////////////////////////////////////////////////////// */

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
exportMA_Sparse err_code MA_LequSparseCodegen(struct sparse* sA,
		struct spcode* code)
#ifdef NOT_USED
    /* channel for debug output*/
	FILE *debf;
#endif
/*-----------------------------------------------------------------*/
/*SparseDebug :
 0 - nothing
 1 - step, line etc. if singular
 2 - matrix after dec., code
 3 - tolerance criteria
 4 - step, pivot candidates
 5 - full information
 */
{
	count_near ret; /* return variable */
	count_far i, i1, j1 = 0, i2, i3, k = 0, kk, j, i6, lvgl = 0,
			mi = 0, i7, m1, line, i4, jlast, coljlast,
			coli, /* column in pivot line */
			colj, /* column in nonpivot line */
			ipc, /* number of element in pivot column, sparse list */
			n = 0, /* dimension */
			nn, /* dimension - 1 */
			nel, /* number elements */
			niz, /* dimension of the matrix to be decomposed */
			pnew, /* pointer to the last element in the sparse list */
			l1, /* index of first el. in pivot line */
			mark, /* Markowitz sum */
			lz, /* pivot line   */
			l, /* pivot number in sparse list */
			ls; /* pivot column */

	count_far *ip, /* line permutation vector */
	*jp, /* column permutation vactor */
	*jz, /* Markowitz sums of pivot proposals in the line */
	*ik, /* indexes in a list of pivot proposals in the line */
	*is; /* numbers of elements in undecomposed matrix part */

	value su, w, rk, schwell, p, pmin, al; /* pivot value */

	count_far pivot_flop, ndec;
	pivot_flop = ndec = 0;

	code->pivot_flop = code->dec_flop = code->sol_flop = code->dec_ma = 0;

	/* get correct order of entries in sparse list */
	MA_SortSparseList(sA);

#ifdef NOT_USED
	/*Should never happen*/
	if (MA_GetSparseFullDescription(sA))
	{	fprintf(debf,"System matrix used as a full description  \n");
		return 6;
	}
#endif

	sA->decomp = 1; /* note: matrix decomposed */

	ret = 0;
	if (sA->nd < 1 || sA->nel < 1)
	{
		return 3;
	}

	ip = (count_far *) calloc((unsigned) (sA->nd + 1),
			(unsigned) sizeof(count_far));
	jp = (count_far *) calloc((unsigned) (sA->nd + 1),
			(unsigned) sizeof(count_far));
	jz = (count_far *) calloc((unsigned) (sA->nd + 1),
			(unsigned) sizeof(count_far));
	ik = (count_far *) calloc((unsigned) (sA->nd + 1),
			(unsigned) sizeof(count_far));
	is = (count_far *) calloc((unsigned) (sA->nd + 1),
			(unsigned) sizeof(count_far));

	if (ip == NULL || jp == NULL || jz == NULL || ik == NULL || is == NULL)
	{
		ret = 2;
		goto retour;
	}

#ifdef NOT_USED
	brauchen wir nicht
	if (code->SparseDebug > 0)
	{	if (code->SparseDebug == 5)
		{	fprintf(debf,"start lequsparse_codegen  \n");
			MA_PrintSparse(sA, debf);
		}
	}
#endif

	/* Initialization */
	/* ============== */
	n = sA->nd;
	nn = n - 1;
	nel = sA->nel;

	/* Code initialization */
	code->ndecmax = code->nsolmax = 3 * 2 * nel;
	if (code->dec != NULL)
	{
		free(code->dec);
		code->dec = NULL;
	}

	if (code->sol != NULL)
	{
		free(code->sol);
		code->sol = NULL;
	}

	if (code->isort != NULL)
	{
		free(code->isort);
		code->isort = NULL;
	}

	code->dec = (count_far *) calloc((unsigned) (code->ndecmax),
			(unsigned) sizeof(count_far));
	code->sol = (count_far *) calloc((unsigned) (code->nsolmax),
			(unsigned) sizeof(count_far));
	code->isort = (count_far *) calloc((unsigned) (sA->nd + 1),
			(unsigned) sizeof(count_far));
	if (code->sol == NULL || code->dec == NULL || code->isort == NULL)
	{

		if(code->sol!=NULL) free(code->sol);
		code->sol=NULL;

		if(code->dec!=NULL) free(code->dec);
		code->dec=NULL;

		if(code->isort!=NULL) free(code->isort);
		code->isort=NULL;

		ret = 2;
		goto retour;
	}
	code->ndec = code->nsol = 0;
	code->critical_column = code->critical_line = -1; /* default regular */

	/*niz = n;  never used ????? */ /* dimension of remaining matrix, still to be decomposed */
	pnew = nel - 1;/* pointer to the last element in the sparse list */

	schwell = 1.0 / code->gener_piv_scope;

	code->fill_ins = nel; /* original number of fill ins before decomposition */

	/* ip, jp, is  - initialization */
	for (i = 0; i < n; i++)
	{
		ip[i] = jp[i] = i;
		is[i] = 0;
	}
	for (i = 0; i < nel; i++)
		is[sA->ja[i]]++;

#ifdef NOT_USED
	if (code->SparseDebug > 0)
	{	if (code->SparseDebug == 5)
		{	fprintf(debf,"\n before first step \n");
			MA_PrintSparse(sA, debf);
			for (i = 0; i < n; i++)
			fprintf(debf,"%6d   ip %6d   jp %6d   is %6d   jz %6d   ik %6d \n",
				(int)i,(int)ip[i],(int)jp[i],(int)is[i],(int)jz[i],(int)ik[i]);
		}
	}
#endif

	for (niz = n; niz > 1; niz--)
	{

#ifdef NOT_USED
		if (code->SparseDebug > 0)
		{
			if (code->SparseDebug == 3 || code->SparseDebug == 4
					|| code->SparseDebug == 5)
			{	fprintf(debf,"\n\n\n pivot step, niz = %d  \n\n",(int)niz);
			}
		}
#endif

		kk = n - niz; /* number of finished decomposition steps */
		code->rank = kk;

		/* Pivot candidates (stored in ik), Markowitz sums (stored in jz) */
		/* ============================================================== */
		for (j = kk; j < n; j++)
		{
			i1 = ip[j]; /* line number, formerly i1 = iz[ ip[j]]; */
			if (sA->ia[i1] == sA->ia[i1 + 1])
			{ /* line is empty by structure */
				ret = 4; /* singular */

#ifdef NOT_USED
				if (code->SparseDebug > 0)
				{
					if (code->SparseDebug == 1 || code->SparseDebug == 5)
					{
						fprintf(debf,"structural singular in "
								"decomposition step %d  \n",(int)kk);
						fprintf(debf,"in line:  %6d  no pivot "
								"candidate \n",(int)i1);
						fprintf(debf,"already decomposed:  \n");
						for (i = 0; i < kk; i++)
						{
							fprintf(debf,"line:  %6d  column:  "
									"%6d\n",(int)ip[i],(int)jp[i]);
						}
					}
				}
#endif

#ifdef NOT_USED
				if (code->name_of_variable != NULL
						&& code->name_of_equation != NULL)
				fprintf(debf,"structural singular at variable: %s, equation: %s",
						code->name_of_variable((count_near)jp[j]),
						code->name_of_equation((count_near)ip[j]));
#endif

				code->critical_line = ip[j];
				code->critical_column = jp[j];

				/* if (j == n - 1) goto backward; goto retour; */
				ik[i1] = -1; /* no pivot candidate in this line */
				jz[i1] = 0; /* therefore, markowitz sum is zero */
			}
			else
			{ /* there exist elements in the line */
				i3 = l1 = sA->ia[i1]; /* first el. in line */
				su = 0.0; /* maximum of line */
				i2 = 0; /* number of valid el. of line */

				while (i3 > -1) /* search maximum el. of line */
				{
					if (sA->ja[i3] > -1)
					{
						i2++;
						w = absol(sA->a[i3]);
						if (w > su)
						{
							su = w;
							k = i3; /* column of maximum */
						}
					}
					i3 = sA->fa[i3];
				}
				/* fprintf(debf," k  %ld  max:  %e  \n",(int)k,(float)su); */
				if (su < code->piv_abs_tol)
				{
					ret = 4; /* singular */

#ifdef NOT_USED
					if (code->SparseDebug > 0)
					{
						if (code->SparseDebug == 1 || code->SparseDebug == 5)
						{
							fprintf(debf,"numerical singular in "
									"decomposition step %d  \n",(int)kk);
							fprintf(debf,"in line:  %6d  no pivot "
									"candidate \n",(int)i1);
							fprintf(debf,"already decomposed:  \n");
							for (i = 0; i < kk; i++)
							{
								fprintf(debf,"line:  %6d  column:  %6d\n",
										(int)ip[i],(int)jp[i]);
							}
						}
					}
#endif

#ifdef NOT_USED
					if (code->name_of_variable != NULL
							&& code->name_of_equation != NULL)
					fprintf(debf,"numerical singular at variable: %s,  "
							"equation: %s",
							code->name_of_variable((count_near)jp[j]),
							code->name_of_equation((count_near)ip[j]));
#endif

					code->critical_line = ip[j];
					code->critical_column = jp[j];

					/* if (j == n - 1) goto backward; goto retour; */
					ik[i1] = -1; /* no pivot candidate in this line */
					jz[i1] = 0; /* therefore, markowitz sum is zero */
				}
				else
				{
					if (i2 == 1)
					{ /* there is only one element in the line */
						ik[i1] = k;
						jz[i1] = is[sA->ja[k]];
					}
					else
					{
						pmin = 1.e20;
						i = l1; /* stepping line */
						while (i != -1)
						{ /* fprintf(debf,"element %d \n",i); */
							i6 = sA->ja[i]; /* column */
							if (i6 > -1) /* column not decomposed */
							{
								w = absol(sA->a[i]);
								if (w > code->piv_abs_tol)
								{
									rk = su / w;

									pivot_flop++;
								}
								else
								{
									rk = 1.e20;
								}
								if (rk <= schwell)
								{
									mark = (i2 - 1) * (is[i6] - 1);
									i7 = i2 + is[i6];
									p = mark + code->piv_rel_tol * rk;
									if (p < pmin || (p == pmin && i7 < lvgl))
									{
										pmin = p;
										lvgl = i7;
										mi = mark;
										ik[i1] = i;
									}
								}
							}
							i = sA->fa[i];
						}
						jz[i1] = mi; /* stores markowitz sum */
					}
				}
			}
		}

#ifdef NOT_USED
		if (code->SparseDebug > 0)
		{	if (code->SparseDebug == 5)
			{	fprintf(debf,"after finding pivot candidates \n");
				MA_PrintSparse(sA, debf);
				for (i = 0; i < n; i++)
				fprintf(debf,"%6d   ip %6d   jp %6d   is %6d   jz %6d   "
						"ik %6d \n",
						(int)i,(int)ip[i],(int)jp[i],(int)is[i],(int)jz[i],
						(int)ik[i]);
			}
		}
#endif

		/* lu decomposition in actual step */
		/* =============================== */

		/* finding pivot line - minimal Markowitz sum */
		/* ========================================== */

		mark = -1;
		i2 = -1;
		for (j = kk; j < n; j++)
		{
			if (ik[ip[j]] > -1)
			{
				if (mark == -1)
				{ /* first valid pivot candidate */
					mark = jz[ip[j]];
					i2 = j;
				}
				i1 = jz[ip[j]];
				if (mark > i1)
				{ /* search for minimal Markowitz sum */
					mark = i1;
					i2 = j;
				}
			}
		}

		if (i2 == -1)
		{ /* no further pivot element found */
#ifdef NOT_USED
			if (code->SparseDebug > 0)
			{
				if (code->SparseDebug == 5)
				{
					fprintf(debf,"after  %d successfull decomposition steps",
							(int)kk);
					fprintf(debf," no further pivot element found \n");
				}
			}
#endif
			goto backward;
		}

		lz = ip[i2]; /* pivot line   */
		l = ik[lz]; /* pivot number in sparse list */
		ls = sA->ja[l]; /* pivot column */
		l1 = sA->ia[lz]; /* first el. pivot line in sparse list */
		al = sA->a[l]; /* pivot value */

#ifdef NOT_USED
		if (code->SparseDebug > 0)
		{
			if (code->SparseDebug == 5 || code->SparseDebug == 4)
			{
				fprintf(debf,"pivot is: %e in line %d, column %d ",al,(int)lz,
						(int)ls);
				fprintf(debf,"list number %d \n  first el. in pivot "
						"line is %d\n",
						(int)l,(int)l1);
			}
		}
#endif

		/* moving pivot to position kk,kk */
		ip[i2] = ip[kk];
		ip[kk] = lz; /* line */
		i1 = jp[kk];
		jp[kk] = ls;
		for (i = kk + 1; i < n; i++)
		{
			if (jp[i] == ls)
			{
				jp[i] = i1;
				break;
			}
		}

		/* modification pivot line */
		/* ======================= */
		if (absol(al) < code->piv_abs_tol)
		{
			if (al < 0)
			{
				al = -code->piv_abs_tol;
			}
			else
			{
				al = code->piv_abs_tol;
			}
		}
		al = 1.0 / al;
		sA->a[l] = al;
		if (tocodedec(code, l, -1, -1) != 0)
		{
			ret = 2;
			goto retour;
		}
		i = l1; /* stepping through pivot line */
		while (i > -1)
		{
			if (sA->ja[i] > -1 /* neglect decomposed part */
			&& i != l) /* leaving out the pivot element */
			{
				sA->a[i] *= al; /* division by pivot element */
				if (tocodedec(code, i, l, -1) != 0)
				{
					ret = 2;
					goto retour;
				}
			}
			i = sA->fa[i];
		}
		if (tocodesol(code, lz, l, -1) != 0)
		{
			ret = 2;
			goto retour;
		}

#ifdef NOT_USED
		if (code->SparseDebug > 0)
		{
			if (code->SparseDebug == 5)
			{	fprintf(debf,"after modifikation pivot line  \n");
				MA_PrintSparse(sA, debf);
				for (i = 0; i < n; i++)
				fprintf(debf,"%6d   ip %6d   jp %6d   is %6d   jz %6d   "
						"ik %6d \n",
						(int)i,(int)ip[i],(int)jp[i],(int)is[i],(int)jz[i],
						(int)ik[i]);
			}
		}
#endif

		/* non pivot lines */
		/* =============== */
		for (m1 = kk + 1; m1 < n; m1++)
		{
			line = ip[m1]; /* actual line */
#ifdef NOT_USED
			if (code->SparseDebug > 0)
			{
				if (code->SparseDebug == 5)
				{
					fprintf(debf, "\n actual nonpivot line %d \n", (int) line);
				}
			}
#endif

			/* is in line in the pivot column a nonzero element? */
			ipc = -1;
			i = sA->ia[line]; /* stepping through line */
			if (sA->ia[line + 1] != i) /* otherwise line is empty */
			{
				while (i > -1)
				{
					if (sA->ja[i] > -1) /* neglect decomposed part */
					{
						if (sA->ja[i] == ls)
						{
							ipc = i;
							sA->ja[i] = -sA->ja[i] - 1;
							break; /* column el. marked */
						}
					}
					i = sA->fa[i];
				}
			}

			if (ipc > -1)
			{ /* within pivot column of line there is a fill in */
				if (tocodesol(code, line, lz, ipc) != 0)
				{
					ret = 2;
					goto retour;
				}

				/* stepping through pivot line and through nonpivot line */
				/* jumping over pivot element */
#ifdef NOT_USED
				if (code->SparseDebug > 0)
				{
					if (code->SparseDebug == 5)
					{
						fprintf(debf,"line %d has a nonzero in the pivot "
								"column\n",
								(int)line);
					}
				}
#endif

				i = l1; /* start pivot line*/
				j = sA->ia[line]; /* start nonpivot line*/
				jlast = -1;
				/* coljlast = -1;   never used ????*/ /* last element in line */
				while (i > -1)
				{
					coli = sA->ja[i];
					if (coli > -1 && i != l) /* neglect dec. part and pivot */
					{ /* fprintf(debf,"column in pivot line %d\n",(int)coli); */
						/* first el. in pivot line found */

						colj = sA->ja[j];
						if (colj < 0)
							colj = -colj - 1;
						while (colj < coli)
						{
							jlast = j;
							/*coljlast = colj; never used ????????? */
							j = sA->fa[j]; /* printf(" %d  \n",(int)j); */
							if (j == (count_far) -1)
								break;
							colj = sA->ja[j];
							if (colj < 0)
								colj = -colj - 1;
						}

						if (j == -1)
						{ /* end of nonpivot line */
#ifdef NOT_USED
							if (code->SparseDebug > 0)
							{
								if (code->SparseDebug == 5)
								{
									fprintf(debf," new fill in after last "
											"element, col. %d \n",
											(int)coli);
								}
							}
#endif
							pnew = sA->nel;
							sA->nel += 1; /* number nonzeros */
							sA->ia[sA->nd] += 1; /* last pointer ia */
							if (sA->nel > sA->nmax)
							{
								ret = MA_ReallocSparse(sA);
								if (ret != 0)
									goto retour;
							}
							sA->ja[pnew] = coli;
							sA->fa[pnew] = -1;
							sA->fa[jlast] = pnew;
							j = pnew;
							/*colj = coli; never used ?????*/
							sA->a[pnew] = -sA->a[ipc] * sA->a[i];
							is[coli] += 1; /* updating is */
							i3 = -i - 2;
							if (tocodedec(code, pnew, ipc, i3) != 0)
							{
								ret = 2;
								goto retour;
							}
						}
						else /* j != -1 */
						{
							colj = sA->ja[j];
							if (colj < 0)
								colj = -colj - 1;
							if (colj == coli)
							{ /* el. in pivot and nonpivot line */
#ifdef NOT_USED
								if (code->SparseDebug > 0)
								{
									if (code->SparseDebug == 5)
									{
										fprintf(debf," el. in piv and nonpiv "
												"line, col. %d \n",
												(int)coli);
									}
								}
#endif

								/* fprintf(debf," j %d ipc %d i %d \n",
								 (int)j,(int)ipc,(int)i);
								 */
								sA->a[j] -= sA->a[ipc] * sA->a[i];
								if (tocodedec(code, j, ipc, i) != 0)
								{
									ret = 2;
									goto retour;
								}
							}
							else /* colj != coli */
							{
								if (colj < coli)
								{
#ifdef NOT_USED
									fprintf(debf,"internal error\n");
#endif
									goto retour;
								}
								/* new fill in necessary */
								if (jlast < 0)
								{
#ifdef NOT_USED
									if (code->SparseDebug > 0)
									{
										if (code->SparseDebug == 5)
										{
											fprintf(debf," new fill in before "
													"first element \n");
										}
									}
#endif
									pnew = sA->nel;
									sA->nel += 1; /* number nonzeros */
									sA->ia[sA->nd] += 1; /* last pointer ia */
									if (sA->nel > sA->nmax)
									{
										ret = MA_ReallocSparse(sA);
										if (ret != 0)
											goto retour;
									}
									sA->ja[pnew] = coli;
									sA->fa[pnew] = sA->ia[line];
									jlast = sA->ia[line] = pnew;
									coljlast = coli;
									sA->a[pnew] = -sA->a[ipc] * sA->a[i];
									is[coljlast] += 1; /* updating is */
									i3 = -i - 2;
									if (tocodedec(code, pnew, ipc, i3) != 0)
									{
										ret = 2;
										goto retour;
									}
								}
								else
								{
#ifdef NOT_USED
									if (code->SparseDebug > 0)
									{
										if (code->SparseDebug == 5)
										{
											fprintf(debf," new fill after "
													"element col %d \n",
													(int)coljlast);
										}
									}
#endif
									pnew = sA->nel;
									sA->nel += 1; /* number nonzeros */
									sA->ia[sA->nd] += 1; /* last pointer ia */
									if (sA->nel > sA->nmax)
									{
										ret = MA_ReallocSparse(sA);
										if (ret != 0)
											goto retour;
									}
									sA->ja[pnew] = coli;
									sA->fa[pnew] = sA->fa[jlast];
									sA->fa[jlast] = pnew;
									jlast = pnew;
									/*coljlast = coli; never used ??? */
									sA->a[pnew] = -sA->a[ipc] * sA->a[i];
									is[coli] += 1; /* updating is */
									i3 = -i - 2;
									if (tocodedec(code, pnew, ipc, i3) != 0)
									{
										ret = 2;
										goto retour;
									}
								}
							}
						}
					}
					i = sA->fa[i];
				} /* end case that line has element in pivot column */
			} /* end stepping through nonpivot lines */
		}
		/* stepping through pivot line, marking */
		i = l1;
		while (i > -1)
		{
			if (sA->ja[i] > -1) /* neglect decomposed part */
			{
				is[sA->ja[i]] -= 1; /* updating is */
				sA->ja[i] = -sA->ja[i] - 1; /* marking */
			}
			i = sA->fa[i];
		}

#ifdef NOT_USED
		if (code->SparseDebug > 0)
		{
			if (code->SparseDebug == 5)
			{
				fprintf(debf,"after modifikation non pivot lines  \n");
				MA_PrintSparse(sA, debf);
				for (i = 0; i < n; i++)
				fprintf(debf,"%6d   ip %6d   jp %6d   is %6d   jz %6d   "
						"ik %6d \n",
						(int)i,(int)ip[i],(int)jp[i],(int)is[i],(int)jz[i],
						(int)ik[i]);
			}
		}
#endif

	} /* end niz */

	code->rank = sA->nd - niz;

	/* last pivot line */
	/* =============== */
	i = ip[nn];
	j = jp[nn];
	i1 = sA->ia[i];
	while (sA->ja[i1] != j)
	{
		i1 = sA->fa[i1]; /* index to pivot in sparse list */
		if (i1 < 0) /* singular, no pivot element */
		{
			ret = 4;

#ifdef NOT_USED
			if (code->SparseDebug > 0)
			{
				if (code->SparseDebug == 1 || code->SparseDebug == 5)
				{
					fprintf(debf,"structurally singular in last decomposition "
							"step  \n");
					fprintf(debf,"line:  %6d  column:  %6d\n",(int)i,(int)j);
				}
			}
#endif

#ifdef NOT_USED
			if (code->name_of_variable != NULL
					&& code->name_of_equation != NULL)
			fprintf(debf,"singular at variable: %s,  equation: %s",
					code->name_of_variable((count_near)jp[nn]),
					code->name_of_equation((count_near)ip[nn]));
#endif

			code->critical_line = ip[nn];
			code->critical_column = jp[nn];

			goto backward;
			/* goto retour; */
		}
	}
	if (absol(sA->a[i1]) < code->piv_abs_tol) /* matrix singular */
	{
		ret = 4;

#ifdef NOT_USED
		if (code->SparseDebug > 0)
		{
			if (code->SparseDebug == 1 || code->SparseDebug == 5)
			{
				fprintf(debf,"numerically singular in last decomposition "
						"step  \n");
				fprintf(debf,"line:  %6d  column:  %6d\n",(int)i,(int)j);
			}
		}
#endif

		code->critical_line = i;
		code->critical_column = j;
		goto backward;
		/* goto retour; */
	}
	sA->a[i1] = 1.0 / sA->a[i1];
	sA->ja[j1] = -sA->ja[j1] - 1;

	if (tocodedec(code, i1, -1, -1) != 0)
	{
		ret = 2;
		goto retour;
	}

	if (tocodesol(code, i, i1, -1) != 0)
	{
		ret = 2;
		goto retour;
	}
	code->rank = n;

	/* ====================== */
	/* backward decomposition */
	/* ====================== */
	backward: code->nsoldec = code->nsol;

#ifdef NOT_USED
	if (code->SparseDebug > 0)
	{
		if (code->SparseDebug == 5)
		{
			fprintf(debf,"before start backward decomposition  \n");
			MA_PrintSparse(sA, debf);
			for (i = 0; i < n; i++)
			fprintf(debf,"%6d   ip %6d   jp %6d   is %6d   jz %6d   ik %6d \n",
					(int)i,(int)ip[i],(int)jp[i],(int)is[i],(int)jz[i],
					(int)ik[i]);
			fprintf(debf,"n  %d \n",(int)n);
		}
	}
#endif

	/* filling zerodemand in singular case */
	if (code->rank < n)
	{
		k = n - code->rank;
		if (code->zerodemand != NULL)
		{
			free(code->zerodemand);
			code->zerodemand = NULL;
		}
		code->zerodemand = (count_far *) calloc((unsigned) k,
				(unsigned) sizeof(count_far));
		if (code->zerodemand == NULL)
		{
			ret = 2;
			goto retour;
		}

		for (i = 0; i < k; i++)
			code->zerodemand[i] = ip[i + code->rank];

#ifdef NOT_USED
		if (code->SparseDebug > 0)
		{
			if (code->SparseDebug == 5)
			{
				fprintf(debf, "after decomposition the following positions ");
				fprintf(debf, "of the solution vector have to be zero  \n");
				for (i = 0; i < n - code->rank; i++)
				fprintf(debf, "%6d  ", (int) code->zerodemand[i]);
				fprintf(debf, " \n");
			}
		}
#endif
	}

	for (k = n - 2; k > -1; k--)
	{
		line = ip[k];
		/*j = jp[k];  never used ????????*/    /* line steps through lines of matrix */

		i4 = sA->ia[line]; /* steps through actual line in sparse list */
		do
		{
			i6 = sA->ja[i4]; /* column of actual element i4 in line */
			if (i6 < 0)
				i6 = -i6 - 1; /* all elements are watched at */
			for (i3 = k + 1; i3 < n; i3++) /* i3 steps through ready lines */
			{
				if (i6 == jp[i3])
				{	/* test: is ready pivot in column i6 of el. i4? */
					if (tocodesol(code, line, ip[i3], i4) != 0)
					{
						ret = 2;
						goto retour;
					}
				} /* if it is, in line a backward subst. step is done with the
				 pivot standing on the right hand vector in line ip[i3] */
			}
			i4 = sA->fa[i4];
		} while (i4 > -1);
	}

	for (i = 0; i < n; i++)
		code->isort[ip[i]] = jp[i]; /* for back permutation
		 of solution */

	code->fill_ins_after_dec = pnew + 1;

	code->pivot_flop = 4.9 * pivot_flop;
	ndec = code->ndec/3;
	code->dec_flop = 3 * code->rank + code->dec_ma + ndec;
	code->sol_flop = 2 * code->fill_ins_after_dec - code->rank;

	/*========================*/
	/*Actions at the very end */
	/*========================*/

	retour:

#ifdef NOT_USED

	if (code->SparseDebug > 0)
	{
		if (code->SparseDebug == 2 || code->SparseDebug == 5)
		{
			fprintf(debf,"matrix at the end of decomposition \n");
			MA_PrintSparse(sA, debf);
			for (i = 0; i < n; i++)
			fprintf(debf,"%6d   ip %6d   jp %6d   is %6d   jz %6d   ik %6d \n",
					(int)i,(int)ip[i],(int)jp[i],(int)is[i],(int)jz[i],
					(int)ik[i]);

			fprintf(debf,"decomposition code \n");
			for (i = 0; i < code->ndec; i +=3)
			{
				fprintf(debf," %8d %8d %8d \n",
						(int)code->dec[i], (int)code->dec[i+1],
						(int)code->dec[i+2]);
			}

			fprintf(debf,"solution code \n");
			for (i = 0; i < code->nsol; i +=3)
			{	fprintf(debf," %8d %8d %8d \n",
						(int)code->sol[i], (int)code->sol[i+1],
						(int)code->sol[i+2]);
			}

			fprintf(debf,"vector for back permutation of solution \n");
			for (i = 0; i < n; i++)
			{
				fprintf(debf," %8d %8d %8d ", (int)code->sol[i],
						(int)code->sol[i+1], (int)code->sol[i+2]);
			}
			fprintf(debf,"\n end of lequsparse_codegen \n\n");

		}
	}

#endif

	if(ip!=NULL) free(ip);
	if(jp!=NULL) free(jp);
	if(jz!=NULL) free(jz);
	if(ik!=NULL) free(ik);
	if(is!=NULL) free(is);

	ip = NULL;
	jp = NULL;
	jz = NULL;
	ik = NULL;
	is = NULL;

	return ret;
}

/* ////////////////////////////////////////////////////////////////////////// */

/**
 * @return
 *  <ul><li>    0 - okay
 *  <li>		5 - zero demand not satisfied
 *  </ul>
 */
exportMA_Sparse err_code MA_LequSparseSolut(struct sparse* sA,
		struct spcode* code,value* r,value* x)
{
	count_far k, l;
	err_code ret;

	/* printf("\n in Solut:  rank  %d   nsol  %d   nsoldec  %d\n",
	 (int)code->rank, (int)code->nsol, (int)code->nsoldec); */

	ret = 0;

	if (code->rank == sA->nd)
	{ /* solution in regular case, decomposition and backward subst. together */
		for (l = 0; l < code->nsol; l += 3)
		{
			k = code->sol[l + 2];
			if (k < 0)
			{
				*(r + code->sol[l]) *= sA->a[code->sol[l + 1]];

			/*	 printf(" rl = %e  a=%e \n",
				 (double)(*(r + code->sol[l])),
				 (double)(sA->a[ code->sol[l+1] ])); */
			}
			else
			{
				*(r + code->sol[l]) -= *(r + code->sol[l + 1]) * sA->a[k];
			/*	 printf(" rl = %e  rl+1=%e  a=%e \n", *(r + code->sol[l]),
				 *(r + code->sol[l+1]), sA->a[ k ]); */
			}
		}
	}
	else
	{ /* singular case */
		for (l = 0; l < code->nsoldec; l += 3) /* decomposition */
		{
			k = code->sol[l + 2];
			if (k < 0)
			{
				*(r + code->sol[l]) *= sA->a[code->sol[l + 1]];
			}
			else
			{
				*(r + code->sol[l]) -= *(r + code->sol[l + 1]) * sA->a[k];
			}
		}

		for (l = 0; l < sA->nd - code->rank; l++)
		{
			if (r[code->zerodemand[l]] > code->piv_abs_tol
					|| r[code->zerodemand[l]] < -code->piv_abs_tol)
			{
				ret = 5;
			    code->critical_line=code->isort[l];
			}
		}

		/* backward substitution */
		for (l = code->nsoldec; l < code->nsol; l += 3)
		{
			k = code->sol[l + 2];
			if (k < 0)
			{
				*(r + code->sol[l]) *= sA->a[code->sol[l + 1]];
			}
			else
			{
				*(r + code->sol[l]) -= *(r + code->sol[l + 1]) * sA->a[k];
			}
		}
	}

	/* back permutation */
	for (k = 0; k < sA->nd; k++)
	{
		x[code->isort[k]] = r[k];
	}

	return ret;
}

/* /// end of file ////////////////////////////////////////////////////////// */
