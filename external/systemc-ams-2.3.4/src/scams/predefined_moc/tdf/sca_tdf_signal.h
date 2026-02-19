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

 sca_tdf_signal.h - tdf signal

 Original Author: Karsten Einwich Fraunhofer IIS/EAS Dresden

 Created on: 04.03.2009

 SVN Version       :  $Revision: 2132 $
 SVN last checkin  :  $Date: 2020-03-27 13:40:11 +0000 (Fri, 27 Mar 2020) $ (UTC)
 SVN checkin by    :  $Author: karsten $
 SVN Id            :  $Id: sca_tdf_signal.h 2132 2020-03-27 13:40:11Z karsten $

 *****************************************************************************/
/*
 LRM clause 4.1.1.4.
 The class sca_tdf::sca_signal shall define a primitive channel for the
 TDF MoC. It shall be used for connecting modules of class sca_tdf::sca_module
 using port classes sca_tdf::sca_in and sca_tdf::sca_out. There shall not be
 any application-specific access to the associated interface.

 */

/*****************************************************************************/

#ifndef SCA_TDF_SIGNAL_H_
#define SCA_TDF_SIGNAL_H_

namespace sca_tdf
{

template<class T>
//class sca_signal : protected implementation-derived-from sca_tdf::sca_signal_in_if,
//                                                         sca_tdf::sca_signal_out_if,
//                                                         sca_core::sca_prim_channel
class sca_signal: public sca_tdf::sca_implementation::sca_tdf_signal_impl<T>
{
public:
	sca_signal();
	explicit sca_signal(const char* name_);

	virtual const char* kind() const;

	virtual ~sca_signal(){}

	virtual void print( std::ostream& = std::cout ) const;
	virtual void dump( std::ostream& = std::cout ) const;

private:
	// Disabled
	sca_signal(const sca_tdf::sca_signal<T>&);
};

// begin implementation specific

template<class T>
inline void sca_signal<T>::print( std::ostream& str) const
{
	str << this->get_typed_trace_value();
}

template<class T>
inline void sca_signal<T>::dump( std::ostream&  str) const
{
	str << this->kind() << " : " << this->name() << " value: "<< this->get_typed_trace_value();
}


template<class T>
inline sca_signal<T>::sca_signal() : sca_tdf::sca_implementation::sca_tdf_signal_impl<T>(sc_core::sc_gen_unique_name("sca_tdf_signal"))
{

}

///////

template<class T>
inline sca_signal<T>::sca_signal(const char* name_) : sca_tdf::sca_implementation::sca_tdf_signal_impl<T>(name_)
{

}


//////

template<class T>
inline const char* sca_signal<T>::kind() const
{
	return "sca_tdf::sca_signal";
}

////////


template<class T>
inline sca_signal<T>::sca_signal(const sca_tdf::sca_signal<T>&)
{

}

// end implementation specific



} // namespace sca_tdf

#endif /* SCA_TDF_SIGNAL_H_ */
