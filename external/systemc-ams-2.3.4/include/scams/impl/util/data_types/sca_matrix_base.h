/*****************************************************************************

    Copyright 2010-2013
    Fraunhofer-Gesellschaft zur Foerderung der angewandten Forschung e.V.
    
    Copyright 2015-2023 COSEDA Technologies GmbH


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

  sca_matrix_base.h - description

  Original Author: Karsten Einwich Fraunhofer IIS/EAS Dresden

  Created on: 09.10.2009

   SVN Version       :  $Revision: 2320 $
   SVN last checkin  :  $Date: 2023-06-08 12:47:38 +0000 (Thu, 08 Jun 2023) $
   SVN checkin by    :  $Author: pehrlich $
   SVN Id            :  $Id: sca_matrix_base.h 2320 2023-06-08 12:47:38Z pehrlich $

 *****************************************************************************/

/*****************************************************************************/

#ifndef SCA_MATRIX_BASE_H_
#define SCA_MATRIX_BASE_H_

namespace sca_util
{
namespace sca_implementation
{

template<class T>
class sca_function_vector;

template<class T>
class sca_matrix_base;

template<class T>
std::ostream& operator<<  ( std::ostream& os,const sca_matrix_base<T>&);

template<class T>
class sca_matrix_base : public sca_matrix_base_typeless
{

	friend class sca_tdf::sca_implementation::sca_ct_ltf_nd_proxy;
	friend class sca_tdf::sca_implementation::sca_ct_ltf_zp_proxy;
	friend class sca_tdf::sca_implementation::sca_ct_vector_ss_proxy;

    mutable T*             dummy;


protected:


    std::valarray<T>    matrix;

protected:

    void reset();
    void remove();

    sca_matrix_base();                                  //default matrix
    sca_matrix_base(unsigned long x);                   //vector not resizable
    sca_matrix_base(unsigned long y, unsigned long x);  //matrix not resizable
    sca_matrix_base(const sca_matrix_base<T>& m);       //copied matrix


    sca_matrix_base(const std::initializer_list<T>&);
    sca_matrix_base(const std::initializer_list<std::initializer_list<T> >&);

    sca_matrix_base(const std::vector<T>&);     //constructor from std::vector for convenience

    virtual ~sca_matrix_base();

    sca_matrix_base<T>& operator= (const sca_matrix_base<T>& m);


    T* get_flat();


    T& operator[] (unsigned long x);
    const T& operator[] (unsigned long x) const;


    T& operator() (long y, long x);
    const T& operator() (long y, long x) const;
    T& operator() (long xi);  //vector
    const T& operator() (long xi) const;  //vector

    void resize(unsigned long xy);

    void resize(unsigned long ys, unsigned long xs);
    
    bool is_equal(const sca_matrix_base<T>&) const;
    

    friend std::ostream& operator<<  <T>( std::ostream& os,const sca_matrix_base<T>&);

   template<class T1,class T2>
   friend std::ostream& operator<<  ( std::ostream& os,const sca_matrix_base<std::pair<T1,T2> >&);


