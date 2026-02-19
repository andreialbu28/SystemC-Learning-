/*****************************************************************************

    Copyright 2010
    Fraunhofer-Gesellschaft zur Foerderung der angewandten Forschung e.V.

 	Copyright 2015-2022
 	COSEDA Technologies GmbH

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

 sca_trace_value_handler.h - description

 Original Author: Karsten Einwich COSEDA Technologies GmbH

 Created on: 13.11.2009

 SVN Version       :  $Revision: 2320 $
 SVN last checkin  :  $Date: 2023-06-08 12:47:38 +0000 (Thu, 08 Jun 2023) $
 SVN checkin by    :  $Author: pehrlich $
 SVN Id            :  $Id: sca_trace_value_handler.h 2320 2023-06-08 12:47:38Z pehrlich $

 *****************************************************************************/

/*****************************************************************************/

#ifndef SCA_TRACE_VALUE_HANDLER_H_
#define SCA_TRACE_VALUE_HANDLER_H_


namespace sca_util
{
namespace sca_implementation
{

class sca_trace_value_handler_mm;

template<class T>
class sca_trace_value_handler: public sca_trace_value_handler_base
{
	T* valuepointer;

	T& value() const {return *valuepointer;}

    void remove_value() {valuepointer=NULL;}

public:

    //this methods are called from the trace side
    //only operators of the datatype with have no side effect can be used inside this methods (including specializations)
	sca_trace_value_handler_base* duplicate();
	virtual sca_trace_value_handler_base& hold(const sca_core::sca_time& ctime);
	virtual sca_trace_value_handler_base& interpolate(const sca_core::sca_time& ctime); //default hold value

	void swap_values_base(sca_trace_value_handler_base*);


	virtual void print(std::ostream& ostr);


	const sca_type_explorer_base& get_typed_value(const void*& data) const;
    const sca_type_explorer_base& get_type() const;


private:

	friend class sca_trace_value_handler_mm;

	sca_trace_value_handler(const sca_core::sca_time& ctime, std::int64_t id, const T& val);
	sca_trace_value_handler(sca_trace_value_handler<T>*);

	void copy_as_reference(sca_trace_value_handler<T>*);

	virtual ~sca_trace_value_handler();

	//only used by memory manager at the maim program side - it can be used at the
	//trace side if the copy operator of the datatype has no side effects
	void reset(const sca_core::sca_time& ctime, std::int64_t id, const T& val);

	void swap_values(sca_trace_value_handler<T>*);  //swaps the value pointer - used to overwrite existing time stamp

	sca_trace_value_handler<T>* get_new_local(sca_trace_value_handler<T>*);

