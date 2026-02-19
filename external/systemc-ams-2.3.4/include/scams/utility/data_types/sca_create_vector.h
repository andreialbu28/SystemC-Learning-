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

 sca_create_vector.h - utility function for creating vector constants

 Original Author: Karsten Einwich Fraunhofer IIS/EAS Dresden

 Created on: 08.03.2009

 SVN Version       :  $Revision: 1914 $
 SVN last checkin  :  $Date: 2016-02-23 18:06:06 +0000 (Tue, 23 Feb 2016) $
 SVN checkin by    :  $Author: karsten $
 SVN Id            :  $Id: sca_create_vector.h 1914 2016-02-23 18:06:06Z karsten $

 *****************************************************************************/
/*
 * The functions sca_util::sca_create_vector shall create an object of class
 * sca_util::sca_vector of the size according to the number of arguments. The
 * vector elements shall be initialized by the values of the arguments. The
 * first argument shall initialize the first vector element, the second argument
 * shall initialize the second vector element, and so on till the last argument.
 * An implementation shall provide at least functions to initialize vectors
 * of 1 up to 16 arguments.
 */

/*****************************************************************************/

#ifndef SCA_CREATE_VECTOR_H_
#define SCA_CREATE_VECTOR_H_

namespace sca_util
{

template<class T>
sca_util::sca_vector<T> sca_create_vector (const T& a0);

template<class T>
sca_util::sca_vector<T> sca_create_vector (const T& a0, const T& a1);

template<class T>
sca_util::sca_vector<T> sca_create_vector (const T& a0, const T& a1,
		const T& a2);

template<class T>
sca_util::sca_vector<T> sca_create_vector (const T& a0, const T& a1,
		const T& a2, const T& a3);

template<class T>
sca_util::sca_vector<T> sca_create_vector (const T& a0, const T& a1,
		const T& a2, const T& a3, const T& a4);

template<class T>
sca_util::sca_vector<T> sca_create_vector (const T& a0, const T& a1,
		const T& a2, const T& a3, const T& a4, const T& a5);

template<class T>
sca_util::sca_vector<T> sca_create_vector (const T& a0, const T& a1,
		const T& a2, const T& a3, const T& a4, const T& a5, const T& a6);

template<class T>
sca_util::sca_vector<T> sca_create_vector (const T& a0, const T& a1,
		const T& a2, const T& a3, const T& a4, const T& a5, const T& a6,
		const T& a7);

template<class T>
sca_util::sca_vector<T> sca_create_vector (const T& a0, const T& a1,
		const T& a2, const T& a3, const T& a4, const T& a5, const T& a6,
		const T& a7, const T& a8);

template<class T>
sca_util::sca_vector<T> sca_create_vector (const T& a0, const T& a1,
		const T& a2, const T& a3, const T& a4, const T& a5, const T& a6,
		const T& a7, const T& a8, const T& a9);

template<class T>
sca_util::sca_vector<T> sca_create_vector (const T& a0, const T& a1,
		const T& a2, const T& a3, const T& a4, const T& a5, const T& a6,
		const T& a7, const T& a8, const T& a9, const T& a10);

template<class T>
sca_util::sca_vector<T> sca_create_vector (const T& a0, const T& a1,
		const T& a2, const T& a3, const T& a4, const T& a5, const T& a6,
		const T& a7, const T& a8, const T& a9, const T& a10, const T& a11);

template<class T>
sca_util::sca_vector<T> sca_create_vector (const T& a0, const T& a1,
		const T& a2, const T& a3, const T& a4, const T& a5, const T& a6,
		const T& a7, const T& a8, const T& a9, const T& a10, const T& a11,
		const T& a12);

template<class T>
sca_util::sca_vector<T> sca_create_vector (const T& a0, const T& a1,
		const T& a2, const T& a3, const T& a4, const T& a5, const T& a6,
		const T& a7, const T& a8, const T& a9, const T& a10, const T& a11,
		const T& a12, const T& a13);

template<class T>
sca_util::sca_vector<T> sca_create_vector (const T& a0, const T& a1,
		const T& a2, const T& a3, const T& a4, const T& a5, const T& a6,
		const T& a7, const T& a8, const T& a9, const T& a10, const T& a11,
		const T& a12, const T& a13, const T& a14);

template<class T>
sca_util::sca_vector<T> sca_create_vector (const T& a0, const T& a1,
		const T& a2, const T& a3, const T& a4, const T& a5, const T& a6,
		const T& a7, const T& a8, const T& a9, const T& a10, const T& a11,
		const T& a12, const T& a13, const T& a14, const T& a15);



// begin implementation specific

template<class T>
inline sca_util::sca_vector<T> sca_create_vector (const T& a0)
{
	sca_util::sca_vector<T> tmp(1);
	tmp(0)=a0;

	return tmp;
}

////////

template<class T>
inline sca_util::sca_vector<T> sca_create_vector (const T& a0, const T& a1)
{
	sca_util::sca_vector<T> tmp(2);
	tmp(0)=a0;
	tmp(1)=a1;

	return tmp;
}

////////

template<class T>
inline sca_util::sca_vector<T> sca_create_vector (const T& a0, const T& a1,
		const T& a2)
{
	sca_util::sca_vector<T> tmp(3);
	tmp(0)=a0;
	tmp(1)=a1;
	tmp(2)=a2;

	return tmp;
}

////////

template<class T>
sca_util::sca_vector<T> sca_create_vector (const T& a0, const T& a1,
		const T& a2, const T& a3)
{
	sca_util::sca_vector<T> tmp(4);
	tmp(0)=a0; tmp(1)=a1; tmp(2)=a2; tmp(3)=a3;

	return tmp;
}

/////////

template<class T>
inline sca_util::sca_vector<T> sca_create_vector (const T& a0, const T& a1,
		const T& a2, const T& a3, const T& a4)
{
	sca_util::sca_vector<T> tmp(5);
	tmp(0)=a0; tmp(1)=a1; tmp(2)=a2; tmp(3)=a3; tmp(4)=a4;

	return tmp;
}

////////

template<class T>
inline sca_util::sca_vector<T> sca_create_vector (const T& a0, const T& a1,
		const T& a2, const T& a3, const T& a4, const T& a5)
{
	sca_util::sca_vector<T> tmp(6);
	tmp(0)=a0; tmp(1)=a1; tmp(2)=a2; tmp(3)=a3; tmp(4)=a4;
	tmp(5)=a5;

	return tmp;
}


///////

template<class T>
inline sca_util::sca_vector<T> sca_create_vector (const T& a0, const T& a1,
		const T& a2, const T& a3, const T& a4, const T& a5, const T& a6)
{
	sca_util::sca_vector<T> tmp(7);
	tmp(0)=a0; tmp(1)=a1; tmp(2)=a2; tmp(3)=a3; tmp(4)=a4;
	tmp(5)=a5; tmp(6)=a6;

	return tmp;
}

/////////////

template<class T>
inline sca_util::sca_vector<T> sca_create_vector (const T& a0, const T& a1,
		const T& a2, const T& a3, const T& a4, const T& a5, const T& a6,
		const T& a7)
{
	sca_util::sca_vector<T> tmp(8);
	tmp(0)=a0; tmp(1)=a1; tmp(2)=a2; tmp(3)=a3; tmp(4)=a4;
	tmp(5)=a5; tmp(6)=a6; tmp(7)=a7;

	return tmp;
}

////////////

template<class T>
inline sca_util::sca_vector<T> sca_create_vector (const T& a0, const T& a1,
		const T& a2, const T& a3, const T& a4, const T& a5, const T& a6,
		const T& a7, const T& a8)
{
	sca_util::sca_vector<T> tmp(9);
	tmp(0)=a0; tmp(1)=a1; tmp(2)=a2; tmp(3)=a3; tmp(4)=a4;
	tmp(5)=a5; tmp(6)=a6; tmp(7)=a7; tmp(8)=a8;

	return tmp;
}

//////////

template<class T>
inline sca_util::sca_vector<T> sca_create_vector (const T& a0, const T& a1,
		const T& a2, const T& a3, const T& a4, const T& a5, const T& a6,
		const T& a7, const T& a8, const T& a9)
{
	sca_util::sca_vector<T> tmp(10);
	tmp(0)=a0; tmp(1)=a1; tmp(2)=a2; tmp(3)=a3; tmp(4)=a4;
	tmp(5)=a5; tmp(6)=a6; tmp(7)=a7; tmp(8)=a8; tmp(9)=a9;

	return tmp;
}

///////////


template<class T>
inline sca_util::sca_vector<T> sca_create_vector (const T& a0, const T& a1,
		const T& a2, const T& a3, const T& a4, const T& a5, const T& a6,
		const T& a7, const T& a8, const T& a9, const T& a10)
{
	sca_util::sca_vector<T> tmp(11);
	tmp(0)=a0; tmp(1)=a1; tmp(2)=a2; tmp(3)=a3; tmp(4)=a4;
	tmp(5)=a5; tmp(6)=a6; tmp(7)=a7; tmp(8)=a8; tmp(9)=a9;
	tmp(10)=a10;

	return tmp;
}

//////////

template<class T>
inline sca_util::sca_vector<T> sca_create_vector (const T& a0, const T& a1,
		const T& a2, const T& a3, const T& a4, const T& a5, const T& a6,
		const T& a7, const T& a8, const T& a9, const T& a10, const T& a11)
{
	sca_util::sca_vector<T> tmp(12);
	tmp(0)=a0; tmp(1)=a1; tmp(2)=a2; tmp(3)=a3; tmp(4)=a4;
	tmp(5)=a5; tmp(6)=a6; tmp(7)=a7; tmp(8)=a8; tmp(9)=a9;
	tmp(10)=a10; tmp(11)=a11;

	return tmp;
}

///////////

template<class T>
inline sca_util::sca_vector<T> sca_create_vector (const T& a0, const T& a1,
		const T& a2, const T& a3, const T& a4, const T& a5, const T& a6,
		const T& a7, const T& a8, const T& a9, const T& a10, const T& a11,
		const T& a12)
{
	sca_util::sca_vector<T> tmp(13);
	tmp(0)=a0; tmp(1)=a1; tmp(2)=a2; tmp(3)=a3; tmp(4)=a4;
	tmp(5)=a5; tmp(6)=a6; tmp(7)=a7; tmp(8)=a8; tmp(9)=a9;
	tmp(10)=a10; tmp(11)=a11; tmp(12)=a12;

	return tmp;
}


/////////

template<class T>
inline sca_util::sca_vector<T> sca_create_vector (const T& a0, const T& a1,
		const T& a2, const T& a3, const T& a4, const T& a5, const T& a6,
		const T& a7, const T& a8, const T& a9, const T& a10, const T& a11,
		const T& a12, const T& a13)
{
	sca_util::sca_vector<T> tmp(14);
	tmp(0)=a0; tmp(1)=a1; tmp(2)=a2; tmp(3)=a3; tmp(4)=a4;
	tmp(5)=a5; tmp(6)=a6; tmp(7)=a7; tmp(8)=a8; tmp(9)=a9;
	tmp(10)=a10; tmp(11)=a11; tmp(12)=a12; tmp(13)=a13;

	return tmp;
}

////////

template<class T>
inline sca_util::sca_vector<T> sca_create_vector (const T& a0, const T& a1,
		const T& a2, const T& a3, const T& a4, const T& a5, const T& a6,
		const T& a7, const T& a8, const T& a9, const T& a10, const T& a11,
		const T& a12, const T& a13, const T& a14)
{
	sca_util::sca_vector<T> tmp(15);
	tmp(0)=a0; tmp(1)=a1; tmp(2)=a2; tmp(3)=a3; tmp(4)=a4;
	tmp(5)=a5; tmp(6)=a6; tmp(7)=a7; tmp(8)=a8; tmp(9)=a9;
	tmp(10)=a10; tmp(11)=a11; tmp(12)=a12; tmp(13)=a13; tmp(14)=a14;

	return tmp;
}

////////

template<class T>
inline sca_util::sca_vector<T> sca_create_vector (const T& a0, const T& a1,
		const T& a2, const T& a3, const T& a4, const T& a5, const T& a6,
		const T& a7, const T& a8, const T& a9, const T& a10, const T& a11,
		const T& a12, const T& a13, const T& a14, const T& a15)
{
	sca_util::sca_vector<T> tmp(16);
	tmp(0)=a0; tmp(1)=a1; tmp(2)=a2; tmp(3)=a3; tmp(4)=a4;
	tmp(5)=a5; tmp(6)=a6; tmp(7)=a7; tmp(8)=a8; tmp(9)=a9;
	tmp(10)=a10; tmp(11)=a11; tmp(12)=a12; tmp(13)=a13; tmp(14)=a14; tmp(15)=a15;

	return tmp;

}


//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
/// convienient functions for pre C++11


template<class T>
sca_util::sca_vector<std::pair<T,T> > sca_create_pair_vector (const T& a0, const T& a1);



template<class T>
sca_util::sca_vector<std::pair<T,T> > sca_create_pair_vector (const T& a0, const T& a1,
		const T& a2, const T& a3);



template<class T>
sca_util::sca_vector<std::pair<T,T> > sca_create_pair_vector (const T& a0, const T& a1,
		const T& a2, const T& a3, const T& a4, const T& a5);



template<class T>
sca_util::sca_vector<std::pair<T,T> > sca_create_pair_vector (const T& a0, const T& a1,
		const T& a2, const T& a3, const T& a4, const T& a5, const T& a6,
		const T& a7);



template<class T>
sca_util::sca_vector<std::pair<T,T> > sca_create_pair_vector (const T& a0, const T& a1,
		const T& a2, const T& a3, const T& a4, const T& a5, const T& a6,
		const T& a7, const T& a8, const T& a9);




template<class T>
sca_util::sca_vector<std::pair<T,T> > sca_create_pair_vector (const T& a0, const T& a1,
		const T& a2, const T& a3, const T& a4, const T& a5, const T& a6,
		const T& a7, const T& a8, const T& a9, const T& a10, const T& a11);



template<class T>
sca_util::sca_vector<std::pair<T,T> > sca_create_pair_vector (const T& a0, const T& a1,
		const T& a2, const T& a3, const T& a4, const T& a5, const T& a6,
		const T& a7, const T& a8, const T& a9, const T& a10, const T& a11,
		const T& a12, const T& a13);


template<class T>
sca_util::sca_vector<std::pair<T,T> > sca_create_pair_vector (const T& a0, const T& a1,
		const T& a2, const T& a3, const T& a4, const T& a5, const T& a6,
		const T& a7, const T& a8, const T& a9, const T& a10, const T& a11,
		const T& a12, const T& a13, const T& a14, const T& a15);

//////////////////////////////////////////////////////////////////////////////

template<class T>
inline sca_util::sca_vector<std::pair<T,T> > sca_create_pair_vector (const T& a0, const T& a1)
{
	sca_util::sca_vector<std::pair<T,T> > tmp(1);
	tmp.set_auto_resizable();
	tmp(0)=std::pair<T,T>(a0,a1);
	return tmp;
}



template<class T>
inline sca_util::sca_vector<std::pair<T,T> > sca_create_pair_vector (const T& a0, const T& a1,
		const T& a2, const T& a3)
{
	sca_util::sca_vector<std::pair<T,T> > tmp(sca_create_pair_vector(a0,a1));
	tmp(1)=std::pair<T,T>(a2,a3);
	return tmp;
}

template<class T>
inline sca_util::sca_vector<std::pair<T,T> > sca_create_pair_vector (const T& a0, const T& a1,
		const T& a2, const T& a3, const T& a4, const T& a5)
{
	sca_util::sca_vector<std::pair<T,T> > tmp(sca_create_pair_vector(a0,a1,a2,a3));
	tmp(2)=std::pair<T,T>(a4,a5);
	return tmp;
}



template<class T>
inline sca_util::sca_vector<std::pair<T,T> > sca_create_pair_vector (const T& a0, const T& a1,
		const T& a2, const T& a3, const T& a4, const T& a5, const T& a6,
		const T& a7)
{
	sca_util::sca_vector<std::pair<T,T> > tmp(sca_create_pair_vector(a0,a1,a2,a3,a4,a5));
	tmp(3)=std::pair<T,T>(a6,a7);
	return tmp;
}



template<class T>
inline sca_util::sca_vector<std::pair<T,T> > sca_create_pair_vector (const T& a0, const T& a1,
		const T& a2, const T& a3, const T& a4, const T& a5, const T& a6,
		const T& a7, const T& a8, const T& a9)
{
	sca_util::sca_vector<std::pair<T,T> > tmp(sca_create_pair_vector(a0,a1,a2,a3,a4,a5,a6,a7));
	tmp(4)=std::pair<T,T>(a8,a9);
	return tmp;
}


template<class T>
inline sca_util::sca_vector<std::pair<T,T> > sca_create_pair_vector (const T& a0, const T& a1,
		const T& a2, const T& a3, const T& a4, const T& a5, const T& a6,
		const T& a7, const T& a8, const T& a9, const T& a10, const T& a11)
{
	sca_util::sca_vector<std::pair<T,T> > tmp(sca_create_pair_vector(a0,a1,a2,a3,a4,a5,a6,a7,a8,a9));
	tmp(5)=std::pair<T,T>(a10,a11);
	return tmp;
}




template<class T>
inline sca_util::sca_vector<std::pair<T,T> > sca_create_pair_vector (const T& a0, const T& a1,
		const T& a2, const T& a3, const T& a4, const T& a5, const T& a6,
		const T& a7, const T& a8, const T& a9, const T& a10, const T& a11,
		const T& a12, const T& a13)
{
	sca_util::sca_vector<std::pair<T,T> > tmp(sca_create_pair_vector(a0,a1,a2,a3,a4,a5,a6,a7,a8,a9,a10,a11));
	tmp(6)=std::pair<T,T>(a12,a13);
	return tmp;
}


template<class T>
inline sca_util::sca_vector<std::pair<T,T> > sca_create_pair_vector (const T& a0, const T& a1,
		const T& a2, const T& a3, const T& a4, const T& a5, const T& a6,
		const T& a7, const T& a8, const T& a9, const T& a10, const T& a11,
		const T& a12, const T& a13, const T& a14, const T& a15)
{
	sca_util::sca_vector<std::pair<T,T> > tmp(sca_create_pair_vector(a0,a1,a2,a3,a4,a5,a6,a7,a8,a9,a10,a11,a12,a13));
	tmp(7)=std::pair<T,T>(a14,a15);
	return tmp;
}

// end implementation specific

} //namespace sca_util
#endif /* SCA_CREATE_VECTOR_H_ */
