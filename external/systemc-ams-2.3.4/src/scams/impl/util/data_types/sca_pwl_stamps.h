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

  sca_pwl_stamps.h - description

  Original Author: Karsten Einwich Fraunhofer IIS/EAS Dresden

  Created on: Mar 20, 2014

   SVN Version       :  $Revision$
   SVN last checkin  :  $Date$
   SVN checkin by    :  $Author$
   SVN Id            :  $Id$

 *****************************************************************************/


#ifndef SCA_PWL_STAMPS_H_
#define SCA_PWL_STAMPS_H_

#include <vector>

namespace sca_util
{
namespace sca_implementation
{

/**
 * pwl matrix stamp for matrix B
 *    nl_eq_idx : index of additional equation with pwl stamp
 *    var_idx   : idx of variable for pwl nonlinearity
 *
 *    pwl_vector: pwl nonlinearity length 2*number of points
 *       2*i   : value of variable
 *       2*i+1 : contribution to equation with index nl_eq_idx
 */
class sca_pwl_stamp
{
public:
	unsigned long nl_eq_idx;  //idy
	unsigned long var_idx;    //idx
	unsigned long arg_idx;   //control variable ( f(x[ctrl_idx]) )

	std::vector<double> pwl_vector;

	bool b_stamps;  //if true vector contains B matrix stamps directly

};

/**
 * This class stores the piece wise linear stamps of an equation system
 * this are always stamps of the B matrix
 */
class sca_pwl_stamps
{
public:

	void add_pwl_stamp(unsigned long idy, unsigned long idx,
			const std::vector<double>& pwl_vector);

	void add_pwl_b_stamp(unsigned long idy, unsigned long idx,
			unsigned long arg_idx,
			const std::vector<double>& pwl_vector);

	unsigned long get_number_of_stamps() {return static_cast<unsigned long>(pwl_stamps.size());}

	bool get_pwl_stamp( //returns true if number of available stamps is
			            // greater than stamp_idx
			unsigned long& stamp_idx, //index of current stamp (is incremeted)
			unsigned long& idy,       //equation index of current stamp
			unsigned long& idx,       //variable index of stamp
			unsigned long& arg_idx,  //index of argument of pwl function
			std::vector<double>& pwl_vector, //pwl vector of stamp
			bool&                b_stamps    //vector of b stamps
	);

	void clear();

private:

	std::vector<sca_pwl_stamp> pwl_stamps;
};

} //namespace sca_implementation
} //namespace sca_implementation





#endif /* SCA_PWL_STAMPS_H_ */

