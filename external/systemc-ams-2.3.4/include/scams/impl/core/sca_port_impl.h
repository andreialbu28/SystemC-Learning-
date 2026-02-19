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

 sca_port_impl.h - description

 Original Author: Karsten Einwich Fraunhofer IIS/EAS Dresden

 Created on: 23.06.2009

 SVN Version       :  $Revision: 2320 $
 SVN last checkin  :  $Date: 2023-06-08 12:47:38 +0000 (Thu, 08 Jun 2023) $
 SVN checkin by    :  $Author: pehrlich $
 SVN Id            :  $Id: sca_port_impl.h 2320 2023-06-08 12:47:38Z pehrlich $

 *****************************************************************************/

/*****************************************************************************/

#ifndef SCA_PORT_IMPL_H_
#define SCA_PORT_IMPL_H_

namespace sca_core
{

namespace sca_implementation
{

/**
 Template sca_port base class for all analog
 ports
 */
template<class IF>
class sca_port_impl: public ::sc_core::sc_port<IF,1,sc_core::SC_ONE_OR_MORE_BOUND>,
                     public ::sca_core::sca_implementation::sca_port_base
{
public:

	// typdefs
	typedef ::sc_core::sc_port<IF,1,sc_core::SC_ONE_OR_MORE_BOUND> sc_base_type;
	typedef ::sca_core::sca_implementation::sca_port_impl<IF> this_type;

	typedef sca_port_base base_type;

	// constructors
	sca_port_impl();
	explicit sca_port_impl(const char* name_);

	// destructor
	virtual ~sca_port_impl();

	virtual const char* kind() const;

	// get the first sc_interface without checking for nil
	virtual ::sc_core::sc_interface* sc_get_interface();

	virtual const ::sc_core::sc_interface* sc_get_interface() const;


	// gets always the sca_interface without checking for nil
	virtual ::sc_core::sc_interface* get_interface();

	virtual const ::sc_core::sc_interface* get_interface() const;

/*  !!!!! should be only required for converter ports ?????????
	// overwrite sc_port operator ->  always access the sca_interface
	sca_interface* operator ->();

	const sca_interface* operator ->() const;
*/

protected:

	virtual void end_of_elaboration();

private:

	static const char* const kind_string;

	// disabled
	//sca_port( const this_type& );
	this_type& operator =(const this_type&){return *this;}

	// allow to call methods provided by interface at index disabled
	//due multiple sca_interfaces currently not supported
	IF* operator [](int index_);

	const IF* operator [](int index_) const;

};

///////////////////////////////////////////////////////////////////

//constructors
template<class IF>
inline sca_port_impl<IF>::sca_port_impl() :
	sc_base_type(), base_type()
{
    this->construct_parent(this);
}

template<class IF>
inline sca_port_impl<IF>::sca_port_impl(const char* name_) :
	sc_base_type(name_), base_type()
{
    this->construct_parent(this);
}

// destructor (does nothing)
template<class IF>
inline sca_port_impl<IF>::~sca_port_impl()
{
}

template<class IF>
inline const char* sca_port_impl<IF>::kind() const
{
	return kind_string;
}

// get the first sc_interface without checking for nil
template<class IF>
inline ::sc_core::sc_interface* sca_port_impl<IF>::sc_get_interface()
{
	return ::sc_core::sc_port<IF,1>::get_interface();
}

template<class IF>
inline const ::sc_core::sc_interface* sca_port_impl<IF>::sc_get_interface() const
{
	return ::sc_core::sc_port<IF,1>::get_interface();
}

// gets always the sca_interface without checking for nil
template<class IF>
inline ::sc_core::sc_interface* sca_port_impl<IF>::get_interface()
{
	return sca_get_interface();
}

template<class IF>
inline const ::sc_core::sc_interface* sca_port_impl<IF>::get_interface() const
{
	return sca_get_interface();
}

/*

// overwrite sc_port operator ->  always access the sca_interface
template<class IF>
sca_interface* sca_port_impl<IF>::operator ->()
{
	return dynamic_cast<sca_interface*> (sca_get_interface());
}

template<class IF>
const sca_interface* sca_port_impl<IF>::operator ->() const
{
	return dynamic_cast<sca_interface*> (sca_get_interface());
}

*/

template<class IF>
inline void sca_port_impl<IF>::end_of_elaboration()
{
	//assign variables for hierarchical ports also
	//otherwise elaborate_port activated during systemc_ams_init
	//does this job
	if (m_if == NULL)
	{
		m_if = sc_get_interface();
	}

	if (m_sca_if == NULL)
	{
		m_sca_if = dynamic_cast< ::sca_core::sca_interface*> (m_if);
	}
}


template<class IF>
const char* const sca_port_impl<IF>::kind_string = "sca_port";



// allow to call methods provided by interface at index disabled
//due multiple sca_interfaces currently not supported
template<class IF>
inline IF* sca_port_impl<IF>::operator [](int index_)
{
	std::ostringstream str;
	str << "Not possible error in: " << __FILE__ << " line: " << __LINE__
			<< std::endl;
	SC_REPORT_ERROR("SystemC-AMS",str.str().c_str());
	return NULL;
}


template<class IF>
inline const IF* sca_port_impl<IF>::operator [](int index_) const
{
	std::ostringstream str;
	str << "Not possible error in: " << __FILE__ << " line: " << __LINE__
			<< std::endl;
	SC_REPORT_ERROR("SystemC-AMS",str.str().c_str());
	return NULL;
}



} //namespace sca_implementation

} //namespace sca_core

#endif /* SCA_PORT_IMPL_H_ */
