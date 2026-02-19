/*****************************************************************************

    Copyright 2010
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

 linear_direct_sparse.c - description

 Original Author: Christiane Reuther, Fraunhofer IIS/EAS Dresden

 Created on: October 09, 2012

 SVN Version       :  $Revision: 2320 $
 SVN last checkin  :  $Date: 2023-06-08 12:47:38 +0000 (Thu, 08 Jun 2023) $
 SVN checkin by    :  $Author: pehrlich $
 SVN Id            :  $Id: linear_direct_sparse.c 2320 2023-06-08 12:47:38Z pehrlich $

 *****************************************************************************/

/**
 * @file 	linear_direct_sparse.c
 * @author	Christiane Reuther
 * @date	November 13, 2012
 * @brief	Source-file to define methods to use directly sparse matrix
 * representation
 *
 * The method <i>sparse_write_value</i> provides a reference to an already
 * existing non-zero entry or a new non-zero entry of the matrix \f$sm\f$. The
 * sparse matrix is stored using the CRS-Format, where we additionally use
 * <i>sm->fa</i> to build links between non-zero entries of the sparse matrix
 * belonging to one mutual line. This enables us to store the non-zero entries
 * of the sparse matrix in any order.
 *
 * Example:<br>
 * The aim is to store the matrix
 * \f[
 * A = \left(\begin{array}{c} \\ 1.0 \\  \\ 2.0 \end{array}
 * \begin{array}{c} 6.0 \\  \\ 6.0 \\ 3.0 \end{array}
 * \begin{array}{c}  \\ 5.0 \\  \\ \end{array}
 * \begin{array}{c}  \\  \\  \\ 7.0 \end{array}\right)
 * \f]
 * in CRS-Format with additional information about successors within a line
 * (<i>sm->fa</i>).<br>
 * One possible choice of storage is given in the following list:
 * \f[
 * \begin{array}{c} i \\\hline 0 \\ 1 \\ 2 \\ 3 \\ 4 \\ 5 \\ 6 \end{array}
 * \begin{array}{c} a[i] \\\hline 6.0 \\ 7.0 \\ 1.0 \\ 2.0 \\ 3.0 \\ 6.0 \\ 5.0
 * \end{array}
 * \begin{array}{c} ja[i] \\\hline 1 \\ 3 \\ 0 \\ 0 \\ 1 \\ 1 \\ 2 \end{array}
 * \begin{array}{c} fa[i] \\\hline -1 \\ 3 \\ 6 \\ 4 \\ -1 \\ -1 \\ -1
 * \end{array}
 * \f]
 * Here, \f$i\f$ denotes the storage index, <i>sm->a</i> denotes the value of
 * non-zero entry, <i>sm->ja</i> gives the column of the entry and <i>sm->fa</i>
 * denotes the successor and equals to \f$-1\f$ if the entry is the last one in
 * line. Moreover, the list
 * \f[
 * \begin{array}{c} i \\\hline 0 \\ 1 \\ 2 \\ 3 \end{array}
 * \begin{array}{c} ia[i] \\\hline 5 \\ 2 \\ 0 \\ 1 \end{array}
 * \f]
 * gives the index of the first entry in each line (<i>sm->ia</i> with line
 * index \f$i\f$).
 */

/*****************************************************************************/

#include <stdlib.h>
#include <stdio.h>

#include "ma_util.h"
#include "ma_sparse.h"
#include <assert.h>

/*****************************************************************************/

sparse_matrix* sparse_generate(unsigned m, unsigned n)
{
	count_near i;
	sparse_matrix* sm = NULL;

    sm = (sparse_matrix*)calloc(1,(unsigned)sizeof(sparse_matrix));
    if(sm==NULL) return NULL;

	MA_InitSparse(sm);

	sm->m = m;	/* number of lines */
	sm->n = n;	/* number of columns */
	sm->nd = MA_min(sm->m, sm->n);	/* dimension of matrix */
	sm->sparse_list_ordered = 0; /* sparse list unordered */

	/* allocate memory for sm->ia */
	sm->ia = (count_far *) calloc((unsigned) (m + 1),
			(unsigned) sizeof(count_far));
	if (sm->ia == NULL)  return NULL;

	for (i = 0; i < (count_near)m+1; ++i) /* initialize sm->ia */
		sm->ia[i] = -1;

	return sm;
}

