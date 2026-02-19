/*****************************************************************************

    Copyright 2010 - 2011
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

 MA_LUdecomposition.c - description

 Original Author: Christiane Reuther Fraunhofer IIS/EAS Dresden

 Created on: 19.12.2011

 SVN Version       :  $Revision: 1754 $
 SVN last checkin  :  $Date: 2014-05-26 09:18:44 +0000 (Mon, 26 May 2014) $
 SVN checkin by    :  $Author: karsten $
 SVN Id            :  $Id: MA_LUdecomposition.c 1754 2014-05-26 09:18:44Z karsten $

 *****************************************************************************/

/**
 * @file 	MA_LUdecomposition.c
 * @author	Christiane Reuther
 * @date	November 12, 2012
 * @brief	Source-file to define functions to decompose sparse matrix and to
 * memory/free sparse for structure <i>triangular</i>
 *
 * The decomposition of the matrix \f$A\f$ in a lower triangular matrix \f$L\f$
 * and an upper triangular matrix \f$R\f$ is used for solving the linear system
 * of equations by the Woodbury formula in case of changing step sizes.
 *
 * Let us assume that the matrix \f$A\f$ has \f$m\f$ lines and columns and is of
 * rank \f$k < m\f$. Let \f$P_1\f$, \f$P_2\f$ be permutation matrices such that
 * in the first \f$k\f$ diagonal elements of the product \f$P_1\,A\,P_2\f$ there
 * are all pivotal elements (pivotal search with Markowitz strategy). Then there
 * exist pure triangular matrices \f$L_0\f$ and \f$R_0\f$ such that
 * \f$A = P_1^T\,L_0\;R_0\,P_2^T\f$. We define the matrices
 * \f$L:= P_1^T\,L_0\f$ and \f$R:= P_2^T\,R_0\f$, where \f$L\f$ is of size
 * \f$m\times k\f$ and \f$R\f$ is of size \f$k\times m\f$.
 *
 * Example:<br>
 * Given is the 3-rank-matrix
 * \f[
 * A = \left(\begin{array}{c} 0 \\ 1 \\ 0 \\2 \end{array}
 * \begin{array}{c} 6 \\ 0 \\ 6 \\ 3 \end{array}
 * \begin{array}{c} 0 \\ 5 \\ 0 \\ 0 \end{array}
 * \begin{array}{c} 0 \\ 0 \\ 0 \\ 7 \end{array}\right).
 * \f]
 * Let the line permutation matrix \f$P_1 := \left(1\;3\;2\;0\right)\f$ and the
 * column permutation matrix \f$P_2 := \left(3\;2\;0\;1\right)\f$, then
 * \f[
 *  P_1\,A\,P_2 =
 *  \left(\begin{array}{c} 5 \\ 0 \\ 0 \\ 0 \end{array}
 * \begin{array}{c} 0 \\ 7 \\ 0 \\ 0 \end{array}
 * \begin{array}{c} 0 \\ 3 \\ 6 \\ 6 \end{array}
 * \begin{array}{c} 1 \\ 2 \\ 0 \\ 0 \end{array}\right)
 * =
 * \left(\begin{array}{c} 5 \\ 0 \\ 0 \\ 0 \end{array}
 * \begin{array}{c} 0 \\ 7 \\ 0 \\ 0 \end{array}
 * \begin{array}{c} 0 \\ 0 \\ 6 \\ 6 \end{array}\right)
 * \left(\begin{array}{c} 1 \\ 0 \\ 0 \end{array}
 * \begin{array}{c} 0 \\ 1 \\ 0 \end{array}
 * \begin{array}{c} 0 \\ \frac37 \\ 1 \end{array}
 * \begin{array}{c} \frac15 \\ \frac27 \\ 0 \end{array}\right)
 * = L_0\,R_0
 * \f]
 * and
 *  \f[
 * L =
 * \left(\begin{array}{c} 0 \\ 5 \\ 0 \\ 0 \end{array}
 * \begin{array}{c} 0 \\ 0 \\ 0 \\ 7 \end{array}
 * \begin{array}{c} 6 \\ 0 \\ 6 \\ 0 \end{array}\right),\;\;\;
 * R =
 * \left(\begin{array}{c} \frac15 \\ \frac27 \\ 0 \end{array}
 * \begin{array}{c} 0 \\ \frac37 \\ 1 \end{array}
 * \begin{array}{c} 1 \\ 0 \\ 0 \end{array}
 * \begin{array}{c} 0 \\ 1 \\ 0 \end{array}\right).
 * \f]
 * The non-zero elements of the triangular matrices \f$L\f$ and \f$R\f$ are
 * stored as follows with \f$i\f$ as storage index:
 * \f[
 * L:\;
 * \begin{array}{c} i \\\hline 0 \\ 1 \\ 2 \\ 3 \end{array}
 * \begin{array}{c} a[i] \\\hline 5 \\ 7 \\ 6 \\ 6 \end{array}
 * \begin{array}{c} line[i] \\\hline 1 \\ 3 \\2 \\ 0 \end{array}
 * \begin{array}{c} column[i] \\\hline 0 \\ 1 \\2 \\ 2 \end{array},\;\;\;
 * R:\;
 * \begin{array}{c} i \\\hline 0 \\ 1 \\ 2 \\ 3 \\ 4 \\ 5 \end{array}
 * \begin{array}{c} a[i] \\\hline 1 \\ \frac15 \\ 1 \\ \frac27 \\ \frac37 \\ 1
 * \end{array}
 * \begin{array}{c} line[i] \\\hline 0 \\ 0 \\ 1 \\ 1 \\ 1 \\ 2 \end{array}
 * \begin{array}{c} column[i] \\\hline 2 \\ 0 \\ 3 \\ 0 \\ 1 \\ 1 \end{array}
 * \f]
 * This representation is used for the Woodbury formula in the function
 * <i>MA_LowRankModifications</i>.
 */

