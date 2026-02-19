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

 linear_direct_sparse.h - description

 Original Author: Christiane Reuther, Fraunhofer IIS/EAS Dresden

 Created on: October 09, 2012

 SVN Version       :  $Revision: 2320 $
 SVN last checkin  :  $Date: 2023-06-08 12:47:38 +0000 (Thu, 08 Jun 2023) $
 SVN checkin by    :  $Author: pehrlich $
 SVN Id            :  $Id: linear_direct_sparse.h 2320 2023-06-08 12:47:38Z pehrlich $

 *****************************************************************************/

/**
 * @file 	linear_direct_sparse.h
 * @author	Christiane Reuther
 * @date	November 13, 2012
 * @brief	Header-file to define methods to use directly sparse matrix
 * representation
 */

/*****************************************************************************/

#ifndef  LINEAR_DIRECT_SPARSE_H
#define  LINEAR_DIRECT_SPARSE_H

#ifdef __cplusplus
extern "C"
{
#endif

	struct sparse;
	typedef sparse sparse_matrix;
	
	/* linear_direct_sparse.c */

	/**
	 * initializes the sparse matrix and sets number of lines/columns
	 */
	sparse_matrix* sparse_generate(
			unsigned m, 			/**< number of lines */
			unsigned n				/**< number of columns */
			);

	/**
	 * destructs sparse matrix, sets pointers to zero
	 */
	void sparse_delete(
			sparse_matrix *sm		/**< sparse matrix */
			);

	/**
	 * \brief writes value to matrix,
	 * if position is not existing, the position will be created
	 */
	int sparse_write_value(
			sparse_matrix *sm, 		/**< sparse matrix */
			unsigned x, 			/**< line position of entry */
			unsigned y,				/**< column position of entry */
			double value			/**< value at position (x,y) */
			);

	/**
	 * \brief returns reference to the matrix entry,
	 * if the entry is not available NULL will be returned
	 */
	double* sparse_get_value_ref(
			sparse_matrix *sm, 		/**< sparse matrix */
			unsigned x, 			/**< line position of entry */
			unsigned y				/**< column position of entry */
			);

	/**
	 * gets number of lines of sparse matrix
	 */
	unsigned sparse_get_nlines(
			sparse_matrix *sm		/**< sparse matrix */
			);

	/**
	 * gets number of columns of sparse matrix
	 */
	unsigned sparse_get_ncolumns(
			sparse_matrix *sm		/**< sparse matrix */
			);

	/**
	 * gets value of entry at position \f$(x,y)\f$ of sparse matrix
	 */
	double sparse_get_value(
			sparse_matrix *sm, 		/**< sparse matrix */
			unsigned x, 			/**< line position of entry */
			unsigned y				/**< column position of entry */
			);

	/**
	 * \brief resets number of lines/columns of sparse matrix
	 */
	int sparse_resize(
			sparse_matrix *sm, 		/**< sparse matrix */
			unsigned m, 			/**< new number of lines */
			unsigned n				/**< new number of columns */
			);

	/**
	 * \brief copies matrix sm2 to matrix sm1, returns pointer to sm1
	 */
	sparse_matrix* sparse_copy(
			sparse_matrix *sm1,     /**< sparse matrix destination*/
			sparse_matrix *sm2 		/**< sparse matrix source*/
			);

	/**
	 * \brief gets position (x,y) of successor entry to
	 * (x_actual_pos, y_actual_pos) in matrix or NULL in case of end
	 * of list of entries in sparse matrix.
	 *
	 * This method reads the sparse matrix line by line and column by column.
	 * If actual_index < 0 then the cur_algorithm starts at position
	 * (0,0) to find an element of the sparse list.
	 */
	double* sparse_get_next_entry(
			sparse_matrix *sm,		/**< sparse matrix */
			long *x_actual_pos,		/**< actual line position of entry */
			long *y_actual_pos,		/**< actual column position of entry */
			long *actual_index		/**< actual index of entry in sparse list */
			);

	/**
	 * \brief converts matrix in full matrix representation to sparse matrix
	 * representation using CRS-format, the list of matrix elements is ordered
	 * line by line and column by column
	 */
	int sparse_convert_full_to_sparse(
			double *A,				/**< matrix in full representation */
			sparse_matrix *sa,		/**< matrix in sparse representation */
			unsigned long m,		/**< number of lines of matrix */
			unsigned long n			/**< number of columns of matrix */
			);

	/**
	 * \brief converts matrix in sparse matrix representation to full matrix
	 * representation
	 *
	 * Before applying this method the memory of the matrix \f$A\f$ has to be
	 * allocated and set to 0.0.
	 */
	int sparse_convert_sparse_to_full(
			sparse_matrix *sa,		/**< matrix in sparse representation */
			double *A,				/**< matrix in full representation */
			unsigned long m,		/**< number of lines of matrix */
			unsigned long n			/**< number of columns of matrix */
			);
	
	/**
	   returns 1 if the matrixes are equal
	*/		
    int sparse_compare(sparse_matrix *sa,sparse_matrix *sb); 

#ifdef __cplusplus
}
#endif

#endif
