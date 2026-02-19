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

 sca_tdf_ss_proxy.h - helper class for state space value conversion/sampling

 Original Author: Karsten Einwich Fraunhofer IIS/EAS Dresden

 Created on: 04.03.2009

 SVN Version       :  $Revision: 1908 $
 SVN last checkin  :  $Date: 2016-02-15 17:59:13 +0000 (Mon, 15 Feb 2016) $ (UTC)
 SVN checkin by    :  $Author: karsten $
 SVN Id            :  $Id: sca_tdf_ct_vector_proxy.h 1908 2016-02-15 17:59:13Z karsten $

 *****************************************************************************/
/*
 LRM clause 4.1.4.4.
 The class sca_tdf::sca_ss_proxy shall be a helper class, which shall map the
 computed continuous-time solution to sampled output values. An instance of
 this class shall exist only as reference returned by the member functions
 calculate or operator() of class sca_tdf::sca_ss.
 */

/*****************************************************************************/

#ifndef SCA_TDF_CT_VECTOR_PROXY_H_
#define SCA_TDF_CT_VECTOR_PROXY_H_

namespace sca_tdf
{
namespace sca_implementation
{
class sca_ct_vector_ss_proxy;
}


class sca_ct_vector_proxy : public sca_core::sca_assign_from_proxy<sca_util::sca_matrix<double> >,
                     public sca_core::sca_assign_from_proxy<sca_tdf::sca_out_base<sca_util::sca_vector<double> > >,
                     public sca_core::sca_assign_from_proxy<sca_tdf::sca_de::sca_out<sca_util::sca_vector<double> > >
{
public:
	const sca_util::sca_vector<double>& to_vector() const;
	void  to_matrix(sca_util::sca_matrix<double>&, unsigned long nsamples = 0) const;
	const sca_util::sca_matrix<double>& to_matrix(unsigned long nsamples = 0) const;
	void to_port(sca_tdf::sca_out_base<sca_util::sca_vector<double> >&) const;
	void to_port(sca_tdf::sca_de::sca_out<sca_util::sca_vector<double> >&) const;

    operator const sca_util::sca_vector<double>&() const;


private:

	// Disabled
	sca_ct_vector_proxy();

	void assign_to(sca_util::sca_matrix<double>&);
	void assign_to(sca_tdf::sca_out_base<sca_util::sca_vector<double> >&);
	void assign_to(sca_tdf::sca_de::sca_out<sca_util::sca_vector<double> >&);

	//begin implementation specific

	friend class sca_tdf::sca_implementation::sca_ct_vector_ss_proxy;

	virtual const sca_util::sca_vector<double>& convert_to_double_vector()=0;
	virtual void  convert_to_sca_port(sca_tdf::sca_out_base<sca_util::sca_vector<double> >&)=0;
	virtual void  convert_to_matrix(sca_util::sca_matrix<double>&, unsigned long nsamples)=0;
	virtual void  convert_to_sc_port(sca_tdf::sca_de::sca_out<sca_util::sca_vector<double> >& port)=0;

	sca_util::sca_matrix<double> internal_matrix;

	//end implementation specific



};


//begin implementation specific


inline sca_ct_vector_proxy::sca_ct_vector_proxy()
{
}

inline const sca_util::sca_vector<double>& sca_ct_vector_proxy::to_vector() const
{
	return const_cast<sca_ct_vector_proxy*> (this)->convert_to_double_vector();
}

inline sca_ct_vector_proxy::operator const sca_util::sca_vector<double>&() const
{
	return to_vector();
}

inline void sca_ct_vector_proxy::assign_to(sca_util::sca_matrix<double>& matrix)
{
	convert_to_matrix(matrix, 0);
}

inline void sca_ct_vector_proxy::assign_to(sca_tdf::sca_out_base<sca_util::sca_vector<double> >& port)
{
	convert_to_sca_port(port);
}

inline void sca_ct_vector_proxy::assign_to(sca_tdf::sca_de::sca_out<sca_util::sca_vector<double> >& port)
{
	convert_to_sc_port(port);
}

inline void sca_ct_vector_proxy::to_matrix(sca_util::sca_matrix<double>& matrix, unsigned long nsamples) const
{
	const_cast<sca_ct_vector_proxy*>(this)->convert_to_matrix(matrix,nsamples);
}

inline const sca_util::sca_matrix<double>& sca_ct_vector_proxy::to_matrix(unsigned long nsamples) const
{
	const_cast<sca_ct_vector_proxy*>
		(this)->convert_to_matrix(
				*(const_cast<sca_util::sca_matrix<double>*>(&internal_matrix)),nsamples);
	return internal_matrix;
}

inline void sca_ct_vector_proxy::to_port(sca_tdf::sca_out_base<sca_util::sca_vector<double> >& port) const
{
	const_cast<sca_ct_vector_proxy*> (this)->convert_to_sca_port(port);
}

inline void sca_ct_vector_proxy::to_port(sca_tdf::sca_de::sca_out<sca_util::sca_vector<double> >& port) const
{
	const_cast<sca_ct_vector_proxy*>(this)->convert_to_sc_port(port);
}

// end implementation specific

} // namespace sca_tdf


#endif /* SCA_TDF_CT_VECTOR_PROXY_H_ */