/*****************************************************************************/

void sparse_delete(sparse_matrix *sm)
{
	if (sm != NULL)
	{
		MA_FreeSparse(sm);
		free(sm);
		sm = NULL;
	}
}

/*****************************************************************************/

/**
 *  @return
 *  <ul><li>    0 - okay
 *  <li>        2 - not enough memory
 *  <li>		3 - dimension erroneous
 *  </ul>
 */
int sparse_write_value(sparse_matrix *sm, unsigned x, unsigned y,double val)
{
	count_far k, last_k = -1;
	int err = 0;

	if ((count_near)x >= sm->m || (count_near)y >= sm->n) /* sm not resized */
	{
		return 3; /*access outside matrix dimension*/
	}

	if (sm->ia == NULL)
		return 3;	/* memory for ia not allocated */

	if (sm->a == NULL) /* allocate memory */
	{
		sm->nmax = MA_max(sm->m, sm->n);

		sm->a = (value *) calloc((unsigned) sm->nmax,
				(unsigned) sizeof(value));
		sm->ja = (count_near *) calloc((unsigned) sm->nmax,
				(unsigned) sizeof(count_near));
		sm->fa = (count_far *) calloc((unsigned) sm->nmax,
				(unsigned) sizeof(count_far));

		if (sm->a == NULL || sm->ja == NULL || sm->fa == NULL)
			return 2;
	}
	else
	{
		if (sm->nel >= sm->nmax)	/* reallocate memory */
		{
			err = MA_ReallocSparse(sm);
			if (err)
				return 2;
		}
	}

	k = sm->ia[x]; /* first element in line x */
	while (k > -1  && (sm->ia[x] != sm->ia[x+1]))
	{	/* k = -1 in case of line empty or k is last element in line */
		if (sm->ja[k] == y)  /* position (x,y) exists */
		{
			sm->a[k] = val;
			return 0;  /* successful: write to existing position */
		}

		last_k = k;
		k = sm->fa[k]; /* go to next element in line x */
	}

	/* insert new position */
	sm->ja[sm->nel] = y; /* save column y of element sm->nel */

	if (last_k == -1) /* first entry in line */
	{
		sm->ia[x] = sm->nel; /* pointer to first element in line */
	}
	else
	{
		sm->fa[last_k] = sm->nel; /* sm->nel is successor of last_k */
	}

	sm->fa[sm->nel] = -1;
	sm->a[sm->nel] = val; /* write value to new position */

	sm->nel++;
	sm->ia[sm->m] = sm->nel;

	sm->sparse_list_ordered = 0; /* sparse list unordered */

	return 0;
}

/*****************************************************************************/

double* sparse_get_value_ref(sparse_matrix *sm, unsigned x, unsigned y)
{
	count_far k;

	if ((count_near)x >= sm->m || (count_near)y >= sm->n)
		return NULL;

	if (sm->ia != NULL && sm->a != NULL)
	{
		k = sm->ia[x]; /* first element in line x */
		while (k > -1 && (sm->ia[x] != sm->ia[x+1]))
		{
			if (sm->ja[k] == y) /* element k is in column y */
			{
				return &(sm->a[k]);
				break;
			}

			k = sm->fa[k];
		}
	}

	return NULL;
}

/*****************************************************************************/

unsigned sparse_get_nlines(sparse_matrix *sm)
{
	return sm->m;
}

/*****************************************************************************/

unsigned sparse_get_ncolumns(sparse_matrix *sm)
{
	return sm->n;
}

/*****************************************************************************/

double sparse_get_value(sparse_matrix *sm, unsigned x, unsigned y)
{
	double val = 0.0;
	count_far k;

	k = sm->ia[x]; /* first element in line x */
	while (k > -1 && (sm->ia[x] != sm->ia[x+1]))
	{
		if (sm->ja[k] == y) /* element k is in column y */
		{
			val = sm->a[k];
			break;
		}

		k = sm->fa[k];
	}

	return val; /* val = 0.0 in case of (x,y) is not occupied in sm */
}

/*****************************************************************************/

