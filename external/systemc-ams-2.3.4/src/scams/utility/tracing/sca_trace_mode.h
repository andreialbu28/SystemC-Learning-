/*****************************************************************************

    Copyright 2010-2012
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

 sca_trace_mode.h - sca_trace_mode manipulators

 Original Author: Karsten Einwich Fraunhofer IIS/EAS Dresden

 Created on: 08.03.2009

 SVN Version       :  $Revision: 1305 $
 SVN last checkin  :  $Date: 2012-04-11 15:13:49 +0000 (Wed, 11 Apr 2012) $
 SVN checkin by    :  $Author: karsten $
 SVN Id            :  $Id: sca_trace_mode.h 1305 2012-04-11 15:13:49Z karsten $

 *****************************************************************************/
/*
 * LRM clause 6.1.1.1.
 * The class sca_util::sca_trace_mode_base shall define the base class for trace
 * mode manipulators. The manipulators, which shall be derived from this base
 * class, are predefined. An application shall not create an instance and shall
 * not derive from this class.
 * Instances of derived classes can only be used as argument to the member
 * function set_mode of class sca_util::sca_trace_file (see LRM clause 6.1.2.5.
 * sca_trace_file::set_mode).
 * An implementation shall at least support the trace mode manipulators as
 * defined in this subclause.
 */

/*****************************************************************************/

#ifndef SCA_TRACE_MODE_H_
#define SCA_TRACE_MODE_H_

//LRM clause 6.1.1.1.
namespace sca_util
{

class sca_trace_mode_base
{
public:
	virtual ~sca_trace_mode_base()=0;
};

enum sca_ac_fmt
{
	SCA_AC_REAL_IMAG, SCA_AC_MAG_RAD, SCA_AC_DB_DEG
};

class sca_ac_format: public sca_util::sca_trace_mode_base
{
public:
	sca_ac_format(sca_util::sca_ac_fmt format = sca_util::SCA_AC_REAL_IMAG);

	// begin implementation specific
	const sca_util::sca_ac_fmt ac_format;


	// end implementation specific

};

enum sca_noise_fmt
{
	SCA_NOISE_SUM, SCA_NOISE_ALL
};

class sca_noise_format: public sca_util::sca_trace_mode_base
{
public:
	sca_noise_format(sca_util::sca_noise_fmt format = sca_util::SCA_NOISE_SUM);

	// begin implementation specific
	const sca_util::sca_noise_fmt noise_format;
	// end implementation specific
};

class sca_decimation: public sca_util::sca_trace_mode_base
{
public:
	sca_decimation(unsigned long n);

	// begin implementation specific
	const unsigned long decimation_factor;
	// end implementation specific
};

class sca_sampling: public sca_util::sca_trace_mode_base
{
public:
	sca_sampling(const sca_core::sca_time& tstep,
			const sca_core::sca_time& toffset = sc_core::SC_ZERO_TIME);

	sca_sampling(double tstep,sc_core::sc_time_unit unit,
			     double toffset=0.0,sc_core::sc_time_unit ounit=sc_core::SC_SEC);

	// begin implementation specific
	const sca_core::sca_time time_step;
	const sca_core::sca_time time_offset;
	// end implementation specific
};

enum sca_multirate_fmt { SCA_INTERPOLATE, SCA_DONT_INTERPOLATE, SCA_HOLD_SAMPLE};


class sca_multirate : public sca_util::sca_trace_mode_base
{
public:
	sca_multirate(sca_util::sca_multirate_fmt format=sca_util::SCA_INTERPOLATE);

// begin implementation specific

	const sca_multirate_fmt fmt;

// end implem,entation specific

};



} // namespace sca_util

#endif /* SCA_TRACE_MODE_H_ */
