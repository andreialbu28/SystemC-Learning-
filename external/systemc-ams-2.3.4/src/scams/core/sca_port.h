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

 sca_port.h - base class for all SystemC AMS ports

 Original Author: Karsten Einwich Fraunhofer IIS/EAS Dresden

 Created on: 03.03.2009

 SVN Version       :  $Revision: 1265 $
 SVN last checkin  :  $Date: 2011-11-19 20:43:31 +0000 (Sat, 19 Nov 2011) $ (UTC)
 SVN checkin by    :  $Author: karsten $
 SVN Id            :  $Id: sca_port.h 1265 2011-11-19 20:43:31Z karsten $

 *****************************************************************************/
/*
 LRM clause 3.2.4.
 The class sca_core::sca_port shall define the base class to derive ports
 for the predefined models of computation. The class sca_core::sca_port shall
 implement the interface of class sca_util::sca_traceable_object, in such a
 way that the bound channel can be traced.
 */

/*****************************************************************************/

#ifndef SCA_PORT_H_
#define SCA_PORT_H_


namespace sca_core
{



template<class IF>
//  class sca_port : public implementation-derived-from sc_core::sc_port<IF,1>,
//                   protected sca_util::sca_traceable_object
class sca_port: public sca_core::sca_implementation::sca_port_impl<IF>
{
public:
	virtual const char* kind() const;

protected:

	sca_port();
	explicit sca_port(const char*);
	virtual ~sca_port();

//begin implementation defined section
	typedef sca_core::sca_implementation::sca_port_impl<IF> base_type;

private:

	static const char* const kind_string;

//end implementation defined section
};


//////////////////////

template<class IF>
inline sca_port<IF>::sca_port() : sca_core::sca_implementation::sca_port_impl<IF>()
{
}

template<class IF>
inline sca_port<IF>::sca_port(const char* nm): sca_core::sca_implementation::sca_port_impl<IF>(nm)
{
}

template<class IF>
inline const char* sca_port<IF>::kind() const
{
	return kind_string;
}

template<class IF>
inline sca_port<IF>::~sca_port()
{
}

template<class IF>
const char* const sca_port<IF>::kind_string = "sca_core::sca_port";

} // namespace sca_core







#endif /* SCA_PORT_H_ */
