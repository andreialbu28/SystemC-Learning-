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

  sca_parameter_base.cpp - description

  Original Author: Karsten Einwich Fraunhofer IIS/EAS Dresden

  Created on: 05.08.2009

   SVN Version       :  $Revision: 1914 $
   SVN last checkin  :  $Date: 2016-02-23 18:06:06 +0000 (Tue, 23 Feb 2016) $
   SVN checkin by    :  $Author: karsten $
   SVN Id            :  $Id: sca_parameter_base.cpp 1914 2016-02-23 18:06:06Z karsten $

 *****************************************************************************/

/*****************************************************************************/

#include <systemc>
#include "scams/core/sca_parameter_base.h"


namespace sca_core
{

const char* sca_parameter_base::kind() const
{
	return "sca_core::sca_parameter_base";
}

void sca_parameter_base::lock()
{
	locked_flag=true;
}

void sca_parameter_base::unlock()
{
	locked_flag=false;
	unlock_flag=true;
}

bool sca_parameter_base::is_locked() const
{
	return locked_flag;
}


bool sca_parameter_base::is_assigned() const
{
	return is_assigned_flag;
}


sca_parameter_base::sca_parameter_base() :
		sc_core::sc_object(sc_core::sc_gen_unique_name("sca_parameter_base"))
{
	locked_flag=false;
	unlock_flag=false;
	is_assigned_flag=false;
}


sca_parameter_base::sca_parameter_base(const char* nm): sc_core::sc_object(nm)
{
	locked_flag=false;
	unlock_flag=false;
	is_assigned_flag=false;
}

sca_parameter_base::~sca_parameter_base()
{
}




sca_parameter_base::sca_parameter_base(const sca_parameter_base& p) :
         sc_core::sc_object(p)
{
	locked_flag=p.locked_flag;
	unlock_flag=p.unlock_flag;
	is_assigned_flag=p.is_assigned_flag;
}


//disabled
sca_core::sca_parameter_base& sca_parameter_base::operator=(const sca_core::sca_parameter_base& p)
{
	return const_cast<sca_core::sca_parameter_base&>(p);
}

std::ostream& operator << (std::ostream& str, const sca_core::sca_parameter_base& par)
{
	par.print(str);
	return str;
}


}

