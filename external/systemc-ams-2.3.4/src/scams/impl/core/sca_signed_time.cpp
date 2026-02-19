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

  sca_signed_time.cpp - provides class and operators for signed time
                      (a time which can be negative)

  Original Author: Karsten Einwich Fraunhofer IIS/EAS Dresden

  Created on: 14.03.2013

   SVN Version       :  $Revision: 2320 $
   SVN last checkin  :  $Date: 2023-06-08 12:47:38 +0000 (Thu, 08 Jun 2023) $
   SVN checkin by    :  $Author: pehrlich $
   SVN Id            :  $Id: sca_signed_time.cpp 2320 2023-06-08 12:47:38Z pehrlich $

 *****************************************************************************/

/*****************************************************************************/
#include "systemc-ams"

//IMPROVE: implement as 65Bit signed to prevent positive range reduction !!!!!!

namespace sca_core
{
namespace sca_implementation
{

//maximum positive value
static const sc_dt::uint64 MAX_POS=(sc_dt::uint64(0-1))>>1;

//constructors
sca_signed_time::sca_signed_time() : sca_core::sca_time() , is_max_time(false)
{
}

sca_signed_time::sca_signed_time(double val, sc_core::sc_time_unit unit) :
		sca_core::sca_time(fabs(val),unit) , is_max_time(false)
{
	if(val<0.0) (*this)=sc_core::SC_ZERO_TIME-(*this);
}


sca_signed_time::sca_signed_time(const sca_signed_time& tm) :
		sca_core::sca_time(tm) , is_max_time(tm.is_max_time)
{
}

sca_signed_time::sca_signed_time(const sca_core::sca_time& tm) :
		sca_core::sca_time(tm)
{
	is_max_time=false;
	if(tm.value()>MAX_POS)
	{
		if(tm==sca_core::sca_max_time())
		{
			is_max_time=true;
		}
		else
		{
//		SC_REPORT_WARNING("SystemC-AMS",
//				"Internal: initialized signed time with unsigned time larger than uint64/2");
		}
	}
}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////



double sca_signed_time::to_double() const
{
	if(this->value()>MAX_POS)
	{
		return -sc_dt::uint64_to_double(sc_dt::uint64(0) - this->value() );
	}
	else
	{
		return base_class::to_double();
	}
}


double sca_signed_time::to_seconds() const
{
	if(this->value()>MAX_POS)
	{
		return -(sc_dt::uint64_to_double( sc_dt::uint64(0) - this->value() ) *
				sc_core::sc_get_time_resolution().to_seconds());
	}
	else
	{
		return base_class::to_seconds();
	}
}


const std::string sca_signed_time::to_string() const
{
	if(this->value()>MAX_POS)
	{
		return  "-" +
				sca_core::sca_time(sc_dt::uint64_to_double(sc_dt::uint64(0) - this->value() ) *
				sc_core::sc_get_time_resolution().to_seconds(),sc_core::SC_SEC).to_string();
	}
	else
	{
		return base_class::to_string();
	}
}



bool sca_signed_time::operator <  ( const sca_core::sca_time& tm) const
{
	if(this->value()>MAX_POS)
	{
		return true;
	}

	return (this->value()<tm.value());
}


bool sca_signed_time::operator <= ( const sca_core::sca_time& tm) const
{
	if(this->value()>MAX_POS)
	{
		return true;
	}

	return (this->value()<=tm.value());
}

bool sca_signed_time::operator >  ( const sca_core::sca_time& tm) const
{
	if(this->value()>MAX_POS)
	{
		return false;
	}

	return (this->value() > tm.value());
}

bool sca_signed_time::operator >= ( const sca_core::sca_time& tm) const
{
	if(this->value()>MAX_POS)
	{
		return false;
	}

	return (this->value() >= tm.value());
}



bool sca_signed_time::operator <(const sca_signed_time& tm) const
{
	return (sc_dt::int64)(this->value()) < (sc_dt::int64)(tm.value());
}


bool sca_signed_time::operator <=(const sca_signed_time& tm) const
{
	return (sc_dt::int64)(this->value()) <= (sc_dt::int64)(tm.value());
}


bool sca_signed_time::operator >(const sca_signed_time& tm) const
{
	return (sc_dt::int64)(this->value()) > (sc_dt::int64)(tm.value());
}


bool sca_signed_time::operator >=(const sca_signed_time& tm) const
{
	return (sc_dt::int64)(this->value()) >= (sc_dt::int64)(tm.value());
}



void sca_signed_time::print( std::ostream& os ) const
{
	os << this->to_string();
}



std::ostream& operator << (std::ostream& str, const sca_signed_time& tm)
{
	str << tm.to_string();
	return str;
}


} // namespace sca_implementation
} // namespace sca_core
