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

  sca_matrix_base.h - description

  Original Author: Karsten Einwich COSEDA Technologies GmbH

  Created on: 09.10.2009

   SVN Version       :  $Revision: 2361 $
   SVN last checkin  :  $Date: 2023-07-06 15:00:20 +0000 (Thu, 06 Jul 2023) $
   SVN checkin by    :  $Author: pehrlich $
   SVN Id            :  $Id: sca_matrix_base_typeless.h 2361 2023-07-06 15:00:20Z pehrlich $

 *****************************************************************************/

/*****************************************************************************/

#ifndef SCA_MATRIX_BASE_TYPE_LESS_H_
#define SCA_MATRIX_BASE_TYPE_LESS_H_

namespace sca_core
{
namespace sca_implementation
{
class sca_linear_solver;
class sca_conservative_cluster;
}
}


namespace sca_tdf
{
namespace sca_implementation
{
class sca_ct_ltf_nd_proxy;
class sca_ct_ltf_zp_proxy;
class sca_ct_vector_ss_proxy;
}
}


namespace sca_eln
{
namespace sca_implementation
{
class sca_eln_view;
class lin_eqs_cluster;
}
}

namespace sca_lsf
{
namespace sca_implementation
{
class sca_lsf_view;
class lin_eqs_cluster;
}
}



namespace sca_ac_analysis
{
namespace sca_implementation
{
class sca_ac_domain_entity;
class sca_ac_domain_eq;
class sca_ac_domain_solver;
class solve_linear_complex_eq_system;
}
}


namespace sca_util
{
namespace sca_implementation
{


class sca_matrix_base_typeless
{
	friend class sca_tdf::sca_implementation::sca_ct_ltf_nd_proxy;
	friend class sca_tdf::sca_implementation::sca_ct_ltf_zp_proxy;
	friend class sca_tdf::sca_implementation::sca_ct_vector_ss_proxy;

protected:

    unsigned long  sizes[2];    // -> dim=1 (vector) and dim=2 supported
    bool           auto_dim, auto_sizable;
    bool           ignore_negative;


    bool                square;
    mutable long                last_val;
    unsigned long       dim;      //currently dimension is set to 2 or 1
    bool                accessed;

protected:

    sca_matrix_base_typeless();                                  //default matrix
    sca_matrix_base_typeless(unsigned long x);                   //vector not resizable
    sca_matrix_base_typeless(unsigned long y, unsigned long x);  //matrix not resizable
    sca_matrix_base_typeless(const sca_matrix_base_typeless& m);       //copied matrix

    virtual ~sca_matrix_base_typeless();


    unsigned long dimx() const;
    unsigned long dimy() const;
    void set_autodim();
    void reset_autodim();
    bool get_autodim() const;
    void set_sizable();
    void reset_sizable();

    void reset_ignore_negative();
    void set_ignore_negative();

    void reset_access_flag();
    bool get_access_flag() const;



    friend class sca_core::sca_implementation::sca_linear_solver;
    friend class sca_core::sca_implementation::sca_conservative_cluster;
    friend class sca_eln::sca_implementation::sca_eln_view;
    friend class sca_lsf::sca_implementation::sca_lsf_view;
    friend class sca_lsf::sca_implementation::lin_eqs_cluster;
    friend class sca_eln::sca_implementation::lin_eqs_cluster;
    friend class sca_ac_analysis::sca_implementation::sca_ac_domain_entity;
    friend class sca_ac_analysis::sca_implementation::sca_ac_domain_eq;
    friend class sca_ac_analysis::sca_implementation::sca_ac_domain_solver;
    friend class sca_ac_analysis::sca_implementation::solve_linear_complex_eq_system;
};


}
}


#endif /* SCA_MATRIX_BASE_TYPE_LESS_H_ */
