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

  sca_change_matrix.h - class for storing xhanges of a matrix

  Original Author: Karsten Einwich Fraunhofer IIS/EAS Dresden

  Created on: Mar 20, 2014

   SVN Version       :  $Revision: 1523 $
   SVN last checkin  :  $Date: 2013-02-17 21:36:57 +0100 (Sun, 17 Feb 2013) $
   SVN checkin by    :  $Author: karsten $
   SVN Id            :  $Id: sca_eln_tdf_l.cpp 1523 2013-02-17 20:36:57Z karsten $

 *****************************************************************************/

/*****************************************************************************/

#ifndef SCA_CHANGE_MATRIX_H_
#define SCA_CHANGE_MATRIX_H_

#include <vector>

namespace sca_util
{
namespace sca_implementation
{

struct change_stamp
{
	unsigned long idx;
	unsigned long idy;

	bool          continous_change;

	double* value_ref;
	double cur_value;
	double old_value;

	bool   cur_val_not_updated;
};

class sca_change_matrix
{
public:

	//add change to buffer
	void add_change(long idy, long idx,double* value_ref, bool continous);

	//reset change buffer (set number of changes to zero
	void reset();



	bool get_change(                        //returns true if chidx is smaller
			                                //than the number of available changes
			unsigned long  chidx,           //index of change
			unsigned long& idy,             //returns x-index of change
			unsigned long& idx,             //returns y-index of change
			double&        cur_value,       //returns value after change finished
			double&        old_value,       //returns value before change
			bool&          continous_change //returns whether change is continous
			);

	unsigned long get_number_of_changes() {return n_stamps;}

	sca_change_matrix();

private:

	std::vector<change_stamp> changed_stamps;
	unsigned long n_stamps;

};

} //namespace sca_implementation
} //namespace sca_util

#endif /* SCA_CHANGE_MATRIX_H_ */
