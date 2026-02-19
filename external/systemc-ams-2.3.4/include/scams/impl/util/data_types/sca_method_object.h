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

  sca_method_object.h - description

  Original Author: Karsten Einwich Fraunhofer IIS/EAS Dresden

  Created on: 06.11.2009

   SVN Version       :  $Revision: 1376 $
   SVN last checkin  :  $Date: 2012-08-09 08:28:06 +0000 (Thu, 09 Aug 2012) $
   SVN checkin by    :  $Author: reuther $
   SVN Id            :  $Id: sca_method_object.h 1376 2012-08-09 08:28:06Z reuther $

 *****************************************************************************/

/*****************************************************************************/


#ifndef SCA_METHOD_OBJECT_H_
#define SCA_METHOD_OBJECT_H_

namespace sca_util
{
namespace sca_implementation
{

/**
 * object for handling method pointers from sc_object
 */
template<class TR>
class sca_method_object
{
public:
	// type of method to be evaluated
	typedef TR (sc_core::sc_object::*eval_method)();

private:

	sc_core::sc_object* object;
	eval_method method;

public:

	TR call_method();
	void set_method(sc_core::sc_object* object_v, eval_method method_v);
	void clear();
	bool is_valid();

	sca_method_object();
	sca_method_object(sc_core::sc_object* obj, eval_method meth);
	bool operator==(const sca_method_object<TR>& obj);

};

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////


//instead typedef to permit forward declaration
class sca_method_vector : public std::vector<sca_method_object<void> >
{
};

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////


//class of sca_method_vectors used for set equations
class sca_solve_methods
{
public:
	sca_method_vector post_solve_methods;
	sca_method_vector pre_solve_methods;
	sca_method_vector pre_solve_reinit_methods;
	sca_method_vector test_methods;
	sca_method_vector local_test_methods;

	void resize(int n)
	{
		post_solve_methods.resize(n);
		pre_solve_methods.resize(n);
		pre_solve_reinit_methods.resize(n);
		test_methods.resize(n);
		local_test_methods.resize(n);
	}
};


typedef sca_method_object<void>::eval_method  sca_void_method;

template<class TR>
inline TR sca_method_object<TR>::call_method()
{
	if(object==NULL) return TR();
	return (object->*method)();
}

//////////////////////////////////////////////////////////////////////////////

template<class TR>
inline void sca_method_object<TR>::set_method(sc_core::sc_object* object_v,
		eval_method method_v)
{
	object = object_v;
	method = method_v;
}

template<class TR>
inline void sca_method_object<TR>::clear()
{
	object = NULL;
	method = NULL;
}

template<class TR>
inline bool sca_method_object<TR>::is_valid()
{
	return (object != NULL) && (method != NULL);
}



//////////////////////////////////////////////////////////////////////////////

template<class TR>
inline sca_method_object<TR>::sca_method_object()
{
	object = NULL;
	method = NULL;
}

//////////////////////////////////////////////////////////////////////////////

template<class TR>
inline sca_method_object<TR>::sca_method_object(sc_core::sc_object* obj, eval_method meth) :
	object(obj), method(meth)
{

}


//////////////////////////////////////////////////////////////////////////////

template<class TR>
inline bool sca_method_object<TR>::operator==(const sca_method_object<TR>& obj)
{
		return (object == obj.object) && (method == obj.method);
}

} // namespace sca_implementation
} // namespace sca_util


#define SCA_MPTR(A) static_cast<sca_util::sca_implementation::sca_method_object<double>::eval_method>(&A)
#define SCA_VMPTR(A) sca_util::sca_implementation::sca_method_object<void>(this,static_cast<sca_util::sca_implementation::sca_method_object<void>::eval_method>(&A))

#endif /* SCA_METHOD_OBJECT_H_ */
