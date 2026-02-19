/*****************************************************************************

    Copyright 2010-2014
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

 sca_tdf_out_ct_interpolator.cpp - tdf declustering outport

 Original Author: Karsten Einwich Fraunhofer IIS/EAS Dresden

 Created on: 18.10.2011

 SVN Version       :  $Revision: 2320 $
 SVN last checkin  :  $Date: 2023-06-08 12:47:38 +0000 (Thu, 08 Jun 2023) $ (UTC)
 SVN checkin by    :  $Author: pehrlich $
 SVN Id            :  $Id: sca_tdf_default_interpolator.cpp 2320 2023-06-08 12:47:38Z pehrlich $

 *****************************************************************************/
/*
 * New SystemC-AMS 1.x feature
 */
/*****************************************************************************/

#include "scams/impl/predefined_moc/tdf/sca_ct_delay_buffer.h"
#include "scams/predefined_moc/tdf/sca_tdf_default_interpolator.h"

namespace sca_tdf
{

sca_default_interpolator<double>::sca_default_interpolator()
{
	previous_value=0.0;
	next_value=0.0;
}


void sca_default_interpolator<double>::store_value(const sca_core::sca_time& time,const double& value)
{
	if(time>next_time)
	{
		previous_value=next_value;
		previous_time=next_time;

	}
	else if(time<=previous_time)
	{
		previous_time=time;
		previous_value=value;
	}

	next_value=value;
	next_time=time;

}

double sca_default_interpolator<double>::get_value(const sca_core::sca_time& time) const
{
	if(next_time==previous_time) return next_value;

	double val;

	//linear interpolation
	val = (next_value-previous_value)/(next_time-previous_time).to_seconds() *
			(time-previous_time).to_seconds() + previous_value;

	return val;
}


///////////////////////////////////////////////////////////////////////////
//specialization for sca_complex

sca_default_interpolator<sca_util::sca_complex>::sca_default_interpolator()
{
	previous_value = sca_util::sca_complex(0.0,0.0);
	next_value     = sca_util::sca_complex(0.0,0.0);
}


void sca_default_interpolator<sca_util::sca_complex>::store_value(const sca_core::sca_time& time,const sca_util::sca_complex& value)
{
	if(time>next_time)
	{
		previous_value=next_value;
		previous_time=next_time;

	}
	else if(time<=previous_time)
	{
		previous_time=time;
		previous_value=value;
	}

	next_value=value;
	next_time=time;
}

sca_util::sca_complex sca_default_interpolator<sca_util::sca_complex>::get_value(const sca_core::sca_time& time) const
{
	if(next_time==previous_time) return next_value;

	sca_util::sca_complex val;

	//linear interpolation
	val = (next_value-previous_value)/(next_time-previous_time).to_seconds() *
			(time-previous_time).to_seconds() + previous_value;

	return val;
}

}
