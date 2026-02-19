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

  sca_view_base.h - description

  Original Author: Karsten Einwich Fraunhofer IIS/EAS Dresden

  Created on: 14.05.2009

   SVN Version       :  $Revision: 1563 $
   SVN last checkin  :  $Date: 2013-05-03 15:06:50 +0000 (Fri, 03 May 2013) $
   SVN checkin by    :  $Author: karsten $
   SVN Id            :  $Id: sca_view_base.h 1563 2013-05-03 15:06:50Z karsten $

 *****************************************************************************/

/*****************************************************************************/


#ifndef SCA_VIEW_BASE_H_
#define SCA_VIEW_BASE_H_

#include <string>


////////////////////////////////////////////////////

namespace sca_core
{
namespace sca_implementation
{
class sca_view_object;
class sca_solver_manager;

////////////////////////////////////////////////////

/** SystemC-AMS channel base for views.
    A view partitionates the module graphs,
    setup the equation systems and instatiates the
    required solvers.
*/
class sca_view_base
{

public:

	sca_view_base();
	virtual ~sca_view_base();

	virtual bool is_equal(sca_view_base* view_interface);
	virtual void setup_equations() = 0;


	sca_view_object* create_view();


	const std::string& get_name() { return name; }

protected:


	std::string              name;
	sca_view_object*    view_datas;
	sca_solver_manager* solvers;

};

} //namespace sca_implementation
} //namespace sca_core

#endif /* SCA_VIEW_BASE_H_ */
