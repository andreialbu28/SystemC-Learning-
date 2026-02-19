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

  sca_change_matrix.cpp - description

  Original Author: Karsten Einwich Fraunhofer IIS/EAS Dresden

  Created on: Mar 20, 2014

   SVN Version       :  $Revision$
   SVN last checkin  :  $Date$
   SVN checkin by    :  $Author$
   SVN Id            :  $Id$

 *****************************************************************************/


#include "sca_change_matrix.h"
#include  <cstdio>

namespace sca_util
{
namespace sca_implementation
{


sca_change_matrix::sca_change_matrix()
{
	n_stamps=0;
}

///////////////////////////////////////////////////////////////////////////////

void sca_change_matrix::add_change(
		long  idy,
		long  idx,
		double*        value_ref,
		bool continous)
{
	if((idx<0) || (idy<0)) return;

	if(changed_stamps.size()<=n_stamps)
	{
		changed_stamps.resize(n_stamps+10);
	}

	change_stamp& stamp(changed_stamps[n_stamps]);

	stamp.idx=idx;
	stamp.idy=idy;
	stamp.value_ref=value_ref;

	stamp.continous_change=continous;
	stamp.cur_val_not_updated=true;

	if(value_ref!=NULL)
	{
		stamp.old_value=*value_ref;
	}
	else  stamp.cur_val_not_updated=false;

	n_stamps++;
}




//////////////////////////////////////////////////////////////////////////////

bool sca_change_matrix::get_change(
		unsigned long  chidx,
		unsigned long  & idy,
		unsigned long  & idx,
		double&  cur_value,
		double & old_value,
		bool & continous_change)
{
	if(chidx>=n_stamps) return false;

	change_stamp& stamp(changed_stamps[chidx]);

	if(stamp.cur_val_not_updated)
	{
		for(unsigned long i=chidx+1;i<n_stamps;++i) //solves problem wit doubled stamps
		{
			if(stamp.value_ref==changed_stamps[i].value_ref) //remove doubled entries (use first)
			{
				for(unsigned long j=i+1;j<n_stamps;++j)
				{
					changed_stamps[j-1]=changed_stamps[j];
				}
				n_stamps--;
				i--;
			}
		}

		stamp.cur_value=*(stamp.value_ref);
		stamp.cur_val_not_updated=false;
	}

	idx=stamp.idx;
	idy=stamp.idy;
	cur_value=stamp.cur_value;
	old_value=stamp.old_value;
	continous_change=stamp.continous_change;

	return true;
}


//////////////////////////////////////////////////////////////////////////////


void sca_change_matrix::reset()
{
	n_stamps=0;
}

//////////////////////////////////////////////////////////////////////////////

} //namespace sca_implementation
} //namespace sca_util


