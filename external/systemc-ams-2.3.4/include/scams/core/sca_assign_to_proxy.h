/*****************************************************************************

    Copyright 2010
    Fraunhofer-Gesellschaft zur Foerderung der angewandten Forschung e.V.

    Copyright 2015-2016
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

 sca_assign_to_proxy.h - description

 Original Author: Karsten Einwich Fraunhofer IIS/EAS / COSEDA Technologies

 Created on: 05.02.2010

 SVN Version       :  $Revision: 1907 $
 SVN last checkin  :  $Date: 2016-02-13 19:18:55 +0000 (Sat, 13 Feb 2016) $
 SVN checkin by    :  $Author: karsten $
 SVN Id            :  $Id: sca_assign_to_proxy.h 1907 2016-02-13 19:18:55Z karsten $

 *****************************************************************************/

/*****************************************************************************/

#ifndef SCA_ASSIGN_TO_PROXY_H_
#define SCA_ASSIGN_TO_PROXY_H_

namespace sca_core
{

template<class T, class TV>
class sca_assign_to_proxy
{
public:
	sca_core::sca_assign_to_proxy<T,TV>& operator=(const TV& value);

//begin implementation specific
	T* obj;
	unsigned long index;

//end implementation specific

};

//begin implementation specific

template<class T, class TV>
inline sca_core::sca_assign_to_proxy<T,TV>& sca_assign_to_proxy<T,TV>::operator=(const TV& value)
{
	obj->write(value,index);
	return *this;
}

//end implementation specific

} //namespace sca_core
#endif /* SCA_ASSIGN_TO_PROXY_H_ */
