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

 sca_linear_solver.cpp - description

 Original Author: Karsten Einwich Fraunhofer / COSEDA Technologies

 Created on: 10.11.2009

 SVN Version       :  $Revision: 2320 $
 SVN last checkin  :  $Date: 2023-06-08 12:47:38 +0000 (Thu, 08 Jun 2023) $
 SVN checkin by    :  $Author: pehrlich $
 SVN Id            :  $Id: sca_linear_solver.cpp 2320 2023-06-08 12:47:38Z pehrlich $

 *****************************************************************************/

/*****************************************************************************/


#include "scams/impl/solver/linear/sca_linear_solver.h"
#include "scams/impl/core/sca_simcontext.h"
#include "scams/impl/core/sca_object_manager.h"
#include "scams/impl/synchronization/sca_synchronization_layer.h"
#include "scams/impl/solver/util/sparse_library/linear_analog_solver.h"
#include "scams/impl/solver/util/sparse_library/linear_direct_sparse.h"
#include "scams/impl/analysis/ac/sca_ac_domain_globals.h"

#include <climits>
#include<cstring>


//#define DEBUG_PWL

namespace sca_core
{
namespace sca_implementation
{

//////////////////////////////////////////////////////////////////////

sca_linear_solver::sca_linear_solver(std::vector<sca_module*>& mods,
		std::vector<sca_interface*>& chans) :
	sca_core::sca_implementation::sca_solver_base(sc_core::sc_gen_unique_name(
			"sca_linear_solver"), mods,chans)
{

#ifdef SCA_IMPLEMENTATION_DEBUG
	std::cout << get_name() << ": constructed." << std::endl;
#endif


	//solver supports ac-simulation
	this->ac_enable();

	call_id=0;

	 equation_if=NULL;
	 dt=0.0;
	 pwl_dt_last=0.0;
	 checkp_dt=0.0;
	 ac_equation_initialized=false;
	 pwl_coeff_available=false;

	 reinit_methods=NULL;
	 internal_solver_data=NULL;
	 current_time=0.0;
	 x_flat=NULL;
	 equations=NULL;
	 pwl_stamps=NULL;
	 B=NULL;
	 lin_solver_methods=NULL;
	 reinit_dt=0.0;
	 B_change=NULL;
	 q=NULL;
	 A=NULL;


	 number_of_timesteps = 0;
	 number_of_reinit = 0;

	 cp_restored = false;
	 first_timestep = true;

	 force_implicit_euler_method = false;
	 reinitialization_steps=-1;
	 algorithm_set=false;
	 algorithm_module=NULL;


	 pwl_iteration_cp =NULL;
	 global_cp=NULL;

	 q_current=NULL;
	 q_current_tmp=NULL;

	 reached_dt=0.0;

	 pwl_dt=0.0;
	 dt_global=0.0;

	 statistics_pwl_segment_changes=0;
	 statistics_pwl_timestep_changes=0;
	 statistics_max_pwl_segment_iterations=0;
	 statistics_max_pwl_timestep_iterations=0;
	 statistics_cur_pwl_segment_iterations=0;
	 statistics_cur_pwl_timestep_iterations=0;

	 statistics_pwl_min_timestep=1e64;

	 //default use euler
	 last_reinit_flag=1;

	 collect_profile_data=sca_core::sca_implementation::sca_get_curr_simcontext()->is_performance_data_collection_enabled();

}

//////////////////////////////////////////////////////////////////////

sca_linear_solver::~sca_linear_solver()
{
}

//////////////////////////////////////////////////////////////////////

void sca_linear_solver::set_solver_parameter(
		  sca_core::sca_module* mod,
		  const std::string& par,
		  const std::string& val)
{

	if(par=="algorithm")
	{
		if(algorithm_set)
		{
			if(algorithm_value!=val)
			{
				std::ostringstream str;
				str << "The integration algorithm is set by module: ";
				str << algorithm_module->name() << " to: " << algorithm_value;
				str << " and by the module: " << mod->name();
				str << " of the same cluster to: " << val;
				str << " - second set is ignored";
				SC_REPORT_WARNING("SystemC-AMS",str.str().c_str());
			}

			return;
		}

		if(mod!=NULL)
		{
			algorithm_set=true;
			algorithm_value=val;
			algorithm_module=mod;
		}

		if(val=="euler")
		{
			force_implicit_euler_method=true;
			return;
		}

		if(val=="default")
		{
			force_implicit_euler_method=false;
			return;
		}

		std::ostringstream str;
		str << "Unknown value: " << val << " for solver parameter: " << par;
		if(mod==NULL)
		{
			str << " for default solver parameter";
		}
		else
		{
			str << " set by module: " << mod->name() << " of kind: " << mod->kind();
		}
		str << " valid valiues are: euler and default - ignore parameter";
		SC_REPORT_WARNING("SystemC-AMS",str.str().c_str());

		return;
	}

	if(par=="reinitialization_steps")
	{
		std::istringstream istr(val);
		int nsteps;
		istr>>nsteps;
		if(istr.fail())
		{
			std::ostringstream str;
			str << "Value: " << val << " for solver parameter: " << par;
			str << " can't be read as integer value - parameter ignored";
			SC_REPORT_WARNING("SystemC-AMS",str.str().c_str());
			return;
		}

		reinitialization_steps=nsteps;

		return;
	}


	//parameter unknown -> print warning from base class
	this->sca_solver_base::set_solver_parameter(mod,par,val);
}


//////////////////////////////////////////////////////////////////////

void sca_linear_solver::initialize()
{
	//register solver to synchronization layer
	sca_core::sca_implementation::sca_get_curr_simcontext()->get_sca_object_manager()->get_synchronization_if()-> registrate_solver_instance(
			this);

#ifdef SCA_IMPLEMENTATION_DEBUG
	std::cout << "\t\t" << " linear solver instance initialized" << std::endl;
#endif
}

//////////////////////////////////////////

sc_core::sc_object* sca_linear_solver::get_object_of_equation(long eq_nr)
{
	if(eq_nr<0) return NULL;

	for(unsigned long i=0;i<associated_channel_list.size();i++)
	{
		sca_core::sca_implementation::sca_conservative_signal* sig;
		sig=dynamic_cast<sca_core::sca_implementation::sca_conservative_signal*>(associated_channel_list[i]);

		if(sig!=NULL)
		{
			if(sig->get_node_number()==eq_nr) return sig;
		}
	}

	for(unsigned long i=0;i<associated_module_list.size();i++)
	{
		sca_core::sca_implementation::sca_conservative_module* mod;
		mod=dynamic_cast<sca_core::sca_implementation::sca_conservative_module*>(associated_module_list[i]);

		if(mod!=NULL)
		{
			for(unsigned long j=0;j<mod->add_equations.size();j++)
			{
				if(mod->add_equations[j] == eq_nr) return mod;
			}
		}
	}

	return NULL;
}

//////////////////////////////////////////

//this methods transforms the pwl_vector data which represent x-y value pairs
//into n-1 segments of b coefficients and offset values
//
//   x0
//   x1 -> segment 0
//   x2 -> segment 1
//   x3 -> segment 2
//          :
void sca_linear_solver::init_pwl_data(bool is_ac)
{
	unsigned long num_of_pwl_coeffs=equations->pwl_stamps.get_number_of_stamps();

	pwl_coefficients.resize(num_of_pwl_coeffs);

	for(unsigned long i=0; i<num_of_pwl_coeffs;++i)
	{
		pwl_data& data(pwl_coefficients[i]);


		std::vector<double> pwl_vector;

		bool b_stamps;
		unsigned long next_idx=i;
		equations->pwl_stamps.get_pwl_stamp(next_idx,data.eq_idx,data.x_idx,data.arg_idx,pwl_vector,b_stamps);

		unsigned long n_segments;

		if(b_stamps)
		{
			n_segments=static_cast<unsigned long>(pwl_vector.size()/2);
			data.x_values.resize(n_segments+1);
			data.b_coefficients.resize(n_segments);
			data.offsets.resize(n_segments);
			data.n_segments=n_segments;

			for(unsigned long j=0;j<n_segments;++j)
			{
				data.x_values[j]     = pwl_vector[2*j];
				data.b_coefficients[j] = pwl_vector[2*j+1];
				data.offsets[j]        = 0.0;
			}

			//last value should be greater (will not be used)
			data.x_values[n_segments]=data.x_values[n_segments-1]+
					fabs(data.x_values[n_segments-1]);
		}
		else
		{
			//a pwl segment is characterized by a start and end point (x_value)
			//each segment is characterized by one b coefficient and one offset
			//due the segments are connected, we have one more x_value than
			//offset and b coefficient values

			//it is guaranteed, that we have at least two points (4 values) and thus
			//at least one segemnt in the pwl_vector, it's also guaranteed, that
			//pwl_vector contains an odd number of elements (pairs)
			//it is also guaranteed, that the x_values are increasing

			n_segments=static_cast<unsigned long>(pwl_vector.size()/2-1);
			data.x_values.resize(n_segments + 1);
			data.b_coefficients.resize((std::size_t)n_segments);
			data.offsets.resize(n_segments);
			data.n_segments=n_segments;

			data.x_values[0] = pwl_vector[0];
			for(unsigned long j=0;j<n_segments;++j)
			{
				data.x_values[j+1] = pwl_vector[2*(j+1)];

				double dx=data.x_values[j+1] - data.x_values[j];
				double dy=pwl_vector[2*(j+1)+1] - pwl_vector[2*j+1];


				// y = m * (x - x_last) + y(x_last)
				// y= m*x - m*x_last + y(x_last)
				// -> B=m  ;  offset=q = -m*x_last + y(x_last)

				data.b_coefficients[j] = dy/dx;
				data.offsets[j]        = pwl_vector[2*j+1] - dy/dx*data.x_values[j];
			}
		}

#ifdef DEBUG_PWL

		std::cout << "PWL segemts for: " << i << std::endl;

		for(unsigned long j=0;j<n_segments;++j)
		{
			std::cout << "       " << data.x_values[j+1]  << " b/q "
			          << data.b_coefficients[j] << " / "
			          << data.offsets[j]<< std::endl ;
		}
		std::cout << std::endl;

#endif


		if(!is_ac) //during ac we cannot access the x-vector -> we use the old state
		{
			//initialize for x_val = 0.0
			unsigned long idx=0;
			for(idx=0;(idx+1)<n_segments;++idx)
			{
				//first and last point must not be checked, due before is segment 0
				//and after remains the last segment
				if(data.x_values[idx+1]>=x[data.arg_idx]) break;
			}

			data.last_valid_segment= idx;
			data.current_segment   = idx;
			data.new_segment       = idx;
		}

		(*B)(data.eq_idx,data.x_idx) = data.b_coefficients[data.current_segment];

		if( (sca_ac_analysis::sca_ac_is_running()) &&(std::abs(sca_ac_analysis::sca_ac_w())>1e-6))
		{
			(*q)[data.eq_idx].set_value(0.0); //for ac-no offset
		}
		else
		{
			(*q)[data.eq_idx].set_value(data.offsets[data.current_segment]);
		}


		if (!is_ac)
        {
            //initialize tmp vector for interpolation
            double *qtmp = q->get_calc_flat();

            const std::size_t length(q->length());

            q_current_tmp = new double[length];
            sc_assert(q_current_tmp!=NULL);
            for (unsigned long i = 0; i < length; i++)
            {
                q_current_tmp[i] = qtmp[i];
            }
        }

	}

	if(pwl_coefficients.size()>0) pwl_coeff_available=true;
	else                          pwl_coeff_available=false;
}

/** searches current segment after restore*/
void sca_linear_solver::search_valid_segments(double* x)
{
	unsigned long num_of_pwl_coeffs=equations->pwl_stamps.get_number_of_stamps();

	for(unsigned long i=0; i<num_of_pwl_coeffs;++i)
	{
		pwl_data& data(pwl_coefficients[i]);

		long arg_idx=data.arg_idx;
		bool changed=false;

		data.new_segment=data.current_segment;

		if(x[arg_idx]>data.x_values[data.new_segment])
		{
			while(((data.new_segment+1)<data.n_segments)&&
			      ((x[arg_idx] > data.x_values[data.new_segment+1])))
			{
				data.new_segment++;
				changed=true;
			}
		}
		else
		{
			while((data.new_segment>0)&&
			      ((x[arg_idx] < data.x_values[data.new_segment])))
			{
				data.new_segment--;
				changed=true;
			}
		}

		if(changed)
		{
			change_pwl_coefficients();
			equation_if->get_reinit_request() = last_reinit_flag;
		}
	}
}

//////////////////////////////////////////////////////////////////////////



//////////////////////////////////////////////////////////////////////////

//check whether the last calculation was correct - that means the assumed
//segments were correct for the calculated x values
// mode=0 - first calculation after a valid calculation
// mode=1 - check only - do not change segment
// mode=2 - check and change
//
// return value:
//                0   - pwl ok
//                1   - pwl not ok, segment not changed
//                2   - pwl not ok, segment changed
int sca_linear_solver::check_pwl_intervals(int mode)
{
	//last calculation was valid
	if(mode==0)
	{
		for(unsigned long i=0;i<pwl_coefficients.size();++i)
		{
			pwl_coefficients[i].last_valid_segment=pwl_coefficients[i].current_segment;
		}
	}


	bool pwl_wrong       = false;
	bool segment_changed = false;

#ifdef DEBUG_PWL
	std::cout << "------ Start PWL check " << std::endl;
#endif

	for(unsigned long i=0;i<pwl_coefficients.size();++i)
	{
		pwl_data& data(pwl_coefficients[i]);

		//get current x-value
		double x_val=x(data.arg_idx);

#ifdef DEBUG_PWL
		std::cout << "1 currrent segment: " << data.current_segment
				  << " segment xval: " << data.x_values[data.current_segment]
				  << " x_val: " << x_val
				  << " pwl size: " << pwl_coefficients.size()
				  << " B/q: " << data.b_coefficients[data.current_segment]
				  << " / " << data.offsets[data.current_segment] << std::endl;
#endif


		//if the current value smaller than the left intervall border and if this
		//is not the first point, we are in the wrong intervall
		if((x_val<data.x_values[data.current_segment])   && (data.current_segment>0))
		{
			pwl_wrong=true;
			//we change the segment only, if after the change, it differs
			//not more than one from the last valid segment or we are in the first timestep
			if(((data.current_segment>=data.last_valid_segment) && (mode!=1))||first_timestep)
			{
				segment_changed=true;
				data.new_segment=data.current_segment-1;
			}
		}
		else
			//if the current value is larger than the right interval border and if
			//this is not the last point, we are in the wrong interval
			if(     (data.current_segment+1<data.n_segments) &&
					(x_val>data.x_values[data.current_segment+1])
			  )
			{
				pwl_wrong=true;
				//we change the segment only, if after the change, it differs
				//not more than one from the last valid segment
				if(((data.current_segment<=data.last_valid_segment) && (mode!=1))||first_timestep)
				{
					segment_changed=true;
					data.new_segment=data.current_segment+1;
				}
			}

#ifdef DEBUG_PWL
		if(pwl_wrong)
		std::cout << "   2 new segment: " << data.new_segment
				  << " segment xval: " << data.x_values[data.new_segment]
				  << " segment_changed: " << segment_changed << " B/q: "
				  << data.b_coefficients[data.new_segment]
				  << " / " << data.offsets[data.new_segment] << std::endl;
#endif

	} //for(unsigned long i=0;i<pwl_coefficients.size();++i)

#ifdef DEBUG_PWL
	std::cout << std::endl;
#endif

	if(pwl_wrong)
	{
		if(segment_changed)
		{
			statistics_pwl_segment_changes++;
			statistics_cur_pwl_segment_iterations++;
			return 2;
		}
		else
		{
			statistics_pwl_timestep_changes++;
			statistics_cur_pwl_timestep_iterations++;
			return 1;
		}
	}


	for(unsigned long i=0;i<pwl_coefficients.size();++i)
	{
		pwl_coefficients[i].last_valid_segment=pwl_coefficients[i].current_segment;
	}

#ifdef DEBUG_PWL
	std::cout <<"---------------PWL OK----------------------"<< std::endl;
#endif

	return 0;
}


///////////////////////////////////////////////////////////////////////////////

void sca_linear_solver::change_pwl_coefficients()
{
	for(unsigned long i=0;i<pwl_coefficients.size();++i)
	{
		pwl_data& data(pwl_coefficients[i]);

		if(data.current_segment!=data.new_segment)
		{
#ifdef DEBUG_PWL
			std::cout << "yidx: " << data.eq_idx << " xidx: " << data.x_idx
					<< " B " << data.b_coefficients[data.new_segment] << std::endl;
#endif

			(*B)(data.eq_idx,data.x_idx) = data.b_coefficients[data.new_segment];

			double offset=data.offsets[data.new_segment];
			(*q)[data.eq_idx].set_value(offset);

			//set offset to temporarly and interpolation vector
			q->get_flat_vector()[data.eq_idx] = offset;
			q_current_tmp[data.eq_idx]        = offset;

			data.current_segment=data.new_segment;
		}

	}
}


//////////////////////////////////////////


void sca_linear_solver::init_eq_system()
{
	//allow access to tdf port attributes
	(*this->get_allow_processing_access_flag_ref())=true;

	equation_if->reinit_equations();

	ac_equation_initialized = false;

	x.resize(A->n_cols());
	x_flat = x.get_flat();

	unsigned long& info_mask(
	            sca_core::sca_implementation::sca_get_curr_simcontext()->
	            get_information_mask());

	if (info_mask & (
                          sca_util::sca_info::sca_eln_solver.mask |
                          sca_util::sca_info::sca_lsf_solver.mask))
	{
	  if((info_mask & sca_util::sca_info::sca_eln_solver.mask) &&
	     ( dynamic_cast<sca_eln::sca_module*>(associated_module_list[0])!=NULL))
	    {
		std::ostringstream sta_str;
		sta_str << "\n\tELN solver instance: " << get_name()
            << " (cluster " << get_synchronization_object_data()->cluster_id << ")"
            << std::endl << "\t\thas "
			<< x.length() << " equations for "
			<< associated_module_list.size() << " modules (e.g. "
			<< associated_module_list[0]->name() << ")," << std::endl << "\t\t"
			<< from_analog.size() << " inputs and " << to_analog.size()
			<< " outputs to other (TDF) SystemC-AMS domains,\n" << "\t\t"
			<< from_systemc.size() << " inputs and " << to_systemc.size()
			<< " outputs to SystemC de." << std::endl;
		sta_str << "\t\t" << get_current_period() << " initial time step"
				<< std::endl;

		SC_REPORT_INFO("SystemC-AMS", sta_str.str().c_str());
	    }

	   if((info_mask & sca_util::sca_info::sca_lsf_solver.mask) &&
	       (dynamic_cast<sca_lsf::sca_module*>(associated_module_list[0])!=NULL))
	   {
	       std::ostringstream sta_str;
	       sta_str << "\n\tLSF solver instance: " << get_name()
	               << " (cluster " << get_synchronization_object_data()->cluster_id << ")"
	               << std::endl << "\t\thas "
	               << x.length() << " equations for "
	               << associated_module_list.size() << " modules (e.g. "
	               << associated_module_list[0]->name() << ")," << std::endl << "\t\t"
	               << from_analog.size() << " inputs and " << to_analog.size()
	               << " outputs to other (TDF) SystemC-AMS domains,\n" << "\t\t"
	               << from_systemc.size() << " inputs and " << to_systemc.size()
	               << " outputs to SystemC de." << std::endl;
	      sta_str << "\t\t" << get_current_period() << " initial time step"
	                                  << std::endl;

	                  SC_REPORT_INFO("SystemC-AMS", sta_str.str().c_str());
           }
	}

	dt = reinit_dt = get_current_period().to_seconds();
	dt_global=dt;

	//initialize piece wise linear coefficient data
	init_pwl_data(false);


	int err = 0;
	if (A->is_sparse_mode())
	{
	 	err = ana_init_sparse(A->get_sparse_matrix(), B->get_sparse_matrix(),
	 			dt, &internal_solver_data, 0);
	}
	else
	{
		err = ana_init(A->get_flat(), B->get_flat(), A->n_cols(), dt,
			&internal_solver_data, 0);
	}


	if(force_implicit_euler_method)
	{
		ana_set_algorithm(internal_solver_data,0);
	}
	else
	{
		ana_set_algorithm(internal_solver_data,1);

	}

	if(reinitialization_steps>0)
	{
		ana_set_reinit_steps(internal_solver_data, reinitialization_steps-1);
	}
			
	if (err) error_message(err, 0, 0.0);

	call_id++;

	//forbid access to tdf port attributes
	(*this->get_allow_processing_access_flag_ref())=false;
}

//////////////////////////////////////////

//prints error message for solv_eq_system
void sca_linear_solver::print_reinitialization_error()
{
	long row=-1, column=-1;
	ana_get_error_position(internal_solver_data,&row,&column);

	sc_core::sc_object* obj_row=get_object_of_equation(row);
	sc_core::sc_object* obj_column=get_object_of_equation(column);

	std::ostringstream str;
	str << "Reinitialization failed in " << get_name()
			<< std::endl;
	str << "  during reinitialization equation system at "
			<< get_current_time() << " (dt = " << dt << ")." << std::endl;
	str << "  the error is in the following net (max. 50): " << std::endl;
	for (unsigned long i = 0; i < associated_module_list.size(); ++i)
	{
		if (i >= 50)
			break;
		str << "\t" << associated_module_list[i]->name() << std::endl;
	}

	str << std::endl;
	str << "Parameters of the following modules changed for the current time step:" << std::endl;
	for (unsigned long i = 0; i < associated_module_list.size(); ++i)
	{
		sca_core::sca_implementation::sca_conservative_module* cmod;
		cmod=dynamic_cast<sca_core::sca_implementation::sca_conservative_module*>(associated_module_list[i]);

		if(call_id==cmod->call_id_request_init)
		{
			str << "\t" << cmod->name();
			if(cmod->value_set) str << " changed to " << cmod->new_value;
			str << std::endl;
		}
	}

	if((obj_row!=NULL)||(obj_column!=NULL))
	{
		str << std::endl;
		str << "The error is may  be near: " << std::endl << "\t\t";
		if(obj_row!=NULL)
		{
			str << obj_row->name();
			if(obj_column!=NULL)
			{
				if(obj_column!=obj_row)
				{
					str << "  and " << std::endl << "\t\t";
			        str << obj_column->name();
				}
			}
		}
		else
		{
			str << obj_column->name();
		}
		str << std::endl;
	}


	SC_REPORT_ERROR("SystemC-AMS", str.str().c_str());
	return;

}

//////////////////////////////////////////////////////////////////////////


bool sca_linear_solver::do_pwl_iteration(int& check_mode,double& timestep)
{
#ifdef DEBUG_PWL

	std::cout << "pwl iteration with timestep: " << timestep  << std::endl;

#endif

	int ch_res=check_pwl_intervals(check_mode);



	if(ch_res==0)
	{
		check_mode=2;
		return false;
	}


	ana_restore_solver_check_point(internal_solver_data,x_flat,&pwl_iteration_cp);


	switch (ch_res)
	{
	case 1: //no segment changes
	{
		timestep = timestep / 2.0;

		if (timestep < 1e-15)
		{
			std::ostringstream str;
			str << "Piece wise linear iteration does not converge for ";
			str << "the electrical network containing the following elements: ";
			str << std::endl;
			str << this->get_name_associated_names(20);
			SC_REPORT_ERROR("SystemC-AMS",str.str().c_str());
			return false;
		}

		if(timestep < statistics_pwl_min_timestep)
		{
			statistics_pwl_min_timestep = timestep;
		}

		//we must interpolate q
		interpolate_B_q(reached_dt+timestep);

		int err = ana_reinit_sparse(
				A->get_sparse_matrix(),
				B->get_sparse_matrix(),
				timestep,
				&internal_solver_data, 2 //initialize for trapez -> no euler step
				);
		if (err)
		{
			print_reinitialization_error();
			return false;
		}

#ifdef DEBUG_PWL
		std::cout << "solve with timestep1: " << timestep
				<< " time reached: " << reached_dt+timestep << std::endl;
#endif

		ana_solv(q_current, x_flat, internal_solver_data);

		check_mode = 2; //next iteration segment change allowed


		return true;
	}
	case 2: //segment has changed
	{
		change_pwl_coefficients();


		int err = ana_reinit_sparse(
				A->get_sparse_matrix(),
				B->get_sparse_matrix(),
				timestep,
				&internal_solver_data,
				2 //initialize for trapez -> no euler step
				);
		if (err)
		{
			print_reinitialization_error();
			return false;
		}



		ana_solv(q_current, x_flat, internal_solver_data);

#ifdef DEBUG_PWL
		std::cout << "solved with timestep2: " << timestep
				<< " time reached: " << reached_dt+timestep << std::endl;
/*
		std::cout << "  x: ";
		for(unsigned long i=0;i<q->length();i++) std::cout << "  " << x_flat[i];
		std::cout << std::endl;

		std::cout << "  q: ";
		for(unsigned long i=0;i<q->length();i++) std::cout << "  " << q_current[i];
		std::cout << std::endl;

		std::cout << std::endl;
		std::cout << " B: " << std::endl;
		std::cout << (*B) << std::endl;
*/
#endif

		check_mode = 1; //do not change the segment again -> change timestep

		return true;
	}
	default:

		SC_REPORT_ERROR("SystemC-AMS","Internal error which should not be possible");

	}

	return false;
}

//////////////////////////////////////////////////////////////////////////////

void sca_linear_solver::interpolate_B_q(double dtime)
{
	//interpolate only if time different
	if(pwl_dt_last!=dtime)
	{
		//use interpolation vector
		q_current = q_current_tmp;


		//for all q entries calculated by a method
		for(unsigned long i=0;i<q->length();i++)
		{
			if(!(*q)(i).is_method_list_set()) continue;


			if(q_next[i]!=q_last[i])
			{
				q_current[i]=(q_next[i]-q_last[i])/dt*dtime + q_last[i];
			}
			else
			{
				q_current[i]=q_next[i];
			}

#ifdef DEBUG_PWL
			std::cout << "Interpolate q: " << i  << " from "
							<< q_last[i] << " to " << q_next[i] << " at "
							<< dtime << " : " << q_current[i] << std::endl;
#endif
		}



		//// interpolate eventually changed B coefficients ///////
		for(unsigned long i=0;i<B_change->get_number_of_changes();i++)
		{
			unsigned long idx,idy;
			double cur_value;
			double old_value;
			bool cont;

			B_change->get_change(i,idy,idx,cur_value,old_value,cont);

#ifdef DEBUG_PWL
			std::cout << "Interpolate B: " << idy << " " << idx
					<< " last val: " << (*B)(idy,idx);
#endif

			(*B)(idy,idx)=(cur_value-old_value)/dt*dtime + old_value;

#ifdef DEBUG_PWL
			std::cout << " from " << old_value << " to " << cur_value << " at "
							<< dtime << " : " << (*B)(idy,idx) << std::endl;
#endif
		}

		pwl_dt_last=dtime;
	}
}


void sca_linear_solver::iterate_pwl_intervalls()
{
	ana_restore_solver_check_point(internal_solver_data,x_flat,&pwl_iteration_cp);

	change_pwl_coefficients();

	double remaining_dt=dt;
	pwl_dt=dt;

	reached_dt=0.0;

	int check_mode=-1;


	do //run until global timestep is calculated
	{
		//we must interpolate q
		interpolate_B_q(dt);

		//the next end time is the time of the global step
		//q_current=q_next;

		pwl_dt=remaining_dt;

		int err = ana_reinit_sparse
				(
						A->get_sparse_matrix(),
						B->get_sparse_matrix(),
						pwl_dt,
						&internal_solver_data,
						2  //initialize for trapet -> no euler step
				 );
		if (err)
		{
			print_reinitialization_error();
			return;
		}

#ifdef DEBUG_PWL
		std::cout << "solve with timestep3: " << pwl_dt
				<< " time reached: " << reached_dt+remaining_dt
				<< std::endl;

		std::cout << "  x: ";
		for(unsigned long i=0;i<q->length();i++) std::cout << "  " << x_flat[i];
		std::cout << std::endl;

		std::cout << "  q: ";
		for(unsigned long i=0;i<q->length();i++) std::cout << "  " << q_current[i];
		std::cout << std::endl;

#endif



		ana_solv(q_current, x_flat, internal_solver_data);


		pwl_dt=pwl_dt_last=remaining_dt;

		//iterate until valid timepoint found
		while(do_pwl_iteration(check_mode,pwl_dt));


		//we ensure a minimal time progress
		reached_dt+=pwl_dt + std::numeric_limits<double>::epsilon()*dt;

		remaining_dt=dt-reached_dt;
		remaining_dt-=std::numeric_limits<double>::epsilon()*dt;


#ifdef DEBUG_PWL

		std::cout << " reached_dt: " << reached_dt << " reamaining dt: "
				<< remaining_dt << " dt: " << dt
				<< " eps:  " << std::numeric_limits<double>::epsilon()*dt
				<< std::endl << std::endl;

#endif



		//must be stored only if we continue
		if(remaining_dt>1e-15)
		{
			ana_store_solver_check_point(internal_solver_data,x_flat,&pwl_iteration_cp);
		}

	}while(remaining_dt>1e-15);

}


//////////////////////////////////////////////////////////////////////////

void sca_linear_solver::solve_eq_system()
{
#ifndef DISABLE_PERFORMANCE_STATISTICS
	if(this->collect_profile_data)
	{
		this->activation_cnt++;
		start = std::chrono::high_resolution_clock::now();
	}
#endif

	//allow access to tdf port attributes
	(*this->get_allow_processing_access_flag_ref())=true;


	current_time = get_current_time().to_seconds();
	number_of_timesteps++;


	if(pwl_coeff_available)
	{
	    std::size_t ql=q->length();
		//prepare q-vector for interpolation
		if((q_last.size()!=ql) || (q_next.size()!=ql))
		{
			std::vector<double> q_cur(q->length());
		    std::memcpy(q_cur.data(),q->get_calc_flat(),sizeof(double)*q->length());
			if(q_last.size()!=ql)
			{
				q_last=q_cur;
			}

			if(q_next.size()!=ql)
			{
				q_next.swap(q_cur);
			}
		}

		//reset matrix changes to zero (no pending change)
		//if we restore, the changes remain, due they may not be set again
		//by the modules
		if(!cp_restored)
		{
			B_change->reset();
		}
	}

	double new_dt;
	if(cp_restored) //repeat timestep
	{
		new_dt=checkp_dt;
		cp_restored=false;
	}
	else
	{
		new_dt = get_current_period().to_seconds();
		dt_global = new_dt;

		if(pwl_coeff_available)
		{
			//store last q
			for(unsigned long i=0; i<q_last.size();i++) q_last[i]=q_next[i];
		}
	}


	int init_flag = 0;

	if (ac_equation_initialized) //restore time domain equations
	{
		equation_if->reinit_equations();
		init_pwl_data(false);
		ac_equation_initialized = false;
	}

	if (fabs(new_dt - dt) > 1e-15)
	{
		init_flag = 2; // in general no Euler step after initialization

		// !!!!!!!!! workaround for iteration !!!!!!!!!!!!!
		if (new_dt < 1e-18) // time step smaller than 1e-3 fs
		{
			new_dt = 1e-18; // limit to very small step size (1e-3 fs)
			init_flag = 1; // Euler step after init - may there are jumps at inputs
		}
	}

	dt = new_dt;

	call_methods(&(lin_solver_methods->pre_solve_methods));

	//may during a synchronization wait a ac simulation is performed
	if (ac_equation_initialized) //restore time domain equations
	{
		equation_if->reinit_equations();
        init_pwl_data(false);

		ac_equation_initialized = false;
	}


	if (equation_if->get_reinit_request() != 0)
	{
		//for statistics
		number_of_reinit++;

		// Euler step after reinitialization yields a necessary DC step
		// !!!!!! as long as there are no changes in matrix A !!!!!!

		init_flag=equation_if->get_reinit_request();

		if (init_flag == 3 || init_flag == 4)
		{
			init_flag -= 2;
		}

		equation_if->get_reinit_request() = 0;  //reset request

		last_reinit_flag=init_flag;
	}
	else
	{
		last_reinit_flag=2;
	}

	if (init_flag)
	{
		int err = ana_reinit_sparse(A->get_sparse_matrix(), B->get_sparse_matrix(), new_dt, &internal_solver_data,
				   init_flag);


		if (err)
		{
			print_reinitialization_error();
		}
	}


	if(pwl_coeff_available)
	{
		ana_store_solver_check_point(internal_solver_data,x_flat,&pwl_iteration_cp);
	}

	//store vector for pwl-iteration
	q_current=q->get_calc_flat();


	ana_solv(q_current, x_flat, internal_solver_data);

	if(pwl_coeff_available)
	{
	    q_next.resize(q->length());
	    std::memcpy(q_next.data(),q_current,sizeof(double)*q->length());

		if(check_pwl_intervals(0)!=0)
		{
			pwl_dt=dt;
			this->iterate_pwl_intervalls();

			dt=pwl_dt;  //last timestep (timestep of equation system
		}


#ifdef DEBUG_PWL

	std::cout << std::endl;
	std::cout << "================= Global timestep ===========================" << std::endl;
	std::cout << (*A) << std::endl;
	std::cout << (*B) << std::endl;
	for(unsigned long i=0;i<q->length();i++) std::cout << q_current[i] << "  ";
	std::cout << std::endl;
	std::cout << std::endl;

#endif

	if(statistics_cur_pwl_segment_iterations>statistics_max_pwl_segment_iterations)
	{
		statistics_max_pwl_segment_iterations=statistics_cur_pwl_segment_iterations;
	}
	statistics_cur_pwl_segment_iterations=0;

	if(statistics_cur_pwl_timestep_iterations>statistics_max_pwl_timestep_iterations)
	{
		statistics_max_pwl_timestep_iterations=statistics_cur_pwl_timestep_iterations;
	}
	statistics_cur_pwl_timestep_iterations=0;

	}

	//the call_id is used to identify modules which requested a reinit
	call_id++; //post solve contains to the new call id, due it may requests a reinit


	call_methods(&lin_solver_methods->post_solve_methods);


	for (std::vector<sca_util::sca_implementation::sca_trace_object_data*>::iterator
			it =  solver_traces.begin();
	        it != solver_traces.end(); ++it)
	{
		(*it)->trace();
	}

	(*this->get_allow_processing_access_flag_ref())=false;

	first_timestep=false;

#ifndef DISABLE_PERFORMANCE_STATISTICS
	if(this->collect_profile_data)
	{
		this->duration+=std::chrono::high_resolution_clock::now() - start;
	}
#endif

}


///////////////////////////////////////////

void sca_linear_solver::assign_equation_system(sca_linear_equation_if& eqs)
{
	init_method
			= static_cast<sca_core::sca_implementation::sc_object_method> (&sca_linear_solver::init_eq_system);
	init_method_object = this;

	processing_method
			= static_cast<sca_core::sca_implementation::sc_object_method> (&sca_linear_solver::solve_eq_system);
	processing_method_object = this;

	post_method = NULL;
	post_method_object = NULL;

	equations = &(eqs.get_equation_system());
	equation_if = &eqs;

	//for faster access
	A = &(equations->A);
	B = &(equations->B);
	q = &(equations->q);
	pwl_stamps = &(equations->pwl_stamps);

	B_change = &(equations->B_change);

	lin_solver_methods = &(equations->lin_eq_if_methods);
	reinit_methods = &(equations->reinit_methods);
}

//////////////////////////////////////////////////////////////////////////////

/** Methods for supporting ac-simulation */
void sca_linear_solver::ac_domain_eq_method(sca_util::sca_matrix<double>*& A_o,
		sca_util::sca_matrix<double>*& B_o, sca_util::sca_vector<sca_util::sca_complex >*& q_o)
{
	//this allows frequency dependent matrix stamps - maybe slow downs the performance
	equation_if->reinit_equations();
	init_pwl_data(true);

	ac_equation_initialized = true;

	A_o = A;
	B_o = B;

	double* qf = q->get_calc_flat(); //real only !!!!
	q_ac.resize(q->dimx());
	for (unsigned long i = 0; i < q->dimx(); ++i)
		q_ac(i) = qf[i];

	q_o = &q_ac;
}

//////////////////////////////////////////////////////////////////////////////

/** Methods for supporting ac-simulation */
void sca_linear_solver::ac_add_eq_cons_method(
		sca_util::sca_matrix<sca_util::sca_complex >*& con_matr,
		sca_util::sca_vector<sca_util::sca_complex >& y)
{
	//???? we assume static linear equtions (f-independent) only ??????

	unsigned long nstate = A->n_cols();

	//store current x-vector and than set to zero
	//the ac vector size is may different to the time domain size
	//-> thus we must resize the vector and restore it afterwards
	std::vector<double> x_state(x.length());
	for (unsigned long i = 0; i < x_state.size(); ++i)
	{
		x_state[i] = x_flat[i];
		x_flat[i] = 0.0;
	}

	x.resize(nstate);
	x_flat=x.get_flat();

	//for all x set to 1
	//call all post solve
	//for all outs
	//insert influence in matrix
	for (unsigned long i = 0; i < nstate; ++i)
	{
		x_flat[i] = 1.0;

		//call all post-solve methods
		call_methods(&lin_solver_methods->post_solve_methods);

		for (unsigned long j = 0; j < y.length(); ++j)
			con_matr_ac(i,j) = y(j);

		x_flat[i] = 0.0;
	}

	con_matr = &con_matr_ac;

	//restore old state
	x.resize(x_state.size());
	x_flat=x.get_flat();
	for (unsigned long i = 0; i < x_state.size(); ++i)
		x_flat[i] = x_state[i];
}

//////////////////////////////////////////

std::string sca_linear_solver::get_name_associated_names(int max_num) const
{
	std::ostringstream str;
	str << get_name() << " containing modules";

	int max=max_num;
	if(max<0) max=(int)(associated_module_list.size());
	else
	{
		if((unsigned int)max>associated_module_list.size())
		{
			max=(int)(associated_module_list.size());
		}
		else
		{
			str << " (max. " << max << " printed)";
		}
	}

	str << ":" <<std::endl;
	for(int i=0;i<max;i++)
	{
		str << "\t\t" << associated_module_list[i]->name() << std::endl;
	}

	return str.str();
}

//////////////////////////////////////////

void sca_linear_solver::print_post_solve_statisitcs()
{
	//number of modules with most re-initialization to print
	static const unsigned long N_TOP_MOD=10;

	sca_eln::sca_module* elnm=NULL;
	sca_lsf::sca_module* lsfm=NULL;

	unsigned long& info_mask(
	            sca_core::sca_implementation::sca_get_curr_simcontext()->
	            get_information_mask());

	if(!(info_mask & sca_util::sca_info::sca_lsf_solver.mask)) return;

	//find 5 modules with highest reinit request cnt
	std::vector<sca_core::sca_implementation::sca_conservative_module*> top_vec;
	for (unsigned long k = 0; k < associated_module_list.size(); k++)
	{
		sca_core::sca_implementation::sca_conservative_module* curm;
		curm= dynamic_cast<sca_core::sca_implementation::sca_conservative_module*>
																(associated_module_list[k]);
		if (curm == NULL) continue;

		if((elnm==NULL)&&(lsfm==NULL))
		{
			elnm=dynamic_cast<sca_eln::sca_module*>(curm);
			lsfm=dynamic_cast<sca_lsf::sca_module*>(curm);
		}

		if (curm->reinit_request_cnt<=0) continue;

		for (unsigned int i = 0; i < top_vec.size(); i++)
		{
				if (top_vec[i]->reinit_request_cnt < curm->reinit_request_cnt)
				{
					sca_core::sca_implementation::sca_conservative_module* tmpm;
					tmpm = top_vec[i];
					top_vec[i] = curm;
					curm = tmpm;
				}
		}
		if(top_vec.size()< N_TOP_MOD) top_vec.push_back(curm);
	}

	std::ostringstream str;


	str << std::endl;
	if (elnm != NULL)
	{
		str << "\tELN solver instance: " << get_name() << " (cluster "
				<< get_synchronization_object_data()->cluster_id << ")";
	}
	else
	{
		str << "\tLSF solver instance: " << get_name() << " (cluster "
				<< get_synchronization_object_data()->cluster_id << ")";
	}

		str << std::endl;
		str << "\t\thas calculated " << number_of_timesteps << " time steps the equation system was ";
		str << (number_of_reinit) << " times re-initialized"<< std::endl;
		if(top_vec.size()>0)
		{
			str << "\t\tthe following max. " << N_TOP_MOD;
			str << " modules requested the most re-initializations:";
			str << std::endl;
			for (unsigned long j = 0; j < top_vec.size(); j++)
			{
			   str << "\t\t\t" << top_vec[j]->name() << "\t"
					<< top_vec[j]->reinit_request_cnt;
			   str << std::endl;
			}
		}
		if(pwl_coeff_available)
		{
			str << "\t\tNumber of pwl iterations: ";
			str << statistics_pwl_segment_changes+statistics_pwl_timestep_changes;
			str << std::endl;
			str << "\t\t\tNumber of segement changes: " << statistics_pwl_segment_changes;
			str << std::endl;
			str << "\t\t\tNumber of timestep changes: " << statistics_pwl_timestep_changes;
			str << std::endl;
			str << "\t\tMaximum iterations per global timestep: ";
			str << statistics_max_pwl_segment_iterations+statistics_max_pwl_timestep_iterations;
			str << std::endl;
			str << "\t\t\tMaximum segment changes per global step: ";
			str << statistics_max_pwl_segment_iterations;
			str << std::endl;
			str << "\t\t\tMaximum timestep changes per global step: ";
			str << statistics_max_pwl_timestep_iterations;
			str << std::endl;
			if(statistics_max_pwl_timestep_iterations>0)
			{
				str << "\t\t\tMinimum timestep during pwl iteration: ";
				str << statistics_pwl_min_timestep << " sec";
				str << std::endl;
			}
		}

#ifndef DISABLE_PERFORMANCE_STATISTICS

		if(this->collect_profile_data)
		{
			double overalltime=sca_get_curr_simcontext()->get_consumed_wallclock_time_in_sec();
			str << "\tThis solver instance consumed: " << this->duration.count() << " s /  " << int(this->duration.count()/overalltime*100.0+0.5) << "% wall clock time";
		}
#endif

		SC_REPORT_INFO("SystemC-AMS",str.str().c_str());


}


const char* sca_linear_solver::kind() const
{
	return "sca_linear_solver";
}

//////////////////////////////////////////

void sca_linear_solver::store_checkpoint()
{
	ana_store_solver_check_point(internal_solver_data,x_flat,&global_cp);
	checkp_dt=dt_global;
}

//////////////////////////////////////////

void sca_linear_solver::restore_checkpoint()
{
	ana_restore_solver_check_point(internal_solver_data,x_flat,&global_cp);
	cp_restored=true;


	if(pwl_coeff_available)
	{
		search_valid_segments(x_flat);

		//due the last timestep was calculated to the end, the B matrix
		//and the q vector
		//should contain the current value, however the equation is may
		//initialized with the wrong stepsize - was before restore a reinit
		//we  must use the same re-initialization mode

		if((dt!=dt_global) || (last_reinit_flag==1))
		{
			equation_if->get_reinit_request() = last_reinit_flag;
		}


	}
}


///////////////////////////////////////////

void sca_linear_solver::call_methods(sca_util::sca_implementation::sca_method_vector* methods)
{
	for (sca_util::sca_implementation::sca_method_vector::iterator
			it = methods->begin(); it
			!= methods->end(); ++it)
	{
		it->call_method();
	}
}

///////////////////////////////////////////

void sca_linear_solver::error_message(int error, int method, double n_dt)
{
	//error message for error caused during (re)initialization or Woodbury formula

	using namespace std;

	long row=-1, column=-1;
	ana_get_error_position(internal_solver_data,&row,&column);

	sc_core::sc_object* obj_row=get_object_of_equation(row);
	sc_core::sc_object* obj_column=get_object_of_equation(column);

	ostringstream str;

	if (method == 0)
		str << "Initialization equation system failed in " << get_name() << ": " << error << endl;
	else
	{
		str << "Reinitialization failed in " << get_name() << ": " << error << endl;

		if (method == 1)
		{
			str << "  during reinitialization equation system using woodbury algorithm at "
					<< get_current_time() << " (dt = " << n_dt << ")." << endl;
		}
		else if (method == 2)
		{
			str << "  during reinitialization equation system at "
					<< get_current_time() << " (dt = " << n_dt << ")." << endl;
		}
		else if (method == 3)
		{
			str << "  during LU decomposition of A at "
					<< get_current_time() << endl;
		}
	}

	str << "  The error is in the following net (max. 50): " << endl;
	for (unsigned long i = 0; i < associated_module_list.size(); ++i)
	{
		if (i >= 50)
			break;
		str << "\t" << associated_module_list[i]->name() << endl;
	}

	if ((method != 0) && (method != 3))
	{
		str << endl;
		str << "Parameters of the following modules changed for the current time step:" << endl;
		for (unsigned long i = 0; i < associated_module_list.size(); ++i)
		{
			sca_core::sca_implementation::sca_conservative_module* cmod;
			cmod=dynamic_cast<sca_core::sca_implementation::sca_conservative_module*>(associated_module_list[i]);

			if(call_id==cmod->call_id_request_init)
			{
				str << "\t" << cmod->name();
				if(cmod->value_set) str << " changed to " << cmod->new_value;
				str << endl;
			}
		}
	}

	if((obj_row!=NULL)||(obj_column!=NULL))
	{
		str << endl;
		str << "The error is may  be near: " << endl << "\t\t";
		if(obj_row!=NULL)
		{
			str << obj_row->name();
			if(obj_column!=obj_row) str << "  and " << endl << "\t\t";
		}

		if(obj_column!=NULL) str << obj_column->name();

		str << endl;
	}

	SC_REPORT_ERROR("SystemC-AMS", str.str().c_str());

	return;
}

} // namespace sca_implementation
} // namespace sca_core