/**
 *  @return
 *  <ul><li>    0 - okay
 *  <li>        1 - reallocation of NULL pointer
 *  <li>        2 - not enough memory
 *  </ul>
 */
int sparse_resize(sparse_matrix *sm, unsigned m, unsigned n)
{
	count_near i, m_old = sm->m;
	count_far* ia;

	sm->m = m; /* reset m, n, nd */
	sm->n = n;
	sm->nd = MA_min(sm->m, sm->n);	/* dimension of matrix */

	if (sm->ia != NULL) /* reallocate memory for sm->ia */
	{
		ia = (count_far *)realloc((char *)sm->ia,
				(unsigned)((m+1) * sizeof(count_far)) );
		if (ia == NULL) return 2;
		sm->ia =ia;

		for (i = m_old; i < (count_near)m+1; ++i) /* initialize sm->ia */
			sm->ia[i] = -1;
	}
	else
		return 1;

	return 0;
}

/*****************************************************************************/

/**
 * \brief copies matrix sm2 to matrix sm1, returns pointer to sm1
 */
sparse_matrix* sparse_copy(
		sparse_matrix *sm1,     /**< sparse matrix destination*/
		sparse_matrix *sm2 		/**< sparse matrix source*/
		)
{
	if(sm1 == NULL)
		sm1=sparse_generate(0,0);

	if(sm2 == NULL)
	{
		if(sm1 != NULL)
			sparse_delete(sm1);
		return NULL;
	}

	MA_CopySparse(sm1,sm2);

	return sm1;
}

/*****************************************************************************/

double* sparse_get_next_entry(sparse_matrix *sm, long *x_actual_pos,
		long *y_actual_pos, long *actual_index)
{
	count_near i;
	count_far k;

	/* get correct order of entries in sparse list */
	MA_SortSparseList(sm);

	if (sm->nel > 0) /* there are elements in sparse list */
	{
		if (*actual_index < 0)
		{
			for (i = 0; i < sm->m; ++i)
			{
				if (sm->ia[i] != sm->ia[i+1]) /* line i not empty */
				{
					k = sm->ia[i];

					*actual_index = k;
					*x_actual_pos = i;
					*y_actual_pos = sm->ja[k];

					return &sm->a[k];
				}
			}
		}
		else /* *actual_index >= 0 */
		{
			k = sm->fa[*actual_index]; /* go to next element in sparse list */

			if (k > - 1) /* there is a successor in line x_actual_pos */
			{
				*actual_index = k;
				*y_actual_pos = sm->ja[k];

				return &sm->a[k];
			}
			else /* there is no successor in line x_actual_pos */
			{
				for (i = *x_actual_pos+1; i < sm->m; ++i)
				{
					if (sm->ia[i] != sm->ia[i+1]) /* line i not empty */
					{
						k = sm->ia[i];

						*actual_index = k;
						*x_actual_pos = i;
						*y_actual_pos = sm->ja[k];

						return &sm->a[k];
					}
				}
			}
		}
	}

	return NULL;
}

/*****************************************************************************/

/**
 * This method is consistent with MA_ConvertFullToSparse in
 * MA_generate_sparse.c.
 *
 *  @return
 *  <ul><li>    0 - okay
 *  <li>        2 - not enough memory
 *  <li>		3 - dimension erroneous
 *  </ul>
 */
