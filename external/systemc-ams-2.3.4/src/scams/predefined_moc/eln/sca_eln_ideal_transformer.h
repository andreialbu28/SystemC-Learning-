/*****************************************************************************

    Copyright 2010
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

 sca_eln_ideal_transformer.h - electrical linear

 Original Author: Karsten Einwich Fraunhofer IIS/EAS Dresden

 Created on: 07.03.2009

 SVN Version       :  $Revision: 2106 $
 SVN last checkin  :  $Date: 2020-02-26 15:58:39 +0000 (Wed, 26 Feb 2020) $ (UTC)
 SVN checkin by    :  $Author: karsten $
 SVN Id            :  $Id: sca_eln_ideal_transformer.h 2106 2020-02-26 15:58:39Z karsten $

 *****************************************************************************/
/*
 * LRM clause 4.3.1.14.
 * The class sca_eln::sca_ideal_transformer shall implement a primitive module
 * for the ELN MoC that represents an ideal transformer. The primitive shall
 * contribute the following equations to the equation system:
 *
 *        v(p1,n1)(t) = ratio * v(p2,n2)(t)
 *        i(p2,n2)(t) = ratio * i(p1,n1)(t)
 *
 * where ratio is the transformation ratio, v(p2,n2)(t) is the voltage across
 * terminals p2 and n2,v(p1,n1)(t) is the voltage across terminals p1 and n1,
 * i(p1,n1)(t) is the current flowing through the primitive from terminal p1
 * to terminal n1, and i(p2,n2)(t) is the current flowing through the primitive
 * from terminal p2 to terminal n2.
 */

/*****************************************************************************/

#ifndef SCA_ELN_IDEAL_TRANSFORMER_H_
#define SCA_ELN_IDEAL_TRANSFORMER_H_

namespace sca_eln
{

//  class sca_ideal_transformer : public implementation-derived-from sca_core::sca_module
class sca_ideal_transformer: public sca_eln::sca_module
{
public:
	sca_eln::sca_terminal p1;
	sca_eln::sca_terminal n1;

	sca_eln::sca_terminal p2;
	sca_eln::sca_terminal n2;

	sca_core::sca_parameter<double> ratio;

	virtual const char* kind() const;

	explicit sca_ideal_transformer(sc_core::sc_module_name, double ratio_ = 1.0);

	//begin implementation specific

private:
		 virtual void matrix_stamps();

		 long nadd;

	//end implementation specific

};

} // namespace sca_eln


#endif /* SCA_ELN_IDEAL_TRANSFORMER_H_ */
