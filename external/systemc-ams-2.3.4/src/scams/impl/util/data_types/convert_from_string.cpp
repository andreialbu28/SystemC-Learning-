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

  convert_from_string.cpp - description

  Original Author: Karsten Einwich COSEDA Technologies GmbH Dresden

  Created on: Aug 27, 2015

   SVN Version       :  $Revision$
   SVN last checkin  :  $Date$
   SVN checkin by    :  $Author$
   SVN Id            :  $Id$

 *****************************************************************************/

#include <systemc>
#include "convert_from_string.h"


namespace sca_util
{
namespace sca_implementation
{

template<>
bool convert_from_string(double& val, const std::string& str)
{
	return convert_by_istream<double>(val,str);
}


template<>
bool convert_from_string(float& val,const std::string& str)
{
	return convert_by_istream<float>(val,str);
}

template<>
bool convert_from_string(int& val,const std::string& str)
{
	return convert_by_istream<int>(val,str);
}

template<>
bool convert_from_string(long& val,const std::string& str)
{
	return convert_by_istream<long>(val,str);
}

template<>
bool convert_from_string(unsigned int& val,const std::string& str)
{
	return convert_by_istream<unsigned int>(val,str);
}

template<>
bool convert_from_string(unsigned long& val,const std::string& str)
{
	return convert_by_istream<unsigned long>(val,str);
}

template<>
bool convert_from_string(bool& val,const std::string& str)
{
	return convert_by_istream<bool>(val,str);
}

template<>
bool convert_from_string(char& val,const std::string& str)
{
	return convert_by_istream<char>(val,str);
}

template<>
bool convert_from_string(short& val,const std::string& str)
{
	return convert_by_istream<short>(val,str);
}

template<>
bool convert_from_string(unsigned short& val,const std::string& str)
{
	return convert_by_istream<unsigned short>(val,str);
}

template<>
bool convert_from_string(sc_dt::sc_logic& val,const std::string& str)
{
	return convert_by_istream<sc_dt::sc_logic>(val,str);
}

template<>
bool convert_from_string(sc_dt::int64& val,const std::string& str)
{
	return convert_by_istream<sc_dt::int64>(val,str);
}

template<>
bool convert_from_string(sc_dt::uint64& val,const std::string& str)
{
	return convert_by_istream<sc_dt::uint64>(val,str);
}

}
}
