/*****************************************************************************

    Copyright 2010
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

  sca_tdf_signal_if.h - description

  Original Author: Karsten Einwich Fraunhofer IIS/EAS Dresden

  Created on: Aug 26, 2009

   SVN Version       :  $Revision: 1265 $
   SVN last checkin  :  $Date: 2011-11-19 20:43:31 +0000 (Sat, 19 Nov 2011) $
   SVN checkin by    :  $Author: karsten $
   SVN Id            :  $Id: sca_tdf_signal_if.h 1265 2011-11-19 20:43:31Z karsten $

 *****************************************************************************/
/*
 *
 LRM clause 4.1.1.3.
 The class sca_tdf::sca_signal_out_if shall define an interface proper
 for a primitive channel of class sca_tdf::sca_signal.
 The interface gives write access to the values of the signal. The interface
 class member functions are implementation-defined.

 */

/*
 LRM clause 4.1.1.2.
 The class sca_tdf::sca_signal_in_if shall define an interface proper for a
 primitive channel of class sca_tdf::sca_signal. The interface gives read
 access to the values of the signal. The interface class member functions are
 implementation-defined.
 */

/*****************************************************************************/


#ifndef SCA_TDF_SIGNAL_IF_H_
#define SCA_TDF_SIGNAL_IF_H_

namespace sca_tdf
{

template<class T>
class sca_signal_if : virtual public sca_core::sca_interface
{
protected:
	sca_signal_if();

private:
	// Other members
	//implementation-defined

	// Disabled
	sca_signal_if(const sca_tdf::sca_signal_if<T>&);
	sca_tdf::sca_signal_if<T>& operator=(const sca_tdf::sca_signal_if<T>&);

};


//begin implementation specific

template<class T>
inline sca_signal_if<T>::sca_signal_if()
{
}

template<class T>
inline sca_signal_if<T>::sca_signal_if(const sca_tdf::sca_signal_if<T>&)
{

}


template<class T>
inline sca_signal_if<T>& sca_signal_if<T>::operator=(const sca_tdf::sca_signal_if<T>&)
{
   sca_signal_if<T>* inf=new sca_signal_if<T>;
   return *inf;
}


//end implementation specific

} // namespace sca_tdf



#endif /* SCA_TDF_SIGNAL_IF_H_ */
