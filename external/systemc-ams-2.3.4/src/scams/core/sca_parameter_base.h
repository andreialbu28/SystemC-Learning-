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

 sca_parameter_base.h - base class for SystemC AMS parameters

 Original Author: Karsten Einwich Fraunhofer IIS/EAS Dresden

 Created on: 03.03.2009

 SVN Version       :  $Revision: 2127 $
 SVN last checkin  :  $Date: 2020-03-23 13:09:32 +0000 (Mon, 23 Mar 2020) $ (UTC)
 SVN checkin by    :  $Author: karsten $
 SVN Id            :  $Id: sca_parameter_base.h 2127 2020-03-23 13:09:32Z karsten $

 *****************************************************************************/

/*
 LRM clause 3.2.6.
 The class sca_core::sca_parameter_base shall define a type independent base
 class for module parameters. After construction, parameters shall be unlocked.
 */

/*****************************************************************************/

#ifndef SCA_PARAMETER_BASE_H_
#define SCA_PARAMETER_BASE_H_

namespace sca_core
{

class sca_parameter_base: public sc_core::sc_object
{
public:
	virtual const char* kind() const;

	virtual std::string to_string() const = 0;

	void lock();
	void unlock();
	bool is_locked() const;

	bool is_assigned() const;

protected:
	sca_parameter_base();
	explicit sca_parameter_base(const char*);
	virtual ~sca_parameter_base();

	sca_parameter_base(const sca_parameter_base&);

private:
	// Disabled
	sca_core::sca_parameter_base& operator=(const sca_core::sca_parameter_base&);

protected:
	//begin implementation defined

	bool locked_flag;
	bool unlock_flag;
	bool is_assigned_flag;

	//end implementation defined
};

std::ostream& operator << (std::ostream&, const sca_core::sca_parameter_base&);

} // namespace sca_core


#endif /* SCA_PARAMETER_BASE_H_ */
