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

 sca_sync_value_handle.h - description

 Original Author: Karsten Einwich Fraunhofer IIS/EAS Dresden

 Created on: 27.08.2009

 SVN Version       :  $Revision: 2038 $
 SVN last checkin  :  $Date: 2017-03-06 14:39:39 +0000 (Mon, 06 Mar 2017) $
 SVN checkin by    :  $Author: karsten $
 SVN Id            :  $Id: sca_sync_value_handle.h 2038 2017-03-06 14:39:39Z karsten $

 *****************************************************************************/

/*****************************************************************************/


#ifndef SCA_SYNC_VALUE_HANDLE_H_
#define SCA_SYNC_VALUE_HANDLE_H_

namespace sca_core
{
namespace sca_implementation
{


class sca_sync_value_handle_base
{
public:

	typedef void (::sc_core::sc_object::*value_method)();

	virtual ~sca_sync_value_handle_base();

	void set_method(::sc_core::sc_object* obj, value_method method_);
	void call_method();
	::sc_core::sc_object* get_object();

	void set_index(long index);
	long get_index();

	void set_id(long id);
	long get_id();

	void virtual resize(int i) = 0;
	void virtual store_tmp() = 0;
	void virtual read_tmp() = 0;
	void virtual dump(std::ostream& str) = 0;
	virtual void backup_tmp() = 0;
	virtual void restore_tmp() = 0;

protected:
	long id;
	long index;

private:
	::sc_core::sc_object* object;
	value_method method;

};



template<class T>
class sca_sync_value_handle: public sca_core::sca_implementation::sca_sync_value_handle_base
{
	std::vector<T> values;
	void resize(int i);

	mutable T value;
	T value_backup;

public:

	void store_tmp();
	void read_tmp();
	void dump(std::ostream& str);

	void write_tmp(T value_tmp);
	void write(T value_tmp);
	const T& read() const;
	void backup_tmp();
	void restore_tmp();

	virtual ~sca_sync_value_handle();

};

/////////////////////////////

template<class T>
inline void sca_sync_value_handle<T>::backup_tmp()
{
	value_backup=value;
}

template<class T>
inline void sca_sync_value_handle<T>::restore_tmp()
{
	value=value_backup;
}


template<class T>
inline void sca_sync_value_handle<T>::resize(int i)
{
	values.resize(i);
}

template<class T>
inline void sca_sync_value_handle<T>::store_tmp()
{
	values[index] = value;
}


template<class T>
inline void sca_sync_value_handle<T>::read_tmp()
{
	value = values[index];
}


template<class T>
inline void sca_sync_value_handle<T>::dump(std::ostream& str)
{
	str << value;
}

template<class T>
inline void sca_sync_value_handle<T>::write_tmp(T value_tmp)
{
	value = value_tmp;
}


template<class T>
inline void sca_sync_value_handle<T>::write(T value_tmp)
{
	values[index] = value = value_tmp;
}

template<class T>
inline const T& sca_sync_value_handle<T>::read() const
{
	if(values.size()>std::size_t(index)) value = values[index];
	return value;
}

template<class T>
inline sca_sync_value_handle<T>::~sca_sync_value_handle()
{
}

}
}

#endif /* SCA_SYNC_VALUE_HANDLE_H_ */
