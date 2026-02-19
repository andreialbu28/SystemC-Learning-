/*****************************************************************************

    Copyright 2010
    Fraunhofer-Gesellschaft zur Foerderung der angewandten Forschung e.V.

    Copyright 2015-2023 COSEDA Technologies GmbH

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

 ma_sparse.h - description

 Original Author: Christoph Clauss Fraunhofer IIS/EAS Dresden

 Created on: 22.10.2009

 SVN Version       :  $Revision: 2320 $
 SVN last checkin  :  $Date: 2023-06-08 12:47:38 +0000 (Thu, 08 Jun 2023) $
 SVN checkin by    :  $Author: pehrlich $
 SVN Id            :  $Id: ma_sparse.h 2320 2023-06-08 12:47:38Z pehrlich $

 *****************************************************************************/

/**
 * @file 	ma_sparse.h
 * @author	Christiane Reuther
 * @date	November 12, 2012
 * @brief	Header-file to define methods for sparse matrices and solution code
 * for Gaussian elimination with Markowitz pivotal search
 */

/*****************************************************************************/



/* /// C Header ///////////////////////////////////////// -*- Mode: C -*- /// */
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
 *  File    :  ma_sparse.h
 *  Purpose :  sparse matrix linear equation solver
 *  Notes   :
 *  Author  :  C.Clauss
 *  Version :  $Id: ma_sparse.h 2320 2023-06-08 12:47:38Z pehrlich $
 *
 */

#ifndef _ma_sparse_h_
#define _ma_sparse_h_

/* /// Headers ////////////////////////////////////////////////////////////// */

#include "ma_typedef.h"


/* /// Defines ////////////////////////////////////////////////////////////// */

#define exportMA_Sparse

/* /// Data Structures ////////////////////////////////////////////////////// */

 /**
  * sparse matrix definition using Compressed Row Storage
  */
typedef struct sparse
{     count_far  nd;         /**< dimension matrix                 */
      count_far  nel;        /**< number of nonzero fill-ins       */
      count_far  nmax;       /**< allocated maximum of fill-ins    */
      count_near m;			 /**< number of lines of matrix 		 */
      count_near n;			 /**< number of columns of matrix		 */
      value      *a;         /**< pointer to values                */
      count_near *ja;        /**< column position within line      */
      count_far  *ia;        /**< pointer to start of new line,
                                nd+1 elements, last el. is nel   */
      count_far  *fa;        /**< pointer to next el. in line,
                                -1 in case of end of line        */
      unsigned   sparse_list_ordered; /**< = 1: list is ordered line by line and
      	  	  	  	  	  	  	  column by column; = 0: list is unordered */

      unsigned   full : 1;   /**< if 1 then a and nd used for full
                                matrix representation
                                if 0 then sparse representation  */
      unsigned   decomp : 1; /**< if 1 then a is decomposed
                                if 0 then a is not decomposed    */
} sparse_matrix;

/**
 * solution code for sparse matrix
 */
struct spcode
{
      count_far  ndec;       /**< length code for decomposition    */
      count_far  nsol;       /**< length code for solution         */
      count_far  nsoldec;    /**< length code for solution, decom-
                                position steps only,
                                without backward substitution    */
      count_far  ndecmax;    /**< maximum length of decomp. code   */
      count_far  nsolmax;    /**< maximum length of solution code  */
      count_far  rank;       /**< rank of matrix                   */
      count_far  *dec;       /**< decomposition code               */
      count_far  *sol;       /**< solution code                    */
      count_far  *isort;     /**< vector for sorting solution after
                                within lequsparse_solut          */
      count_far  *zerodemand;/**< positions of right hand side after
                                decomposition, which has to be zero
                                in case of a singular matrix, if
                                solutions are possible */
      count_near  critical_line;	/**< line in which singularity occurs */
      count_near  critical_column;	/**< column in which singularity occurs */

      /* parameters */
      value  piv_abs_tol;    /**< absolute tol. for finding pivot (egpiva) */
      value  piv_rel_tol;    /**< relative tol. for finding pivot (egpivr) */
      value  gener_piv_scope;/**< pivot range in order to limit new fill ins
      	  	  	  	  	  	  (egpivsw) */
      debug  SparseDebug;    /**< debug control */
      char *(*name_of_variable)(void);
      char *(*name_of_equation)(void);

      /* statistics */
      count_far  fill_ins;   /**< original number of non-zero elements */
      count_far  fill_ins_after_dec;	/**< number of non-zero elements after
      	  	  	  	  	  	  	  	  	  decomposition */

      count_far pivot_flop;		/**< number of flops for pivotal search */
      count_far dec_flop;		/**< number of flops for decomposition */
      count_far dec_ma;			/**< intermediate number of flops for
      	  	  	  	  	  	  	  decomposition*/
      count_far sol_flop;		/**< number of flops for solution */
};

