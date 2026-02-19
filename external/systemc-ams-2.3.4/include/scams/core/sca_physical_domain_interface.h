/*****************************************************************************

    Copyright 2020-2020
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

  sca_interface.h - base class for all SystemC AMS interfaces

  Original Author: Karsten Einwich COSEDA Technologies GmbH

  Created on: 20.02.2020

   SVN Version       :  $Revision: 1265 $
   SVN last checkin  :  $Date: 2011-11-19 12:43:31 -0800 (Sat, 19 Nov 2011) $
   SVN checkin by    :  $Author: karsten $
   SVN Id            :  $Id: sca_interface.h 1265 2011-11-19 20:43:31Z karsten $

 *****************************************************************************/
   /*
     LRM clause 3.2.2.
     The class sca_core::sca_interface shall define the base class
     to derive interfaces for the predefined models of computation.
   */

/*****************************************************************************/


#ifndef SCA_PHYSICAL_DOMAIN_INTERFACE_H_
#define SCA_PHYSICAL_DOMAIN_INTERFACE_H_


namespace sca_core
{

  class sca_physical_domain_interface
  {
  public:

	  virtual void set_unit(const std::string& unit)=0;
	  virtual const std::string& get_unit() const=0;

	  virtual void set_unit_prefix(const std::string& prefix)=0;
	  virtual const std::string& get_unit_prefix() const=0;

	  virtual void set_domain(const std::string& domain)=0;
	  virtual const std::string& get_domain() const=0;

	  virtual ~sca_physical_domain_interface(){}
  };

} // namespace sca_core



#endif /* SCA_PHYSICAL_DOMAIN_INTERFACE_H_ */