	//this method cannot be used for non-side effect free datatypes
	sca_trace_value_handler<T>* get_new_local(const sca_core::sca_time& ctime, std::int64_t id, const T& val);

};

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////


template<class TV>
inline void print_value(std::ostream& ostr,const TV& value)
{
	ostr << value;
}

template<>
inline void print_value<sca_util::sca_complex>(std::ostream& ostr,
		const sca_util::sca_complex& value)
{
	ostr << value.real() << " " <<  value.imag();
}

template<class T>
inline const sca_type_explorer_base& sca_trace_value_handler<T>::get_type() const
{
	static const sca_type_explorer<T> type_info;
	return type_info;
}


template<class T>
inline const sca_type_explorer_base& sca_trace_value_handler<T>::get_typed_value(const void*& data) const
{
	data=static_cast<const void*>(&this->value());
	return this->get_type();
}



template<class T>
inline sca_trace_value_handler<T>::sca_trace_value_handler(const sca_core::sca_time& ctime, std::int64_t id, const T& val) :
	valuepointer(new T(val))
{
	this->ref_cnt=new std::uint64_t(1);
	this->id=id;
	this->this_time=ctime;
	this->systemc_time=sc_core::sc_time_stamp();
}





template<class T>
inline sca_trace_value_handler<T>::sca_trace_value_handler(sca_trace_value_handler<T>* hd): valuepointer(hd->valuepointer)
{
	this->ref_cnt=       hd->ref_cnt;
	(*ref_cnt)++;

	this->id=            hd->id;
	this->memory_manager=hd->memory_manager;
	this->next_time=     hd->next_time;
	this->next_value=    hd->next_value;
	this->systemc_time=  hd->systemc_time;
	this->this_time=     hd->this_time;
}

template<class T>
inline void sca_trace_value_handler<T>::copy_as_reference(sca_trace_value_handler<T>* hd)
{
	this->ref_cnt=hd->ref_cnt;
	(*ref_cnt)++;

	this->id=             hd->id;
	this->memory_manager= hd->memory_manager;
	this->next_time=      hd->next_time;
	this->next_value=     hd->next_value;
	this->systemc_time=   hd->systemc_time;
	this->this_time=      hd->this_time;

	this->valuepointer=   hd->valuepointer;
}

//swaps the value pointer - used to overwrite existing time stamp
template<class T>
inline void sca_trace_value_handler<T>::swap_values(sca_trace_value_handler<T>* vh)
{
	auto vp=this->valuepointer;
	this->valuepointer=vh->valuepointer;
	vh->valuepointer=vp;

	auto rcnt=this->ref_cnt;
	this->ref_cnt=vh->ref_cnt;
	vh->ref_cnt=rcnt;
}

template<class T>
inline void sca_trace_value_handler<T>::swap_values_base(sca_trace_value_handler_base* vhb)
{
	auto vh=dynamic_cast<sca_trace_value_handler<T>*>(vhb);
	if(vh==NULL)
	{
		SC_REPORT_ERROR("SystemC-AMS","This error should not be possible");
	}

	this->swap_values(vh);
}

template<class T>
inline void sca_trace_value_handler<T>::reset(const sca_core::sca_time& ctime, std::int64_t id, const T& val)
{
	this->id=id;
	this->this_time=ctime;
	this->systemc_time=sc_core::sc_time_stamp();
	this->next_value=NULL;
	this->next_time=sc_core::SC_ZERO_TIME;

	if(this->valuepointer==NULL) this->valuepointer=new T(val);
	else                         this->value()=val;

	if(this->ref_cnt==NULL)      this->ref_cnt=new std::uint64_t(1);
	else                         *this->ref_cnt=1;
}

/////////////////////

template<class T>
inline sca_trace_value_handler<T>* sca_trace_value_handler<T>::get_new_local(sca_trace_value_handler<T>* ref)
{
	sca_trace_value_handler_base* th=this->get_new_local_base();
	sca_trace_value_handler<T>* ret;

	if(th==NULL)
	{
		ret=new sca_trace_value_handler<T>(ref);
	}
	else
	{
		ret=dynamic_cast<sca_trace_value_handler<T>*>(th);
		if(ret==NULL)
		{
			SC_REPORT_ERROR("sca_trace_value_handler","Should not be possible");
		}
		ret->copy_as_reference(ref);
	}

	return ret;
}

template<class T>
inline sca_trace_value_handler<T>* sca_trace_value_handler<T>::get_new_local(const sca_core::sca_time& ctime, std::int64_t id, const T& val)
{
	sca_trace_value_handler_base* th=this->get_new_local_base();
	sca_trace_value_handler<T>* ret;

	if(th==NULL)
	{
		ret=new sca_trace_value_handler<T>(ctime,id,val);
	}
	else
	{
		ret=dynamic_cast<sca_trace_value_handler<T>*>(th);
		if(ret==NULL)
		{
			SC_REPORT_ERROR("sca_trace_value_handler","Should not be possible");
		}
		ret->reset(ctime,id,val);
	}

	ret->set_local(); //signs value as local generated value

	return ret;
}

template<class T>
inline sca_trace_value_handler_base* sca_trace_value_handler<T>::duplicate()
{
	return this->get_new_local(this);
}



////////////////////////////////////


template<class T>
inline void sca_trace_value_handler<T>::print(std::ostream& ostr)
{
	print_value<T>(ostr,this->value());
}

////////////////////////////////////

template<class T>
inline sca_trace_value_handler_base& sca_trace_value_handler<T>::hold(
		const sca_core::sca_time& ctime)
{
	auto rv=dynamic_cast<sca_trace_value_handler<T>*>(this->duplicate());
	rv->this_time = ctime;

	return *rv;
}

////////////////////////////////////

//default hold value
template<class T>
inline sca_trace_value_handler_base& sca_trace_value_handler<T>::interpolate(
		const sca_core::sca_time& ctime)
{
	return hold(ctime);
}

//template specialization for double
template<>
sca_trace_value_handler_base&
sca_trace_value_handler<double>::interpolate(const sca_core::sca_time& ctime);

//template specialization for complex<double>
template<>
sca_trace_value_handler_base&
sca_trace_value_handler<std::complex<double> >::interpolate(const sca_core::sca_time& ctime);

////////////////////////////////////

template<class T>
inline sca_trace_value_handler<T>::~sca_trace_value_handler()
{
	if(valuepointer!=NULL)
	{
		delete valuepointer;
		valuepointer=NULL;
	}

	if(ref_cnt!=NULL)
	{
		delete ref_cnt;
		ref_cnt=NULL;
	}
}

}
}

#endif /* SCA_TRACE_VALUE_HANDLER_H_ */
