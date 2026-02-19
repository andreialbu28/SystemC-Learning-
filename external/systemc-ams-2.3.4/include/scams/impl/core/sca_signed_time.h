/*****************************************************************************

    Copyright 2010-2013
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

  sca_signed_time.h - provides class and operators for signed time
                      (a time which can be negative)

  Original Author: Karsten Einwich Fraunhofer / COSEDA Technologies

  Created on: 14.03.2013

   SVN Version       :  $Revision: 2133 $
   SVN last checkin  :  $Date: 2020-03-27 14:06:08 +0000 (Fri, 27 Mar 2020) $
   SVN checkin by    :  $Author: karsten $
   SVN Id            :  $Id: sca_signed_time.h 2133 2020-03-27 14:06:08Z karsten $

 *****************************************************************************/

/*****************************************************************************/



#ifndef _SCA_SIGNED_TIME_H_
#define _SCA_SIGNED_TIME_H_

namespace sca_core
{
namespace sca_implementation
{


class sca_signed_time : public sca_core::sca_time
{
public:

	sca_signed_time();
	sca_signed_time(double, sc_core::sc_time_unit);
	sca_signed_time(const sca_signed_time&);
	sca_signed_time(const sca_core::sca_time&);


	double to_double() const;
	double to_seconds() const;
	const std::string to_string() const;


    bool operator <  ( const sca_core::sca_time& ) const;
    bool operator <= ( const sca_core::sca_time& ) const;
    bool operator >  ( const sca_core::sca_time& ) const;
    bool operator >= ( const sca_core::sca_time& ) const;


    bool operator <  ( const sca_signed_time& ) const;
    bool operator <= ( const sca_signed_time& ) const;
    bool operator >  ( const sca_signed_time& ) const;
    bool operator >= ( const sca_signed_time& ) const;


    void print(std::ostream& os = std::cout ) const;

    sca_signed_time(const sc_dt::uint64&,bool);

private:

    typedef sca_core::sca_time base_class;


    bool is_max_time;

};


std::ostream& operator << ( std::ostream&, const sca_signed_time& );

inline
sca_signed_time::sca_signed_time(const sc_dt::uint64& v,bool s) : sca_core::sca_time(v,s)
{
	is_max_time=false;
}

inline
const sca_signed_time
operator + ( const sca_signed_time& t1, const sca_signed_time& t2 )
{
#if defined(SYSTEMC_VERSION) & (SYSTEMC_VERSION > 20120701)
    return sca_signed_time(sc_core::sc_time::from_value(t1.value() + t2.value()));
#else
    return sca_signed_time(t1.value() + t2.value(),false);
#endif
}



inline
const sca_signed_time
operator + ( const sc_core::sc_time& t1, const sca_signed_time& t2 )
{
#if defined(SYSTEMC_VERSION) & (SYSTEMC_VERSION > 20120701)
    return sca_signed_time(sc_core::sc_time::from_value(t1.value() + t2.value()));
#else
    return sca_signed_time(t1.value() + t2.value(),false);
#endif
}


inline
const sca_signed_time
operator + ( const sca_signed_time& t1, const sc_core::sc_time& t2 )
{
#if defined(SYSTEMC_VERSION) & (SYSTEMC_VERSION > 20120701)
    return sca_signed_time(sc_core::sc_time::from_value(t1.value() + t2.value()));
#else
    return sca_signed_time(t1.value() + t2.value(),false);
#endif
}






inline
const sca_signed_time
operator - ( const sca_signed_time& t1, const sca_signed_time& t2 )
{
#if defined(SYSTEMC_VERSION) & (SYSTEMC_VERSION > 20120701)
    return sca_signed_time(sc_core::sc_time::from_value(t1.value() - t2.value()));
#else
    return sca_signed_time(t1.value() - t2.value(),false);
#endif
}


inline
const sca_signed_time
operator - ( const sc_core::sc_time& t1, const sca_signed_time& t2 )
{
#if defined(SYSTEMC_VERSION) & (SYSTEMC_VERSION > 20120701)
	sca_signed_time tm(sc_core::sc_time::from_value(t1.value() - t2.value()));
#else
	sca_signed_time tm(t1.value() - t2.value(),false);
#endif

	return tm;
}


inline
const sca_signed_time
operator - ( const sca_signed_time& t1, const sc_core::sc_time& t2 )
{
#if defined(SYSTEMC_VERSION) & (SYSTEMC_VERSION > 20120701)
    return sca_signed_time(sc_core::sc_time::from_value(t1.value() - t2.value()));
#else
    return sca_signed_time(t1.value() - t2.value(),false);
#endif
}



}
}

#endif
