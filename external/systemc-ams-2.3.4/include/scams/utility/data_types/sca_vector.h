/*****************************************************************************

    Copyright 2010-2012
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

 sca_vector.h - data type for handling vectors

 Original Author: Karsten Einwich Fraunhofer IIS/EAS Dresden

 Created on: 08.03.2009

 SVN Version       :  $Revision: 2361 $
 SVN last checkin  :  $Date: 2023-07-06 15:00:20 +0000 (Thu, 06 Jul 2023) $
 SVN checkin by    :  $Author: pehrlich $
 SVN Id            :  $Id: sca_vector.h 2361 2023-07-06 15:00:20Z pehrlich $

 *****************************************************************************/
/*
 * LRM clause 6.2.1.3.
 * The class sca_util::sca_vector shall provide a type for handling vectors of
 * an arbitrary type.
 */

/*****************************************************************************/

#ifndef SCA_VECTOR_H_
#define SCA_VECTOR_H_



namespace sca_util
{


template<class T>
//class sca_vector
class sca_vector  : public sca_util::sca_implementation::sca_matrix_base<T>
{
public:
	sca_vector();

	explicit sca_vector(unsigned long len);
	sca_vector(const sca_util::sca_vector<T>& vector);
	sca_vector(const std::initializer_list<T>&);


	void resize(unsigned long len);
	void set_auto_resizable();
	void unset_auto_resizable();
	bool is_auto_resizable() const;
	unsigned long length() const;

	T& operator()(unsigned long pos);
	const T& operator()(unsigned long pos) const;

	sca_util::sca_vector<T>& operator=(const sca_util::sca_vector<T>& vector);
	sca_util::sca_vector<T>& operator=(sca_core::sca_assign_from_proxy<sca_util::sca_vector<T> >&);

	bool operator== (const sca_util::sca_vector<T>&) const;
	bool operator!= (const sca_util::sca_vector<T>&) const;

	const std::string to_string() const;
	void print(std::ostream& = std::cout) const;


	//begin implementation specific

	sca_vector(const std::vector<T>& vector);
	operator std::vector<T>();




private:

    typedef sca_util::sca_implementation::sca_matrix_base<T> base_type;

    //end implementation specific
};

template<class T>
std::ostream& operator<< (std::ostream&,const sca_util::sca_vector<T>&);


// begin implementation specific

template<class T>
inline sca_vector<T>::sca_vector() : base_type(0ul)
{
    this->set_auto_resizable();
}


template<class T>
inline sca_vector<T>::sca_vector(unsigned long _len) : base_type(_len)
{
    this->set_auto_resizable();
}


template<class T>
inline sca_vector<T>::sca_vector(const sca_util::sca_vector<T>& _vector):base_type(_vector)
{
}

template<class T>
inline sca_vector<T>::sca_vector(const std::vector<T>& _vector):base_type(_vector)
{
    this->set_auto_resizable();
}

template<class T>
inline sca_vector<T>::operator std::vector<T>()
{
	std::vector<T> tmp(this->length());

	for(std::size_t idx=0;idx<tmp.size();++idx)
	{
		tmp[idx]=(*this)[idx];
	}

	return tmp;
}

template<class T>
inline sca_vector<T>::sca_vector(const std::initializer_list<T>& lst):base_type(lst)
{
    this->set_auto_resizable();
}


template<class T>
inline void sca_vector<T>::resize(unsigned long _len)
{
	base_type::resize(_len);
}


template<class T>
inline void sca_vector<T>::set_auto_resizable()
{
	base_type::set_autodim();
}


template<class T>
inline void sca_vector<T>::unset_auto_resizable()
{
	base_type::reset_autodim();
}


template<class T>
inline bool sca_vector<T>::is_auto_resizable() const
{
	return base_type::get_autodim();
}

template<class T>
inline unsigned long sca_vector<T>::length() const
{
	return base_type::dimx();
}

template<class T>
inline T& sca_vector<T>::operator()(unsigned long pos)
{
	return base_type::operator()(pos);
}


template<class T>
inline const T& sca_vector<T>::operator()(unsigned long pos) const
{
	return base_type::operator()(pos);
}



template<class T>
inline sca_util::sca_vector<T>& sca_vector<T>::operator=(const sca_util::sca_vector<T>& vector)
{
	base_type::operator=(vector);
	return *this;
}


template<class T>
inline sca_util::sca_vector<T>& sca_vector<T>::operator=(sca_core::sca_assign_from_proxy<sca_util::sca_vector<T> >& vec)
{
	vec.assign_to(*this);
	return *this;
}

template<class T>
inline const std::string sca_vector<T>::to_string() const
{
	std::ostringstream str;
	str << *this;

	return str.str();
}


template<class T>
inline void sca_vector<T>::print(std::ostream& str) const
{
	str << *this;
}

template<class T>
inline std::ostream& operator<<(std::ostream& str,const sca_util::sca_vector<T>& vec)
{
	const sca_util::sca_implementation::sca_matrix_base<T>* bvec=&vec;
	return operator<<(str,*bvec);
}

template<class T>
inline bool sca_vector<T>::operator== (const sca_util::sca_vector<T>& b) const
{
	return this->is_equal(b);
}

template<class T>
inline bool sca_vector<T>::operator!= (const sca_util::sca_vector<T>& b) const
{
	return !this->is_equal(b);
}


} // namespace sca_util

#endif /* SCA_VECTOR_H_ */