/*****************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "ma_sparse.h"

/* ////////////////////////////////////////////////////////////////////////// */

exportMA_Sparse void MA_InitTriangular(struct triangular* T)
{
	T->a = NULL;
	T->column = NULL;
	T->line = NULL;
	T->nel = 0;
}

/* ////////////////////////////////////////////////////////////////////////// */

exportMA_Sparse void MA_FreeTriangular(struct triangular  *tri)
{
	tri->nel = 0;

	if (tri->a != NULL)
	{
		free (tri->a);
		tri->a = NULL;
	}

	if (tri->line != NULL)
	{
		free (tri->line);
		tri->line = NULL;
	}

	if (tri->column != NULL)
	{
		free (tri->column);
		tri->column = NULL;
	}
}

/* ******************************************************** */

/**
 * @return
 *  <ul><li>    0 - okay
 *  <li>        1 - reallocation of NULL pointer
 *  <li>        2 - not enough memory
 *  <li>		3 - dimension erroneous
 *  </ul>
 */
exportMA_Sparse err_code MA_LUdecomposition(struct sparse* sA,
		struct spcode* code, struct triangular* L, struct triangular* R)
{
	count_near ret; /* return variable */
	count_far i, i1, j1 = 0, i2, i3, k = 0, kk, j, i6, lvgl = 0,
			mi = 0, i7, m1, line, jlast,
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
			ls, /* pivot column */
			indl = 0, indr = 0; /* indices for L and R */

	count_far *ip, /* line permutation vector */
	*jp, /* column permutation vactor */
	*jz, /* Markowitz sums of pivot proposals in the line */
	*ik, /* indexes in a list of pivot proposals in the line */
	*is; /* numbers of elements in undecomposed matrix part */

	value su, w, rk, schwell, p, pmin, al; /* pivot value */

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

	/* Initialization */
	/* ============== */
	n = sA->nd;
	nn = n - 1;
	nel = sA->nel;

	/* niz = n; never read ????*/ /* dimension of remaining matrix, still to be decomposed */
	/*pnew = nel - 1;  never read ?????*/ /* pointer to the last element in the sparse list */

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