int sparse_convert_full_to_sparse(double *A, sparse_matrix *sa,
		unsigned long m, unsigned long n)
{
	count_far i, j, anz, mn;
	size_t nmax;
	const size_t vsize = (unsigned) sizeof(value);
	const size_t cfsize = (unsigned) sizeof(count_far);
	const size_t cnsize = (unsigned) sizeof(count_near);


	if (n < 1)
		return 3;

	MA_FreeSparse(sa);
	sa->m = (count_far) m;
	sa->n = (count_far) n;
	sa->nd = (count_far)MA_min(m,n);

	mn = (count_far)(m * n);

	/* number of non-zero fill ins */
	anz = 0;
	for (i = 0; i < mn; i++)
	{
		if (*(A + i) != 0.0)
			anz++;
	}
	sa->nel = anz;

	nmax = (size_t)(anz + (count_far) anz/10 + 20); /* prophylactical */
	sa->nmax = (count_far)nmax;

	sa->a = (value *) calloc(nmax, vsize);
	sa->ja = (count_near *) calloc(nmax, cnsize);
	sa->fa = (count_far *) calloc(nmax,  cfsize);
	sa->ia = (count_far *) calloc((size_t) (m + 1),   cfsize);

	if (sa->a == NULL || sa->ja == NULL || sa->fa == NULL || sa->ia == NULL)
		return 2;

	/*******************************/

	sa->ia[0] = anz = 0;
	sa->fa[0] = -1;

	for (i = 0; i < (count_far)m; i++) /* line */
	{
		sa->ia[i + 1] = sa->ia[i];
		for (j = 0; j < (count_far)n; j++) /* column */
		{
			if (*(A + j * m + i) != 0.0)
			{
				assert((size_t)anz<nmax);
				/*
				 * this should never happen due nmax is > m*n
				 * and anz is counted from 0 inside this nested m-n-loop
				 */
				sa->a[anz] = *(A + j * m + i);
				sa->ja[anz] = j;
				sa->fa[anz] = anz+1;

				anz++;
			}
		}

		sa->ia[i+1] = anz;
		if (anz != 0)
			sa->fa[anz-1] = -1; /* last element within line has no successor */
	}

	sa->sparse_list_ordered = 1; /* sparse list ordered */

	return 0;
}

/*****************************************************************************/

/**
 *  @return
 *  <ul><li>    0 - okay
 *  <li>		3 - dimension erroneous
 *  </ul>
 */
int sparse_convert_sparse_to_full(sparse_matrix *sa, double *A,
		unsigned long m, unsigned long n)
{
	count_near i, j;
	count_far k;

	if (m != sa->m || n != sa->n)
		return 3;

	for (i = 0; i < sa->m; ++i)
	{
		k = sa->ia[i];
		while (k > -1 && (sa->ia[i] != sa->ia[i+1]))
		{
			j = sa->ja[k];
			*(A + i + sa->m*j) = sa->a[k];

			k = sa->fa[k];
		}
	}

	return 0;
}

/*****************************************************************************/


    /**
       returns 1 if the matrixes are equal
    */      
int sparse_compare(sparse_matrix *sA,sparse_matrix *sB)
{
      int flag = 1;
      long idx_sA = 0;
      long idx_sB = 0;
      int line = 0;
      
    /* all dimension paramters are the same */
    if((sA->nd != sB->nd) || (sA->m != sB->m) || (sA->n != sB->n))
    {
        return 0;
    }
    
    /* number of non zero elements has to be the same */
    if(sA->nel != sB->nel)
    {
        return 0;
    }
    
    /* sort both matrices to go through ordered lists */
    MA_SortSparseList(sA);
    MA_SortSparseList(sB);
    
    /* go through lines */
    for (line = 0; line < sA->m; ++line)
    {
        /* line not empty in one or both matrices */
        if ((sA->ia[line] != sA->ia[line+1]) || ((sB->ia[line] != sB->ia[line+1])))
        {
        
            /* get index of first values in new line */
            idx_sA = sA->ia[line];
            idx_sB = sB->ia[line];
        
            /* if the index is not the same, only one of the matrices lines
            was empty or one matrix still had values in the line before */
            if(idx_sA != idx_sB)
            {
                return 0;
            }
        
        
            /* go through current line, if fa[idx] == -1, end of line is reached */
            while(flag)
            {
                /* compare values */
                if(sA->a[idx_sA] != sA->a[idx_sB])
                {
                    return 0;
                }
            
                /* compare column coordinates */
                if(sA->ja[idx_sA] != sB->ja[idx_sA])
                {
                    return 0;
                }
            
                /* setting the flag in the while loop is necessary so that the last entry 
                of a row, for which its index returns fa[idx] = -1, is also considered */
                if((sA->fa[idx_sA] == -1) || (sB->fa[idx_sB] == -1))
                {
                    flag = 0;
                }
     
                /* go to next index in current row */
                ++idx_sA;
                ++idx_sB;
            } //while(flag)
        } //if ((sA->ia[line] != sA->ia[line+1]) || ((sB->ia[line] != sB->ia[line+1])))
    } //for (int line = 0; line < sA->m; ++line)
    
    return 1;        
}
