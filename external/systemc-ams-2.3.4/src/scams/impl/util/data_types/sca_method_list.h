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

 sca_method_list.h - description

 Original Author: Karsten Einwich Fraunhofer IIS/EAS Dresden

 Created on: 05.11.2009

 SVN Version       :  $Revision: 1706 $
 SVN last checkin  :  $Date: 2014-04-23 13:50:03 +0000 (Wed, 23 Apr 2014) $
 SVN checkin by    :  $Author: karsten $
 SVN Id            :  $Id: sca_method_list.h 1706 2014-04-23 13:50:03Z karsten $

 *****************************************************************************/

/*****************************************************************************/

#ifndef SCA_METHOD_LIST_H_
#define SCA_METHOD_LIST_H_

namespace sca_util
{
namespace sca_implementation
{

/////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

/**
 * list of methods with return value TR and an argument of type TA
 * the elements will be added or subtract using the method
 * calc_value
 * The following operators must be defined for TR :
 *                        copy constructor
 *                        TR(long)  //for reset to zero
 *                        +=
 *                        -=
 */
template<class TR>
class sca_method_list
{
public:
	// type of method to be evaluated
	typedef TR (sc_core::sc_object::*eval_func)();
	typedef sca_method_object<TR> sca_method_objectT;

private:

	struct method_obj
	{
		sc_core::sc_object* module;
		eval_func func;
		bool sign;

		inline TR calc_func()
		{
			if (sign)
				return -(module->*func)();
			else
				return (module->*func)();
		}

		inline method_obj(sc_core::sc_object* mod, eval_func fn, bool sig) :
			module(mod), func(fn), sign(sig)
		{
		}

	};

	TR value;
	TR const_value;
	std::vector<method_obj> method_list;

	bool value_set;
	bool method_list_set;

    typedef	typename std::vector<typename
	sca_method_list<TR>::method_obj>::iterator method_list_it;

public:

	void clear_list();

	sca_method_list();

	~sca_method_list();

	/** returns true if a value or method list is set */
	bool is_set() const;

	/** returns true if a method list is set */
	bool is_method_list_set() const;


	/** add method with positive sign */
	void add_element(eval_func fn, sc_core::sc_object* ele);

	/** add method with negative sign */
	void sub_element(eval_func fn, sc_core::sc_object* ele);

	/** calculate current value */
	const TR& calc_value();

	/** set constant (offset) value */
	void set_value(TR val);


	/** add value tu constant (offset) value */
	void add_value(TR val);
};

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

template<class TR>
inline void sca_method_list<TR>::clear_list()
{
	method_list.clear();
	value = (TR)0;
	const_value = (TR)0;

	value_set=false;
	method_list_set=false;
}

template<class TR>
inline sca_method_list<TR>::sca_method_list()
{
	value = (TR)0;
	const_value = (TR)0;

	value_set=false;
	method_list_set=false;
}



template<class TR>
inline sca_method_list<TR>::~sca_method_list()
{
	clear_list();
}

/** add method with positive sign */
template<class TR>
inline void sca_method_list<TR>::add_element(eval_func fn, sc_core::sc_object* ele)
{
	method_obj tmp_obj(ele, fn, false);
	method_list.push_back(tmp_obj);

	method_list_set=true;
}

/** add method with negative sign */
template<class TR>
inline void sca_method_list<TR>::sub_element(eval_func fn, sc_core::sc_object* ele)
{
	method_obj tmp_obj(ele, fn, true);
	method_list.push_back(tmp_obj);

	method_list_set=true;
}

/** calculate current value */
template<class TR>
inline const TR& sca_method_list<TR>::calc_value()
{
	value = const_value;
	if(method_list.empty()) return value;

	for(method_list_it it = method_list.begin();
			it != method_list.end();
			++it) value += it->calc_func();
	return value;
}

/** set constant (offset) value */
template<class TR>
inline void sca_method_list<TR>::set_value(TR val)
{
	const_value = val;

	value_set=true;
}

/** add value to constant (offset) value */
template<class TR>
inline void sca_method_list<TR>::add_value(TR val)
{
	const_value += val;

	value_set=true;
}

/** returns true if a value or method list is set */
template<class TR>
inline bool sca_method_list<TR>::is_set() const
{
	return value_set || method_list_set;
}

/** returns true if a method list is set */
template<class TR>
inline bool sca_method_list<TR>::is_method_list_set() const
{
	return method_list_set;
}

}
}

#endif /* SCA_METHOD_LIST_H_ */
