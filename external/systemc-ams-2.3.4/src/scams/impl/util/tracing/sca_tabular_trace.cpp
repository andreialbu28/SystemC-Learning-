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

 sca_tabular_trace.cpp - description

 Original Author: Karsten Einwich Fraunhofer IIS/EAS Dresden

 Created on: 16.11.2009

 SVN Version       :  $Revision: 2320 $
 SVN last checkin  :  $Date: 2023-06-08 12:47:38 +0000 (Thu, 08 Jun 2023) $
 SVN checkin by    :  $Author: pehrlich $
 SVN Id            :  $Id: sca_tabular_trace.cpp 2320 2023-06-08 12:47:38Z pehrlich $

 *****************************************************************************/

/*****************************************************************************/

#include "systemc-ams"
#include "scams/impl/util/tracing/sca_tabular_trace.h"
#include<limits>

namespace sca_util
{
namespace sca_implementation
{

void sca_tabular_trace::construct()
{
	ac_format = real_imag;
	enable_sampling = false;
	decimation_factor = 1;
	noise_all_flag = false;
	no_interpolation = false;
	hold_sample=false;

	dec_cnt = 1;
}

//////////////////////////////////////////////////////////////////////////////

sca_tabular_trace::sca_tabular_trace(const std::string& name)
{
	if(name.find('.')==std::string::npos)
	{
		create_trace_file(name+".dat");
	}
	else
	{
		create_trace_file(name);
	}
	construct();
}

//////////////////////////////////////////////////////////////////////////////

void sca_tabular_trace::reopen_impl(const std::string& name, std::ios_base::openmode m)
{
	std::string nm;

	if(name.find('.')==std::string::npos)
	{
		nm=name+".dat";
	}
	else
	{
		nm=name;
	}

	sca_implementation::sca_trace_file_base::reopen(nm,m);

}

//////////////////////////////////////////////////////////////////////////////

sca_tabular_trace::sca_tabular_trace(std::ostream& str)
{
	create_trace_file(str);
	construct();
}

//////////////////////////////////////////////////////////////////////////////

static bool SCA_ENABLE_PHYSICAL_UNIT_TRACING=false;
void sca_enable_physical_unit_tracing() {SCA_ENABLE_PHYSICAL_UNIT_TRACING=true;}


void sca_tabular_trace::write_domain_unit(sca_util::sca_traceable_object* trace_object)
{
	if(!SCA_ENABLE_PHYSICAL_UNIT_TRACING) return;

	sca_core::sca_physical_domain_interface* phd=
			dynamic_cast<sca_core::sca_physical_domain_interface*>(trace_object);

	if (phd != NULL)
	{
		if (phd->get_domain() != "")
		{
			(*outstr) << "[" << phd->get_domain();
			if (phd->get_unit() != "")
			{
				(*outstr) << "(" << phd->get_unit_prefix();
				(*outstr) << phd->get_unit() << ")";
			}
			(*outstr) << "]";
		}
		else
		{
			if (phd->get_unit() != "")
			{
				(*outstr) << "(" << phd->get_unit_prefix();
				(*outstr) << phd->get_unit() << ")";
			}
		}
	}

}

//////////////////////////////////////////////////////////////////////////////


void sca_tabular_trace::write_header()
{
	outstr->precision(std::numeric_limits< double >::digits10);
	(*outstr) << "%time";
	for (std::vector<sca_trace_object_data>::iterator it = traces.begin(); it
			!= traces.end(); it++)
	{
		if(it->get_type_info().type_id==sca_type_explorer_base::SCA_COMPLEX)
		{
			(*outstr) << " " << (*it).name << ".real";
			write_domain_unit(it->trace_object);
			(*outstr) << " " << (*it).name << ".imag";
			write_domain_unit(it->trace_object);
		}
		else
		{
			(*outstr) << " " << (*it).name;
			write_domain_unit(it->trace_object);
		}

	}
	(*outstr) << std::endl;
}

//////////////////////////////////////////////////////////////////////////////

void sca_tabular_trace::write_waves()
{
	if (enable_sampling)
	{
		write_waves_sampled();
		return;
	}

	bool ready_flag = false;
	sca_core::sca_time ctime;
	sca_trace_buffer::value_buffer* values;

	while (!ready_flag)
	{
		if (!no_interpolation)
		{

			ready_flag = buffer->read_next_stamp_interpolate(ctime, values,hold_sample);
		}
		else
		{
			ready_flag = buffer->read_next_stamp(ctime, values);
		}

		//no point available
		if (values == NULL)
			break;

		if (decimation_factor > 1)
		{
			dec_cnt--;
			if (dec_cnt <= 0)
			{
				dec_cnt = decimation_factor;
			}
			else
				continue;
		}

		(*outstr) << ctime.to_seconds();

		for (int i = 0; i < values->size(); i++)
		{
			(*outstr) << " ";

			if ((*values)[i] != NULL)
			{
				(*values)[i]->print(*outstr);
			}
			else
			{
				if (no_interpolation)
				{
					if(this->traces[i].get_type_info().type_id==sca_type_explorer_base::SCA_COMPLEX)
					{
						(*outstr) << "* *";
					}
					else
					{
						(*outstr) << '*';
					}
				}
				else
				{
					SC_REPORT_FATAL("SystemC-AMS","Error due a bug");
				}
			}
		}
		(*outstr) << std::endl;
	}
}

///////////////////////////////////////////////////////////////////////////

void sca_tabular_trace::write_waves_sampled()
{
	bool ready_flag = false;
	sca_core::sca_time ctime;
	sca_trace_buffer::value_buffer* values;

	while (!ready_flag)
	{
		ready_flag = buffer->read_sample_interpolate(next_sample_time, values);
		if (ready_flag)
			break;

		(*outstr) << next_sample_time.to_seconds();

		for (int i = 0; i < values->size(); i++)
		{
			(*outstr) << " ";
			((*values)[i])->print(*outstr);
		}

		next_sample_time += sample_time;

		(*outstr) << std::endl;
	}

}

///////////////////////////////////////////////////////////////////////////

//trace type specific mode manipulations
void sca_tabular_trace::set_mode_impl(const sca_util::sca_trace_mode_base& mode)
{
	this->sync_with_thread();

	if (dynamic_cast<const sca_util::sca_ac_format*> (&mode))
	{
		const sca_util::sca_ac_fmt fmt =
				dynamic_cast<const sca_util::sca_ac_format*> (&mode)->ac_format;
		switch (fmt)
		{
		case sca_util::SCA_AC_REAL_IMAG:
			ac_format = real_imag;
			break;
		case sca_util::SCA_AC_MAG_RAD:
			ac_format = mag_rad;
			break;
		case sca_util::SCA_AC_DB_DEG:
			ac_format = db_deg;
			break;
		default:
			SC_REPORT_ERROR("SystemC-AMS", "Unknown sca_ac_format");
		}
	}
	else if (dynamic_cast<const sca_util::sca_noise_format*> (&mode))
	{
		const sca_util::sca_noise_fmt
				fmt =
						dynamic_cast<const sca_util::sca_noise_format*> (&mode)->noise_format;
		switch (fmt)
		{
		case sca_util::SCA_NOISE_SUM:
			noise_all_flag = false;
			break;
		case sca_util::SCA_NOISE_ALL:
			noise_all_flag = true;
			break;
		default:
			SC_REPORT_ERROR("SystemC-AMS", "Unknown sca_noise_format");
		}
	}
	else if (dynamic_cast<const sca_util::sca_decimation*> (&mode))
	{
		decimation_factor
				= dynamic_cast<const sca_util::sca_decimation*> (&mode)->decimation_factor;
	}
	else if (dynamic_cast<const sca_util::sca_sampling*> (&mode))
	{

		const sca_util::sca_sampling* mobj =
				dynamic_cast<const sca_util::sca_sampling*> (&mode);

		sca_core::sca_time tstep = mobj->time_step;
		if (tstep != sc_core::SC_ZERO_TIME)
		{
			enable_sampling = true;
			sample_time = tstep;
			next_sample_time = sc_core::sc_time_stamp() + mobj->time_offset;
		}
		else
		{
			enable_sampling = false;
		}
	}
	else if (dynamic_cast<const sca_util::sca_multirate*> (&mode))
	{
		const sca_util::sca_multirate* mobj =
				dynamic_cast<const sca_util::sca_multirate*> (&mode);


		if(mobj->fmt==sca_util::SCA_DONT_INTERPOLATE) no_interpolation = true;
		else                                          no_interpolation = false;

		if(mobj->fmt==sca_util::SCA_HOLD_SAMPLE)      hold_sample=true;
		else                                          hold_sample=false;
	}

}


void sca_tabular_trace::write_ac_domain_init ()
{
    (*outstr) << "%frequency";

    std::string first_val, second_val;
    if(ac_format==mag_rad)
    {
        first_val=".mag";
        second_val=".rad";
    }
    else if(ac_format==db_deg)
    {
        first_val=".db";
        second_val=".deg";
    }
    else
    {
        first_val=".real";
        second_val=".imag";
    }

    for(std::vector<sca_util::sca_implementation::sca_trace_object_data>::iterator
    		it  = traces.begin();
            it != traces.end();
            it++ )
    {
        (*outstr) << " " << (*it).name + first_val;
        (*outstr) << " " << (*it).name + second_val;
    }
    (*outstr) << std::endl;
    outstr->precision(std::numeric_limits< double >::digits10);

}

void sca_tabular_trace::write_ac_noise_domain_init(sca_util::sca_vector<std::string>& src_name)
{

	   (*outstr) << "%frequency";

	    std::string first_val, second_val;
	    if(ac_format==mag_rad)
	    {
	        first_val=".mag";
	        second_val=".rad";
	    }
	    else if(ac_format==db_deg)
	    {
	        first_val=".db";
	        second_val=".deg";
	    }
	    else
	    {
	        first_val=".real";
	        second_val=".imag";
	    }

	    if(noise_all_flag)
	    {
	        for(std::vector<sca_util::sca_implementation::sca_trace_object_data>::iterator
	        		it  = traces.begin();
	                it != traces.end();
	                it++ )
	        {
	            (*outstr) << " " << (*it).name + first_val;
	            for(unsigned int i=0;i<src_name.length();i++)
	            {
	                (*outstr) << " " << (*it).name + "(" +src_name(i)+")" + first_val;
	                (*outstr) << " " << (*it).name + "(" +src_name(i)+")" + second_val;
	            }
	        }
	    }
	    else
	    {
	        for(std::vector<sca_util::sca_implementation::sca_trace_object_data>::iterator
	        		it  = traces.begin();
	                it != traces.end();
	                it++ )
	        {
	            (*outstr) << " " << (*it).name + first_val;
	        }
	    }
	    (*outstr) << std::endl;
	    outstr->precision(std::numeric_limits< double >::digits10);
}

void sca_tabular_trace::write_ac_domain_stamp (double w,
                                    std::vector <sca_util::sca_complex >&tr_vec)
{
	ac_active = true;

    (*outstr) << w / (2.0*M_PI) ;

    for(std::vector<sca_util::sca_complex >::iterator it  = tr_vec.begin();
            it != tr_vec.end();
            it++)
    {
        if(ac_format==mag_rad)
        {
            (*outstr) << "  " << abs(*it) << " " << arg(*it);
        }
        else if(ac_format==db_deg)
        {
            double abs_val=abs(*it);
            if(abs_val<1e-32)
                abs_val=1e-32;
            (*outstr) << "  " << 20.0*log10(abs_val)
            << " " << 180.0/M_PI*arg(*it);
        }
        else //real_imag
        {
            (*outstr) << "  " << (*it).real() << " " << (*it).imag();
        }
    }

    (*outstr) << std::endl;
}


void sca_tabular_trace::write_ac_noise_domain_stamp(
    double w,
    sca_util::sca_matrix<sca_util::sca_complex >& tr_matrix)
{
	ac_active = true;


    (*outstr) << w / (2.0*M_PI) ;

    long n_src;
    if(noise_all_flag)
        n_src=tr_matrix.n_cols();
    else
        n_src=1; //only overall noise


    for(unsigned long point=0;point<tr_matrix.n_rows();point++)
    {
        for(long nsrc=0;nsrc<n_src;nsrc++)
        {
        	double val1, val2;
        	sca_util::sca_complex res=tr_matrix(point,nsrc);

            if(ac_format==mag_rad)
            {
            	val1=abs(res);
            	val2=arg(res);
            }
            else if(ac_format==db_deg)
            {
                double abs_val=abs(res);
                if(abs_val<1e-32)
                    abs_val=1e-32;
                val1=20.0*log10(abs_val);
                val2=180.0/M_PI*arg(res);
            }
            else //real_imag
            {
            	val1=res.real();
            	val2=res.imag();
            }
            if(nsrc==0) (*outstr) << "  " << val1;
            else        (*outstr) << "  " << val1 << " " << val2;
        }

    }
    (*outstr) << std::endl;
}

} // namespace sca_implementation
} // namespace sca_util

