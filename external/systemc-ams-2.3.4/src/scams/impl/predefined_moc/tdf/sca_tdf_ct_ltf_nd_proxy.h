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

 sca_tdf_ct_ltf_nd_proxy.h - description

 Original Author: Karsten Einwich Fraunhofer IIS/EAS Dresden

 Created on: 20.10.2009

 SVN Version       :  $Revision: 2152 $
 SVN last checkin  :  $Date: 2021-04-28 15:46:33 +0000 (Wed, 28 Apr 2021) $
 SVN checkin by    :  $Author: karsten $
 SVN Id            :  $Id: sca_tdf_ct_ltf_nd_proxy.h 2152 2021-04-28 15:46:33Z karsten $

 *****************************************************************************/

/*****************************************************************************/

#ifndef SCA_TDF_CT_LTF_ND_PROXY_H_
#define SCA_TDF_CT_LTF_ND_PROXY_H_

#include "systemc-ams"
#include "scams/predefined_moc/tdf/sca_tdf_ct_proxy.h"
#include "scams/impl/predefined_moc/tdf/sca_ct_delay_buffer.h"

struct sca_solv_data;
struct sca_solv_checkpoint_data;

namespace sca_tdf
{
namespace sca_implementation
{
class sca_ct_ltf_nd_proxy: public sca_tdf::sca_ct_proxy
{
public:

	void set_max_delay(sca_core::sca_time mdelay);


	void register_nd(
			const sca_util::sca_vector<double>& num,
			const sca_util::sca_vector<double>& den,
			sca_core::sca_time ct_delay,
			sca_util::sca_vector<double>& state,
			double input,
			double k,
			sca_core::sca_time tstep);


	void register_nd(
			const sca_util::sca_vector<double>& num,
			const sca_util::sca_vector<double>& den,
			sca_core::sca_time ct_delay,
			sca_util::sca_vector<double>& state,
			const sca_util::sca_vector<double>& input,
			double k,
			sca_core::sca_time tstep);


	void register_nd(
			const sca_util::sca_vector<double>& num,
			const sca_util::sca_vector<double>& den,
			sca_core::sca_time ct_delay,
			sca_util::sca_vector<double>& state,
			const sca_tdf::sca_in<double>& input,
			double k);

	void register_nd(
			const sca_util::sca_vector<double>& num,
			const sca_util::sca_vector<double>& den,
			sca_core::sca_time ct_delay,
			sca_util::sca_vector<double>& state,
			const sca_tdf::sca_de::sca_in<double>& input,
			double k);

	void register_nd(
			const sca_util::sca_vector<double>& num,
			const sca_util::sca_vector<double>& den,
			sca_core::sca_time ct_delay,
			double input, double k,
			sca_core::sca_time tstep);

	void register_nd(
			const sca_util::sca_vector<double>& num,
			const sca_util::sca_vector<double>& den,
			sca_core::sca_time ct_delay,
			const sca_util::sca_vector<double>& input,
			double k,
			sca_core::sca_time tstep);

	void register_nd(
			const sca_util::sca_vector<double>& num,
			const sca_util::sca_vector<double>& den,
			sca_core::sca_time ct_delay,
			const sca_tdf::sca_in<double>& input,
			double k);

	void register_nd(
			const sca_util::sca_vector<double>& num,
			const sca_util::sca_vector<double>& den,
			sca_core::sca_time ct_delay,
			const sca_tdf::sca_de::sca_in<double>& input,
			double k);

	sca_ct_ltf_nd_proxy(::sc_core::sc_object* ltf_object);
	virtual ~sca_ct_ltf_nd_proxy();

	void enable_iterations();
	double estimate_next_value();


private:

	bool iterations_enabled;
	bool module_is_dynamic;

	void register_nd_port(
			const sca_util::sca_vector<double>& num,
			const sca_util::sca_vector<double>& den,
			sca_core::sca_time&                 ct_delay,
			sca_util::sca_vector<double>&       state,
			double                              k,
			unsigned long                       port_rate,
			sca_core::sca_time&                 first_port_timestep,
			sca_core::sca_time&                 port_timestep,
			double                              port_value);

	void convert_to_port( sca_core::sca_time& first_port_timestep,
			              sca_core::sca_time& port_timestep,
			              sca_core::sca_time& port_time,
			              unsigned long       port_rate );

protected:

	//overloaded virtual methods
	double convert_to_double();
	void   convert_to_sca_port(sca_tdf::sca_out_base<double>&);
	void   convert_to_vector(sca_util::sca_vector<double>&, unsigned long nsamples);
	void   convert_to_sc_port(sca_tdf::sca_de::sca_out<double>& port);



	const sca_util::sca_vector<double>* num_old_ref;
	const sca_util::sca_vector<double>* den_old_ref;

	sca_util::sca_vector<double> num_old;
	sca_util::sca_vector<double> den_old;

	sca_util::sca_vector<double> s_intern;                //internal state vector
	sca_solv_checkpoint_data* s_intern_module_backup;  //module activation backup of state vector
	sca_solv_checkpoint_data* s_intern_tstep_module_backup;   //tstep backup of state vector
	sca_solv_checkpoint_data* s_intern_tstep_backup; //module activation backup for tstep

	sca_core::sca_implementation::sca_signed_time last_in_time_module_backup;
	sca_core::sca_implementation::sca_signed_time last_in_time_module_tstep_backup;
	sca_core::sca_implementation::sca_signed_time last_in_time_tstep_backup;

