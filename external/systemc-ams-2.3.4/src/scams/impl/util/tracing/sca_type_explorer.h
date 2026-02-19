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

  sca_type_explorer.h - description

  Original Author: Karsten Einwich Fraunhofer IIS/EAS Dresden

  Created on: Jan 24, 2010

   SVN Version       :  $Revision: 2110 $
   SVN last checkin  :  $Date: 2020-03-04 16:36:49 +0000 (Wed, 04 Mar 2020) $
   SVN checkin by    :  $Author: karsten $
   SVN Id            :  $Id: sca_type_explorer.h 2110 2020-03-04 16:36:49Z karsten $

 *****************************************************************************/

/*****************************************************************************/


#ifndef SCA_TYPE_EXPLORER_H_
#define SCA_TYPE_EXPLORER_H_


namespace sca_util
{

namespace sca_implementation
{

class sca_trace_object_data;

class sca_type_explorer_base
{
public:
	enum types
	{
		UNKNOWN,
		BOOL,
		INT,
		LONG,
		SHORT,
		UINT,
		ULONG,
		USHORT,
		INT64,
		UINT64,
		FLOAT,
		DOUBLE,
		CHAR,
		UCHAR,
		STRING,

		SC_INT,
		SC_INT_BASE,
		SC_UINT,
		SC_UINT_BASE,
		SC_LOGIC,
		SC_BV,
		SC_BV_BASE,
		SC_LV,
		SC_LV_BASE,
		SC_BIGINT,
		SC_SIGNED,
		SC_BIGUINT,
		SC_UNSIGNED,

		SC_FIXED,
		SC_FIX,
		SC_UFIXED,
		SC_UFIX,
		SC_FIXED_FAST,
		SC_FIX_FAST,
		SC_UFIXED_FAST,
		SC_UFIX_FAST,

		SCA_COMPLEX

	};

	const enum types type_id;
	const void* data;
	const unsigned long length;

	virtual void manipulate_trace_data(sca_trace_object_data*){}

	virtual ~sca_type_explorer_base(){}

protected:

