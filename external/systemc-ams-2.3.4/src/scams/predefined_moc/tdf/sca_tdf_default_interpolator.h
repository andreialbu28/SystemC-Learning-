/*****************************************************************************

    Copyright 2010-2011
    Fraunhofer-Gesellschaft zur Foerderung der angewandten Forschung e.V.

    Copyright 2015-2016
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

 sca_tdf_default_interpolator.h - tdf declustering outport interpolator

 Original Author: Karsten Einwich Fraunhofer / COSEDA Technologies

 Created on: 13.11.2011

 SVN Version       :  $Revision: 2320 $
 SVN last checkin  :  $Date: 2023-06-08 12:47:38 +0000 (Thu, 08 Jun 2023) $ (UTC)
 SVN checkin by    :  $Author: pehrlich $
 SVN Id            :  $Id: sca_tdf_default_interpolator.h 2320 2023-06-08 12:47:38Z pehrlich $

 *****************************************************************************/
/*
 * New SystemC-AMS 2.0 feature
 */
/*****************************************************************************/

#ifndef SCA_TDF_DEFAULT_INTERPOLATOR_H_
#define SCA_TDF_DEFAULT_INTERPOLATOR_H_

namespace sca_tdf
{

template<class T>
class sca_default_interpolator
{
public:
	void store_value(const sca_core::sca_time& time,const T&);
	T get_value(const sca_core::sca_time&) const;

private:

	T previous_value;
	T next_value;

	sca_core::sca_time previous_time;
	sca_core::sca_time next_time;

};



//specialization for double
template<>
class sca_default_interpolator<double>
{
public:

	void store_value(const sca_core::sca_time&,const double&);
	double get_value(const sca_core::sca_time&) const;


	sca_default_interpolator();

private:

	double previous_value;
	double next_value;

	sca_core::sca_time previous_time;
	sca_core::sca_time next_time;

};



//specialization for sca_complex
template<>
class sca_default_interpolator<sca_util::sca_complex>
{
public:

	void store_value(const sca_core::sca_time&,const sca_util::sca_complex&);
	sca_util::sca_complex get_value(const sca_core::sca_time&) const;


	sca_default_interpolator();

private:

	sca_util::sca_complex previous_value;
	sca_util::sca_complex next_value;

	sca_core::sca_time previous_time;
	sca_core::sca_time next_time;

};




//////////////////// Implementation /////////

template<class T>
inline void sca_default_interpolator<T>::store_value(const sca_core::sca_time& time,const T& value)
{
	if(time>next_time)
	{
		previous_value=next_value;
		previous_time=next_time;

	}
	else if(time<=previous_time)
	{
		previous_time=time;
		previous_value=value;
	}

	next_value=value;
	next_time=time;
}

template<class T>
inline T sca_default_interpolator<T>::get_value(const sca_core::sca_time& time) const
{
	//default sample&hold semantic
	if(time>=next_time) return next_value;
	return previous_value;
}

}

#endif /* SCA_TDF_DEFAULT_INTERPOLATOR_H_ */
