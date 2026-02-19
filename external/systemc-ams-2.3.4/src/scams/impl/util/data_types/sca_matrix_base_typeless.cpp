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

   SVN Version       :  $Revision: 2109 $
   SVN last checkin  :  $Date: 2020-03-03 14:22:27 +0000 (Tue, 03 Mar 2020) $
   SVN checkin by    :  $Author: karsten $
   SVN Id            :  $Id: sca_matrix_base_typeless.cpp 2109 2020-03-03 14:22:27Z karsten $

 *****************************************************************************/

/*****************************************************************************/

#include "sca_matrix_base_typeless.h"

namespace sca_util
{
namespace sca_implementation
{



sca_matrix_base_typeless::sca_matrix_base_typeless()       //default matrix
{
    dim      = 0;
    sizes[0] = 0;
    sizes[1] = 0;
    square   = 0;
    auto_dim = 1;
    auto_sizable=0;
    last_val = -1;
    accessed = true;    //after creation matrix is assumed as changed
    ignore_negative=1;
}



sca_matrix_base_typeless::sca_matrix_base_typeless(unsigned long x)       //vector not resizable
{
    dim      = 1;
    sizes[0] = 0;
    sizes[1] = 0;
    square   = 0;
    auto_dim = 0;
    auto_sizable=0;
    last_val = x-1;
    ignore_negative=1;
    accessed = true;    //after creation matrix is assumed as changed
}



sca_matrix_base_typeless::sca_matrix_base_typeless(unsigned long y, unsigned long x)       //matrix not resizable
{
    dim      = 2;
    sizes[0] = 0;
    sizes[1] = 0;
    square   = 0;
    auto_dim = 0;
    ignore_negative=1;
    accessed = true;    //after creation matrix is assumed as changed
    auto_sizable=0;
    last_val = x-1;
}


sca_matrix_base_typeless::~sca_matrix_base_typeless()
{
}


sca_matrix_base_typeless::sca_matrix_base_typeless(const sca_matrix_base_typeless& m)
{
    dim      = m.dim;
    sizes[0] = m.sizes[0];
    sizes[1] = m.sizes[1];
    square   = m.square;
    auto_dim = m.auto_dim;
    auto_sizable=m.auto_sizable;
    last_val = m.last_val;
    accessed = true;
    ignore_negative=m.ignore_negative;
}




unsigned long sca_matrix_base_typeless::dimx() const
{
	return(sizes[0]);
}


unsigned long sca_matrix_base_typeless::dimy() const
{
	return(sizes[1]);
}


void sca_matrix_base_typeless::set_autodim()
{
	auto_dim=1;
}


void sca_matrix_base_typeless::reset_autodim()
{
	auto_dim=0;
}


bool sca_matrix_base_typeless::get_autodim() const
{
	return auto_dim;
}


void sca_matrix_base_typeless::set_sizable()
{
	auto_sizable=1;
}



void sca_matrix_base_typeless::reset_sizable()
{
	auto_sizable=0;
}



void sca_matrix_base_typeless::reset_ignore_negative()
{
	ignore_negative=0;
}


void sca_matrix_base_typeless::set_ignore_negative()
{
	ignore_negative=1;
}


void sca_matrix_base_typeless::reset_access_flag()
{
	accessed=false;
}


bool sca_matrix_base_typeless::get_access_flag() const
{
	return accessed;
}

}
}

