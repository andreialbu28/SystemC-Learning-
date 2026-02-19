/*****************************************************************************

    Copyright 2010-2012
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

 sca_tdf_module.h - base class for tdf modules

 Original Author: Karsten Einwich Fraunhofer IIS/EAS / COSEDA Technologies

 Created on: 04.03.2009

 SVN Version       :  $Revision: 2082 $
 SVN last checkin  :  $Date: 2020-01-24 13:06:35 +0000 (Fri, 24 Jan 2020) $ (UTC)
 SVN checkin by    :  $Author: karsten $
 SVN Id            :  $Id: sca_tdf_module.h 2082 2020-01-24 13:06:35Z karsten $

 *****************************************************************************/
/*

 LRM clause 4.1.1.1.
 The class sca_tdf::sca_module shall define the base class for all
 TDF primitive modules.

 */

/*****************************************************************************/

#ifndef SCA_TDF_MODULE_H_
#define SCA_TDF_MODULE_H_

namespace sca_tdf
{

//begin implementation specific
namespace sca_implementation
{
class sca_tdf_view;
class sca_tdf_solver;
class sca_ct_ltf_nd_proxy;
class sca_ct_vector_ss_proxy;
class sca_port_attributes;
}

//end implementation specific

//class sca_module : public implementation-derived-from sca_core::sca_module
class sca_module: public sca_core::sca_module,
                  public sca_ac_analysis::sca_ac_module
{
public:

	 virtual const char* kind() const;

protected:

	typedef void (sca_tdf::sca_module::*sca_module_method)();

	virtual void set_attributes();
	virtual void change_attributes();
	virtual void initialize();
	virtual void reinitialize();
	virtual void processing();
	virtual void ac_processing();

	void register_processing(sca_tdf::sca_module::sca_module_method);
	void register_ac_processing(sca_tdf::sca_module::sca_module_method);

	void request_next_activation(const sca_core::sca_time&);
	void request_next_activation(double,sc_core::sc_time_unit);
	void request_next_activation(const sc_core::sc_event&);
	void request_next_activation(const sca_core::sca_time&,const sc_core::sc_event&);
	void request_next_activation(double,sc_core::sc_time_unit,const sc_core::sc_event&);
	void request_next_activation(const sc_core::sc_event_or_list&);
	void request_next_activation(const sc_core::sc_event_and_list&);
	void request_next_activation(const sca_core::sca_time&,const sc_core::sc_event_or_list&);
	void request_next_activation(double,sc_core::sc_time_unit,const sc_core::sc_event_or_list&);
	void request_next_activation(const sca_core::sca_time&,const sc_core::sc_event_and_list&);
	void request_next_activation(double,sc_core::sc_time_unit,const sc_core::sc_event_and_list&);

	template<class T>
	void request_next_activation(const sca_tdf::sca_de::sca_in<T>&);


	void accept_attribute_changes();
	void reject_attribute_changes();
	void does_attribute_changes();
	void does_no_attribute_changes();

	sca_core::sca_time get_time() const;
	sca_core::sca_time get_last_timestep() const;

	bool is_dynamic() const;
	bool are_attribute_changes_allowed() const;
	bool are_attributes_changed() const;
	bool is_timestep_changed() const;


	explicit sca_module(const sc_core::sc_module_name&);
	sca_module();

	virtual ~sca_module()=0;

//implementation specific

	explicit sca_module(const char*);
	explicit sca_module(const std::string&);


public:

	//implementation of sca_core::sca_module methods
	virtual void set_max_timestep(const sca_core::sca_time&);
	virtual void set_max_timestep(double,sc_core::sc_time_unit);

	virtual sca_core::sca_time get_timestep() const;
    virtual sca_core::sca_time get_max_timestep() const;

	 // new SystemC-AMS 2.0 dtdf extension
	 sca_module_method  change_attributes_method;
	 bool no_change_attributes;
	 bool no_reinitialize;

	 void set_timestep(const sca_core::sca_time&);
	 void set_timestep(double,sc_core::sc_time_unit);


	  //new SystemC-AMS 2.0 feature implementation specific
	  //IMPROVE: make private
	  bool does_attribute_changes_flag;
	  mutable bool last_does_attribute_changes_flag;
	  bool accept_attribute_changes_flag;
	  mutable bool last_accept_attribute_changes_flag;
	  bool has_executed_attribute_change_method;
	  sca_module_method reinitialize_method;
	  bool port_attribute_validation_required;
	  void validate_port_attributes();



private:

	friend class sca_tdf::sca_implementation::sca_tdf_solver;
	friend class sca_tdf::sca_implementation::sca_tdf_view;
	friend class sca_tdf::sca_implementation::sca_ct_ltf_nd_proxy;
	friend class sca_tdf::sca_implementation::sca_ct_vector_ss_proxy;
	friend class sca_core::sca_implementation::sca_synchronization_alg;
	friend class sca_tdf::sca_implementation::sca_port_attributes;

	  /**
	    Method for registering the attribute method. The registered method will
	    will be called from the view layer before datastructure set-up
	  */
	  void register_attr_method(sca_module_method funcp);

	  /**
	    Method for registering the init method. The registered method will
	    will be called from the solver layer immedately before simulation start,
	    after all simulation datastructures set-up.
	  */
	  void register_init_method(sca_module_method funcp);


	  /**
	    Method for registering the post processing method. The registered method
	    will be called from the solver layer before simulation finished (before leaving
	    sc_main).
	  */
	  void register_post_method(sca_module_method funcp);


	  /** Default method for post processing see @ref #void register_post_method */
	  virtual void post_proc() {}


	  //reference to counter with number of module calls
	  sc_dt::int64* call_counter;

	  //the view class is allowed to access the assigned methods
	  friend class sca_tdf_view;

	  sca_module_method attr_method;
	  sca_module_method init_method;
//	  sca_module_method reinitialize_method;  //error because of private
	  sca_module_method sig_proc_method;
	  sca_module_method ac_processing_method;
	  sca_module_method post_method;


	  bool reg_attr_method;
	  bool reg_init_method;
	  bool reg_processing_method;
	  bool reg_ac_processing_method;
	  bool reg_post_method;

	  bool no_default_processing;
	  bool no_default_ac_processing;
	  bool check_for_processing;

	  bool is_port_attribute_changed() const;


	  void construct();

	  //overload sca_core::sca_module method
	  void elaborate();

	  //event list handling for request_next_activation
	  mutable sc_dt::int64 does_attr_ch_id;
	  mutable sc_dt::int64 accept_attr_ch_id;

	  //empty ac implementation -> executed in the default case
	  virtual void ac_dummy();

// end implementation specific section


};


template<class T>
void sca_module::request_next_activation(const sca_tdf::sca_de::sca_in<T>& port)
{
	request_next_activation(port.default_event());
}


#define SCA_TDF_MODULE(name) struct name : sca_tdf::sca_module

} // namespace sca_tdf

#endif /* SCA_TDF_MODULE_H_ */