/**
 * \brief upper or lower triangular matrix after LU-decomposition
 */
struct triangular
{
	value *a;			/**< pointer to values                */
	count_near *column;	/**< column position      			*/
	count_near *line;	/**< line position			        */
	count_far nel;		/**< number of nonzero fill-ins       */
};

/*MA_matspars.c*/

/**
 * initializes sparse matrix
 */
exportMA_Sparse void MA_InitSparse(
		struct sparse* sA	/**< sparse matrix */
		);

/**
 * destructs sparse matrix, sets pointers to zero
 */
exportMA_Sparse void MA_FreeSparse(
		struct sparse* sA	/**< sparse matrix */
		);

/**
 * \brief reallocates memory for sparse matrix
 */
exportMA_Sparse err_code MA_ReallocSparse(
		struct sparse* sA	/**< sparse matrix */
		);

/**
 * \brief gets product of sparse matrix \f$A\f$ with vector \f$pb\f$
 */
exportMA_Sparse err_code MA_ProductSparseVector(
		struct sparse* sA,	/**< sparse matrix */
		value* pb,			/**< multiplier vector */
		value* pc			/**< solution vector */
		);

/**
 * sorts indices of columns in sparse matrix within each line
 */
void MA_SortSparseColumms(
		struct sparse *sA	/**< sparse matrix */
		);

/**
 * sorts indices of entries of sparse list line by line and column by column
 */
void MA_SortSparseList(
		struct sparse *sA	/**< sparse matrix */
		);

/**
 * initializes code for decomposition and substitution of sparse matrix
 */
exportMA_Sparse void MA_InitCode(
		struct spcode* code	/**< code */
		);

/**
 * destructs code, sets pointers to zero
 */
exportMA_Sparse void MA_FreeCode(
		struct spcode *code	/**< code */
		);

/*MA_generate_sparse.c*/

/**
 * \brief generates sparse matrix in CRS-Format from full-matrix \f$A\f$
 */
exportMA_Sparse err_code MA_ConvertFullToSparse(
		value* A,			/**< matrix */
		count_near n,		/**< number of lines/columns */
		struct sparse* sA,	/**< sparse matrix */
		count_far maxlen	/**< memory bound for sparse matrix */
		);

/**
 * \brief generates sparse matrix in CRS-Format as product of sparse matrix
 * with value
 */
exportMA_Sparse err_code MA_GenerateProductValueSparse(
		struct sparse* sA,	/**< solution sparse matrix */
		struct sparse* sB,	/**< given sparse matrix */
		value val			/**< multiplier value */
		);

/**
 * \brief generates sparse matrix in CRS-Format as sum of two weighted sparse
 * matrices
 */
exportMA_Sparse err_code MA_GenerateSumMatrixWeighted(
		struct sparse* sA,	/**< solution sparse matrix */
		value vb,			/**< weight of matrix sB */
		struct sparse* sB,	/**< given sparse matrix */
		value vc,			/**< weight of matrix sc */
		struct sparse* sC	/**< given sparse matrix */
		);

/**
 * \brief copies sparse matrix <i>sB</i> into sparse matrix <i>sA</i>
 */
exportMA_Sparse err_code MA_CopySparse(
		struct sparse* sA,	/**< target sparse matrix */
		struct sparse* sB	/**< source sparse matrix */
		);

/*MA_lequspar.c*/

/**
 * \brief pivotal search and generation of code by Gaussian elimination with
 * Markowitz pivotal search
 */
exportMA_Sparse err_code MA_LequSparseCodegen(
		struct sparse* sA,	/**< sparse matrix */
		struct spcode* code	/**< code */
		);

/**
 * \brief solves linear system of equations with the aid of the generated
 * solution code
 */
exportMA_Sparse err_code MA_LequSparseSolut(
		struct sparse* sA,	/**< sparse matrix */
		struct spcode* code,/**< code */
		value* r,			/**< righthandside vector */
		value* x			/**< solution vector */
		);

/*MA_LUdecomposition.c*/

/**
 * initializes triangular matrix
 */
exportMA_Sparse void MA_InitTriangular(
		struct triangular* T	/**< triangular matrix */
		);

/**
 * destructs triangular matrix, sets pointers to Zero
 */
exportMA_Sparse void MA_FreeTriangular(
		struct triangular  *tri	/**< triangular matrix */
		);

/**
 * \brief decomposition of sparse matrix in product of triangular matrices
 * \f$A = L\,R\f$
 */
exportMA_Sparse err_code MA_LUdecomposition(
		struct sparse* sA,		/**< sparse matrix */
		struct spcode* code,	/**< code */
		struct triangular* L,	/**< left triangular matrix */
		struct triangular* R	/**< right triangular matrix */
		);
		

#endif /* _ma_sparse_h_ */

/* /// end of file ////////////////////////////////////////////////////////// */