	for (niz = n; niz > 1; niz--)
	{
		kk = n - niz; /* number of finished decomposition steps */
		code->rank = kk;

		/* Pivot candidates (stored in ik), Markowitz sums (stored in jz) */
		/* ============================================================== */
		for (j = kk; j < n; j++)
		{
			i1 = ip[j]; /* line number, formerly i1 = iz[ ip[j]]; */
			if (sA->ia[i1] == sA->ia[i1 + 1])
			{ /* line is empty by structure */
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
						w = fabs(sA->a[i3]);
						if (w > su)
						{
							su = w;
							k = i3; /* column of maximum */
						}
					}
					i3 = sA->fa[i3];
				}

				if (su < code->piv_abs_tol)
				{
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
								w = fabs(sA->a[i]);
								if (w > code->piv_abs_tol)
								{
									rk = su / w;
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
			goto backward;
		}

		lz = ip[i2]; /* pivot line   */
		l = ik[lz]; /* pivot number in sparse list */
		ls = sA->ja[l]; /* pivot column */
		l1 = sA->ia[lz]; /* first el. pivot line in sparse list */
		al = sA->a[l]; /* pivot value */

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
		if (fabs(al) < code->piv_abs_tol)
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

		L->a[indl] = al;
		L->line[indl] = ip[kk];
		L->column[indl] = kk;
		indl++;

		R->a[indr] = 1;
		R->column[indr] = sA->ja[l];
		R->line[indr] = kk;
		indr++;

		al = 1.0 / al;
		sA->a[l] = al;

		i = l1; /* stepping through pivot line */
		while (i > -1)
		{
			if (sA->ja[i] > -1 /* neglect decomposed part */
			&& i != l) /* leaving out the pivot element */
			{
				sA->a[i] *= al; /* division by pivot element */

				if (fabs(sA->a[i]) > 1.0e-18)
				{
					R->a[indr] = sA->a[i];
					R->column[indr] = sA->ja[i];
					R->line[indr] = kk;
					indr++;
				}
			}
			i = sA->fa[i];
		}

		/* non pivot lines */
		/* =============== */
		for (m1 = kk + 1; m1 < n; m1++)
		{
			line = ip[m1]; /* actual line */

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
				if (fabs(sA->a[ipc]) > 1.0e-18)
				{
					L->a[indl] = sA->a[ipc];
					L->column[indl] = kk;
					L->line[indl] = line;
					indl++;
				}

				/* stepping through pivot line and through nonpivot line */
				/* jumping over pivot element */

				i = l1; /* start pivot line*/
				j = sA->ia[line]; /* start nonpivot line*/
				jlast = -1;
				while (i > -1)
				{
					coli = sA->ja[i];
					if (coli > -1 && i != l) /* neglect dec. part and pivot */
					{
						colj = sA->ja[j];
						if (colj < 0)
							colj = -colj - 1;
						while (colj < coli)
						{
							jlast = j;
							j = sA->fa[j]; /* printf(" %d  \n",(int)j); */
							if (j == (count_far) -1)
								break;
							colj = sA->ja[j];
							if (colj < 0)
								colj = -colj - 1;
						}

						if (j == -1)
						{ /* end of nonpivot line */
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
							/*colj = coli; never read ??????*/
							sA->a[pnew] = -sA->a[ipc] * sA->a[i];
							is[coli] += 1; /* updating is */
							/* i3 = -i - 2; never read ????? */
						}
						else
						{
							colj = sA->ja[j];
							if (colj < 0)
								colj = -colj - 1;
							if (colj == coli)
							{ /* el. in pivot and nonpivot line */
								sA->a[j] -= sA->a[ipc] * sA->a[i];
							}
							else
							{
								if (colj < coli)
								{
									goto retour;
								}
								/* new fill in necessary */
								if (jlast < 0)
								{
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
									sA->a[pnew] = -sA->a[ipc] * sA->a[i];
									is[coli] += 1; /* updating is */
									/*i3 = -i - 2; never read ???? */
								}
								else
								{
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
									sA->a[pnew] = -sA->a[ipc] * sA->a[i];
									is[coli] += 1; /* updating is */
									/* i3 = -i - 2; never read ???? */
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
			goto backward;
			/* goto retour; */
		}
	}
	if (fabs(sA->a[i1]) < code->piv_abs_tol)
	{
		goto backward;
		/* goto retour; */
	}

	L->a[indl] = sA->a[i1];
	L->column[indl] = nn;
	L->line[indl] = i;
	indl++;

	R->a[indr] = 1;
	R->column[indr] = sA->ja[i1];
	R->line[indr] = nn;
	indr++;

	sA->a[i1] = 1.0 / sA->a[i1];
	sA->ja[j1] = -sA->ja[j1] - 1;

	code->rank = n;

	/* ====================== */
	/* backward decomposition */
	/* ====================== */

	backward:

	L->nel = indl;
	R->nel = indr;


	/*========================*/
	/*Actions at the very end */
	/*========================*/

	retour:

	free(ip);
	free(jp);
	free(jz);
	free(ik);
	free(is);

	is = NULL;
	ik = NULL;
	jz = NULL;
	jp = NULL;
	ip = NULL;

	free(sA->fa);
	sA->fa = NULL;

	return ret;
}

/* /// end of file ////////////////////////////////////////////////////////// */
