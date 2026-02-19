/*****************************************************************************

    Copyright 2010-2013
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

 sca_tdf_sc_in_sc_logic.h - converter port SystemC -> tdf

 Original Author: Karsten Einwich Fraunhofer / COSEDA Technologies

 Created on: 04.03.2009

 SVN Version       :  $Revision: 2320 $
 SVN last checkin  :  $Date: 2023-06-08 12:47:38 +0000 (Thu, 08 Jun 2023) $ (UTC)
 SVN checkin by    :  $Author: pehrlich $
 SVN Id            :  $Id: sca_tdf_sc_in_sc_logic.h 2320 2023-06-08 12:47:38Z pehrlich $


 *****************************************************************************/
/*
 LRM clause 4.1.1.12.
 */

/*****************************************************************************/

#ifndef SCA_TDF_SC_IN_SC_LOGIC_H_
#define SCA_TDF_SC_IN_SC_LOGIC_H_


namespace sca_tdf
{

namespace sca_de
{

//begin implementation specific

template<class T>
class sca_out;

//end implementation specific


template<>
//class sca_in : public implementation-derived-from sca_core::sca_port&lt; sc_core::sc_signal_in_if >
class sca_in<sc_dt::sc_logic>: public sca_core::sca_port< sc_core::sc_signal_in_if<sc_dt::sc_logic> >,
		public sca_tdf::sca_implementation::sca_port_attributes
{
public:
	sca_in();
	explicit sca_in(const char* );

	void set_delay(unsigned long );
	void set_rate(unsigned long );
	void set_timestep(const sca_core::sca_time& );
	void set_timestep(double , sc_core::sc_time_unit );
	void set_max_timestep(const sca_core::sca_time& );
	void set_max_timestep(double , sc_core::sc_time_unit );

	unsigned long get_delay() const;
	unsigned long get_rate() const;
	sca_core::sca_time get_time(unsigned long sample_id = 0) const;
	sca_core::sca_time get_timestep(unsigned long sample_id = 0) const;
	sca_core::sca_time get_max_timestep() const;
	sca_core::sca_time get_last_timestep(unsigned long sample_id=0) const;

	virtual const char* kind() const override;

	void initialize(const sc_dt::sc_logic& value, unsigned long sample_id = 0);
	const sc_dt::sc_logic& read_delayed_value(unsigned long sample_id = 0) const;

	bool is_timestep_changed(unsigned long sample_id=0) const;
	bool is_rate_changed() const;
	bool is_delay_changed() const;

	const sc_dt::sc_logic& read(unsigned long sample_id = 0);
	operator const sc_dt::sc_logic&();
	const sc_dt::sc_logic& operator[](unsigned long sample_id);

	const sc_core::sc_event& default_event() const;
	const sc_core::sc_event& value_changed_event() const;
	const sc_core::sc_event& posedge_event() const;
	const sc_core::sc_event& negedge_event() const;

	bool event() const;
	bool posedge() const;
	bool negedge() const;

	virtual void bind(sc_core::sc_signal_in_if<sc_dt::sc_logic>&) override;
	void operator()(sc_core::sc_signal_in_if<sc_dt::sc_logic>&);

	virtual void bind(sc_core::sc_port<sc_core::sc_signal_in_if<sc_dt::sc_logic> >&);
	void operator()(sc_core::sc_port<sc_core::sc_signal_in_if<sc_dt::sc_logic> >&);

	virtual void bind(sc_core::sc_port<sc_core::sc_signal_inout_if<sc_dt::sc_logic> >&);
	void operator()(sc_core::sc_port<sc_core::sc_signal_inout_if<sc_dt::sc_logic> >&);


	/** return the sc_interface for converter ports*/
	virtual sc_core::sc_interface* get_interface() override;
	virtual const sc_core::sc_interface* get_interface() const override;


	/** method of interactive tracing interface, which returns the value
	 * at the current SystemC time (the time returned by sc_core::sc_time_stamp())
	 */
	const sc_dt::sc_logic& get_typed_trace_value() const;

	virtual const std::string& get_trace_value() const override;


    /**
     * registers trace callback
     */
	virtual bool register_trace_callback(sca_util::sca_traceable_object::callback_functor_base&) override;
	virtual bool remove_trace_callback(sca_util::sca_traceable_object::callback_functor_base&) override;

	/** method of interactive tracing interface, which forces a value
	 */
    void force_typed_value(const sc_dt::sc_logic&);
    virtual void set_force_value(const std::string& stri);


    /** method of interactive tracing interface, which releases a forced value
    */
    virtual void release_value() override;


	void set_timeoffset(const sca_core::sca_time& toffset); //obsolete in 2.0
	void set_timeoffset(double toffset, ::sc_core::sc_time_unit unit); //obsolete in 2.0

	sca_core::sca_time get_timeoffset() const; //obsolete in 2.0

private:
	// Disabled
	sca_in(const sca_tdf::sca_de::sca_in<sc_dt::sc_logic>&);
	sca_tdf::sca_de::sca_in<sc_dt::sc_logic>& operator= (const sca_tdf::sca_de::sca_in<sc_dt::sc_logic>&);

	void end_of_port_elaboration() override;

	//begin implementation specific

	//on this way we forbid port-to-port binding to a converter port
	void bind(sca_tdf::sca_de::sca_out<sc_dt::sc_logic>&);
	void operator()(sca_tdf::sca_de::sca_out<sc_dt::sc_logic>&);
	void bind(sca_tdf::sca_de::sca_in<sc_dt::sc_logic>&);
	void operator()(sca_tdf::sca_de::sca_in<sc_dt::sc_logic>&);

	 //overload sc_port_b bind method
	void bind( sc_core::sc_port_b<sc_core::sc_signal_in_if<sc_dt::sc_logic> >& parent_ ) override;

	void initialize_port() override;
	void reinitialize_port() override;

	void construct();

	typedef sca_core::sca_port< ::sc_core::sc_signal_in_if<sc_dt::sc_logic> > base_type;


	sc_dt::sc_logic ret_tmp;

	const bool* allow_processing_access_flag;
	bool allow_processing_access_default;

	sca_core::sca_module* pmod;

	virtual bool trace_init(sca_util::sca_implementation::sca_trace_object_data& data) override;
	virtual void trace(long id, sca_util::sca_implementation::sca_trace_file_base& tf) override;

	 //sc trace of sc_core::sc_object to prevent clang warning due overloaded
	 //sca_traceable_object function
	 void trace( sc_core::sc_trace_file* tf ) const override;

	bool traces_available;
	std::vector<sca_util::sca_implementation::sca_trace_object_data*> trace_data;

	//method to pass attributes to module after change attributes
	void validate_port_attributes() override;

	//variables for interactive trace callback
	std::vector<sca_util::sca_traceable_object::callback_functor_base*> callbacks;
	bool callback_registered;

	sc_dt::sc_logic forced_value;
	bool value_forced;

	mutable std::string current_trace_value_string;


	sc_dt::sc_logic* buffer;
	std::uint64_t buffer_size;

	const sc_dt::sc_logic& read_buffer(unsigned long n);
	void read_from_sc() override; //will be called by the synchronization layer
	unsigned long write_cnt;

	//end implementation specific
};

//begin implementation specific


} // namespace sca_de

//template<class T>
//typedef sca_tdf::sca_de::sca_in<T> sc_in<T> ;

template<>
class sc_in<sc_dt::sc_logic>: public sca_tdf::sca_de::sca_in<sc_dt::sc_logic>
{
public:
	sc_in() :
		sca_tdf::sca_de::sca_in<sc_dt::sc_logic>()
	{
	}
	explicit sc_in(const char* name_) :
		sca_tdf::sca_de::sca_in<sc_dt::sc_logic>(name_)
	{
	}
};


} // namespace sca_tdf


#endif /* SCA_TDF_SC_IN_SC_LOGIC_H_ */
