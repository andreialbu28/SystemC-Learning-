/*****************************************************************************

    Copyright 2010-2013
    Fraunhofer-Gesellschaft zur Foerderung der angewandten Forschung e.V.

    Copyright 2015-2016
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

  sca_tdf_trace_variable_base.h - description

  Original Author: Karsten Einwich Fraunhofer IIS/EAS Dresden

  Created on: 12.12.2009

   SVN Version       :  $Revision: 2320 $
   SVN last checkin  :  $Date: 2023-06-08 12:47:38 +0000 (Thu, 08 Jun 2023) $
   SVN checkin by    :  $Author: pehrlich $
   SVN Id            :  $Id: sca_tdf_trace_variable_base.h 2320 2023-06-08 12:47:38Z pehrlich $

 *****************************************************************************/

/*****************************************************************************/


#ifndef SCA_TDF_TRACE_VARIABLE_BASE_H_
#define SCA_TDF_TRACE_VARIABLE_BASE_H_

namespace sca_tdf
{
namespace sca_implementation
{


class sca_trace_variable_base :  public sc_core::sc_object,
                                 public sca_util::sca_traceable_object,
		                         public sca_core::sca_physical_domain_interface
{
public:
    sca_trace_variable_base(const char* nm);
    virtual ~sca_trace_variable_base();


	/** overloads sca_traceable_object method */
	virtual const std::string& get_trace_value() const;

	/** overloads sca_traceable_object method */
	bool register_trace_callback(sca_util::sca_traceable_object::sca_trace_callback,void*);
	bool register_trace_callback(sca_util::sca_traceable_object::callback_functor_base&);
	bool remove_trace_callback(sca_util::sca_traceable_object::callback_functor_base&);

	/** overloads sca_traceable_object method */
	bool force_value(const std::string&);

	/** overloads sca_traceable_object method */
	void release_value();

	/**
	   * experimental physical domain interface
	*/
	virtual void set_unit(const std::string& unit);
	virtual const std::string& get_unit() const;

	virtual void set_unit_prefix(const std::string& prefix);
	virtual const std::string& get_unit_prefix() const;

	virtual void set_domain(const std::string& domain);
	virtual const std::string& get_domain() const;

protected:

    unsigned long rate;
    sca_core::sca_time       timeoffset;
    unsigned long calls_per_period;
    sca_core::sca_time       sample_period;
    const sc_dt::int64*    call_counter_ref;
    long          buffer_size;
    std::vector<bool>  write_flags;
    bool          initialized;

    std::vector<sca_core::sca_time> time_buffer;

    sca_core::sca_time get_trace_time(int sample_id);

    bool initialize();

    //type independend part of write
    long get_write_index(unsigned long sample_id);


	virtual const std::string& get_current_trace_value(unsigned long sample) const;
	long get_trace_value_index() const;

	void force_forced_value();
	bool force_value_flag;

private:

    sca_tdf::sca_module*   parent_module;

    bool trace_init(sca_util::sca_implementation::sca_trace_object_data& data);
    void trace(long id,sca_util::sca_implementation::sca_trace_file_base& tf);
    void trace_interactive();

	 //sc trace of sc_core::sc_object to prevent clang warning due overloaded
	 //sca_traceable_object function
	 void trace( sc_core::sc_trace_file* tf ) const;

    virtual void resize_buffer(long n)           = 0;
    virtual void store_to_last(long n)           = 0;
    virtual void trace_last(long id,const sca_core::sca_time& ct,sca_util::sca_implementation::sca_trace_file_base& tf) = 0;

    virtual void set_type_info(sca_util::sca_implementation::sca_trace_object_data& data)=0;

    //registered trace callback
	sca_util::sca_traceable_object::sca_trace_callback trace_cb_ptr;
	void*                                              trace_cb_arg;
	sc_core::sc_event                                  trace_callback_write_event;
	long                                               trace_callback_pos;

	std::vector<sca_traceable_object::callback_functor_base*> callbacks;
	bool callback_registered;

	//callback process
	void trace_callback();


	//process to start signal forcing
	void force_trigger_process();


	virtual void set_force_value(const std::string&)=0;


	bool scheduled_force_value_flag;

	sc_core::sc_process_handle force_proc_handle;

	std::string unit;
	std::string unit_prefix;
	std::string domain;

};


} //namespace sca_implementation

} //namespace sca_util

#endif /* SCA_TDF_TRACE_VARIABLE_BASE_H_ */
