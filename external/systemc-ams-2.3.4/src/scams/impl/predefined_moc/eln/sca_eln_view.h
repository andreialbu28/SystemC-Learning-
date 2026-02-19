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

 sca_eln_view.h - description

 Original Author: Karsten Einwich Fraunhofer IIS/EAS Dresden

 Created on: 06.11.2009

 SVN Version       :  $Revision: 2106 $
 SVN last checkin  :  $Date: 2020-02-26 15:58:39 +0000 (Wed, 26 Feb 2020) $
 SVN checkin by    :  $Author: karsten $
 SVN Id            :  $Id: sca_eln_view.h 2106 2020-02-26 15:58:39Z karsten $

 *****************************************************************************/

/*****************************************************************************/

#ifndef SCA_ELN_VIEW_H_
#define SCA_ELN_VIEW_H_

#include "scams/impl/solver/linear/sca_linear_equation_if.h"
#include "scams/impl/predefined_moc/conservative/sca_conservative_view.h"
#include "scams/impl/synchronization/sca_synchronization_obj_if.h"

namespace sca_core
{
namespace sca_implementation
{
class sca_linear_solver;
}
}

namespace sca_eln
{
namespace sca_implementation
{

class lin_eqs_cluster: public sca_core::sca_implementation::sca_conservative_cluster,
		public sca_core::sca_implementation::sca_linear_equation_if
{
public:
	sca_core::sca_implementation::sca_linear_equation_system* eqs;
	sca_core::sca_implementation::request_parameters view_params;

	friend class sca_linnet_view;

public:


	lin_eqs_cluster();
	~lin_eqs_cluster();

	/**
	 * implementation of methods from sca_linear_equation_if
	 */
	sca_core::sca_implementation::sca_linear_equation_system& get_equation_system()
	{
		return *eqs;
	}

	void reinit_equations();

	long& get_reinit_request()
	{
		return view_params.request_reinit;
	}

	long& get_count_request_woodbury()
	{
		return view_params.count_request_woodbury;
	}

	bool& get_request_decomp_A()
	{
		return view_params.request_decomp_A;
	}

	int& get_request_restore_checkpoint()
	{
		return view_params.request_restore_checkpoint;
	}

	bool& get_request_store_coefficients()
	{
		return view_params.request_store_coefficients;
	}
};

//////////////////////////

class sca_eln_view: public sca_core::sca_implementation::sca_conservative_view
{
protected:

	sca_core::sca_implementation::sca_solver_base* solver;
	sca_core::sca_implementation::sca_synchronization_object_data* solver_time_data;

public:

	sca_core::sca_implementation::sca_conservative_cluster* create_cluster()
	{
		return new lin_eqs_cluster;
	}

	/** Overwritten (virtual) method for equation setup */
	virtual void setup_equations();

private:

	sca_core::sca_implementation::sca_linear_equation_system* eqs;

};

} // namespace sca_implementation
} // namespace sca_eln


#endif /* SCA_ELN_VIEW_H_ */
