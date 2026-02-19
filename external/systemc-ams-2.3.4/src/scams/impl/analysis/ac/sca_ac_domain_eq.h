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

  sca_ac_domain_eq.h - description

  Original Author: Karsten Einwich Fraunhofer IIS/EAS Dresden

  Created on: 02.01.2010

   SVN Version       :  $Revision: 1523 $
   SVN last checkin  :  $Date: 2013-02-17 20:36:57 +0000 (Sun, 17 Feb 2013) $
   SVN checkin by    :  $Author: karsten $
   SVN Id            :  $Id: sca_ac_domain_eq.h 1523 2013-02-17 20:36:57Z karsten $

 *****************************************************************************/

/*****************************************************************************/


#ifndef SCA_AC_DOMAIN_EQ_H_
#define SCA_AC_DOMAIN_EQ_H_

#include "scams/impl/analysis/ac/sca_ac_domain_db.h"

namespace sca_ac_analysis
{
namespace sca_implementation
{

class sca_ac_domain_eq
{
public:

  sca_ac_domain_eq(sca_ac_domain_db& ac_data_a);

  //returns reference to equation system
  sca_util::sca_matrix<sca_util::sca_complex >& get_Ag()
  {
    return Ag;
  }

  sca_util::sca_vector<sca_util::sca_complex >& get_Bg()
  {
    return Bg;
  }

  unsigned long get_number_of_arcs()
  {
    return number_of_arcs;
  }

  unsigned long get_number_of_entities()
  {
	  return (unsigned long)(ac_data.entities.size());
  }

  std::vector<sca_ac_domain_entity*>& get_entites()
  {
    return ac_data.entities;
  }


  sca_util::sca_vector<sca_util::sca_complex >& get_Bgnoise()
  {
  	return Bgnoise;
  }


  sca_util::sca_vector<std::string>& get_noise_src_names()
  {
  	return noise_sources;
  }


  void initialize_equation_system();

  void setup_equations(double w);

private:

  sca_ac_domain_db& ac_data;

  //for runtime speed
  unsigned long number_of_arcs;


  //global complex equation system Ag * x = Bg
  sca_util::sca_matrix<sca_util::sca_complex >	Ag;
  sca_util::sca_vector<sca_util::sca_complex >	Bg;
  sca_util::sca_vector<sca_util::sca_complex >	Bgnoise;
  sca_util::sca_vector<std::string>             noise_sources;

  //references to in  and result vectors
  sca_util::sca_vector<sca_util::sca_complex >&	x_in;
  sca_util::sca_vector<sca_util::sca_complex >&	y_result;

};


} // namespace sca_implementation
} // namespace sca_ac_analysis


#endif /* SCA_AC_DOMAIN_EQ_H_ */
