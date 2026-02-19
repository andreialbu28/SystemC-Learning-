/*****************************************************************************

    Copyright 2010-2012
    Fraunhofer-Gesellschaft zur Foerderung der angewandten Forschung e.V.

    Copyright 2015-2022
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

  sca_matrix_base.h - description

  Original Author: Karsten Einwich Fraunhofer IIS/EAS Dresden

  Created on: 09.10.2009

   SVN Version       :  $Revision: 2361 $
   SVN last checkin  :  $Date: 2023-07-06 15:00:20 +0000 (Thu, 06 Jul 2023) $
   SVN checkin by    :  $Author: pehrlich $
   SVN Id            :  $Id: sca_matrix_base_double.h 2361 2023-07-06 15:00:20Z pehrlich $

 *****************************************************************************/

/*****************************************************************************/

#ifndef SCA_MATRIX_BASE_DOUBLE_H_
#define SCA_MATRIX_BASE_DOUBLE_H_


struct sparse;
typedef sparse sparse_matrix;

namespace sca_util
{
namespace sca_implementation
{

template<>
class sca_matrix_base<double> : public sca_matrix_base_typeless
{

	friend class sca_tdf::sca_implementation::sca_ct_ltf_nd_proxy;
	friend class sca_tdf::sca_implementation::sca_ct_ltf_zp_proxy;
	friend class sca_tdf::sca_implementation::sca_ct_vector_ss_proxy;

    mutable double              dummy=0.0;


    std::valarray<double>    matrix;
    sparse_matrix*           smatrix=NULL;
    bool sparse_mode=false;

    void write_pending() const;
    double* get_ref_for_write(sparse_matrix* smatrix,long x, long y);
    long x_pending=-1;
    long y_pending=-1;
    double value_pending=0.0;
    mutable bool write_is_pending=false;

public:

    bool is_sparse_mode() const;
    void set_sparse_mode();
    void unset_sparse_mode();

protected:

    void reset();
    void remove();
    
    bool is_equal(const sca_matrix_base<double>& b) const;

    sca_matrix_base(bool sparse_mode=false);                                  //default matrix
    sca_matrix_base(unsigned long x, bool sparse_mode=false);                   //vector not resizable
    sca_matrix_base(unsigned long y, unsigned long x, bool sparse_mode=false);  //matrix not resizable
    sca_matrix_base(const sca_matrix_base<double>& m);       //copied matrix

    sca_matrix_base(const std::vector<double>&);     //constructor from std::vector for convenience


    sca_matrix_base(const std::initializer_list<double>&);
    sca_matrix_base(const std::initializer_list<std::initializer_list<double> >&);


    virtual ~sca_matrix_base();

    sca_matrix_base<double>& operator= (const sca_matrix_base<double>& m);


    double* get_flat();
    sparse_matrix* get_sparse_matrix() const;


    double& operator[] (unsigned long x);
    const double& operator[] (unsigned long x) const;


    double& operator() (long y, long x);
    const double& operator() (long y, long x) const;
    double& operator() (long xi);  //vector
    const double& operator() (long xi) const;  //vector

    void resize(unsigned long xy);

    void resize(unsigned long ys, unsigned long xs);


    friend class sca_core::sca_implementation::sca_linear_solver;
    friend class sca_core::sca_implementation::sca_conservative_cluster;
    friend class sca_util::sca_implementation::sca_function_vector<double>;
    friend class sca_eln::sca_implementation::sca_eln_view;
    friend class sca_lsf::sca_implementation::sca_lsf_view;
    friend class sca_lsf::sca_implementation::lin_eqs_cluster;
    friend class sca_eln::sca_implementation::lin_eqs_cluster;
    friend class sca_ac_analysis::sca_implementation::sca_ac_domain_entity;
    friend class sca_ac_analysis::sca_implementation::sca_ac_domain_eq;
    friend class sca_ac_analysis::sca_implementation::sca_ac_domain_solver;
    friend class sca_ac_analysis::sca_implementation::solve_linear_complex_eq_system;
    friend std::ostream& operator<< ( std::ostream&,const sca_matrix_base<double>&);
};

}
}

#endif //#ifndef SCA_MATRIX_BASE_DOUBLE_H_
