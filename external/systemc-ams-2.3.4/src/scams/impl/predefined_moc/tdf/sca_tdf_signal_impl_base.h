/*****************************************************************************

    Copyright 2010-2014
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

 sca_tdf_signal_impl_base.h - description

 Original Author: Karsten Einwich Fraunhofer IIS/EAS Dresden

 Created on: 06.08.2009

 SVN Version       :  $Revision: 2320 $
 SVN last checkin  :  $Date: 2023-06-08 12:47:38 +0000 (Thu, 08 Jun 2023) $
 SVN checkin by    :  $Author: pehrlich $
 SVN Id            :  $Id: sca_tdf_signal_impl_base.h 2320 2023-06-08 12:47:38Z pehrlich $

 *****************************************************************************/

/*****************************************************************************/

#ifndef SCA_TDF_SIGNAL_IMPL_BASE_H_
#define SCA_TDF_SIGNAL_IMPL_BASE_H_

namespace sca_core
{
namespace sca_implementation
{
	struct sca_cluster_synchronization_data;
}
}


namespace sca_tdf
{

namespace sca_implementation
{

/**
 Non template functionality for sca_tdf::sca_signal
 */
class sca_tdf_signal_impl_base: public sca_core::sca_prim_channel
{
public:

	/** Gets the absolute number of samples (from simulation start) samples
	 of the current call are not included */
	unsigned long get_sample_cnt(unsigned long port);

	/** Gets the setted sampling rate of the port */
	unsigned long& get_rate(unsigned long port);

	/** Gets the setted sampling delay of the port */
	unsigned long& get_delay(unsigned long port);

	/** Gets the current sample time distance -> if the time was setted it
	 must be equal to the calculated one - if not the scheduler will through
	 an exception */
	sca_core::sca_time& get_timestep_calculated_ref(unsigned long port) const;


	/**Gets the id of the last timestep change to identify whether the set
	 * timestep of a port is valid
	 */
	sc_dt::int64& get_timestep_change_id(unsigned long port);


	/** Gets the absolute time (from simulation start) of the first sample of the
	 current call */
	sca_core::sca_time get_time_port(unsigned long port, unsigned long sample = 0);

	//prepare channel datastructures
	virtual void end_of_elaboration();

	/** resizes port datastructure */
	void resize_port_data(unsigned long port_id);

	/** Method for buffer resize after rescheduling -
	 * overloaded from virtual method in sca_prim_channel*/
	virtual void resize_buffer();

	long get_driver_port_id();

	const char* kind() const;

	std::vector<unsigned long*> rates;
	std::vector<unsigned long*> rates_old;
	std::vector<unsigned long*> delays;
	std::vector<unsigned long*> delays_old;
	std::vector<sca_core::sca_time*> timestep_calculated;

	//this is a id when this timestep was changed the last time
	//it will be used to identify whether the current set timestep is a valid
	//one, which has to be considered for the consistency analysis
	std::vector<sc_dt::int64*>       port_timestep_change_id;
	//std::vector<sc_dt::int64>       rates_change_id;
	//std::vector<sc_dt::int64>       delays_change_id;

    //attribute change id of the last timestep calculation
	//unsigned long timestep_change_id;

	void construct();

	sca_tdf_signal_impl_base();
	sca_tdf_signal_impl_base(const char* name_);

	virtual ~sca_tdf_signal_impl_base();


	/** overloads sca_traceable_object method */
	virtual const std::string& get_trace_value() const;

	virtual const std::string& get_current_trace_value(unsigned long sample) const;

	bool register_trace_callback(sca_util::sca_traceable_object::sca_trace_callback,void*);
	bool register_trace_callback(sca_util::sca_traceable_object::callback_functor_base&);
	bool remove_trace_callback(sca_util::sca_traceable_object::callback_functor_base&);

	bool force_value(const std::string&);
	void release_value();

	bool* get_quick_signal_enable_ref() {return &quick_access_enabled;}

protected:

	/** function to support get_trace_value and get_typed_trace_value */
	sc_dt::int64 get_number_of_trace_value_sample() const;

	bool force_value_flag;
	bool scheduled_force_value_flag;

	sc_core::sc_process_handle force_proc_handle;

	void force_forced_value();


	virtual bool trace_init(
			sca_util::sca_implementation::sca_trace_object_data& data);

	//is called after cluster was calculated
	virtual void trace(long id,
			sca_util::sca_implementation::sca_trace_file_base& tf);

	virtual void trace_interactive();


	unsigned long buffer_size;
	long* buffer_offsets;

	long** calls_per_period;
	long** last_calls_per_period;
	const sc_dt::int64** call_counter_refs;
	long driver_port;
	bool* is_inport; //flag array for signing inports->check whether write allowed

	//csync_data references
	const sca_core::sca_time* cluster_period;
	const sca_core::sca_time* last_cluster_period;
	const sca_core::sca_time* current_cluster_timestep;
	const unsigned long*      attribute_change_id;
	bool*                     attributes_changed;

	sc_dt::int64 indelay_start_sample;

	mutable bool quick_access_enabled;
	mutable bool quick_access_possible;
	bool signal_traced;

private:

	virtual void set_force_value(const std::string&)=0;


	//method for calling the trace callback
	void trace_callback();

	//process to start signal forcing
	void force_trigger_process();

	sca_util::sca_traceable_object::sca_trace_callback trace_cb_ptr;
	void*                                              trace_cb_arg;
	std::vector<sca_traceable_object::callback_functor_base*> callbacks;
	bool callback_registered;

	sc_dt::int64 trace_callback_cnt;

	sca_core::sca_time dummy_time_val;

	virtual void nr_in_delay_buffer_resize(unsigned long n)=0;
	virtual void in_delay_buffer_create(unsigned long b_nr,unsigned long size)=0;
	virtual void create_buffer(unsigned long size)=0;
    virtual void store_trace_buffer_time_stamp(sca_util::sca_implementation::sca_trace_file_base& tf,unsigned long id,sca_core::sca_time ctime, unsigned long bpos)=0;

    virtual void set_type_info(sca_util::sca_implementation::sca_trace_object_data& data)=0;

    sca_core::sca_implementation::sca_cluster_synchronization_data* csync_data;

    const std::string empty_string;

    unsigned long proc_try_cnt=0;
};

} //namespace sca_implementation
} //namespace sca_tdf



#endif /* SCA_TDF_SIGNAL_IMPL_BASE_H_ */
