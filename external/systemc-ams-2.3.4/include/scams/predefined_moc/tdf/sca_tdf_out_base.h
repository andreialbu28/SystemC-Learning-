/*****************************************************************************

    Copyright 2010-2012
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

 sca_tdf_out_base.h - base class for tdf outports

 Original Author: Karsten Einwich Fraunhofer IIS/EAS Dresden

 Created on: 10.04.2012

 SVN Version       :  $Revision: 1822 $
 SVN last checkin  :  $Date: 2015-05-01 20:26:44 +0000 (Fri, 01 May 2015) $ (UTC)
 SVN checkin by    :  $Author: karsten $
 SVN Id            :  $Id: sca_tdf_out_base.h 1822 2015-05-01 20:26:44Z karsten $

 *****************************************************************************/
/*
 LRM clause 4.1.1.7.

 */

/*****************************************************************************/

#ifndef SCA_TDF_OUT_BASE_H_
#define SCA_TDF_OUT_BASE_H_

namespace sca_tdf
{

template<class T>
class sca_out_base : public sca_tdf::sca_implementation::sca_tdf_port_impl<
sca_tdf::sca_signal_if<T>, T>
{
public:

	explicit sca_out_base(const char* name_);

	//implementation specific declaration of virtual methods
	virtual sca_core::sca_time get_timestep(unsigned long sample_id=0) const = 0;
	virtual sca_core::sca_time get_time(unsigned long sample_id = 0)  const = 0;
	virtual unsigned long get_rate() const = 0;
	virtual void write(const T& value, unsigned long sample_id = 0) = 0;

	virtual const char* kind() const;

	virtual ~sca_out_base();

protected:

	typedef sca_tdf::sca_implementation::sca_tdf_port_impl<
			sca_tdf::sca_signal_if<T>, T> base_type;
};


//////

template<class T>
inline sca_out_base<T>::sca_out_base(const char* name_) :
	sca_tdf::sca_implementation::sca_tdf_port_impl<sca_tdf::sca_signal_if<T>, T>(
			name_)
{
	base_type::sca_port_type = base_type::SCA_OUT_PORT;
}


template<class T>
inline sca_out_base<T>::~sca_out_base()
{

}

////////

template<class T>
inline const char* sca_out_base<T>::kind() const
{
	return "sca_tdf::sca_out_base";
}


} // namespace sca_tdf


#endif /* SCA_TDF_OUT_BASE_H_ */
