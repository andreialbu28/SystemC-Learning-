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

 sca_tdf_ltf_ss_proxy.cpp - description

 Original Author: Karsten Einwich Fraunhofer IIS/EAS Dresden

 Created on: 27.12.2009

 SVN Version       :  $Revision: 2320 $
 SVN last checkin  :  $Date: 2023-06-08 12:47:38 +0000 (Thu, 08 Jun 2023) $
 SVN checkin by    :  $Author: pehrlich $
 SVN Id            :  $Id: sca_tdf_ct_vector_ss_proxy.cpp 2320 2023-06-08 12:47:38Z pehrlich $

 *****************************************************************************/

/*****************************************************************************/

#include "scams/impl/predefined_moc/tdf/sca_tdf_ct_vector_ss_proxy.h"
#include "scams/impl/core/sca_simcontext.h"
#include "scams/impl/solver/util/sparse_library/linear_analog_solver.h"
#include <cstring>

namespace sca_tdf
{
namespace sca_implementation
{

sca_ct_vector_ss_proxy::sca_ct_vector_ss_proxy(::sc_core::sc_object* object)
{
	a_old_ref = NULL;
	b_old_ref = NULL;
	c_old_ref = NULL;
	d_old_ref = NULL;

	reinit_request = true;

	q_ss = NULL;
	a_ss = NULL;
	b_ss = NULL;
	memsize = 0;

	first_step = true;
	time_interval = sc_core::SC_ZERO_TIME;
	sdata = NULL;
	last_h = -1.0;
	module_activations = -1;
	pending_calculation = false;
	causal_warning_reported = false;

	in_vector=NULL;
	q_dn=0.0;
    intype=IN_SCALAR;
    in_sca_port=NULL;
    in_index_offset=0;
    in_sc_port=NULL;
    out_sc_port=NULL;
    dn=0;
    state_size=0;
    out_vector=NULL;
    outtype=OUT_SCALAR;
	eq_in=NULL;
    calculate_to_end=false;
    number_of_in_values=0;
    out_sca_port=NULL;
    s=NULL;
    statep=NULL;
    nn=0;
    number_of_equations=0;


	ltf_object = object;
	sc_core::sc_object* pobj = object->get_parent_object();
	parent_module = dynamic_cast<sca_tdf::sca_module*> (pobj);

	if (!parent_module)
	{
		std::ostringstream str;
		if (object != NULL)
			str << object->name() << ":";
		str << "\n\tA " << "sca_tdf::sca_ltf_ss"
				<< " must be used only in the context of a "
				<< "sca_tdf::sca_module (SCA_TDF_MODULE). " << std::endl;

		SC_REPORT_ERROR("SystemC-AMS", str.str().c_str());
	}

	delay_buffer = NULL;
	dc_init=true;

	dc_init_interval=false;

	s_intern_module_backup=NULL;         //module activation backup of state vector
    s_intern_tstep_module_backup=NULL;   //tstep backup of state vector
    s_intern_tstep_backup=NULL;          //module activation backup for tstep

    last_out_time=sca_core::sca_max_time();


    iterations_enabled=false;
    module_is_dynamic=false;

    estimate_cp_data=NULL;
}

sca_ct_vector_ss_proxy::~sca_ct_vector_ss_proxy()
{
	free_checkpoint_data(&s_intern_module_backup);
	free_checkpoint_data(&s_intern_tstep_module_backup);
	free_checkpoint_data(&s_intern_tstep_backup);
	free_checkpoint_data(&estimate_cp_data);
}


//////////////////////////////////////////////////////////////////

void sca_ct_vector_ss_proxy::set_max_delay(sca_core::sca_time mdelay)
{
	if (delay_buffer != NULL)
	{
		delete delay_buffer;
		delay_buffer = NULL;
	}

	max_delay = mdelay;
	if (mdelay != sc_core::SC_ZERO_TIME)
	{
		delay_buffer = new sca_ct_delay_buffer<sca_util::sca_vector<double> >;
		delay_buffer->set_max_delay(mdelay);
	}
}


inline bool sca_ct_vector_ss_proxy::matrix_changed(const sca_util::sca_matrix<
		double>& matrix, sca_util::sca_matrix<double>& matrix_old,
		const sca_util::sca_matrix<double>*& matrix_old_ref)
{
	//was there a write access or has the object changed since the last call
	if (matrix.get_access_flag() || (&matrix_old != matrix_old_ref))
	{
		//has the dimension changed since the last call
		if ((matrix.n_cols() != matrix_old.n_cols()) || (matrix.n_rows()
				!= matrix_old.n_rows()))
		{
			return true;
		}

		//has a value changed since the last call
		for (unsigned int i = 0; i < matrix.n_cols(); i++)
			for (unsigned int j = 0; j < matrix.n_rows(); j++)
			{
				if (matrix(j, i) != matrix_old(j, i))
					return true;
			}

	}

	return false;
}

//quick check for coefficient change
inline bool sca_ct_vector_ss_proxy::coeff_changed(const sca_util::sca_matrix<
		double>& a, const sca_util::sca_matrix<double>& b,
		const sca_util::sca_matrix<double>& c, const sca_util::sca_matrix<
				double>& d)
{
	if (matrix_changed(a, a_old, a_old_ref) || matrix_changed(b, b_old,
			b_old_ref) || matrix_changed(c, c_old, c_old_ref)
			|| matrix_changed(d, d_old, d_old_ref))
	{
		a_old = a;
		b_old = b;
		c_old = c;
		d_old = d;

		a_old_ref = &a;
		b_old_ref = &b;
		c_old_ref = &c;
		d_old_ref = &d;

		const_cast<sca_util::sca_matrix<double>*> (&a)->reset_access_flag();
		const_cast<sca_util::sca_matrix<double>*> (&b)->reset_access_flag();
		const_cast<sca_util::sca_matrix<double>*> (&c)->reset_access_flag();
		const_cast<sca_util::sca_matrix<double>*> (&d)->reset_access_flag();

		return true;
	}

	return false;

}

inline void sca_ct_vector_ss_proxy::setup_equation_system()
{
	number_of_equations = a_old.n_cols();

	//preparing memory for holding the two equation systems
	unsigned long new_memsize;
	new_memsize = 2 * number_of_equations * number_of_equations
			+ number_of_equations;

	if (new_memsize != memsize)
	{
		if (q_ss != NULL)
			delete[] q_ss;
		q_ss = new double[new_memsize];

		if (q_ss == NULL)
		{
			std::ostringstream str;
			str << "Can't allocate enough memory for sca_ltf_ss:  "
					<< ltf_object->name() << " required size: " << memsize
					<< " * " << sizeof(double) << " Bytes" << std::endl;
			SC_REPORT_ERROR("SystemC-AMS", str.str().c_str());
		}

		memsize = new_memsize;
	}

	a_ss = q_ss + number_of_equations;
	b_ss = a_ss + number_of_equations * number_of_equations;

	if(new_memsize>0)
	{
		std::memset((void*) (q_ss), 0, new_memsize * sizeof(double)); //set all to zero
	}


	// 0 = ds - As
	double* b_ref = b_ss;
	for (unsigned long m = 0; m < b_old.n_rows(); ++m)
	{
		a_ss[m * (number_of_equations + 1)] = 1.0; //ds

		double* a = a_old.get_flat() + m * a_old.n_cols(); // row m
		for (unsigned long n = 0; n < a_old.n_cols(); ++n, ++b_ref)
		{
			*b_ref = -a[n]; // -a[n,m]*s
		}
	}

}

inline void sca_ct_vector_ss_proxy::initialize_equation_system(int init_mode,
		double h)
{

	if(h<1e-15) //for zero timesteps perform dc-step
	{
		dc_init=true;
		return;
	}

	//if no difference do not initialize
	if((fabs(h - last_h) < 1e-15) && !reinit_request)
	{
		return;
	}

	last_h=h;

	if (number_of_equations > 0)
	{
		int err = ana_reinit(a_ss, b_ss, number_of_equations, h, &sdata,
				init_mode);
		if (err)
		{
			std::ostringstream str;
			str << ltf_object->name()
					<< ":\n\tInitializing DAE system returned code: " << err
					<< ", step size = " << sca_core::sca_time(h,
					sc_core::SC_SEC) << "." << std::endl;
			SC_REPORT_ERROR("SystemC-AMS", str.str().c_str());
		}
	}

}

inline void sca_ct_vector_ss_proxy::initialize()
{
	unsigned long ax, ay, bx, by, cx, cy, dx, dy;

	//if internal S is used it will be reset for
	//every re-initialization
	s_intern.remove();

	ax = a_old.n_cols();
	ay = a_old.n_rows();
	bx = b_old.n_cols();
	by = b_old.n_rows();
	cx = c_old.n_cols();
	cy = c_old.n_rows();
	dy = d_old.n_rows();
	dx = d_old.n_cols();

	if (ay != by)
	{
		std::ostringstream str;
		str << ltf_object->name()
				<< ":\n\tDimension mismatch with matrices A and B."
				<< std::endl;
		SC_REPORT_ERROR("SystemC-AMS", str.str().c_str());
	}
	if ((ax != cx) & (cy > 0))
	{
		std::ostringstream str;
		str << ltf_object->name()
				<< ":\n\tDimension mismatch with matrices A and C."
				<< std::endl;
		SC_REPORT_ERROR("SystemC-AMS", str.str().c_str());
	}

	if (ax != ay)
	{
		std::ostringstream str;
		str << ltf_object->name() << ":\n\tMatrix A must be square."
				<< std::endl;
		SC_REPORT_ERROR("SystemC-AMS", str.str().c_str());
	}

	if ((cy != dy) & (cx > 0))
	{
		std::ostringstream str;
		str << ltf_object->name()
				<< ":\n\tDimension mismatch with matrices C and D."
				<< std::endl;
		SC_REPORT_ERROR("SystemC-AMS", str.str().c_str());
	}

	if ((bx != dx) & (by > 0))
	{
		std::ostringstream str;
		str << ltf_object->name()
				<< ":\n\tDimension mismatch with matrices B and D."
				<< std::endl;
		SC_REPORT_ERROR("SystemC-AMS", str.str().c_str());
	}

	state_size = ax;
	unsigned long s_size = 0;

	if (statep != NULL)
	{
		if (statep->length() == 0)
		{
			statep->resize(state_size);
		}

		s_size = statep->length();
	}

	if (state_size != s_size)
	{
		std::ostringstream str;
		str << ltf_object->name() << ":\n\tThe size of the state vector ("
				<< s_size
				<< ") does not match the number of equations (e.g. dimension of square matrix A: "
				<< state_size << ")." << std::endl;
		SC_REPORT_ERROR("SystemC-AMS", str.str().c_str());
	}

	if(first_step)
	{
		//allocate memory for backup vectors
		ana_allocate_solver_check_point(state_size,&s_intern_module_backup);
		ana_allocate_solver_check_point(state_size,&s_intern_tstep_backup);
		ana_allocate_solver_check_point(state_size,&s_intern_tstep_module_backup);

		if((s_intern_module_backup==NULL) || (s_intern_tstep_backup==NULL) ||
			(s_intern_tstep_module_backup==NULL))
		{
			std::ostringstream str;
			str << "Can't allocate enough memory for: " << ltf_object->name();
			SC_REPORT_ERROR("SystemC-AMS",str.str().c_str());
		}
	}

}

inline sca_util::sca_vector<double> sca_ct_vector_ss_proxy::calculate(
		sca_util::sca_vector<double>& invalue)
{

	for(unsigned long i=0;i<invalue.length();i++)
	{
		//for estimate method
		last_delta_in(i)=invalue(i)-last_calculated_in(i);
	}
	last_calculated_in=invalue;

	s = statep->get_flat();



		double* b = b_old.get_flat();
		unsigned long by = b_old.n_rows();
		for (unsigned n = 0; n < by; ++n)
		{
			q_ss[n] = 0.0;
			for (unsigned m = 0; m < b_old.n_cols(); ++m)
			{
				//q_ss[n] -= b_old(m,n) * invalue[m];  // 0 = ds - As - Bx
				q_ss[n] -= b[m * by + n] * invalue[m]; // 0 = ds - As - Bx
			}
		}

		if (number_of_equations > 0) //solve fractional part
		{
			if(!dc_init) ana_solv(q_ss, s, sdata);
			dc_init=false;
		}



	unsigned long nout = (c_old.n_rows() == 0) ? d_old.n_rows()
			: c_old.n_rows();
	sca_util::sca_vector<double> tmp_out(nout);
	double* tmp_out_ref = tmp_out.get_flat();
	double* c_ref = c_old.get_flat();
	double* d_ref = d_old.get_flat();
	double* in_ref = invalue.get_flat();
	unsigned long cx = c_old.n_cols();
	unsigned long dx = d_old.n_cols();
	unsigned long dy = d_old.n_rows();

	for (unsigned long n = 0; n < dy; ++n, ++tmp_out_ref)
	{
		(*tmp_out_ref) = 0.0;
		for (unsigned long m = 0; m < cx; ++m)
		{
			//*tmp_out_ref+=c_old(m,n) * s[m];    // y = C*s
			*tmp_out_ref += c_ref[m * dy + n] * s[m]; // y = C*s
		}

		for (unsigned long m = 0; m < dx; ++m)
		{
			//*tmp_out_ref+=d_old(m,n) * in_ref[m];    // y = C*s + D*x
			*tmp_out_ref += d_ref[m * dy + n] * in_ref[m]; // y = C*s + D*x
		}
	}

	return tmp_out;
}


/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////

inline sca_util::sca_vector<double> sca_ct_vector_ss_proxy::get_in_value_by_index(
		unsigned long k)
{
	switch (intype)
	{
	case IN_SCALAR:
		return in_scalar;
	case IN_VECTOR:
	{
		sca_util::sca_vector<double> tmp((*in_vector).n_rows());
		for (unsigned long i = 0; i < tmp.length(); i++)
			tmp[i] = (*in_vector)(i,k);
		return tmp;
	}
	case IN_SCA_PORT:
		return in_sca_port->read(k);
	case IN_SC_PORT:
		return const_cast<sca_tdf::sca_de::sca_in<sca_util::sca_vector<double> >*> (in_sc_port)->read(
				k);
	default:
		SC_REPORT_ERROR("SystemC-AMS","Internal not possible error");
	}
	return sca_util::sca_vector<double>();
}

inline void sca_ct_vector_ss_proxy::write_out_value_by_index(
		const sca_util::sca_vector<double>& val, unsigned long k)
{
	switch (outtype)
	{
	case OUT_SCALAR:
		out_scalar = val;
		return;
	case OUT_VECTOR:
		for (unsigned long i = 0; i < (*out_vector).n_rows(); i++)
			(*out_vector)(i, k) = val(i);
		return;
	case OUT_SCA_PORT:
		out_sca_port->write(val, k);
		return;
	case OUT_SC_PORT:
		(out_sc_port)->write(val, k);
		return;
	default:
		SC_REPORT_ERROR("SystemC-AMS","Internal not possible error");
	}
	return;
}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

//general register_nd functionality
inline void sca_ct_vector_ss_proxy::register_ss_common(
		const sca_util::sca_matrix<double>& a,
		const sca_util::sca_matrix<double>& b,
		const sca_util::sca_matrix<double>& c,
		const sca_util::sca_matrix<double>& d,
		const sca_core::sca_time& tstep)
{
	//check, that the corresponding cast operator for the last call was executed
	if (pending_calculation)
	{
		std::ostringstream str;
		str
				<< "The previous ltf_nd calculation was performed without assigning"
					" the result for: " << ltf_object->name();
		SC_REPORT_ERROR("SystemC-AMS",str.str().c_str());
	}
	pending_calculation = true;

	if(!iterations_enabled && (tstep==sc_core::SC_ZERO_TIME))
	{
		std::ostringstream str;
		str << "the argument tstep can only be sc_core::SC_ZERO_TIME if the ";
		str << "method enable_iteration() was called before end of elaboration ";
		str << " (see LRM) for the ltf object: " << ltf_object->name();
		SC_REPORT_ERROR("SystemC-AMS",str.str().c_str());
	}

	//check for coefficient change
	reinit_request = coeff_changed(a, b, c, d);


	//if first execution inside a module call reset current time and
	//get module timestep
	sc_dt::int64 ccnt = *(parent_module->call_counter);
	if (ccnt != module_activations) //first activation in current modules call
	{
		current_time = sc_core::SC_ZERO_TIME;
		module_time_step = parent_module->get_timestep();
		module_current_time= parent_module->get_time();
		module_is_dynamic=parent_module->is_dynamic();
	}


	//if there is a delay or we have more than one sample we create a delay buffer
	if((delay_buffer==NULL) &&
			( (ct_in_delay>sc_core::SC_ZERO_TIME) ||
			  (number_of_in_values>1)
			) )
	{
		delay_buffer = new sca_ct_delay_buffer<sca_util::sca_vector<double> >;
		delay_buffer->set_max_delay(module_time_step);
	}

	//set limit of delay buffer to enable remove of old sample
	if(delay_buffer!=NULL)
	{
		//the minimum delay is always the module timestep
		if(max_delay<module_time_step)
			delay_buffer->set_max_delay(module_time_step);
		else
			delay_buffer->set_max_delay(max_delay);
	}

	//if module time zero we perform dc-steps only (we do not integrate
	if(module_current_time==sc_core::SC_ZERO_TIME)
	{
		dc_init=true;
	}

	//remaining time from last module activation
	sca_core::sca_time remainig_time(sc_core::SC_ZERO_TIME);

	//if tstep=sca_max_time -> calculate the whole timeintervall of the module
	if (tstep == sca_core::sca_max_time())
	{
		time_interval = module_time_step - current_time;
		calculate_to_end = true;
	}
	else //calculate only until tstep
	{
		time_interval = tstep;
		calculate_to_end = false;
	}

	//if not yet assigned calculate in_time_step
	if(in_time_step==sc_core::SC_ZERO_TIME)
		in_time_step = time_interval/number_of_in_values;
	first_in_time_step = in_time_step;

	if(ccnt != module_activations) //first activation of current module call
	{
		//first module activation
		if (ccnt == 0) // as long as first module activation last time is zero
		{              //we do not update module_activations
			last_calculated_in_time = sc_core::SC_ZERO_TIME - in_time_step*number_of_in_values;
			last_in_time_module_backup=last_calculated_in_time;
			last_in_time_module_tstep_backup=last_calculated_in_time;

			last_out_time_module_backup=last_out_time;
			last_out_time_module_tstep_backup=last_out_time;

			last_calculated_time_module_backup=last_calculated_time;
			last_calculated_time_module_tstep_backup=last_calculated_time;

			in_last_module_backup=in_last;
			in_last_module_tstep_backup=in_last;

			last_tstep_backup=time_interval;
			last_in_time_tstep_backup=last_calculated_in_time;
			last_out_time_tstep_backup=last_out_time;
			last_calculated_time_tstep_backup=last_calculated_time;
			in_last_tstep_backup=in_last;

			//the solver backup is initialized in initialize
		}
		else
		{
			if(module_is_dynamic)
			{
				//repeat module time step
				if((module_current_time<=last_calculated_time)||(module_time_step<=sc_core::SC_ZERO_TIME))
				{
					//the module timestep becomes the timestep of the last module execution
					module_time_step=parent_module->get_last_timestep();

					//reset the internal state vector to the module backup vector
					//s_intern=s_intern_module_backup;
					if(s_intern.length()>0) ana_restore_solver_check_point(sdata,s_intern.get_flat(),&s_intern_module_backup);

					//reset the last calculated time to the module backup value
					//(corresponds to the state vector)
					last_calculated_in_time=last_in_time_module_backup;
					last_out_time=last_out_time_module_backup;
					last_calculated_time=last_calculated_time_module_backup;
					in_last=in_last_module_backup;

					if(delay_buffer!=NULL)
					{
						delay_buffer->remove_timestamps_after(last_calculated_time);
					}

					if(iterations_enabled)
					{
						//due the module repeats the time interval we must reset the
						//tstep backup of the state vector to the value before
						//s_intern_tstep_backup=s_intern_tstep_module_backup;
						ana_copy_solver_check_point(&s_intern_tstep_backup,s_intern_tstep_module_backup);
						last_in_time_tstep_backup=last_in_time_module_tstep_backup;
						last_out_time_tstep_backup=last_out_time_module_tstep_backup;
						last_calculated_time_tstep_backup=last_calculated_time_module_tstep_backup;
						in_last_tstep_backup=in_last_module_tstep_backup;
					}

					//reset current time and correct time interval to calculate to end
					current_time=sc_core::SC_ZERO_TIME;
					if(calculate_to_end)
					{
						time_interval = module_time_step;
						in_time_step = time_interval/number_of_in_values;
						first_in_time_step = in_time_step;
					}
				}
				else //the module time progresses
				{
					if(iterations_enabled)
					{
						//store the statevector and time of the previous calculation
						//for the case, that the module timeinterval is repeated and
						//the tstep argument is zero
						//s_intern_tstep_module_backup=s_intern_tstep_backup;
						ana_copy_solver_check_point(&s_intern_tstep_module_backup,s_intern_tstep_backup);
						last_in_time_module_tstep_backup=last_in_time_tstep_backup;
						last_out_time_module_tstep_backup=last_out_time_tstep_backup;
						last_calculated_time_module_tstep_backup=last_calculated_time_tstep_backup;
						in_last_module_tstep_backup=in_last_tstep_backup;
					}

					//backup state vector
					//s_intern_module_backup=s_intern;
					if(s_intern.length()>0) ana_store_solver_check_point(sdata,s_intern.get_flat(),&s_intern_module_backup);
					last_in_time_module_backup=last_calculated_in_time;
					last_out_time_module_backup=last_out_time;
					last_calculated_time_module_backup=last_calculated_time;
					in_last_module_backup=in_last;
				}
			} //if(module_is_dynamic)


			//remaining time from last execution
			//current time - current module timestep -> current start time
			//current start time - last calculated time ->
			//                       remaining time from last module activation
			//remaining time from last execution + current tstep
			if(module_time_step<module_current_time)
			{   //calculate only if current time > 0
				remainig_time= module_current_time - module_time_step - last_calculated_in_time;
			}
			first_in_time_step = remainig_time + in_time_step;

			//used for new module activation detection (if not equal->new module activation)
			module_activations = ccnt;

			//store/restore solver checkpoint due tstep=0
			//tstep=0 and no remaining time for the first ltf activation of a module call
			if(iterations_enabled)
			{
				//tstep=0 and no remaining timeinterval ->
				//we must reset to state before previous execution
				if(first_in_time_step==sc_core::SC_ZERO_TIME)
				{
					//reset statevector to value before last calculation
					//s_intern=s_intern_tstep_backup;
					if(s_intern.length()>0) ana_restore_solver_check_point(sdata,s_intern.get_flat(),&s_intern_tstep_backup);
					last_calculated_in_time=last_in_time_tstep_backup;
					last_out_time=last_out_time_tstep_backup;
					last_calculated_time=last_calculated_time_tstep_backup;
					in_last=in_last_tstep_backup;

					if(delay_buffer!=NULL)
					{
						delay_buffer->remove_timestamps_after(last_calculated_time);
					}

					//the remaining time is the difference between the current time
					//and the last calculated time - the current time step, due
					//we  are at the start of the current interval (the current time is the end)
					if(module_time_step<module_current_time)
					{
						remainig_time=module_current_time-last_calculated_in_time -module_time_step;
					}

					in_time_step = remainig_time/number_of_in_values;
					first_in_time_step = in_time_step;
				}
				else //local (tstep) time progresses
				{
					//backup current calculation state
					//s_intern_tstep_backup=s_intern;
					if(s_intern.length()>0) ana_store_solver_check_point(sdata,s_intern.get_flat(),&s_intern_tstep_backup);
					last_in_time_tstep_backup=last_calculated_in_time;
					last_out_time_tstep_backup=last_out_time;
					last_calculated_time_tstep_backup=last_calculated_time;
					in_last_tstep_backup=in_last;

				}

				last_tstep_backup=time_interval;
			} //if(iterations_enabled)

		} // if(cnt==0) ... else {
	} //if(ccnt != module_activations)
	else //following calculation in the same module activation
	{
		if(iterations_enabled)
		{
			//tstep=0 for following call
			//repeat time step
			if(time_interval==sc_core::SC_ZERO_TIME)
			{
				//at start of module time interval
				if(current_time==sc_core::SC_ZERO_TIME)
				{
					//restore state vector and time
					//s_intern=s_intern_tstep_backup;
					if(s_intern.length()>0) ana_restore_solver_check_point(sdata,s_intern.get_flat(),&s_intern_tstep_backup);
					last_calculated_in_time=last_in_time_tstep_backup;
					last_out_time=last_out_time_tstep_backup;
					last_calculated_time=last_calculated_time_tstep_backup;
					in_last=in_last_tstep_backup;

					if(delay_buffer!=NULL)
					{
						delay_buffer->remove_timestamps_after(last_calculated_time);
					}

					//the time interval is the difference between the current time
					//and the last calculated time
					if(module_time_step<module_current_time)
					{
						remainig_time=module_current_time-last_calculated_in_time-module_time_step;
					}
					first_in_time_step = remainig_time/number_of_in_values;

					time_interval=sc_core::SC_ZERO_TIME;
					in_time_step = time_interval;
				}
				else  //restore due tstep zero
				{
					//the time interval is the difference between the last and the current
					time_interval=last_calculated_time-last_calculated_time_tstep_backup;

					//reset current_time
					current_time-=time_interval;

					//restore state vector and time
					//s_intern=s_intern_tstep_backup;
					if(s_intern.length()>0) ana_restore_solver_check_point(sdata,s_intern.get_flat(),&s_intern_tstep_backup);
					last_calculated_in_time=last_in_time_tstep_backup;
					last_out_time=last_out_time_tstep_backup;
					last_calculated_time=last_calculated_time_tstep_backup;
					in_last=in_last_tstep_backup;

					if(delay_buffer!=NULL)
					{
						delay_buffer->remove_timestamps_after(last_calculated_time);
					}

					first_in_time_step = time_interval/number_of_in_values;
					in_time_step       = first_in_time_step;
				}
			}
			else //time progresses - backup for tstep restore
			{
				//backup tstep
				//s_intern_tstep_backup=s_intern;
				if(s_intern.length()>0) ana_store_solver_check_point(sdata,s_intern.get_flat(),&s_intern_tstep_backup);
				last_in_time_tstep_backup=last_calculated_in_time;
				last_out_time_tstep_backup=last_out_time;
				last_calculated_time_tstep_backup=last_calculated_time;
				in_last_tstep_backup=in_last;
			}
		}
	}


	if((first_in_time_step==sc_core::SC_ZERO_TIME)||(ccnt==0))
	{   //all activation in the first call considered as dc-steps
		dc_init=true;
	}

	if (current_time + time_interval > module_time_step)
	{
		std::ostringstream str;
		str << "The requested timestep ( " << tstep
				<< " ) is larger than the time distance between "
					"the time reached by the last execution and the time of the "
					"current module activation (see LRM) for: "
				<< ltf_object->name() << " ltf time since activation: "
				<< current_time << " module timestep: " << module_time_step;
		SC_REPORT_ERROR("SystemC-AMS",str.str().c_str());
	}





	if ((delay_buffer == NULL)||(max_delay==sc_core::SC_ZERO_TIME))
	{
		if (ct_in_delay > time_interval)
		{
			std::ostringstream str;
			str << "The parameter ct_delay (" << ct_in_delay << ") of: "
					<< ltf_object->name()
					<< " must be smaller or equal than the calculated time interval: "
					<< time_interval
					<< " otherwise you have to specify the maximum delay by the method: "
					<< " set_max_delay (see LRM for details)";
			SC_REPORT_ERROR("SystemC-AMS",str.str().c_str());
		}
	}
	else // delay_buffer != NULL
	{
		if (ct_in_delay > max_delay)
		{
			std::ostringstream str;
			str << "The parameter ct_delay (" << ct_in_delay << ") of: "
					<< ltf_object->name()
					<< " must be smaller or equal than the maximum delay: "
					<< max_delay
					<< " set by the method: set_max_delay (see LRM for details)";
			SC_REPORT_ERROR("SystemC-AMS",str.str().c_str());
		}
	}
}




/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////

//scalar vector input
void sca_ct_vector_ss_proxy::register_ss(
		const sca_util::sca_matrix<double>& a,
		const sca_util::sca_matrix<double>& b,
		const sca_util::sca_matrix<double>& c,
		const sca_util::sca_matrix<double>& d,
		sca_core::sca_time ct_delay,
		sca_util::sca_vector<double>& state,
		const sca_util::sca_vector<double>& x,
		sca_core::sca_time tstep)
{
	number_of_in_values=1;
	in_time_step=sc_core::SC_ZERO_TIME; //will be calculated later

	intype = IN_SCALAR;
	in_scalar = x;
	number_of_in_values = 1;
	in_index_offset = 0;

	statep = &state;
	ct_in_delay = ct_delay;

	register_ss_common(a,b,c,d,tstep);

	last_available_in_time=module_current_time+current_time;
	if(delay_buffer!=NULL)
	{
		//current_time is time since current module activation
		delay_buffer->store_value(last_available_in_time, x);
	}

	current_time += time_interval;
}


//matrix (vector) vector input
void sca_ct_vector_ss_proxy::register_ss(
		const sca_util::sca_matrix<double>& a,
		const sca_util::sca_matrix<double>& b,
		const sca_util::sca_matrix<double>& c,
		const sca_util::sca_matrix<double>& d,
		sca_core::sca_time ct_delay,
		sca_util::sca_vector<double>& state,
		const sca_util::sca_matrix<double>& x,
		sca_core::sca_time tstep)
{
	number_of_in_values=x.n_cols();
	in_time_step=sc_core::SC_ZERO_TIME; //will be calculated later

	in_index_offset = 0;
	intype = IN_VECTOR;
	in_vector = &x;

	statep = &state;
	ct_in_delay = ct_delay;

	register_ss_common(a,b,c,d,tstep);


	last_available_in_time=module_current_time+current_time;
	if(delay_buffer!=NULL)
	{
		//current_time is time since current module activation
		sca_core::sca_implementation::sca_signed_time ctime=last_available_in_time;
		for (unsigned i = 0; i < number_of_in_values; i++)
		{
			sca_core::sca_implementation::sca_signed_time stime=(number_of_in_values-1-i) * in_time_step;

			last_available_in_time=ctime-stime;

			sca_util::sca_vector<double> tmp(x.n_rows());
			for (unsigned long j = 0; j < tmp.length(); j++) tmp(j) = x(j, i);

			delay_buffer->store_value(last_available_in_time,tmp);
		}
	}

	current_time += time_interval;

	in_last.resize(b.n_cols()?b.n_cols():d.n_cols());
}

//////////////////////////////////////////////////////////////////////////////

//port input
inline void sca_ct_vector_ss_proxy::register_ss_port(
		const sca_util::sca_matrix<double>& a,
		const sca_util::sca_matrix<double>& b,
		const sca_util::sca_matrix<double>& c,
		const sca_util::sca_matrix<double>& d,
		sca_core::sca_time&                 ct_delay,
		sca_util::sca_vector<double>&       state,
		unsigned long                       port_rate,
		sca_core::sca_time&                 first_port_timestep,
		sca_core::sca_time&                 port_timestep,
		sca_util::sca_vector<double>        port_value)
{

	number_of_in_values=port_rate;
	in_time_step=first_port_timestep;

	in_index_offset = 0;

	statep = &state;
	ct_in_delay = ct_delay;

	register_ss_common(a,b,c,d,sca_core::sca_max_time());

	//if a delay buffer available (we can't delete pending values or if may
	//later a delay is set) or if ct_delay > zero we use the buffer
	//or if port_rate>1 due in this case the time goes may beyond the last
	//out sample
	if(delay_buffer!=NULL )
	{
		sca_core::sca_time ctime=sc_core::SC_ZERO_TIME;
		for(unsigned long i=0;i<port_rate;i++)
		{
			//only timepoints after/equal to the current time will be stored
			if(ctime>=current_time)
			{
				delay_buffer->store_value(module_current_time+ctime, get_in_value_by_index(i));
			}
			ctime+=port_timestep;
		}
	}

	last_available_in_time=module_current_time+(port_rate-1)*port_timestep;
	current_time += time_interval;

	in_last.resize(b.n_cols()?b.n_cols():d.n_cols());

}


//////////////////////////////////////////

//sca_tdf::sca_port
void sca_ct_vector_ss_proxy::register_ss(
		const sca_util::sca_matrix<double>& a,
		const sca_util::sca_matrix<double>& b,
		const sca_util::sca_matrix<double>& c,
		const sca_util::sca_matrix<double>& d,
		sca_core::sca_time ct_delay,
		sca_util::sca_vector<double>& state,
		const sca_tdf::sca_in<sca_util::sca_vector<double> >& x)
{
	intype = IN_SCA_PORT;
	in_sca_port = &x;

	unsigned long      port_rate     = x.get_rate();
	sca_core::sca_time first_port_timestep = x.get_timestep();
	sca_core::sca_time port_timestep=first_port_timestep;
	sca_util::sca_vector<double>   port_value    = x.read();


	if(port_rate>1)
	{
		port_timestep=x.get_timestep(1);
	}

	register_ss_port(a,b,c,d,ct_delay,state,port_rate,first_port_timestep,port_timestep,port_value);
}


//converter port input
void sca_ct_vector_ss_proxy::register_ss(
		const sca_util::sca_matrix<double>& a,
		const sca_util::sca_matrix<double>& b,
		const sca_util::sca_matrix<double>& c,
		const sca_util::sca_matrix<double>& d,
		sca_core::sca_time ct_delay,
		sca_util::sca_vector<double>& state,
		const sca_tdf::sca_de::sca_in<sca_util::sca_vector<double> >& x)
{
	intype = IN_SC_PORT;
	in_sc_port = &x;

	unsigned long      port_rate     = x.get_rate();
	sca_core::sca_time first_port_timestep = x.get_timestep();
	sca_core::sca_time port_timestep=first_port_timestep;
	sca_util::sca_vector<double>   port_value    =
			const_cast<sca_tdf::sca_de::sca_in<sca_util::sca_vector<double> >*>(&x)->read();


	if(port_rate>1)
	{
		port_timestep=x.get_timestep(1);
	}

	register_ss_port(a,b,c,d,ct_delay,state,port_rate,first_port_timestep,port_timestep,port_value);
}


//////////////////////////////////////////////////////////////////////////////

void sca_ct_vector_ss_proxy::register_ss(
		const sca_util::sca_matrix<double>& a,
		const sca_util::sca_matrix<double>& b,
		const sca_util::sca_matrix<double>& c,
		const sca_util::sca_matrix<double>& d,
		sca_core::sca_time ct_delay,
		const sca_util::sca_vector<double>& x,
		sca_core::sca_time tstep)
{
	if (s_intern.length() != state_size)
		s_intern.resize(0);
	register_ss(a, b, c, d, ct_delay, s_intern, x, tstep);
}


//////////////////

void sca_ct_vector_ss_proxy::register_ss(
		const sca_util::sca_matrix<double>& a,
		const sca_util::sca_matrix<double>& b,
		const sca_util::sca_matrix<double>& c,
		const sca_util::sca_matrix<double>& d,
		sca_core::sca_time ct_delay,
		const sca_util::sca_matrix<double>& x,
		sca_core::sca_time tstep)
{
	if (s_intern.length() != state_size)
		s_intern.resize(0);
	register_ss(a, b, c, d, ct_delay, s_intern, x, tstep);
}

///////////////////////

void sca_ct_vector_ss_proxy::register_ss(
		const sca_util::sca_matrix<double>& a,
		const sca_util::sca_matrix<double>& b,
		const sca_util::sca_matrix<double>& c,
		const sca_util::sca_matrix<double>& d,
		sca_core::sca_time ct_delay,
		const sca_tdf::sca_in<sca_util::sca_vector<double> >& x)
{
	if (s_intern.length() != state_size)
		s_intern.resize(0);
	register_ss(a, b, c, d, ct_delay, s_intern, x);
}

//////////////////////////

void sca_ct_vector_ss_proxy::register_ss(
		const sca_util::sca_matrix<double>& a,
		const sca_util::sca_matrix<double>& b,
		const sca_util::sca_matrix<double>& c,
		const sca_util::sca_matrix<double>& d,
		sca_core::sca_time ct_delay,
		const sca_tdf::sca_de::sca_in<sca_util::sca_vector<double> >& x)
{
	if (s_intern.length() != state_size)
		s_intern.resize(0);
	register_ss(a, b, c, d, ct_delay, s_intern, x);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

const sca_util::sca_vector<double>& sca_ct_vector_ss_proxy::convert_to_double_vector()
{
	if (!pending_calculation)
	{
		std::ostringstream str;
		str << "The the ltf_ss proxy object is may assigned twice for: "
				<< ltf_object->name();
		SC_REPORT_ERROR("SystemC-AMS",str.str().c_str());
	}
	pending_calculation = false;

	int init_mode = 0;
	double h = first_in_time_step.to_seconds();

	if (first_step)
	{
		init_mode = 0; //create datastructure, equation system, first step euler
		initialize();
		setup_equation_system();
		initialize_equation_system(init_mode, h);
	}
	else if (reinit_request)
	{
		init_mode = 1; //rebuilt equations, first step euler
		initialize();
		setup_equation_system();
		initialize_equation_system(init_mode, h);
	}

	reinit_request=false;

	//resize in_val vector to required size
	unsigned long nout=c_old.n_rows()?c_old.n_rows():d_old.n_rows();
	unsigned long nin = (b_old.n_cols() == 0) ? d_old.n_cols(): b_old.n_cols();
	sca_util::sca_vector<double> in_val(nin);
	scalar_out.resize(nout);

	if (delay_buffer == NULL)
	{
		if (ct_in_delay == sc_core::SC_ZERO_TIME)
		{

			in_last = get_in_value_by_index(in_index_offset);
			last_calculated_in_time += first_in_time_step;
			last_calculated_time=last_calculated_in_time;

			//if required (h changed) re-initialize equation system
			initialize_equation_system(2, h);

			scalar_out = calculate(in_last);

			first_step = false;
		}
		else // if delay - more complicated (effort)
		{
			//get in value at current time
			sca_util::sca_vector<double> tmp = get_in_value_by_index(in_index_offset);

			//calculate delayed in value
			for (unsigned long i = 0; i < nin; i++) //calculate delayed signals
			{
				if (first_step) //the first value must be the initial value (before invalue)
				{
					in_val[i] = tmp[i];
				}
				else
				{
					in_val[i] = (tmp[i] - in_last[i])
							/ (first_in_time_step.to_seconds())
							* (first_in_time_step.to_seconds() - ct_in_delay.to_seconds()) + in_last[i];
				}
			}

			h = (first_in_time_step - ct_in_delay).to_seconds();

			//if required (h changed) re-initialize equation system
			initialize_equation_system(2, h);

			out_scalar = calculate(in_val); //calculate outvalues for current time
			first_step = false;

			h = ct_in_delay.to_seconds(); //calculate states for current invalue

			//if required (h changed) re-initialize equation system
			initialize_equation_system(2, h);

			in_last = tmp;
			tmp = calculate(in_last);
			last_calculated_in_time += first_in_time_step;
			last_calculated_time=last_calculated_in_time;
		}

		//There are further in timesteps available
		if (number_of_in_values > in_index_offset + 1)
		{
			h = in_time_step.to_seconds(); //calculate following timesteps for in values

			//if required (h changed) re-initialize equation system
			initialize_equation_system(2, h);

			for (unsigned long i = in_index_offset + 1; i < number_of_in_values; i++)
			{
				in_last = get_in_value_by_index(i);
				calculate(in_last);
				last_calculated_in_time += in_time_step;
				last_calculated_time=last_calculated_in_time;
			}
		}
	}
	else //delay_buffer!=NULL
	{
		sca_core::sca_implementation::sca_signed_time next_out_time;
		if(last_out_time==sca_core::sca_max_time()) //first call
		{
			next_out_time=parent_module->get_time();
			last_calculated_time=last_calculated_in_time;
		}
		else
		{
			//timestamp of output value
			next_out_time=last_out_time+time_interval;
		}

		//calculate until output value has been reached
		while(true)
		{
			sca_core::sca_implementation::sca_signed_time sample_timestamp;


			sca_util::sca_vector<double> value;

			bool no_next_value=delay_buffer->get_next_value_after(sample_timestamp, value, last_calculated_time-ct_in_delay);
			sca_core::sca_implementation::sca_signed_time
								value_timestamp=sample_timestamp+ct_in_delay;


			//the last value in buffer is equal to the requested out timestamp
			if(value_timestamp==next_out_time)
			{   //calculate outvalue
				h=(value_timestamp-last_calculated_time).to_seconds();
				initialize_equation_system(2, h);
				scalar_out=calculate(value);
				last_calculated_time=next_out_time;
				last_calculated_in_time=sample_timestamp;
				break; //we are ready
			}

			if(no_next_value)
			{
					std::ostringstream str;
					str << "Internal error due a bug: " << last_calculated_time
							<< " out time: " << next_out_time
							<< " last out time: " << last_out_time
							<< " time interval " << time_interval;
					SC_REPORT_ERROR("SystemC-AMS",str.str().c_str());
					return scalar_out;
			}

			if((value_timestamp<next_out_time)&&(value_timestamp>=sc_core::SC_ZERO_TIME))
			{   //integrate to invalue - ignore values with negative time
				h=(value_timestamp-last_calculated_time).to_seconds();
				initialize_equation_system(2, h);
				calculate(value);
				last_calculated_time=value_timestamp;
				last_calculated_in_time=sample_timestamp;
			}
			else //value_timestamp>next_out_time
			{    //we must interpolate
				if(delay_buffer->get_value(next_out_time-ct_in_delay,value))
				{
					std::ostringstream str;
					str << "Internal error due a bug";
					SC_REPORT_ERROR("SystemC-AMS",str.str().c_str());
					return scalar_out;
				}

				h=(next_out_time-last_calculated_time).to_seconds();
				initialize_equation_system(2, h);
				scalar_out=calculate(value);
				last_calculated_time=next_out_time;
				break; //we are ready
			}
		}

		delay_buffer->set_time_reached(last_calculated_in_time);
		last_out_time=next_out_time;
	}

	first_step = false;
	return scalar_out;
}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

inline void sca_ct_vector_ss_proxy::calculate_timeinterval(
		unsigned long& current_in_index, long& current_out_index,
		long number_of_out_values, sca_core::sca_implementation::sca_signed_time& next_in_time,
		sca_core::sca_implementation::sca_signed_time& next_out_time,
		sca_core::sca_time& next_in_time_step,
		sca_core::sca_time& out_time_step)
{
	if (!pending_calculation)
	{
		std::ostringstream str;
		str << "The the ltf_nd proxy object is may assigned twice for: "
				<< ltf_object->name();
		SC_REPORT_ERROR("SystemC-AMS",str.str().c_str());
	}
	pending_calculation = false;


	int init_mode = 0;
	double h = first_in_time_step.to_seconds();

	if (first_step)
	{
		init_mode = 0; //create datastructure, equation system, first step euler
		initialize();
		setup_equation_system();

		initialize_equation_system(init_mode, h);

		//we propagate the first value to negative time
		in_last=get_in_value_by_index(0);
	}
	else if (reinit_request)
	{
		init_mode = 1; //rebulit equations, first step euler
		initialize();
		setup_equation_system();

		initialize_equation_system(init_mode, h);
	}

	first_step = false;

#ifdef SCA_DEBUG_LTF_SS
	std::cout << "---------------------------------------------------" << std::endl;
#endif

	if (delay_buffer == NULL)
	{
		//we calculate until all out values
		while (current_out_index < number_of_out_values)
		{
			if(dc_init_interval)
			{
				dc_init=true;
			}

			//in and out at the same timepoint (error smaller than 1 resolution time
			//step due timestep calculation may introduces those error
			if (sc_dt::sc_abs(sc_dt::int64((next_in_time.value() + ct_in_delay.value()
					- next_out_time.value()))) <= 1)
			{

				in_last = get_in_value_by_index(current_in_index);
				current_in_index++;
				last_calculated_in_time = next_in_time;
				next_in_time += next_in_time_step;
				next_in_time_step = in_time_step;

				if (number_of_out_values > current_out_index)
				{
					sca_core::sca_time dt = next_out_time - last_calculated_time;
					h
							= (dt == sc_core::SC_ZERO_TIME) ? in_time_step.to_seconds()
									: dt.to_seconds();


					//if required (h changed) re-initialize equation system
					initialize_equation_system(2, h);

					sca_util::sca_vector<double> outp;
					outp = calculate(in_last);

					write_out_value_by_index(outp, current_out_index);
					last_out_time=next_out_time;


					current_out_index++;
					next_out_time += out_time_step;
					last_calculated_time += dt;
				}

			}
			//next time in value only -> ignore result
			else if (next_in_time+ct_in_delay < next_out_time)
			{
				sca_core::sca_time dt = next_in_time+ct_in_delay - last_calculated_time;
				h = (dt == sc_core::SC_ZERO_TIME) ? in_time_step.to_seconds()
						: dt.to_seconds();


				//if required (h changed) re-initialize equation system
				initialize_equation_system(2, h);

				//calculate ltf_ss for in_value - out not used
				in_last = get_in_value_by_index(current_in_index);
				calculate(in_last);

				current_in_index++;
				last_calculated_time += dt;
				last_calculated_in_time = next_in_time;
				next_in_time += next_in_time_step;
				next_in_time_step = in_time_step;
			}
			else //next time calculate out value -> interpolate in (in time after next out time)
			{
				sca_core::sca_time dt = next_out_time+ct_in_delay - last_calculated_time;
				h = (dt == sc_core::SC_ZERO_TIME) ? in_time_step.to_seconds()
						: dt.to_seconds();

				//the calculated last time equals to the output time
				last_calculated_time = next_out_time;


				//interpolation IMPROVE: may higher order??
				unsigned long dimy=c_old.n_rows()>0?c_old.n_rows():d_old.n_rows();
				sca_util::sca_vector<double> inp(dimy);
				sca_util::sca_vector<double> tmp = get_in_value_by_index(current_in_index);

				for (unsigned long i = 0; i < dimy; i++)
				{
					inp[i] = (tmp[i] - in_last[i])
							/ (next_in_time_step.to_seconds())
							* (next_out_time - last_calculated_in_time).to_seconds() + in_last[i];
				}

				//if required (h changed) re-initialize equation system
				initialize_equation_system(2, h);

				sca_util::sca_vector<double> outp = calculate(inp);

				write_out_value_by_index(outp, current_out_index);

				last_out_time=next_out_time;

				current_out_index++;
				next_out_time += out_time_step;

			}
#ifdef SCA_DEBUG_LTF_SS
			std::cout << "ct: " << last_calculated_time << "  not: " << next_out_time
			<< " nit: " << next_in_time << " coi: "
			<< current_out_index << " cii: " << current_in_index
			<< " last_in_time: " << last_calculated_in_time << " h: " << h << std::endl;
#endif
		} //while(current_in_index < number_of_in_values)

	}
	else //delay_buffer!=NULL
	{
		unsigned long nin = (b_old.n_cols() == 0) ? d_old.n_cols()
				: b_old.n_cols();

		long outcnt = current_out_index;

		//last calculated time
		sca_core::sca_implementation::sca_signed_time last_delay_time = last_calculated_time-ct_in_delay;

		//calculate all output sample
		while (outcnt < number_of_out_values)
		{

			sca_util::sca_vector<double> value(nin);
			sca_core::sca_implementation::sca_signed_time ntime;

			//it exists a value after last calculated time and for this value the
			//time is smaller than the next out time - there are input values
			//before the next out time
			if((!delay_buffer->get_next_value_after(ntime, value, last_delay_time)) &&
			   (ntime < (next_out_time-ct_in_delay)))
			{
				//in this case we integrate to this input value

				h = (ntime+ct_in_delay - last_calculated_time).to_seconds();

				//if required (h changed) re-initialize equation system
				initialize_equation_system(2, h);
				last_delay_time=ntime;
				calculate(value);

				last_calculated_time = ntime+ct_in_delay;
				last_calculated_in_time=ntime;
			}
			else //the next value is after or at the next out time
			{
				if (ntime != (next_out_time-ct_in_delay))
				{   //if the next value is after the next out time, we must
					//interpolate
					if(delay_buffer->get_value(next_out_time-ct_in_delay,value))
					{
						//we cannot interpolate due there is no value after the
						//next out time - this should never happen, due we check
						//this in the register methods
						std::ostringstream str;
						str << "Internal Error due a Bug ";
						str << "next_out_time: " << next_out_time << " ct_in_delay: "
							<< ct_in_delay << " next_out_time-ct_in_delay: "
							<< next_out_time-ct_in_delay << " in: " << ltf_object->name();
						SC_REPORT_ERROR("SystemC-AMS",str.str().c_str());
					}
				}
				else //otherwise we use the read value
				{
					last_calculated_in_time=ntime;
				}

				//last calculated in value
				last_delay_time=next_out_time-ct_in_delay;

				h = (next_out_time - last_calculated_time).to_seconds();

				//if required (h changed) re-initialize equation system
				initialize_equation_system(2, h);
				sca_util::sca_vector<double> outp = calculate(value);

				write_out_value_by_index(outp, current_out_index);
				last_out_time=next_out_time;

				current_out_index++;
				last_calculated_time = next_out_time;
				delay_buffer->set_time_reached(next_out_time);
				next_out_time += out_time_step;
				outcnt++;
			}
		} //while (outcnt < number_of_out_values)
	} //else delay_buffer!=NULL
}


/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////

inline void sca_ct_vector_ss_proxy::convert_to_port(
		sca_core::sca_time& first_port_timestep,
		sca_core::sca_time& port_timestep,
		sca_core::sca_time& port_time,
		unsigned long       port_rate
		)
{

	sca_core::sca_time out_time_step = port_timestep;
	sca_core::sca_implementation::sca_signed_time next_out_time = port_time;


	unsigned long current_in_index = in_index_offset;

	//calculate out index offset if the calculation does not start with
	//the module start time
	long current_out_index = 0;
	sc_dt::int64 int_next_out_time = next_out_time.value();

	sc_dt::int64 int_last_out_time = last_out_time.value();
	if(last_out_time==sca_core::sca_max_time())
	{
		int_last_out_time=0;
	}

	if ((int_next_out_time <= int_last_out_time)&&(int_last_out_time!=0))
	{
		current_out_index = (int_last_out_time - int_next_out_time
				+ out_time_step.value() - 1) / out_time_step.value()+1;
		next_out_time += out_time_step * current_out_index;
	}

	sca_core::sca_implementation::sca_signed_time next_in_time;

	if(dc_init_interval)
	{
		next_in_time=sc_core::SC_ZERO_TIME;
	}
	else
	{
		next_in_time= last_calculated_in_time + first_in_time_step;
	}

	//timestep after first step - equals to propagated step
	sca_core::sca_time next_in_time_step = in_time_step;

	//enforce causality if out rate > in rate
	if (calculate_to_end)
	{
		sca_core::sca_time out_end = port_time + out_time_step * (port_rate - 1);
		sca_core::sca_time in_end = last_available_in_time + ct_in_delay;


#ifdef SCA_DEBUG_LTF_SS
		std::cout << this->ltf_object->name() << " oend; " << out_end << " iend: " << in_end << " out_time_step: " << out_time_step << " port_rate: " << port_rate
				<< " next_in_time: " << next_in_time << "  number_of_in_values  " << number_of_in_values << " in_time_step: "<< in_time_step
				<< " last_in_time " << last_calculated_in_time << " first_in_time_step  " << first_in_time_step << " current_in_index: " << current_in_index
				<< " next_out_time: "<< next_out_time << " current_out_index " << current_out_index
				<< std::endl;
#endif
		if (out_end > in_end)
		{
			//required overall delay (missing + set)
			sca_core::sca_time in_causal_delay;
			in_causal_delay = out_end - in_end + ct_in_delay;

			std::ostringstream str;
			str
					<< "The input time intervall ends before the requested output time "
					<< "intervall for: " << ltf_object->name()
					<< " to achieve causality set the parameter ct_delay at least to : "
					<< in_causal_delay << " for details see LRM";
			SC_REPORT_ERROR("SystemC-AMS",str.str().c_str());
		}

		calculate_to_end = false;
	}

	calculate_timeinterval(current_in_index, current_out_index,
			port_rate, next_in_time, next_out_time, next_in_time_step,
			out_time_step);

}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
void sca_ct_vector_ss_proxy::convert_to_sca_port(
		sca_tdf::sca_out_base<sca_util::sca_vector<double> >& port)
{

	outtype = OUT_SCA_PORT;
	out_sca_port = &port;

	sca_core::sca_time first_port_timestep = port.get_timestep();
	sca_core::sca_time port_timestep       = first_port_timestep;
	sca_core::sca_time port_time           = port.get_time();
	unsigned long port_rate                = port.get_rate();

	if(port_rate>1)
	{
		port_timestep=port.get_timestep(1);
	}

	if(port_timestep==sc_core::SC_ZERO_TIME)
	{
		std::ostringstream str;
		str << "Port timestep for port: " << port.name();
		str << " must be greater 0 if used for ss: " << ltf_object->name();
		SC_REPORT_ERROR("SystemC-AMS",str.str().c_str());
	}

	convert_to_port(first_port_timestep,port_timestep,port_time,port_rate);

}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

void sca_ct_vector_ss_proxy::convert_to_sc_port(
		sca_tdf::sca_de::sca_out<sca_util::sca_vector<double> >& port)
{

	outtype = OUT_SC_PORT;
	out_sc_port = &port;

	sca_core::sca_time first_port_timestep = port.get_timestep();
	sca_core::sca_time port_timestep       = first_port_timestep;
	sca_core::sca_time port_time     = port.get_time();
	unsigned long port_rate          = port.get_rate();


	if(port_rate>1)
	{
		port_timestep=port.get_timestep(1);
	}

	if(port_timestep==sc_core::SC_ZERO_TIME)
	{
		std::ostringstream str;
		str << "Port timestep for port: " << port.name();
		str << " must be greater 0 if used for ltf: " << ltf_object->name();
		SC_REPORT_ERROR("SystemC-AMS",str.str().c_str());
	}


	convert_to_port(first_port_timestep,port_timestep,port_time,port_rate);
}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

void sca_ct_vector_ss_proxy::convert_to_matrix(
		sca_util::sca_matrix<double>& vec, unsigned long nsamples)
{
	unsigned long number_of_out_samples = number_of_in_values;

	outtype = OUT_VECTOR;
	out_vector = &vec;

	unsigned long current_in_index = in_index_offset;
	long current_out_index = 0;

	unsigned long nout = (c_old.n_rows()) ? c_old.n_rows() : d_old.n_rows();

	if (nsamples == 0)
	{
		if (vec.n_cols() == 0)
		{
			vec.resize(nout, number_of_out_samples);
		}
		else
		{
			number_of_out_samples = vec.n_cols();
			vec.resize(nout, number_of_out_samples);
		}
	}
	else
	{
		vec.resize(nout, nsamples);
		number_of_out_samples=nsamples;
	}


	sca_core::sca_time out_time_step = time_interval / number_of_out_samples;
	//compensate rounding error
	sca_core::sca_time first_out_time_step = time_interval - (number_of_out_samples-1)*out_time_step;


	sca_core::sca_implementation::sca_signed_time next_in_time = last_calculated_in_time + first_in_time_step;
	sca_core::sca_time next_in_time_step = in_time_step;

	sca_core::sca_implementation::sca_signed_time next_out_time;

	if(last_out_time!=sca_core::sca_max_time())
	{
		next_out_time = last_out_time+first_out_time_step;

		calculate_timeinterval(current_in_index, current_out_index,
				number_of_out_samples, next_in_time, next_out_time,
				next_in_time_step, out_time_step);

	}
	else //first run - no last_out_time available
	{
		next_out_time=sc_core::SC_ZERO_TIME - out_time_step *(number_of_out_samples-1);
		last_calculated_time=next_out_time-out_time_step;

		calculate_timeinterval(current_in_index, current_out_index,
				number_of_out_samples, next_in_time, next_out_time,
				next_in_time_step, out_time_step);

	}
}


///////////////////////////////////////////////////////////////////////////////

void sca_ct_vector_ss_proxy::enable_iterations()
{
	if(sca_core::sca_implementation::sca_get_curr_simcontext()->elaboration_finished())
	{
		std::ostringstream str;
		str << "enable_iterations() for: " << ltf_object->name();
		str << " can only be executed before end of elaboration (see LRM)";
		SC_REPORT_ERROR("SystemC-AMS",str.str().c_str());
		return;
	}

	iterations_enabled=true;
}


sca_util::sca_vector<double> sca_ct_vector_ss_proxy::estimate_next_value()
{
	unsigned long nout = (c_old.n_rows()) ? c_old.n_rows() : d_old.n_rows();
	unsigned long nin = (b_old.n_cols() == 0) ? d_old.n_cols(): b_old.n_cols();

	sca_util::sca_vector<double> estimated_out(nout);

	if(statep!=NULL)
	{
		sca_util::sca_vector<double> estimated_in(nin);

		for(unsigned long i=0;i<nin;i++)
		{
			estimated_in(i)=last_calculated_in(i)+
					last_delta_in(i)*time_interval.to_seconds()/last_h;
		}

		double h=time_interval.to_seconds();

		ana_store_solver_check_point(sdata,statep->get_flat(),&estimate_cp_data);
		initialize_equation_system(2, h);
		sca_util::sca_vector<double> tmp=last_calculated_in;

		estimated_out=calculate(estimated_in);

		//ignore this calculation
		last_calculated_in=tmp;
		ana_restore_solver_check_point(sdata,statep->get_flat(),&estimate_cp_data);
	}


	return estimated_out;
}





}
}
