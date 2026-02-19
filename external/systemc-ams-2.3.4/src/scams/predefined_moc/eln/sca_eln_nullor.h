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

 sca_eln_nullor.h - electrical linear net nullor (ideal operational amplifier)

 Original Author: Karsten Einwich Fraunhofer IIS/EAS Dresden

 Created on: 07.03.2009

 SVN Version       :  $Revision: 2106 $
 SVN last checkin  :  $Date: 2020-02-26 15:58:39 +0000 (Wed, 26 Feb 2020) $ (UTC)
 SVN checkin by    :  $Author: karsten $
 SVN Id            :  $Id: sca_eln_nullor.h 2106 2020-02-26 15:58:39Z karsten $

 *****************************************************************************/
/*
 * LRM clause 4.3.1.12.
 * The class sca_eln::sca_nullor</classname>shall implement a primitive module
 * for the ELN MoC that represents a nullor. The primitive shall contribute the
 * following equations to the equation system:
 *
 *        v(nip,nin)(t) = 0
 *        i(nip,nin)(t) = 0
 *
 * where v(nip,nin)(t) is the voltage across terminals nip and nin, and
 * i(nip,nin)(t) is the current flowing through the primitive from terminal nip
 * to terminal nin.
 * NOTE: A nullor (a nullator - norator pair) corresponds to an ideal
 *       operational amplifier (an amplifier with an infinite gain).
 */

/*****************************************************************************/

#ifndef SCA_ELN_NULLOR_H_
#define SCA_ELN_NULLOR_H_

namespace sca_eln
{

//  class sca_nullor : public implementation-derived-from sca_core::sca_module
class sca_nullor: public sca_eln::sca_module
{
public:
	sca_eln::sca_terminal nip;
	sca_eln::sca_terminal nin;

	sca_eln::sca_terminal nop;
	sca_eln::sca_terminal non;

	virtual const char* kind() const;

	explicit sca_nullor(sc_core::sc_module_name);

	//begin implementation specific

private:
		 virtual void matrix_stamps();

		 long nadd;

	//end implementation specific

};

} // namespace sca_eln

#endif /* SCA_ELN_NULLOR_H_ */
