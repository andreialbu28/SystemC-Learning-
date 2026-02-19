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

 ma_util.h - description

 Original Author: Christoph Clauss Fraunhofer IIS/EAS Dresden

 Created on: 22.10.2009

 SVN Version       :  $Revision: 1446 $
 SVN last checkin  :  $Date: 2012-12-14 08:19:48 +0000 (Fri, 14 Dec 2012) $
 SVN checkin by    :  $Author: karsten $
 SVN Id            :  $Id: ma_util.h 1446 2012-12-14 08:19:48Z karsten $

 *****************************************************************************/

/**
 * @file 	ma_util.h
 * @author	Christiane Reuther
 * @date	November 13, 2012
 * @brief	Header-file to define methods for operations of matrices in full
 * representation and to define minimum/maximum functions
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
 *  File    :  ma_util.h
 *  Purpose :  header file for utilities of common mathematical purposes
 *  Notes   :
 *  Author  :  C.Clauss
 *  Version :  $Id: ma_util.h 1446 2012-12-14 08:19:48Z karsten $
 *
 */


#ifndef _ma_util_h_
#define _ma_util_h_


/* /// Headers ////////////////////////////////////////////////////////////// */

#include "ma_typedef.h"

/* MA_matfull.c */

/**
 * The function <i>MA_ProductValueMatrix</i> gets the value-matrix-product
 * \f$result = factor \; origin\f$ for matrices in full representation.
 */
void  MA_ProductValueMatrix(
		value* result,			/**< resulting matrix */
		value factor,			/**< real factor */
		value* origin,			/**< original matrix */
		count_near lin,			/**< number of lines */
		count_near col			/**< number of columns */
		);

/**
 * The function <i>MA_SumMatrixWeighted</i> gets the weighted sum
 * \f$C = fa\,A + fb\,B\f$ for matrices in full representation.
 */
void  MA_SumMatrixWeighted(
		value* C,				/**< resulting matrix */
		value fa,				/**< real factor to matrix A */
		value* A,				/**< matrix A */
		value fb,				/**< real factor to matrix B */
		value* B,				/**< matrix B */
		count_near lin,			/**< number of lines */
		count_near col			/**< number of columns */
		);

/**
 * gets minimum of integers x and y
 */
int MA_min(
		int x, 					/**< integer x */
		int y					/**< integer y */
		);

/**
 * gets maximum of integers x and y
 */
int MA_max(
		int x, 					/**< integer x */
		int y					/**< integer y */
		);

#endif /* _ma_util_h_ */


/* /// end of file ////////////////////////////////////////////////////////// */
