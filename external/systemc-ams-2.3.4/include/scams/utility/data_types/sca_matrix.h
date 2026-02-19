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

 sca_matrix.h - data type for handling matrices

 Original Author: Karsten Einwich Fraunhofer IIS/EAS Dresden

 Created on: 08.03.2009

 SVN Version       :  $Revision: 2320 $
 SVN last checkin  :  $Date: 2023-06-08 12:47:38 +0000 (Thu, 08 Jun 2023) $
 SVN checkin by    :  $Author: pehrlich $
 SVN Id            :  $Id: sca_matrix.h 2320 2023-06-08 12:47:38Z pehrlich $

 *****************************************************************************/
/*
 * LRM clause 6.2.1.2.
 * The class sca_util::sca_matrix shall provide a type for handling two
 * dimensional matrices of an arbitrary type.
 */

/*****************************************************************************/

#ifndef SCA_MATRIX_H_
#define SCA_MATRIX_H_

namespace sca_util
{

template<class T>
class sca_matrix: public sca_util::sca_implementation::sca_matrix_base<T>
{
public:
	sca_matrix();
	sca_matrix(unsigned long n_rows_, unsigned long n_cols_);
	sca_matrix(const sca_util::sca_matrix<T>& matrix);
	sca_matrix(const std::initializer_list<std::initializer_list<T> >&);


	void resize(unsigned long n_rows_, unsigned long n_cols_);
	void set_auto_resizable();
	void unset_auto_resizable();
	bool is_auto_resizable() const;
	unsigned long n_rows() const;
	unsigned long n_cols() const;

	T& operator()(unsigned long row, unsigned long col);
	const T& operator()(unsigned long row, unsigned long col) const;

	const sca_util::sca_matrix<T>& operator=(const sca_util::sca_matrix<T>& matrix);
	const sca_util::sca_matrix<T>& operator=(
			sca_core::sca_assign_from_proxy<sca_util::sca_matrix<T> >&);

	bool operator== (const sca_util::sca_matrix<T>&) const;
	bool operator!= (const sca_util::sca_matrix<T>&) const;

	const std::string to_string() const;
	void print(std::ostream& = std::cout) const;

	//begin implementation specific





private:

	typedef sca_util::sca_implementation::sca_matrix_base<T> base_type;

	//end implementation specific
};

template<class T>
std::ostream& operator<<(std::ostream&, const sca_util::sca_matrix<T>&);


//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////

template<class T>
inline sca_matrix<T>::sca_matrix() : sca_util::sca_implementation::sca_matrix_base<T>()
{
}

template<class T>
inline sca_matrix<T>::sca_matrix(unsigned long n_rows_, unsigned long n_cols_):
	sca_util::sca_implementation::sca_matrix_base<T>(n_rows_,n_cols_)
{
}


template<class T>
inline sca_matrix<T>::sca_matrix(const sca_util::sca_matrix<T>& matrix_) :
	sca_util::sca_implementation::sca_matrix_base<T>(matrix_)
{
}



template<class T>
inline sca_matrix<T>::sca_matrix(const std::initializer_list<std::initializer_list<T> >& lst):
		sca_util::sca_implementation::sca_matrix_base<T>(lst)
{
}



template<class T>
inline void sca_matrix<T>::resize(unsigned long n_rows_, unsigned long n_cols_)
{
	base_type::resize(n_rows_,n_cols_);
}



template<class T>
inline void sca_matrix<T>::set_auto_resizable()
{
	base_type::set_autodim();
}


template<class T>
inline void sca_matrix<T>::unset_auto_resizable()
{
	base_type::reset_autodim();
}


template<class T>
inline bool sca_matrix<T>::is_auto_resizable() const
{
	return base_type::get_autodim();
}

template<class T>
inline unsigned long sca_matrix<T>::n_rows() const
{
	return base_type::dimy();
}

template<class T>
inline unsigned long sca_matrix<T>::n_cols() const
{
	return base_type::dimx();
}

template<class T>
inline T& sca_matrix<T>::operator()(unsigned long row, unsigned long col)
{
	return base_type::operator()(row,col);
}

template<class T>
inline const T& sca_matrix<T>::operator()(unsigned long row, unsigned long col) const
{
	return base_type::operator()(row,col);
}

template<class T>
inline const sca_util::sca_matrix<T>& sca_matrix<T>::operator=(const sca_util::sca_matrix<T>& matrix_)
{
	base_type::operator=(matrix_);
	return *this;
}


template<class T>
inline const sca_util::sca_matrix<T>& sca_matrix<T>::operator=(sca_core::sca_assign_from_proxy<
		sca_util::sca_matrix<T> >& mtrx_proxy)
{
	mtrx_proxy.assign_to(*this);
	return *this;
}


template<class T>
inline bool sca_matrix<T>::operator== (const sca_util::sca_matrix<T>& b) const
{
	return this->is_equal(b);
}

template<class T>
inline bool sca_matrix<T>::operator!= (const sca_util::sca_matrix<T>& b) const
{
	return !this->is_equal(b);
}


template<class T>
inline const std::string sca_matrix<T>::to_string() const
{
	std::ostringstream str;
	str << *this;
	return str.str();
}

template<class T>
inline void sca_matrix<T>::print(std::ostream& str) const
{
	str << *this;
}


template<class T>
inline std::ostream& operator<<(std::ostream& str, const sca_util::sca_matrix<T>& matrix)
{
	const sca_util::sca_implementation::sca_matrix_base<T>* bvec=&matrix;
	return operator<<(str,*bvec);
}


} // namespace sca_util

#endif /* SCA_MATRIX_H_ */
