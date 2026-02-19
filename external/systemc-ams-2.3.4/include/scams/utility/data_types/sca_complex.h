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

 sca_complex.h - data type for complex numbers

 Original Author: Karsten Einwich Fraunhofer IIS/EAS Dresden

 Created on: 08.03.2009

 SVN Version       :  $Revision: 1911 $
 SVN last checkin  :  $Date: 2016-02-16 13:51:22 +0000 (Tue, 16 Feb 2016) $
 SVN checkin by    :  $Author: karsten $
 SVN Id            :  $Id: sca_complex.h 1911 2016-02-16 13:51:22Z karsten $

 *****************************************************************************/
/*
 * The type sca_util::sca_complex shall provide a type for complex numbers.
 */

/*****************************************************************************/

#ifndef SCA_COMPLEX_H_
#define SCA_COMPLEX_H_

namespace sca_util
{

typedef std::complex<double> sca_complex;

} // namespace sca_util

#endif /* SCA_COMPLEX_H_ */
