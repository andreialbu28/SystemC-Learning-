/*****************************************************************************

    Copyright 2010-2013
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

  sca_function_vector.h - description

  Original Author: Karsten Einwich Fraunhofer IIS/EAS Dresden

  Created on: 06.11.2009

   SVN Version       :  $Revision: 1523 $
   SVN last checkin  :  $Date: 2013-02-17 20:36:57 +0000 (Sun, 17 Feb 2013) $
   SVN checkin by    :  $Author: karsten $
   SVN Id            :  $Id: sca_function_vector.h 1523 2013-02-17 20:36:57Z karsten $

 *****************************************************************************/

/*****************************************************************************/


#ifndef SCA_FUNCTION_VECTOR_H_
#define SCA_FUNCTION_VECTOR_H_


namespace sca_util
{
namespace sca_implementation
{

template <class TR>
class sca_function_vector: public
	sca_util::sca_vector<sca_util::sca_implementation::sca_method_list<TR> >
{
    typedef sca_util::sca_vector<sca_util::sca_implementation::sca_method_list<TR> > base_type;

    sca_util::sca_vector<TR> val_vector;

public:

    void reset();
    void calc_vector();
    TR* get_flat_vector();
    TR* get_calc_flat();
    sca_util::sca_vector<TR>& get_calc();
};

////////////////////////////////////////////////////////////////////////////

template<class TR>
inline void sca_function_vector<TR>::reset()
{
    for(size_t i=0;i<base_type::matrix.size();++i) base_type::matrix[i].clear_list();
    sca_util::sca_vector<sca_util::sca_implementation::sca_method_list<TR> >::reset();
}

template<class TR>
inline void sca_function_vector<TR>::calc_vector()
{
    if(val_vector.length()<base_type::length() )
        val_vector.resize(base_type::length());

    for(size_t i=0;i<base_type::matrix.size();++i)
    {
        val_vector[(unsigned long)i]=base_type::matrix[(unsigned long)i].calc_value();
    }
}


template<class TR>
inline TR* sca_function_vector<TR>::get_flat_vector()
{
    return val_vector.get_flat();
}

template<class TR>
inline TR* sca_function_vector<TR>::get_calc_flat()
{
    calc_vector();
    return get_flat_vector();
}


template<class TR>
inline sca_util::sca_vector<TR>& sca_function_vector<TR>::get_calc()
{
    calc_vector();
    return val_vector;
}



}
}


#endif /* SCA_FUNCTION_VECTOR_H_ */
