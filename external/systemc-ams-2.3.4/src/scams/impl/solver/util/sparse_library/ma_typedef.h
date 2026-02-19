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

 ma_typedef.h - description

 Original Author: Christoph Clauss Fraunhofer IIS/EAS Dresden

 Created on: 22.10.2009

 SVN Version       :  $Revision: 1401 $
 SVN last checkin  :  $Date: 2012-11-20 13:38:10 +0000 (Tue, 20 Nov 2012) $
 SVN checkin by    :  $Author: reuther $
 SVN Id            :  $Id: ma_typedef.h 1401 2012-11-20 13:38:10Z reuther $

 *****************************************************************************/

/**
 * @file 	ma_typedef.h
 * @author	Christiane Reuther
 * @date	November 13, 2012
 * @brief	Header-file to set type definitions
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
 *  File    :  ma_typedef.h
 *  Purpose :  very common type definitions
 *  Notes   :
 *  Author  :  C.Clauss
 *  Version :  $Id: ma_typedef.h 1401 2012-11-20 13:38:10Z reuther $
 *
 */


#ifndef _ma_typedef_h_
#define _ma_typedef_h_



/* /// Defines ////////////////////////////////////////////////////////////// */

typedef    double     value;      /**< real values                      */

typedef    int        count_near; /**< for counting in a tiny range     */
typedef    long int   count_far;  /**< for counting in a big range      */

typedef    int        err_code;   /**< returned error code in functions */

typedef    int        debug;      /**< has to consist of 16 bits        */

typedef    int       (*fppar)(char*,int*,double*,char*);
                     /**< function pointer for parameter set function */
#ifdef NOT_USED
typedef    int       (*fpprint)(FILE *);
                     /**< function pointer for parameter set function */
#endif

#endif /* _ma_typedef_h_ */

/* /// end of file ////////////////////////////////////////////////////////// */