    friend class sca_core::sca_implementation::sca_linear_solver;
    friend class sca_core::sca_implementation::sca_conservative_cluster;
    friend class sca_util::sca_implementation::sca_function_vector<T>;
    friend class sca_eln::sca_implementation::sca_eln_view;
    friend class sca_lsf::sca_implementation::sca_lsf_view;
    friend class sca_lsf::sca_implementation::lin_eqs_cluster;
    friend class sca_eln::sca_implementation::lin_eqs_cluster;
    friend class sca_ac_analysis::sca_implementation::sca_ac_domain_entity;
    friend class sca_ac_analysis::sca_implementation::sca_ac_domain_eq;
    friend class sca_ac_analysis::sca_implementation::sca_ac_domain_solver;
    friend class sca_ac_analysis::sca_implementation::solve_linear_complex_eq_system;
};


////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////

template<class T>
inline void sca_matrix_base<T>::reset()
{
    matrix.resize(matrix.size());
}


template<class T>
inline void sca_matrix_base<T>::remove()
{
    if(matrix.size())
    {
        matrix.resize(0);
        for(unsigned long i=0;i<dim;i++)
        {
            sizes[i] =0;
        }
    }
}

////////

template<class T>
inline sca_matrix_base<T>::sca_matrix_base() : sca_matrix_base_typeless()      //default matrix
{
	dummy=NULL;
}


template<class T>
inline sca_matrix_base<T>::sca_matrix_base(unsigned long x):sca_matrix_base_typeless(x)     //vector not resizable
{
	dummy=NULL;
	resize(x);
}



template<class T>
inline sca_matrix_base<T>::sca_matrix_base(unsigned long y, unsigned long x) : sca_matrix_base_typeless(y,x)      //matrix not resizable
{
	dummy=NULL;
	resize(y,x);
}

template<class T>
inline sca_matrix_base<T>::~sca_matrix_base()
{
	if(dummy!=NULL) delete dummy;
}

template<class T>
inline sca_matrix_base<T>::sca_matrix_base(const sca_matrix_base<T>& m) :
                                    sca_matrix_base_typeless(m), matrix(m.matrix)     //copied matrix
{
	dummy=NULL;
}



template<class T>
inline sca_matrix_base<T>::sca_matrix_base(const std::vector<T>& vec):
                                         sca_matrix_base_typeless((unsigned long)vec.size())
{
	dummy=NULL;
	resize(vec.size());

	for(std::size_t i=0;i<vec.size();i++)
	{
		matrix[i]=vec[i];
	}
}



template<class T>
sca_matrix_base<T>::sca_matrix_base(const std::initializer_list<T>& lst) :
										sca_matrix_base_typeless(lst.size())
{
	dummy=NULL;
	resize(lst.size());

	auto v=begin(matrix);
	for(const auto& l : lst)
	{
	   *v++=l;
	}

}



template<class T>
sca_matrix_base<T>::sca_matrix_base(const std::initializer_list<std::initializer_list<T> >& lst)
{
	dummy=NULL;
	if(lst.size()>0)
	{
		resize(lst.size(),lst.begin()->size());
	}



	unsigned long i=0;
	for(const auto& l : lst)
	{
		unsigned long j=0;
		if(l.size()!=sizes[0])
		{
			SC_REPORT_ERROR("SystemC-AMS","Matrix initializer list with different number of colums");
			return;
		}

		for(const auto& ity: l)
		{
			matrix[i+j*sizes[1]]=ity;
			++j;
		}


		++i;
	}
}





template<class T>
inline sca_matrix_base<T>& sca_matrix_base<T>::operator= (const sca_matrix_base<T>& m)
{
    dim      = m.dim;
    sizes[0] = m.sizes[0];
    sizes[1] = m.sizes[1];
    square   = m.square;
    auto_dim = m.auto_dim;
    accessed = true;
    ignore_negative=m.ignore_negative;
    matrix.resize(m.matrix.size());
    matrix = m.matrix;
    return *this;
}



template<class T>
inline T* sca_matrix_base<T>::get_flat()
{
	return &matrix[0];
}




template<class T>
inline T& sca_matrix_base<T>::operator[] (unsigned long x)
{
	accessed=true;
	return matrix[x];
}

template<class T>
inline const T& sca_matrix_base<T>::operator[] (unsigned long x) const
{
    //we must use the operator T& valarray::operator[](size_t) instead
    // T valarray::operator[](size_t) const
    return( const_cast<std::valarray<T>*>(&matrix)->operator[] (x) );
}


template<class T>
inline T& sca_matrix_base<T>::operator() (long y, long x) //matrix
{
    bool flag=false;
    unsigned long xr, yr;

    accessed=true;

    if((x<0)||(y<0))
    {
        if(ignore_negative)
        {
			if(dummy!=NULL) delete dummy;
			dummy=new T();
            return(*dummy);
        }
        else
        {
            SC_REPORT_ERROR("SystemC-AMS", "Matrix access with negative indices is not allowed.\n");
        }
    }

    if(dim==0) dim=2;
    if((dim!=2)&&(y>0))  //exception handling has to be included
    {
        SC_REPORT_ERROR("SystemC-AMS", "Wrong matrix access (internal error).\n");
    }

    if((unsigned long)(x)>=sizes[0])
    {
        flag=1;
        xr  =x+1;
    }
    else
    {
        xr=sizes[0];
    }


    if((unsigned long)(y)>=sizes[1])
    {
        flag=1;
        yr  =y+1;
    }
    else
    {
        yr=sizes[1];
    }

    if(square)
    {
        if(yr>xr) xr=yr;
        else yr=xr;
    }

    if(flag)
    {
        if(auto_dim)
        {
        	resize(yr,xr);
            //cout << "resize matrix " << xr << " " << yr << endl;

        }
        else  //exception handling has to be included  !!!!!!!
        {
            SC_REPORT_ERROR("SystemC-AMS", "Not allowed matrix access (index is out of bounds).\n");
        }
    }

    return( matrix[x*sizes[1]+y] );
}




template<class T>
inline const T& sca_matrix_base<T>::operator() (long y, long x) const //matrix
{
    bool flag=false;
    unsigned long xr, yr;

    if((x<0)||(y<0))
    {
        if(ignore_negative)
        {
			if(dummy==NULL) dummy=new T();
			else *dummy=T();

            return(*dummy);
        }
        else
        {
            SC_REPORT_ERROR("SystemC-AMS", "Matrix access with negative indices is not allowed.\n");
        }
    }

    if(dim==0) *const_cast<unsigned long*>(&dim)=2;
    if((dim!=2)&&(y>0))  //exception handling has to be included
    {
        SC_REPORT_ERROR("SystemC-AMS", "Wrong matrix access (internal error).\n");
    }

    if((unsigned long)(x)>=sizes[0])
    {
        flag=1;
        xr  =x+1;
    }
    else
    {
        xr=sizes[0];
    }


    if((unsigned long)(y)>=sizes[1])
    {
        flag=1;
        yr  =y+1;
    }
    else
    {
        yr=sizes[1];
    }

    if(square)
    {
        if(yr>xr) xr=yr;
        else yr=xr;
    }

    if(flag)
    {
        if(auto_dim)
        {
            const_cast<sca_matrix_base<T>*>(this)->resize(yr,xr);

        }
        else  //exception handling has to be included  !!!!!!!
        {
            SC_REPORT_ERROR("SystemC-AMS", "Not allowed matrix access (index is out of bounds).\n");
        }
    }

    //we must use the operator T& valarray::operator[](size_t) instead
    // T valarray::operator[](size_t) const
    return( const_cast<std::valarray<T>&>(matrix)[x*sizes[1]+y] );
}




template<class T>
inline T& sca_matrix_base<T>::operator() (long xi)  //vector
{
    accessed=true;

    if(xi<0)
    {
        if(ignore_negative)
        {
			if(dummy==NULL) dummy=new T();
			else           *dummy=T();

            return(*dummy);
        }
        else
        {
            SC_REPORT_ERROR("SystemC-AMS", "Matrix access with negative indices is not allowed.\n");
        }
    }

    unsigned long x=(unsigned long) xi;
    unsigned long xr;

    if(dim==0) dim=1;


    if(dim!=1) //exception handling has to be included
    {
        SC_REPORT_ERROR("SystemC-AMS", "Wrong vector access (internal error).\n");
    }

    if(x>=sizes[0])
    {
        if(auto_dim)
        {
            if(auto_sizable)
                for(xr=(sizes[0])? sizes[0]:(x+1); xr<=x; xr*=2);
            else xr=x+1;

            resize(1,xr);

        }
        else  //exception handling has to be included  !!!!!!!
        {
            SC_REPORT_ERROR("SystemC-AMS", "Not allowed vector access (index is out of bounds).\n");
        }
    }

    if(xi>last_val) last_val=xi;

    return( matrix[x] );
}


template<class T>
inline const T& sca_matrix_base<T>::operator() (long xi)  const //vector
{

    if(xi<0)
    {
        if(ignore_negative)
        {
        	if(dummy==NULL) dummy=new T();
        	else           *dummy=T();

            return(*dummy);
        }
        else
        {
            SC_REPORT_ERROR("SystemC-AMS", "Matrix access with negative indices is not allowed.\n");
        }
    }

    unsigned long x=(unsigned long) xi;
    unsigned long xr;

    if(dim==0) *(const_cast<unsigned long*>(&dim))=1;


    if(dim!=1) //exception handling has to be included
    {
        SC_REPORT_ERROR("SystemC-AMS", "Wrong vector access (internal error).\n");
    }

    if(x>=sizes[0])
    {
        if(auto_dim)
        {
            if(auto_sizable)
                for(xr=(sizes[0])? sizes[0]:(x+1); xr<=x; xr*=2);
            else xr=x+1;

            const_cast<sca_matrix_base<T>*>(this)->resize(1,xr);

        }
        else  //exception handling has to be included  !!!!!!!
        {
            SC_REPORT_ERROR("SystemC-AMS", "Not allowed vector access (index is out of bounds).\n");
        }
    }

    if(xi>last_val) *const_cast<long*>(&last_val)=xi;

    return( const_cast<std::valarray<T>&>(matrix)[x] );
}




template<class T>
inline void sca_matrix_base<T>::resize(unsigned long xy)
{
    if(square) resize(xy,xy);
    else
    {
        resize(1,xy);
        if((long)xy<=last_val) last_val=xy-1;
    }
}



template<class T>
inline void sca_matrix_base<T>::resize(unsigned long ys, unsigned long xs)
{

    if(!matrix.size())
    {
        matrix.resize((size_t)(xs*ys));
    }

    else
    {
        std::valarray<size_t> blocklengths(2), subsizes(2);

        blocklengths[0]=(size_t)sizes[1];
        blocklengths[1]=1;
        subsizes[0]=(size_t)((sizes[0]>xs)? xs : sizes[0]);
        subsizes[1]=(size_t)((sizes[1]>ys)? ys : sizes[1]);

        std::gslice dim_b(0, subsizes, blocklengths);

        //this is not working for gcc 2.96 on Red Hat
        //valarray<T>     backup (matrix[dim_b]);
        std::valarray<T>     backup (subsizes[0]*subsizes[1]);
        backup=matrix[dim_b];

        matrix.resize((size_t)(xs*ys));

        blocklengths[0]=(size_t)ys;
        std::gslice dim_c(0, subsizes, blocklengths);
        matrix[dim_c] = backup;
    }

    sizes[0]=xs;
    sizes[1]=ys;

    accessed=true;
}


template<class T>
bool sca_matrix_base<T>::is_equal(const sca_matrix_base<T>& matrixb) const
{
    for(std::size_t idx=0;idx<this->matrix.size();++idx)
    {
        if(this->matrix[idx]!=matrixb.matrix[idx]) return false;
    }
        
    return true;
}



template<class T>
#if defined( _MSC_VER ) //Visual does not allow inline for friend functions
std::ostream& operator<< ( std::ostream& os,const sca_matrix_base<T>& matrix)
#else
inline std::ostream& operator<< ( std::ostream& os,const sca_matrix_base<T>& matrix)
#endif
{
    for(unsigned long iy=0;iy<matrix.dimy();iy++)
    {
        for(unsigned long ix=0;ix<matrix.dimx();ix++)
        {
            os << std::setw(20) << matrix(iy,ix) << " ";
        }
        os << std::endl;
    }
    return(os);
}

template<class T1,class T2>
std::ostream& operator<<  ( std::ostream& os,const sca_matrix_base<std::pair<T1,T2> >& matrix)
{
    for(unsigned long iy=0;iy<matrix.dimy();iy++)
    {
        for(unsigned long ix=0;ix<matrix.dimx();ix++)
        {
            os << std::setw(20) << "( "<< matrix(iy,ix).first << " , " <<  matrix(iy,ix).second << " )  ";
        }
        os << std::endl;
    }
    return(os);

}



}
}


#endif /* SCA_MATRIX_BASE_H_ */
