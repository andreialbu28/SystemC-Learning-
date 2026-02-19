/*****************************************************************************

    Copyright 2010-2013
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

  sca_ac_domain_eq.cpp - description

  Original Author: Karsten Einwich Fraunhofer IIS/EAS Dresden

  Created on: 02.01.2010

   SVN Version       :  $Revision: 1920 $
   SVN last checkin  :  $Date: 2016-02-25 12:43:37 +0000 (Thu, 25 Feb 2016) $
   SVN checkin by    :  $Author: karsten $
   SVN Id            :  $Id: sca_ac_domain_eq.cpp 1920 2016-02-25 12:43:37Z karsten $

 *****************************************************************************/

/*****************************************************************************/

#include "systemc-ams"
#include "scams/impl/analysis/ac/sca_ac_domain_eq.h"
#include "scams/impl/analysis/ac/sca_ac_domain_entity.h"



namespace sca_ac_analysis
{
namespace sca_implementation
{

/////////////////////////////////////////////////////////////////////////////

sca_ac_domain_eq::sca_ac_domain_eq(sca_ac_domain_db& ac_data_a) :
    ac_data(ac_data_a),
    x_in(ac_data_a.x_in),
    y_result(ac_data_a.y_result)
{
	number_of_arcs=0;
}


/////////////////////////////////////////////////////////////////////////////

//calculate number of equations and set up
//integration of additional eq's in global
//equation system
void sca_ac_domain_eq::initialize_equation_system()
{
    for(std::vector<sca_ac_domain_entity*>::iterator iit=ac_data.entities.begin();
        iit!=ac_data.entities.end(); ++iit)
    {
        (*iit)->initialize();
    }

    number_of_arcs = (unsigned long)(ac_data.arcs.size());

#ifdef SCA_IMPLEMENTATION_DEBUG
    std::cout << "Number of ac-nodes: " << ac_data.entities.size()
         << " number of ac-arcs: " << number_of_arcs      << std::endl;
#endif

  //determine number of nodes / arcs equations;
  unsigned long number_of_equations = number_of_arcs;


  //allocate additional equations
  for(std::vector<sca_ac_domain_entity*>::iterator iit=ac_data.entities.begin();
      iit!=ac_data.entities.end(); ++iit)
  {
    number_of_equations += (*iit)->
                              init_additional_equations(number_of_equations);
  }


  //reset matrices -> thus they initialized with zero after re-construction
  x_in.resize(0);
  y_result.resize(0);

  //initialize matrices
  Ag.set_sparse_mode();            //matrix a will be used as sparse matrix
  Ag.resize(number_of_equations,number_of_equations);
  Bg.resize(number_of_equations);
  Bgnoise.resize(number_of_equations);
  noise_sources.resize(number_of_equations);


  Ag.unset_auto_resizable();
  Bg.unset_auto_resizable();
  Bgnoise.unset_auto_resizable();
  noise_sources.unset_auto_resizable();

  x_in.resize(number_of_equations);
  y_result.resize(number_of_equations);

}

/////////////////////////////////////////////////////////////////////////////

//sets up the equation system fo a certain frequency point
void sca_ac_domain_eq::setup_equations(double w)
{
  //reset matrices to zero
  Ag.reset();
  Bg.reset();
  Bgnoise.reset();

  //calculate instance matrices and insert in global eq
  for(std::vector<sca_ac_domain_entity*>::iterator iit=ac_data.entities.begin();
      iit<ac_data.entities.end(); ++iit)
  {
    //first setup additional equations
    (*iit)->setup_add_eqs(Ag, Bg, w);

    //then setup node contributions
    (*iit)->computeAB_insert(Ag, Bg, w);
  }
}

/////////////////////////////////////////////////////////////////////////////

} // namespace sca_implementation
} // namespace sca_ac_analysis