	sca_core::sca_implementation::sca_signed_time last_out_time_module_backup;
	sca_core::sca_implementation::sca_signed_time last_out_time_module_tstep_backup;
	sca_core::sca_implementation::sca_signed_time last_out_time_tstep_backup;

	sca_core::sca_implementation::sca_signed_time last_calculated_time_module_backup;
	sca_core::sca_implementation::sca_signed_time last_calculated_time_module_tstep_backup;
	sca_core::sca_implementation::sca_signed_time last_calculated_time_tstep_backup;

	double in_last_module_backup;
	double in_last_module_tstep_backup;
	double in_last_tstep_backup;

	sca_core::sca_time last_tstep_backup;
	sca_core::sca_implementation::sca_signed_time current_time_backup;



	bool reinit_request;

	//allow overloading for skipping test
	virtual bool coeff_changed(const sca_util::sca_vector<double>& num,
			const sca_util::sca_vector<double>& den);

	sca_util::sca_vector<double>* statep;

	sca_tdf::sca_module* parent_module;
	sc_core::sc_object* ltf_object;

	void initialize();
	double calculate(double);

	//solver data
	bool first_step;
	double q_dn;

	double* eq_in;

	//equation system Adx+Bx+q(t) for fractional and non-fractional part
	double *q;
	double *q2;
	sca_util::sca_matrix<double> A, B;

	sca_solv_data   *sdata;
	unsigned long memsize;
	unsigned long dn, nn;


	//references for fast access
	double* den_ltf;
	double* num_ltf;

	//pointer to memory for non fractional numerator coefficients
	double* num2_ltf;

	double* s;

	unsigned long den_size, num_size, state_size;

	unsigned long number_of_equations;
	unsigned long number_of_equations2;
	unsigned long number_of_all_equations;

	void setup_equation_system();
	void initialize_equation_system(int init_mode,double h);


	double last_h;

	//for estimate method
	double last_delta_in;
	double last_calculated_in;
	sca_solv_checkpoint_data* estimate_cp_data;

	double in_last;

	///variables for input handling

	unsigned long number_of_in_values;

	//the time distance of the input sample - except the distance from last to first
	sca_core::sca_time in_time_step;
	//the time distance between the last sample (from the previous calculation)
	//to the first of the current calculation
	sca_core::sca_time first_in_time_step;

	//the time of last calculated/integrated input value - without delay
	sca_core::sca_implementation::sca_signed_time last_calculated_in_time;
	//the time of the last calculated output value
	sca_core::sca_implementation::sca_signed_time last_out_time;
	//the time of the last calculation/integration
	sca_core::sca_implementation::sca_signed_time last_calculated_time;

	sca_core::sca_time dt_last;

	//the time from the module activation to the reached integration time
	//(the sum of the tstep's of the calculations during the current module
	//activation)
	sca_core::sca_implementation::sca_signed_time current_time;

	//time of last available in time sample
	sca_core::sca_implementation::sca_signed_time last_available_in_time;

	//the current time step of the parent module
	sca_core::sca_time module_time_step;

	//the time intervall of the current integration/calculation
	//resulting from tstep or the remaining time (if calculated to the
	//current parent module time)
	sca_core::sca_time time_interval;


	bool calculate_to_end;


	enum inval_type
	{
		IN_SCALAR,
		IN_VECTOR,
		IN_SCA_PORT,
		IN_SC_PORT
	};

	inval_type intype;

	double in_scalar;
	const sca_util::sca_vector<double>* in_vector;
	const sca_tdf::sca_in<double>*      in_sca_port;
	const sca_tdf::sca_de::sca_in<double>* in_sc_port;

	enum outval_type
	{
		OUT_SCALAR,
		OUT_VECTOR,
		OUT_SCA_PORT,
		OUT_SC_PORT
	};

	outval_type outtype;

	double out_scalar;
	sca_util::sca_vector<double>* out_vector;
	sca_tdf::sca_out_base<double>*      out_sca_port;
	sca_tdf::sca_de::sca_out<double>* out_sc_port;

	bool pending_calculation;

	unsigned long in_index_offset;

	sc_dt::int64 module_activations;

	double scale_factor;

	double get_in_value_by_index(unsigned long k);
	void write_out_value_by_index(double val, unsigned long k);

	//common functionality for all register methods
	void register_nd_common(
			const sca_util::sca_vector<double>& num,
			const sca_util::sca_vector<double>& den,
			const sca_core::sca_time& tstep);

	void calculate_timeinterval(
			unsigned long& current_in_index,
			         long& current_out_index,
			         long number_of_out_sample,
			         sca_core::sca_implementation::sca_signed_time& next_in_time,
			         sca_core::sca_implementation::sca_signed_time& next_out_time,
			         sca_core::sca_time& next_in_time_step,
			         sca_core::sca_time& out_time_step);

	bool causal_warning_reported;

	sca_core::sca_time ct_in_delay;
	sca_core::sca_time max_delay;

	sca_core::sca_time module_current_time;

	sca_tdf::sca_implementation::sca_ct_delay_buffer<double>* delay_buffer;

	bool dc_init;
	bool dc_init_interval;

};
}
}

#endif /* SCA_TDF_CT_LTF_ND_PROXY_H_ */
