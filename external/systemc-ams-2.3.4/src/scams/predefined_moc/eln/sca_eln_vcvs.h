/*****************************************************************************

    Copyright 2010-2014
    Fraunhofer-Gesellschaft zur Foerderung der angewandten Forschung e.V.

    Copyright 2015-2020
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

 sca_eln_vcvs.h - electrical linear net voltage controlled voltage source

 Original Author: Karsten Einwich Fraunhofer IIS Dresden /COSEDA Technologies

 Created on: 07.03.2009

 SVN Version       :  $Revision: 2106 $
 SVN last checkin  :  $Date: 2020-02-26 15:58:39 +0000 (Wed, 26 Feb 2020) $
 SVN checkin by    :  $Author: karsten $
 SVN Id            :  $Id: sca_eln_vcvs.h 2106 2020-02-26 15:58:39Z karsten $

 *****************************************************************************/
/*
 * LRM clause 4.3.1.8.
 * The class sca_eln::sca_vcvs shall implement a primitive module for the
 * ELN MoC that represents a voltage controlled voltage source. The primitive
 * shall contribute the following equation to the equation system:
 *
 *       v(np,nn)(t) = value * v(ncp,ncn)(t)
 *
 * where value is the scale coefficient of the control voltage v(ncp,ncn)(t)
 * across terminals ncp and ncn, and v(np,nn)(t) is the voltage across terminals
 * np and nn.
 */

/*****************************************************************************/

#ifndef SCA_ELN_VCVS_H_
#define SCA_ELN_VCVS_H_

namespace sca_eln
{

//  class sca_vcvs : public implementation-derived-from sca_core::sca_module
class sca_vcvs: public sca_eln::sca_module
{
	typedef sca_util::sca_vector<std::pair<double,double> > sca_pwl_pair_vector;

public:
	sca_eln::sca_terminal ncp;
	sca_eln::sca_terminal ncn;

	sca_eln::sca_terminal np;
	sca_eln::sca_terminal nn;

	sca_core::sca_parameter<double> value;
	sca_core::sca_parameter<sca_pwl_pair_vector> pwl_value;

	virtual const char* kind() const;

	explicit sca_vcvs(sc_core::sc_module_name,
			double value_ = 1.0,
			const sca_pwl_pair_vector& pwl_value_ = sca_pwl_pair_vector());

	//begin implementation specific

private:
		 virtual void matrix_stamps();

		 long nadd1, nadd2;

		 //pwl controlled sources
		 unsigned int pwl_vector_size; //number of values in pwl_value

	//end implementation specific

};

} // namespace sca_eln

#endif /* SCA_ELN_VCVS_H_ */
