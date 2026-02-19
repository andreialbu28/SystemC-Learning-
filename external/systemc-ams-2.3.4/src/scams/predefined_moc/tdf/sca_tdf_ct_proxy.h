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

 sca_tdf_ct_proxy.h - helper class for ltf value conversion/sampling

 Original Author: Karsten Einwich Fraunhofer IIS/EAS Dresden

 Created on: 04.03.2009

 SVN Version       :  $Revision: 1908 $
 SVN last checkin  :  $Date: 2016-02-15 17:59:13 +0000 (Mon, 15 Feb 2016) $ (UTC)
 SVN checkin by    :  $Author: karsten $
 SVN Id            :  $Id: sca_tdf_ct_proxy.h 1908 2016-02-15 17:59:13Z karsten $

 *****************************************************************************/
/*
 LRM clause 4.1.4.1.
 The class sca_tdf::sca_ltf_proxy shall be a helper class, which shall map the
 computed continuous-time solution to sampled output values. An instance of
 this class shall exist only as reference returned by the member functions
 calculate or operator() of class sca_tdf::sca_ltf_nd and sca_tdf::sca_ltf_zp.
 */

/*****************************************************************************/

#ifndef SCA_TDF_CT_PROXY_H_
#define SCA_TDF_CT_PROXY_H_

namespace sca_tdf
{

//begin implementation specific

namespace sca_implementation
{
class sca_ct_ltf_nd_proxy;
}

//end implementation specific

class sca_ct_proxy:
	public sca_core::sca_assign_from_proxy<sca_util::sca_vector<double> >,
	public sca_core::sca_assign_from_proxy<sca_tdf::sca_out_base<double> >,
	public sca_core::sca_assign_from_proxy<sca_tdf::sca_de::sca_out<double> >
{
public:
	double to_double() const;
	void   to_vector(sca_util::sca_vector<double>&, unsigned long nsamples = 0) const;
	const sca_util::sca_vector<double>& to_vector(unsigned long nsamples = 0) const;
	void to_port(sca_tdf::sca_out_base<double>&) const;
	void to_port(sca_tdf::sca_de::sca_out<double>&) const;

	operator double() const;

private:

	void assign_to(sca_util::sca_vector<double>&);
	void assign_to(sca_tdf::sca_out_base<double>&);
	void assign_to(sca_tdf::sca_de::sca_out<double>&);

	//begin implementation specific

	friend class sca_tdf::sca_implementation::sca_ct_ltf_nd_proxy;

	virtual double convert_to_double() = 0;
	virtual void convert_to_sca_port(sca_tdf::sca_out_base<double>&)=0;
	virtual void convert_to_vector(sca_util::sca_vector<double>&,
			unsigned long nsamples)=0;
	virtual void convert_to_sc_port(sca_tdf::sca_de::sca_out<double>& port)=0;

	sca_util::sca_vector<double> internal_vector;

	//end implementation specific


	// Disabled
	sca_ct_proxy();
};

//begin implementation specific


inline sca_ct_proxy::sca_ct_proxy()
{
}

inline double sca_ct_proxy::to_double() const
{
	return const_cast<sca_ct_proxy*> (this)->convert_to_double();
}

inline sca_ct_proxy::operator double() const
{
	return to_double();
}

inline void sca_ct_proxy::assign_to(sca_util::sca_vector<double>& vec)
{
	convert_to_vector(vec, 0);
}

inline void sca_ct_proxy::assign_to(sca_tdf::sca_out_base<double>& port)
{
	convert_to_sca_port(port);
}

inline void sca_ct_proxy::assign_to(sca_tdf::sca_de::sca_out<double>& port)
{
	convert_to_sc_port(port);
}

inline void sca_ct_proxy::to_vector(sca_util::sca_vector<double>& vec,
		unsigned long nsamples) const
{
	const_cast<sca_ct_proxy*>(this)->convert_to_vector(vec,nsamples);
}

inline const sca_util::sca_vector<double>& sca_ct_proxy::to_vector(
		unsigned long nsamples) const
{
	const_cast<sca_ct_proxy*>
		(this)->convert_to_vector(
				*(const_cast<sca_util::sca_vector<double>*>(&internal_vector)),
				nsamples);
	return internal_vector;
}

inline void sca_ct_proxy::to_port(sca_tdf::sca_out_base<double>& port) const
{
	const_cast<sca_ct_proxy*> (this)->convert_to_sca_port(port);
}

inline void sca_ct_proxy::to_port(sca_tdf::sca_de::sca_out<double>& port) const
{
	const_cast<sca_ct_proxy*>(this)->convert_to_sc_port(port);
}

} // namespace sca_tdf


#endif /* SCA_TDF_CT_PROXY_H_ */
