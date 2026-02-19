/*****************************************************************************

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

  convert_from_string.h - description

  Original Author: Karsten Einwich COSEDA Technologies GmbH Dresden

  Created on: Aug 27, 2015

   SVN Version       :  $Revision$
   SVN last checkin  :  $Date$
   SVN checkin by    :  $Author$
   SVN Id            :  $Id$

 *****************************************************************************/


#ifndef SRC_SCAMS_IMPL_UTIL_DATA_TYPES_CONVERT_FROM_STRING_H_
#define SRC_SCAMS_IMPL_UTIL_DATA_TYPES_CONVERT_FROM_STRING_H_

#include <string>
#include <sstream>


namespace sca_util
{
namespace sca_implementation
{

template<class T>
inline bool convert_from_string(T& val,const std::string&)
{
	return false;
}

template<class T>
inline bool convert_by_istream(T& val,const std::string& str)
{
	std::istringstream istr(str);

	if(istr >> val) return true;
	else            return false;
}


template<>
bool convert_from_string<double>(double&,const std::string&);


template<>
bool convert_from_string(float&,const std::string&);

template<>
bool convert_from_string(int&,const std::string&);

template<>
bool convert_from_string(long&,const std::string&);

template<>
bool convert_from_string(unsigned int&,const std::string&);

template<>
bool convert_from_string(unsigned long&,const std::string&);

template<>
bool convert_from_string(bool&,const std::string&);

template<>
bool convert_from_string(char&,const std::string&);

template<>
bool convert_from_string(short&,const std::string&);

template<>
bool convert_from_string(unsigned short&,const std::string&);

template<>
bool convert_from_string(sc_dt::sc_logic&,const std::string&);

template<>
bool convert_from_string(sc_dt::int64&,const std::string&);

template<>
bool convert_from_string(sc_dt::uint64&,const std::string&);


template<int N>
inline bool convert_from_string(sc_dt::sc_int<N>& val,const std::string& str)
{
	return convert_by_istream<sc_dt::sc_int<N> >(val,str);
}


template<int N>
inline bool convert_from_string(sc_dt::sc_uint<N>& val,const std::string& str)
{
	return convert_by_istream<sc_dt::sc_uint<N> >(val,str);
}

template<int N>
inline bool convert_from_string(sc_dt::sc_bv<N>& val,const std::string& str)
{
	return convert_by_istream<sc_dt::sc_bv<N> >(val,str);
}

template<int N>
inline bool convert_from_string(sc_dt::sc_lv<N>& val,const std::string& str)
{
	return convert_by_istream<sc_dt::sc_lv<N> >(val,str);
}



}
}


#endif /* SRC_SCAMS_IMPL_UTIL_DATA_TYPES_CONVERT_FROM_STRING_H_ */

