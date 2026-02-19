/*****************************************************************************

    Copyright 2010-2014
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

  sca_ac_functions.cpp - description

  Original Author: Karsten Einwich Fraunhofer IIS/EAS Dresden

  Created on: 03.01.2010

   SVN Version       :  $Revision: 2320 $
   SVN last checkin  :  $Date: 2023-06-08 12:47:38 +0000 (Thu, 08 Jun 2023) $
   SVN checkin by    :  $Author: pehrlich $
   SVN Id            :  $Id: sca_ac_functions.cpp 2320 2023-06-08 12:47:38Z pehrlich $

 *****************************************************************************/

/*****************************************************************************/

#include "systemc-ams"
#include "scams/analysis/ac/sca_ac_functions.h"
#include "scams/impl/analysis/ac/sca_ac_domain_globals.h"
#include "scams/impl/analysis/ac/sca_ac_domain_db.h"
#include "scams/impl/analysis/ac/sca_ac_domain_eq.h"
#include "scams/impl/analysis/ac/sca_ac_domain_solver.h"

namespace sca_ac_analysis
{

namespace sca_implementation
{

static sca_util::sca_complex dummy_complex(0.0,0.0);

sca_util::sca_complex& sca_ac(const sc_core::sc_port_base& port)
{
	if(!sca_ac_is_running())
	{
		std::ostringstream str;
		str << "Function sca_ac can only be called while a ac simulation is ";
		str << "running - only from the context of the ac_processing callback";
		str << " called for port: " << port.name();
		SC_REPORT_ERROR("SystemC-AMS",str.str().c_str());
	}

	if( !get_ac_database().is_initialized()  )
	{
	    	return dummy_complex;
	}

    long port_id = get_ac_database().get_inport_id(&port);
    if(port_id>=0)
    {
        return (get_ac_database().get_current_inp_vector())(port_id);
    }
    else
    {
        port_id = get_ac_database().get_outport_id(&port);
        if(port_id>=0)
        {
            return (get_ac_database().get_current_outp_vector())(port_id);
        }
    }

    dummy_complex=sca_util::sca_complex(0.0,0.0);
    return dummy_complex;
}

////////////////////////////////////////////

sca_util::sca_complex& sca_ac_noise(const sc_core::sc_port_base& port, std::string src_name)
{
	if(!sca_ac_is_running())
	{
		std::ostringstream str;
		str << "Function sca_ac_noise can only be called while a ac simulation is ";
		str << "running - only from the context of the ac_processing callback";
		str << " called for port: " << port.name();
		SC_REPORT_ERROR("SystemC-AMS",str.str().c_str());
	}

    //only if all inports zero and noise domain is enabled the
    //src value can be set -> thus he is independend from the inports
    if( !get_ac_database().is_initialized() || !get_ac_database().set_src_value() || !sca_ac_noise_is_running() )
    {
    	return dummy_complex;
    }


    long port_id = get_ac_database().get_outport_id(&port);
    if(port_id>=0)
    {
        long id=get_ac_database().get_arc_id(const_cast<sc_core::sc_port_base*>(&port)->get_interface());
        if(id>=0)
        {
            if(src_name=="")    src_name=port.name();

            get_ac_database().get_equations().get_noise_src_names()(id)=src_name;
            return (get_ac_database().get_equations().get_Bgnoise())(id);
        }

    }
    else //otherwise it's an error
    {

        port_id = get_ac_database().get_inport_id(&port);
        if(port_id>=0)
        {
            std::ostringstream str;
            str << "AC-Noise-simulation - noise assignment to an inport is not allowed: "
            <<  port.name() << std::endl;
            SC_REPORT_ERROR("SystemC-AMS",str.str().c_str());
        }
    }

    return dummy_complex;
}

}

//LRM clause 5.2.2.3.
bool sca_ac_is_running()
{
	return sca_ac_analysis::sca_implementation::get_ac_database().is_ac_domain();
}

//LRM clause 5.2.2.4.
bool sca_ac_noise_is_running()
{
	return sca_ac_analysis::sca_implementation::get_ac_database().is_noise_domain();
}

//LRM clause 5.2.2.5.
double sca_ac_f()
{
	return sca_ac_analysis::sca_implementation::get_ac_database().get_current_freq();
}

//LRM clause 5.2.2.6.
double sca_ac_w()
{
	return sca_ac_analysis::sca_implementation::get_ac_database().get_current_w();
}


//LRM clause 5.2.2.7.
sca_util::sca_complex sca_ac_s(long n)
{
	return std::pow( sca_util::SCA_COMPLEX_J * sca_ac_w(),double(n));
}

//LRM clause 5.2.2.8.
sca_util::sca_complex sca_ac_z(long n, const sca_core::sca_time& tstep)
{
	return std::exp( sca_util::SCA_COMPLEX_J * (sca_ac_w()*(double(n)*tstep.to_seconds())) );
}


sca_util::sca_complex sca_ac_z(long n)
{
	if(!sca_ac_analysis::sca_ac_is_running())
	{
		std::ostringstream str;
		str << " sca_ac_analysis::sca_ac_z(long n) can only be used during "
		       " ac- or ac-noise simulation";
		SC_REPORT_ERROR("SystemC-AMS",str.str().c_str());

	}


	sc_core::sc_object* obj=
		sca_ac_analysis::sca_implementation::get_ac_database().get_active_object();
	sca_tdf::sca_module* tdf_obj=dynamic_cast<sca_tdf::sca_module*>(obj);

	if(tdf_obj==NULL)
	{
		std::ostringstream str;
		str << " sca_ac_analysis::sca_ac_z(long n) can only be used "
			   " in the context of a sca_tdf::sca_module ";
		SC_REPORT_ERROR("SystemC-AMS",str.str().c_str());
		return sca_util::sca_complex(0.0,0.0);
	}

	double tstep=((sca_core::sca_module*)tdf_obj)->get_timestep().to_seconds();
	return exp( sca_util::SCA_COMPLEX_J * (sca_ac_w()*(double(n)*tstep)) );
}

//LRM clause 5.2.2.9.
sca_util::sca_complex sca_ac_delay(const sca_core::sca_time& delay)
{
	return exp( -sca_util::SCA_COMPLEX_J * (sca_ac_w()*delay.to_seconds()));
}


//LRM clause 5.2.2.10.
sca_util::sca_complex sca_ac_ltf_nd(const sca_util::sca_vector<double>& num,
		const sca_util::sca_vector<double>& den,
		const sca_util::sca_complex& input, double k)
{
    sca_util::sca_complex dens=den(0);
    sca_util::sca_complex nums=num(0);
    sca_util::sca_complex s1 =sca_ac_s(1);
    sca_util::sca_complex c_s=s1;

    for(unsigned long i=1;i<den.length();i++)
    {
        dens+=den(i)*c_s;
        c_s*=s1;
    }

    c_s=s1;
    for(unsigned long i=1;i<num.length();i++)
    {
        nums+=num(i)*c_s;
        c_s*=s1;
    }

    return nums/dens * input * k;
}

sca_util::sca_complex sca_ac_ltf_nd(const sca_util::sca_vector<double>& num,
		const sca_util::sca_vector<double>& den,
		const sca_core::sca_time& ct_delay,
		const sca_util::sca_complex& input, double k)
{
	return sca_ac_ltf_nd(num,den,input,k)*sca_ac_delay(ct_delay);
}

//LRM clause 5.2.2.11.
sca_util::sca_complex sca_ac_ltf_zp(const sca_util::sca_vector<
		sca_util::sca_complex>& zeros, const sca_util::sca_vector<
		sca_util::sca_complex>& poles,
		const sca_util::sca_complex& input, double k)
{
    sca_util::sca_complex s1 =sca_ac_s(1);
    sca_util::sca_complex num, den;

    num=1.0;
    for(unsigned long i=0;i<zeros.length();i++)
    {
        num*=s1-zeros(i);
    }

    den=1.0;
    for(unsigned long i=0;i<poles.length();i++)
    {
        den*=s1-poles(i);
    }

    return k * num/den * input;
}

sca_util::sca_complex sca_ac_ltf_zp(const sca_util::sca_vector<
		sca_util::sca_complex>& zeros, const sca_util::sca_vector<
		sca_util::sca_complex>& poles,
		const sca_core::sca_time& ct_delay,
		const sca_util::sca_complex& input, double k)
{
	return sca_ac_ltf_zp(zeros,poles,input,k)*sca_ac_delay(ct_delay);
}



//LRM clause 5.2.2.12.
sca_util::sca_vector<sca_util::sca_complex> sca_ac_ss(
		const sca_util::sca_matrix<double>& a, const sca_util::sca_matrix<
				double>& b, const sca_util::sca_matrix<double>& c,
		const sca_util::sca_matrix<double>& d, const sca_util::sca_vector<
				sca_util::sca_complex>& input)
{
    if(a.n_rows()!=a.n_cols())
    {
        std::ostringstream str;
        str << "Error in sca_ac_ss: A must be a squre matrix"
        " (dim.: number of states)" << std::endl;
        SC_REPORT_ERROR("SystemC-AMS",str.str().c_str());
    }

    if(a.n_rows()!=b.n_rows())
    {
        std::ostringstream str;
        str << "Error in sca_ac_ss: dim. of A must be equal to number of rows of B"
        " (number of states)" << std::endl;
        SC_REPORT_ERROR("SystemC-AMS",str.str().c_str());
    }

    if(input.length()!=d.n_cols())
    {
        std::ostringstream str;
        str << "Error in sca_ac_ss: size of input vector must be equal to number of columns of D"
        " (number of inputs)" << std::endl;
        SC_REPORT_ERROR("SystemC-AMS",str.str().c_str());
    }

    if(a.n_rows()!=0)
    {
        if(input.length()!=b.n_cols())
        {
            std::ostringstream str;
            str << "Error in sca_ac_ss: size of input vector must be equal tonumber of columns of B"
            " (number of inputs)" << std::endl;
            SC_REPORT_ERROR("SystemC-AMS",str.str().c_str());
        }

        if(a.n_cols()!=c.n_cols())
        {
            std::ostringstream str;
            str << "Error in sca_ac_ss: dim. of A must be equal to number of columns of C"
            " (number of states)" << std::endl;
            SC_REPORT_ERROR("SystemC-AMS",str.str().c_str());
        }

        if(c.n_rows()!=d.n_rows())
        {
            std::ostringstream str;
            str << "Error in sca_ac_ss: number of rows of C must be equal to number of rows of D"
            " (number of outputs)" << std::endl;
            SC_REPORT_ERROR("SystemC-AMS",str.str().c_str());
        }
    }
    else
    {  //only algebraisch part
        sca_util::sca_vector<sca_util::sca_complex> out;
        out.resize(d.n_cols());

        for(unsigned long i=0;i<d.n_rows();i++)
        {
            out(i)=0.0;
            for(unsigned long j=0;j<input.length();j++)
            {
                out(i)+=d(i,j)*input(j);
            }
        }
        return out;
    }

    sca_util::sca_matrix<sca_util::sca_complex> ac;
    ac.set_sparse_mode();
    ac.resize(a.n_rows(),a.n_cols());

    sca_util::sca_complex jw =sca_util::SCA_COMPLEX_J * sca_ac_w();

    for(unsigned long i=0;i<a.n_rows();i++)  // A - E*jw
    {
        for(unsigned long j=0;j<a.n_cols();j++)
        {
            ac(i,j)=a(j,i);
            if(i==j)
                ac(j,i)-=jw;
        }
    }

    sca_util::sca_vector<sca_util::sca_complex> bc;
    bc.resize(b.n_rows());

    for(unsigned long i=0;i<b.n_rows();i++) // -B * x
    {
        bc(i)=0.0;
        for(unsigned long j=0;j<b.n_cols();j++)
        {
            bc(i)-=input(j)*b(i,j);
        }
    }

    sca_util::sca_vector<sca_util::sca_complex> sc;
    sc.resize(b.n_rows());

    //ac * s = bc -> -B*x = A*s -E*jw*s  (x constant input vector)
    //result s-vector
    sca_ac_analysis::sca_implementation::solve_linear_complex_eq_system solver;
    int err=solver.solve(ac,bc,sc);

    if(err!=0)
    {
    	std::ostringstream str;
    	str << "Can't solve equation system for state space equations for frequency: ";
    	str << sca_ac_analysis::sca_ac_f() << " Hz" << std::endl;

    	SC_REPORT_WARNING("SystemC-AMS",str.str().c_str());
    }


    sca_util::sca_vector<sca_util::sca_complex> out;
    out.resize(c.n_rows());

    for(unsigned long i=0;i<c.n_rows();i++)
    {
        out(i)=0.0;
        for(unsigned long j=0;j<sc.length();j++)
        {
            out(i)+=c(i,j)*sc(j);
        }

        for(unsigned long j=0;j<input.length();j++)
        {
            out(i)+=d(i,j)*input(j);
        }
    }

    return out;
}


///////////////////////////////////////////////////////

sca_util::sca_vector<sca_util::sca_complex> sca_ac_ss(
		const sca_util::sca_matrix<double>& a, const sca_util::sca_matrix<
				double>& b, const sca_util::sca_matrix<double>& c,
		const sca_util::sca_matrix<double>& d,
		const sca_core::sca_time& ct_delay,
		const sca_util::sca_vector<sca_util::sca_complex>& input)
{

	sca_util::sca_vector<sca_util::sca_complex> inp_tmp(input.length());

	for(unsigned long i=0;i<input.length();i++) inp_tmp(i)=input(i)*sca_ac_delay(ct_delay);

	return sca_ac_ss(a,b,c,d,inp_tmp);
}


///////////////////////////////////////////

sca_util::sca_vector<sca_util::sca_complex> sca_ac_ss(
		const sca_util::sca_matrix<double>& a, const sca_util::sca_matrix<
				double>& b, const sca_util::sca_matrix<double>& c,
		const sca_util::sca_matrix<double>& d)
{

	sca_util::sca_vector<sca_util::sca_complex> input(b.n_cols());

	for(unsigned long i=0;i<input.length();i++) input(i)=1.0;

	return sca_ac_ss(a,b,c,d,input);
}

///////////////////////////////////////////////////////

sca_util::sca_vector<sca_util::sca_complex> sca_ac_ss(
		const sca_util::sca_matrix<double>& a, const sca_util::sca_matrix<
				double>& b, const sca_util::sca_matrix<double>& c,
		const sca_util::sca_matrix<double>& d,
		const sca_core::sca_time& ct_delay)
{

	sca_util::sca_vector<sca_util::sca_complex> input(b.n_cols());

	for(unsigned long i=0;i<input.length();i++) input(i)=sca_ac_delay(ct_delay);

	return sca_ac_ss(a,b,c,d,input);
}

} // namespace sca_ac_analysis
