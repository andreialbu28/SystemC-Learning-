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

 linear_analog_solver.h - description

 Original Author: Karsten Einwich Fraunhofer IIS/EAS Dresden

 Created on: 22.10.2009

 SVN Version       :  $Revision: 2320 $
 SVN last checkin  :  $Date: 2023-06-08 12:47:38 +0000 (Thu, 08 Jun 2023) $
 SVN checkin by    :  $Author: pehrlich $
 SVN Id            :  $Id: linear_analog_solver.h 2320 2023-06-08 12:47:38Z pehrlich $

 *****************************************************************************/

/**
 * @file 	linear_analog_solver.h
 * @author	Christiane Reuther
 * @date	November 07, 2012
 * @brief	Header-file to define methods for linear solver
 * - interface between C and C++
 */

/*****************************************************************************/

#ifndef  LINEAR_ANALOG_SOLVER_H
#define  LINEAR_ANALOG_SOLVER_H

#ifdef __cplusplus
extern "C"
{
#endif

  struct sca_solv_data;				/**< internal solver data */
  struct sca_solv_checkpoint_data;  /**< internal checkpoint data */
  struct sca_coefficients;			/**< stored matrix B and vector q */

  typedef struct  sparse sparse_matrix;  /**< sparse matrix */

  /* ana_init.c */

  /**
   * \brief initializes internal solver data, generates code for
   * factorization/substitution for matrices \f$A\f$ and \f$B\f$ in 
   * full matrix representation
   */
  int ana_init (
          double* A,             /**< square matrix A */
          double* B,             /**< square matrix B */
          unsigned long size,    /**< number of equations -> dim of A and B */
          double  h,             /**< time step */
          sca_solv_data **sdatap,/**< internal solver data */
          int reinit             /**< integer for allocating memory */
               );

  /**
   * \brief initializes internal solver data, generates code for
   * factorization/substitution for matrices \f$A\f$ and \f$B\f$ in 
   * sparse matrix representation
   */
  int ana_init_sparse (
            sparse_matrix* sA, 		/**< sparse matrix \f$A\f$ */
            sparse_matrix* sB,		/**< sparse matrix \f$B\f$ */
            double  h,             	/**< time step */
            sca_solv_data **sdatap,	/**< internal solver data */
            int reinit             	/**< integer for allocating memory */
               );

  /* ana_reinit.c */

  /**
   * \brief reinitializes internal solver data, generates code for
   * factorization/substitution for matrices \f$A\f$ and \f$B\f$ in
   * full matrix representation
   */
  int ana_reinit (
          double* A,             /**< square matrix A */
          double* B,             /**< square matrix B */
          unsigned long size,    /**< number of equations -> dim of A and B */
          double  h,             /**< time step */
          sca_solv_data **sdatap,/**< internal solver data */
          int reinit             /**< integer for allocating memory */
                 );
  /**
   * \brief reinitializes internal solver data, generates code for
   * factorization/substitution for matrices \f$A\f$ and \f$B\f$ in
   * sparse matrix representation
   */
  int ana_reinit_sparse (
		  sparse_matrix* sA, 		/**< sparse matrix A */
          sparse_matrix* sB,		/**< sparse matrix B */
          double  h,              /**< constant time step */
          sca_solv_data **sdatap, /**< internal solver data */
          int reinit              /**< integer for solution method */
          );

  /* ana_solv.c */

  /**
   * \brief generates solution of linear system of equations
   */
  void ana_solv (
          double* q,      		/**< time dependent vector */
          double* x,     			/**< state vector */
          sca_solv_data* sdata 	/**< internal solver data */
                );

  /* ana_solve_woodbury.c */

  /**
   * \brief generates solution of linear system of equations using the
   * formula of Woodbury
   */
  int ana_solve_woodbury (
          double* q,      			/**< time dependent vector */
          double* x,      			/**< state vector */
          sca_solv_data* sdata, 	/**< internal solver data */
          long k,					/**< number of modifications in matrix*/
          int* mode, 				/**< kind of modifications in matrix*/
          long* n,					/**< indices of modifications in matrix*/
          double* s,				/**< values of modifications in matrix*/
          int first_call			/**< integer for first call of
          	  	  	  	  	  	  	  <i>ana_solve_woodbury</i> */
      );

  /* ana_LUdecomposition.c */

  /**
   * \brief carries out factorization of matrix \f$A\f$ in sparse matrix
   * representation
   */
  int ana_LU_decomp_A_sparse(
		  sca_solv_data **sdatap,	/**< internal solver data */
		  int first_dec				/**< first_dec = 0: allocate memory */
		  );

  /**
   * \brief carries out factorization of matrix \f$A\f$ in full matrix
   * representation
   */
  int ana_LU_decomp_A(
		  sca_solv_data **sdatap,	/**< internal solver data */
		  int first_dec				/**< first_dec = 0: allocate memory */
		  );

  /**
   * \brief gets rank of matrix A
   */
  long ana_get_rank_A(
		  sca_solv_data* data		/**< internal solver data */
		  );

  /**
   * \brief gets number of non-zero elements of lower triangular matrix L
   */
  long ana_get_size_L(
		  sca_solv_data* data		/**< internal solver data */
		  );

  /**
   * \brief gets number of non-zero elements of upper triangular matrix R
   */
  long ana_get_size_R(
		  sca_solv_data* data		/**< internal solver data */
		  );

  /* ana_utilities.c */

  /**
   * The method <i>ana_get_error_position</i> gets position in matrix which
   * causes a singularity in sparse matrix <i>data->sZ_euler</i> or
   * <i>data->sZ_trapez</i>.
   */
  void ana_get_error_position(
		  sca_solv_data* data,		/**< internal solver data */
		  long* row,				/**< erroneous line */
		  long* column				/**< erroneous column */
		  );

  /**
   * The method <i>ana_get_algorithm</i> outputs the solution cur_algorithm
   * <i>data->cur_algorithm</i>:
   * <ul>
   * <li> 1 - backward Euler method
   * <li> 2 - trapezoidal method
   * </ul>
   */
  int ana_get_algorithm(
		  sca_solv_data* data		/**< internal solver data */
		  );

  /**
   * The method <i>ana_set_algorithm</i> sets the solution method
   * <i>data->cur_algorithm</i>:
   * <ul>
   * <li> <i>alg</i> = 1 - backward Euler method: <i>data->cur_algorithm = EULER</i>
   * <li> <i>alg</i> = 2 - trapezoidal method: <i>data->cur_algorithm = TRAPEZ</i>
   * </ul>
   */
  void ana_set_algorithm(
		  sca_solv_data* data,		/**< internal solver data */
		  int alg					/**< cur_algorithm: 1(Euler), 2(Trapez) */
		  );


  /**
   * The method <i>ana_set_algorithm</i> sets the euler steps after reinit
   */
  void ana_set_reinit_steps(
		  sca_solv_data* data,		/**< internal solver data */
		  int steps					/**< cur_algorithm: number of steps */
		  );

  /**
   * The method <i>ana_get_dimension</i> outputs the rank of the system of
   * equations <i>data->dim</i>.
   */
  long int ana_get_dimension(
		  sca_solv_data* data		/**< internal solver data */
		  );

  /**
   * The method <i>ana_get_pivot_flop</i> outputs the number of flops needed for
   * pivotal search during factorization of the coefficient matrix of the system
   * of equations.
   */
  long int ana_get_pivot_flop(
		  sca_solv_data* data,		/**< internal solver data */
		  int alg					/**< cur_algorithm: 1(Euler), 2(Trapez) */
		  );

  /**
   * The method <i>ana_get_dec_flop</i> outputs the number of flops needed for
   * matrix calculations during factorization of the coefficient matrix of the
   * system of equations.
   */
  long int ana_get_dec_flop(
		  sca_solv_data* data,		/**< internal solver data */
		  int alg					/**< cur_algorithm: 1(Euler), 2(Trapez) */
		  );

  /**
   * The method <i>ana_get_sol_flop</i> outputs the number of flops needed for
   * substitution to solve the linear system
   * of equations.
   */
  long int ana_get_sol_flop(
		  sca_solv_data* data,		/**< internal solver data */
		  int alg					/**< cur_algorithm: 1(Euler), 2(Trapez) */
		  );


  /************************************/

  /**
   * The method <i>ana_store_solver_check_point</i> stores the solution vector \f$x\f$
   * and its first derivative <i>sdata->xp</i> at current time.
   */
  int ana_store_solver_check_point(
		  sca_solv_data* sdata,		            /**< internal solver data */
		  double* x,					        /**< solution vector */
		  sca_solv_checkpoint_data** cp_data    /**< internal check point data */
		  );


  /**
   * The method <i>ana_restore_solver_check_point</i> restores the solution vector
   * \f$x\f$ and its first derivative <i>sdata->xp</i> at the last time step.
   */
  int ana_restore_solver_check_point(
		  sca_solv_data* sdata,		            /**< internal solver data */
		  double* x,					        /**< solution vector */
		  sca_solv_checkpoint_data** cp_data    /**< internal check point data */
		  );

  /**
   * The method <i>ana_copy_solver_check_point</i> copies checkpoint data
   * from cp_data_source to cp_data_sink.
   */
  int ana_copy_solver_check_point(
  		  sca_solv_checkpoint_data** cp_data_dest,    /**< internal check point data destination*/
  		  sca_solv_checkpoint_data* cp_data_source    /**< internal check point data source*/
  		  );

  /**
   * The method <i>ana_allocate_solver_check_point</i> allocates memory
   * for checkpoint store
   */
  int ana_allocate_solver_check_point(
  		  unsigned long size,                   /**< number of state variables */
  		  sca_solv_checkpoint_data** cp_data    /**< internal check point data */
  		  );

  void free_checkpoint_data(sca_solv_checkpoint_data** cp_data);


#ifdef __cplusplus
}
#endif

#endif
