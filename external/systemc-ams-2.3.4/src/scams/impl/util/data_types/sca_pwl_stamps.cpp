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

  sca_pwl_stamps.cpp - description

  Original Author: Karsten Einwich Fraunhofer IIS/EAS Dresden

  Created on: Mar 20, 2014

   SVN Version       :  $Revision$
   SVN last checkin  :  $Date$
   SVN checkin by    :  $Author$
   SVN Id            :  $Id$

 *****************************************************************************/


#include "sca_pwl_stamps.h"

namespace sca_util
{
namespace sca_implementation
{



void sca_util::sca_implementation::sca_pwl_stamps::clear()
{
	pwl_stamps.clear();
}

void sca_util::sca_implementation::sca_pwl_stamps::add_pwl_stamp(
		unsigned long  idy,
		unsigned long  idx,
		const std::vector<double> & pwl_vector)
{

	pwl_stamps.resize(pwl_stamps.size()+1);

	sca_pwl_stamp& stamp(pwl_stamps[pwl_stamps.size()-1]);

	stamp.nl_eq_idx=idy;
	stamp.var_idx=idx;
	stamp.arg_idx=idx;
	stamp.pwl_vector=pwl_vector;
	stamp.b_stamps=false;
}

void sca_util::sca_implementation::sca_pwl_stamps::add_pwl_b_stamp(
		unsigned long idy,
		unsigned long idx,
		unsigned long arg_idx,
		const std::vector<double>& pwl_vector)
{
	pwl_stamps.resize(pwl_stamps.size()+1);

	sca_pwl_stamp& stamp(pwl_stamps[pwl_stamps.size()-1]);

	stamp.nl_eq_idx=idy;
	stamp.var_idx=idx;
	stamp.arg_idx=arg_idx;
	stamp.pwl_vector=pwl_vector;
	stamp.b_stamps=true;
}

bool sca_util::sca_implementation::sca_pwl_stamps::get_pwl_stamp(
		unsigned long  & stamp_idx,
		unsigned long  & idy,
		unsigned long  & idx,
		unsigned long  & arg_idx,
		std::vector<double> & pwl_vector,
		bool&            b_stamps)
{
	if(stamp_idx>=pwl_stamps.size()) return false;

	sca_pwl_stamp& stamp(pwl_stamps[stamp_idx]);

	stamp_idx++;

	idy=stamp.nl_eq_idx;
	idx=stamp.var_idx;
	arg_idx=stamp.arg_idx;
	pwl_vector=stamp.pwl_vector;
	b_stamps=stamp.b_stamps;

	return true;
}


} //namespace sca_implementation
} //namespace sca_implementation
