/*****************************************************************************

    Copyright 2010-2013
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

  sca_tdf_port_attributes.h - description

  Original Author: Karsten Einwich Fraunhofer IIS/EAS Dresden

  Created on: 27.08.2009

   SVN Version       :  $Revision: 2320 $
   SVN last checkin  :  $Date: 2023-06-08 12:47:38 +0000 (Thu, 08 Jun 2023) $
   SVN checkin by    :  $Author: pehrlich $
   SVN Id            :  $Id: sca_tdf_port_attributes.h 2320 2023-06-08 12:47:38Z pehrlich $

 *****************************************************************************/

/*****************************************************************************/


#ifndef SCA_TDF_PORT_ATTRIBUTES_H_
#define SCA_TDF_PORT_ATTRIBUTES_H_


namespace sca_tdf
{
namespace sca_implementation
{

class sca_tdf_solver;

class sca_port_attributes
{
public:

	const unsigned long& get_rate() const;
	const unsigned long& get_rate_internal() const;

	const unsigned long& get_delay() const;
	virtual const unsigned long& get_delay_internal() const;

	sca_core::sca_time get_timestep(unsigned long sample_id) const;
	sca_core::sca_time get_timestep_internal(unsigned long sample_id) const;

	sca_core::sca_time get_timeoffset() const;
	sca_core::sca_time get_timeoffset_internal() const;

	void update_last_values();

protected:

	void set_rate(unsigned long rate_);
	void set_delay(unsigned long delay_);
	void set_delay_direct(unsigned long delay_);

	void set_timestep(const sca_core::sca_time& timestep_);
	void set_max_timestep(const sca_core::sca_time& timestep_);
	void set_timeoffset(const sca_core::sca_time& timeoffset_);

	sca_core::sca_time get_max_timestep() const;
	sca_core::sca_time get_max_timestep_internal() const;

	sca_core::sca_time get_last_timestep(unsigned long sample_id) const;
	sca_core::sca_time get_last_timestep_internal(unsigned long sample_id) const;

	bool is_timestep_changed(unsigned long sample_id) const;
	bool is_timestep_changed_internal(unsigned long sample_id) const;

	bool is_rate_changed()     const;
	bool is_rate_changed_internal()     const;

	bool is_delay_changed()    const;
	bool is_delay_changed_internal()    const;


	unsigned long get_last_rate() const;
	unsigned long get_last_rate_internal() const;

	unsigned long get_last_delay() const;
	unsigned long get_last_delay_internal() const;

	sca_core::sca_time get_time(unsigned long sample = 0) const;
	sca_core::sca_time get_time_internal(unsigned long sample = 0) const;

	sca_core::sca_time get_propagated_timestep() const;

	sca_core::sca_time get_declustered_time() const;



	sca_port_attributes(sca_core::sca_implementation::sca_port_base* port);
	virtual ~sca_port_attributes(){}

	//method which allows trace initialization if no AMS cluster is available
	void initialize_all_traces();

	void port_attribute_validation();

	sca_tdf::sca_implementation::sca_tdf_signal_impl_base* sig_base;

	unsigned long port_id;

	unsigned long rate;

	bool decluster_port; //signs port as decluster port -> do not propagate timesteps via tdf signals
    sca_core::sca_implementation::sca_solver_base* declustered_solver;


	void signal_elaboration(sca_tdf::sca_implementation::sca_tdf_signal_impl_base* sig_base);

	/**
	 * sets the parent module for this port attributes
	 * this is required for hierarchical decluster ports due a decluster module is inserted
	 * @param
	 */
	void set_parent_attribute_module(sca_core::sca_module*);

private:

	unsigned long last_rate;
	unsigned long last_last_rate;

	unsigned long last_delay;
	unsigned long last_last_delay;

	unsigned long delay;
	sca_core::sca_time timestep;
	bool timestep_set;
	sca_core::sca_time max_timestep;
	bool max_timestep_set;
	sca_core::sca_time timeoffset;

	sc_dt::int64 timestep_change_id;
	sc_dt::int64 rate_change_id;
	sc_dt::int64 last_rate_change_id;
	sc_dt::int64 delay_change_id;
	sc_dt::int64 last_delay_change_id;


	sca_core::sca_time timestep_calculated;
	sca_core::sca_time max_timestep_calculated;
	sca_core::sca_time timeoffset_calculated;

	sca_core::sca_implementation::sca_port_base* pobj;
	sca_core::sca_module*                        pmod;
	sca_tdf::sca_module*                         pmodtdf;


	//for decluster ports
	sca_core::sca_time decluster_timestep_calculated;
	sc_dt::int64       decluster_timestep_change_id;
	unsigned long      decluster_rate;
	unsigned long      decluster_rate_old;
	unsigned long      decluster_delay;
	unsigned long      decluster_delay_old;


	friend class sca_tdf::sca_module;
	friend class sca_tdf::sca_implementation::sca_tdf_solver;
	friend class sca_tdf::sca_implementation::sca_tdf_view;
	friend class sca_eln::sca_implementation::sca_eln_view;
	friend class sca_lsf::sca_implementation::sca_lsf_view;


};


}
}


#endif /* SCA_TDF_PORT_ATTRIBUTES_H_ */