	sca_type_explorer_base(enum types type_id_,unsigned long length_):
		type_id(type_id_),data(NULL),length(length_)
	{
	}

};

/////////////////////

template<typename T>
class sca_type_explorer : public sca_type_explorer_base
{
public:
	sca_type_explorer(): sca_type_explorer_base(UNKNOWN,0)
	{
	}
};

////

template<>
class sca_type_explorer<bool> : public sca_type_explorer_base
{
public:
	sca_type_explorer():
		sca_type_explorer_base(BOOL,1){}
};


////

template<>
class sca_type_explorer<double> : public sca_type_explorer_base
{
public:
	sca_type_explorer():
		sca_type_explorer_base(DOUBLE,1){}
};


////

template<>
class sca_type_explorer<sca_util::sca_complex> : public sca_type_explorer_base
{
public:
	sca_type_explorer():
		sca_type_explorer_base(SCA_COMPLEX,2){}
};


////

template<>
class sca_type_explorer<float> : public sca_type_explorer_base
{
public:
	sca_type_explorer():
		sca_type_explorer_base(FLOAT,1){}
};


////

template<>
class sca_type_explorer<sc_dt::int64> : public sca_type_explorer_base
{
public:
	sca_type_explorer():
		sca_type_explorer_base(INT64,8*sizeof(sc_dt::int64)){}
};


////

template<>
class sca_type_explorer<sc_dt::uint64> : public sca_type_explorer_base
{
public:
	sca_type_explorer():
		sca_type_explorer_base(UINT64,8*sizeof(sc_dt::uint64)){}
};



////

template<>
class sca_type_explorer<long> : public sca_type_explorer_base
{
public:
	sca_type_explorer():
		sca_type_explorer_base(LONG,8*sizeof(long)){}
};


////

template<>
class sca_type_explorer<unsigned long> : public sca_type_explorer_base
{
public:
	sca_type_explorer():
		sca_type_explorer_base(ULONG,8*sizeof(unsigned long)){}
};


////

template<>
class sca_type_explorer<short> : public sca_type_explorer_base
{
public:
	sca_type_explorer():
		sca_type_explorer_base(SHORT,8*sizeof(short)){}
};

////

template<>
class sca_type_explorer<unsigned short> : public sca_type_explorer_base
{
public:
	sca_type_explorer():
		sca_type_explorer_base(USHORT,8*sizeof(unsigned short)){}
};



////

template<>
class sca_type_explorer<int> : public sca_type_explorer_base
{
public:
	sca_type_explorer():
		sca_type_explorer_base(INT,8*sizeof(int)){}
};


////

template<>
class sca_type_explorer<unsigned int> : public sca_type_explorer_base
{
public:
	sca_type_explorer():
		sca_type_explorer_base(UINT,8*sizeof(unsigned int)){}
};


////

template<>
class sca_type_explorer<char> : public sca_type_explorer_base
{
public:
	sca_type_explorer():
		sca_type_explorer_base(CHAR,8*sizeof(char)){}
};

template<>
class sca_type_explorer<std::string> : public sca_type_explorer_base
{
public:
	sca_type_explorer():
		sca_type_explorer_base(STRING,1){}
};


////

template<>
class sca_type_explorer<unsigned char> : public sca_type_explorer_base
{
public:
	sca_type_explorer():
		sca_type_explorer_base(UCHAR,8*sizeof(unsigned char)){}
};


////

template<>
class sca_type_explorer<sc_dt::sc_logic> : public sca_type_explorer_base
{
public:
	sca_type_explorer():
		sca_type_explorer_base(SC_LOGIC,1){}
};




////

template<int width>
class sca_type_explorer<sc_dt::sc_int<width> > : public sca_type_explorer_base
{
public:
	sca_type_explorer():
		sca_type_explorer_base(SC_INT,width)
		{}
};

////

template<>
class sca_type_explorer<sc_dt::sc_int_base > : public sca_type_explorer_base
{
public:
	sca_type_explorer():
		sca_type_explorer_base(SC_INT_BASE,sc_dt::sc_length_param().len())
		{}
};



////

template<int width>
class sca_type_explorer<sc_dt::sc_uint<width> > : public sca_type_explorer_base
{
public:
	sca_type_explorer():
		sca_type_explorer_base(SC_UINT,width)
		{}
};


////

template<>
class sca_type_explorer<sc_dt::sc_uint_base> : public sca_type_explorer_base
{
public:
	sca_type_explorer():
		sca_type_explorer_base(SC_UINT_BASE,sc_dt::sc_length_param().len())
		{}
};


////

template<int width>
class sca_type_explorer<sc_dt::sc_bv<width> > : public sca_type_explorer_base
{
public:
	sca_type_explorer():
		sca_type_explorer_base(SC_BV,width)
		{}
};


////

template<>
class sca_type_explorer<sc_dt::sc_bv_base > : public sca_type_explorer_base
{
public:
	sca_type_explorer():
		sca_type_explorer_base(SC_BV_BASE,sc_dt::sc_length_param().len())
		{}
};




////

template<int width>
class sca_type_explorer<sc_dt::sc_lv<width> > : public sca_type_explorer_base
{
public:
	sca_type_explorer():
		sca_type_explorer_base(SC_LV,width)
		{}
};

////

template<>
class sca_type_explorer<sc_dt::sc_lv_base > : public sca_type_explorer_base
{
public:
	sca_type_explorer():
		sca_type_explorer_base(SC_LV_BASE,sc_dt::sc_length_param().len())
		{}
};



////

template<int width>
class sca_type_explorer<sc_dt::sc_bigint<width> > : public sca_type_explorer_base
{
public:
	sca_type_explorer():
		sca_type_explorer_base(SC_BIGINT,width)
		{}
};

////

template<>
class sca_type_explorer<sc_dt::sc_signed > : public sca_type_explorer_base
{
public:
	sca_type_explorer():
		sca_type_explorer_base(SC_SIGNED,sc_dt::sc_length_param().len())
		{}
};



////

template<int width>
class sca_type_explorer<sc_dt::sc_biguint<width> > : public sca_type_explorer_base
{
public:
	sca_type_explorer():
		sca_type_explorer_base(SC_BIGUINT,width)
		{}
};

////

template<>
class sca_type_explorer<sc_dt::sc_unsigned > : public sca_type_explorer_base
{
public:
	sca_type_explorer():
		sca_type_explorer_base(SC_UNSIGNED,sc_dt::sc_length_param().len())
		{}
};


#ifdef SC_INCLUDE_FX

template<>
class sca_type_explorer<sc_dt::sc_fix > : public sca_type_explorer_base
{
public:
	sca_type_explorer():
		sca_type_explorer_base(SC_FIX,1)
		{}
};


template<int W,int I,sc_dt::sc_q_mode Q,sc_dt::sc_o_mode O,int N>
class sca_type_explorer<sc_dt::sc_fixed<W,I,Q,O,N> > : public sca_type_explorer_base
{
public:
	sca_type_explorer():
		sca_type_explorer_base(SC_FIXED,1)
		{}
};


template<int W,int I,sc_dt::sc_q_mode Q,sc_dt::sc_o_mode O,int N>
class sca_type_explorer<sc_dt::sc_fixed_fast<W,I,Q,O,N> > : public sca_type_explorer_base
{
public:
	sca_type_explorer():
		sca_type_explorer_base(SC_FIXED_FAST,1)
		{}
};



template<int W,int I,sc_dt::sc_q_mode Q,sc_dt::sc_o_mode O,int N>
class sca_type_explorer<sc_dt::sc_ufixed<W,I,Q,O,N> > : public sca_type_explorer_base
{
public:
	sca_type_explorer():
		sca_type_explorer_base(SC_UFIXED,1)
		{}
};


template<int W,int I,sc_dt::sc_q_mode Q,sc_dt::sc_o_mode O,int N>
class sca_type_explorer<sc_dt::sc_ufixed_fast<W,I,Q,O,N> > : public sca_type_explorer_base
{
public:
	sca_type_explorer():
		sca_type_explorer_base(SC_UFIXED_FAST,1)
		{}
};



template<>
class sca_type_explorer<sc_dt::sc_fix_fast > : public sca_type_explorer_base
{
public:
	sca_type_explorer():
		sca_type_explorer_base(SC_FIX_FAST,1)
		{}
};

template<>
class sca_type_explorer<sc_dt::sc_ufix_fast > : public sca_type_explorer_base
{
public:
	sca_type_explorer():
		sca_type_explorer_base(SC_UFIX_FAST,1)
		{}
};

#endif





} //namespace sca_implementation
} //namespace sca_util

#endif /* SCA_TYPE_EXPLORER_H_ */
