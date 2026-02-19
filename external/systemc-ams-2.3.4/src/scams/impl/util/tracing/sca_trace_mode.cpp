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

 sca_trace_mode.cpp - description

 Original Author: Karsten Einwich Fraunhofer IIS/EAS Dresden

 Created on: 16.11.2009

 SVN Version       :  $Revision: 1305 $
 SVN last checkin  :  $Date: 2012-04-11 15:13:49 +0000 (Wed, 11 Apr 2012) $
 SVN checkin by    :  $Author: karsten $
 SVN Id            :  $Id: sca_trace_mode.cpp 1305 2012-04-11 15:13:49Z karsten $

 *****************************************************************************/

/*****************************************************************************/

#include "systemc-ams"
#include "scams/utility/tracing/sca_trace_mode.h"

namespace sca_util
{

sca_trace_mode_base::~sca_trace_mode_base()
{
}

sca_ac_format::sca_ac_format(sca_util::sca_ac_fmt format) :
	ac_format(format)
{
}

sca_noise_format::sca_noise_format(sca_util::sca_noise_fmt format) :
	noise_format(format)
{
}

sca_decimation::sca_decimation(unsigned long n) :
	decimation_factor(n)
{
}

sca_sampling::sca_sampling(const sca_core::sca_time& tstep,
		const sca_core::sca_time& toffset) :
	time_step(tstep), time_offset(toffset)
{
}


sca_sampling::sca_sampling( double tstep,sc_core::sc_time_unit unit,
	                        double toffset,sc_core::sc_time_unit ounit) :
	time_step(sca_core::sca_time(tstep,unit)),
	time_offset(sca_core::sca_time(toffset,ounit))
{
}


sca_multirate::sca_multirate(sca_util::sca_multirate_fmt format):fmt(format)
{
}



} // namespace sca_util

