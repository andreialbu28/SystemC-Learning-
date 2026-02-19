/*****************************************************************************

    Copyright 2010
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

  sca_tdf_ct_ltf_zp_proxy.cpp - description

  Original Author: Karsten Einwich Fraunhofer IIS/EAS Dresden

  Created on: 15.12.2009

   SVN Version       :  $Revision: 1892 $
   SVN last checkin  :  $Date: 2016-01-10 11:59:12 +0000 (Sun, 10 Jan 2016) $
   SVN checkin by    :  $Author: karsten $
   SVN Id            :  $Id: sca_tdf_ct_ltf_zp_proxy.cpp 1892 2016-01-10 11:59:12Z karsten $

 *****************************************************************************/

/*****************************************************************************/

#include "systemc-ams"
#include "scams/impl/predefined_moc/tdf/sca_tdf_ct_ltf_nd_proxy.h"
#include "scams/impl/predefined_moc/tdf/sca_tdf_ct_ltf_zp_proxy.h"

namespace sca_tdf
{

namespace sca_implementation
{

sca_ct_ltf_zp_proxy::sca_ct_ltf_zp_proxy(sc_core::sc_object* object):sca_ct_ltf_nd_proxy(object)
{
	zeros_old_ref=NULL;
	poles_old_ref=NULL;
}

//quick check for coefficient change
inline bool sca_ct_ltf_zp_proxy::coeff_changed(
		const sca_util::sca_vector<sca_util::sca_complex>& zeros,
		const sca_util::sca_vector<sca_util::sca_complex>& poles)
{
	//was there a write access or has the object changed since the last call
	if (zeros.get_access_flag() || &zeros != zeros_old_ref)
	{
		zeros_old_ref = &zeros;

		//has the dimension changed since the last call
		if( (zeros.length() != zeros_old.length()) || zeros.length()==0)
		{
			zeros_old = zeros;
			poles_old = poles;
			poles_old_ref = &poles;

			const_cast<sca_util::sca_vector<sca_util::sca_complex>*> (&zeros)->reset_access_flag();
			const_cast<sca_util::sca_vector<sca_util::sca_complex>*> (&poles)->reset_access_flag();

			return true;
		}

		//has a value changed since the last call
		for (unsigned int i = 0; i < zeros.length(); i++)
			if (zeros[i] != zeros_old[i])
			{
				zeros_old = zeros;
				poles_old = poles;
				poles_old_ref = &poles;

				const_cast<sca_util::sca_vector<sca_util::sca_complex>*> (&zeros)->reset_access_flag();
				const_cast<sca_util::sca_vector<sca_util::sca_complex>*> (&poles)->reset_access_flag();

				return true;
			}
	}

	//the same for the denumerator
	if (poles.get_access_flag() || &poles != poles_old_ref)
	{
		poles_old_ref = &poles;

		if ((poles.length() != poles_old.length()) || poles.length()==0)
		{
			zeros_old = zeros;
			poles_old = poles;
			zeros_old_ref = &zeros;

			const_cast<sca_util::sca_vector<sca_util::sca_complex>*> (&zeros)->reset_access_flag();
			const_cast<sca_util::sca_vector<sca_util::sca_complex>*> (&poles)->reset_access_flag();

			return true;
		}

		for (unsigned int i = 0; i < poles.length(); i++)
			if (poles[i] != poles_old[i])
			{
				zeros_old = zeros;
				poles_old = poles;
				zeros_old_ref = &zeros;

				const_cast<sca_util::sca_vector<sca_util::sca_complex>*> (&zeros)->reset_access_flag();
				const_cast<sca_util::sca_vector<sca_util::sca_complex>*> (&poles)->reset_access_flag();

				return true;
			}
	}

	return false;
}

//overloaded sca_ltf_nd_proxy method - for skipping the test
bool sca_ct_ltf_zp_proxy::coeff_changed(
		const sca_util::sca_vector<double>& num,
		const sca_util::sca_vector<double>& den)
{
	return reinit_request;
}

/////////////////////


inline void sca_ct_ltf_zp_proxy::calculate_nd_coefficients()
{
	sca_util::sca_vector<sca_util::sca_complex> numc, denc;

     numc.resize(zeros_old.length()+1);
     denc.resize(poles_old.length()+1);

     num_old.resize(zeros_old.length()+1);
     den_old.resize(poles_old.length()+1);

    numc(0)=1.0; //if no zero num=1.0
    for(unsigned long i=0;i<zeros_old.length();i++)
    {
    	numc(i+1)=1.0;
        for(unsigned long j=i;j>0;j--)
        {
            numc(j) = numc(j-1) - numc(j)*zeros_old(i);  // * (s-zeros(i))
        }
        numc(0)=-zeros_old(i)*numc(0);
    }

    denc(0)=1.0; //if no pole den=1.0
    for(unsigned long i=0;i<poles_old.length();i++)
    {
    	denc(i+1)=1.0;
        for(unsigned long j=i;j>0;j--)
        {
            denc(j) =  denc(j-1) - denc(j)*poles_old(i);  // * (s-poles(i))
        }
        denc(0)=-poles_old(i)*denc(0);
    }


    for(unsigned long i=0;i<numc.length();i++)
    {
        if((fabs(numc(i).imag())>1e-32)&&fabs(numc(i).imag()/abs(numc(i)))>1e-13)
        {
            std::ostringstream str;
            str << "Error: Complex zeros must be conjugate complex in: "
            << ltf_object->name() << std::endl;
            SC_REPORT_ERROR("SystemC-AMS",str.str().c_str());
        }
        //copy to base class coefficients
        num_old (i) = numc(i).real();
    }


    for(unsigned long i=0;i<denc.length();i++)
    {
        if((fabs(denc(i).imag())>1e-32)&&fabs(denc(i).imag()/abs(denc(i)))>1e-13)
        {
            std::ostringstream str;
            str << "Error: Complex poles must be conjugate complex in: "
            << ltf_object->name() << " " << denc(i).imag() << std::endl;
            SC_REPORT_ERROR("SystemC-AMS",str.str().c_str());
        }
        //copy to base class coefficients
        den_old(i) = denc(i).real();
    }
}


void sca_ct_ltf_zp_proxy::register_zp(const sca_util::sca_vector<
		sca_util::sca_complex>& zeros, const sca_util::sca_vector<
		sca_util::sca_complex>& poles,
		sca_core::sca_time ct_delay,
		sca_util::sca_vector<double>& state, double input, double k,
		sca_core::sca_time tstep)
{
	if (pending_calculation)
	{
		std::ostringstream str;
		str
				<< "The previous ltf_zp calculation was performed without assigning"
					" the result for: " << ltf_object->name();
		SC_REPORT_ERROR("SystemC-AMS",str.str().c_str());
	}

	reinit_request = coeff_changed(zeros, poles);

	if(reinit_request)
	{
		calculate_nd_coefficients();
	}

	//mapp to nd method
	register_nd(num_old,den_old,ct_delay,state,input,k,tstep);

}

///////////

void sca_ct_ltf_zp_proxy::register_zp(const sca_util::sca_vector<sca_util::sca_complex>& zeros,
		const sca_util::sca_vector<sca_util::sca_complex>& poles,
		sca_core::sca_time ct_delay,
		sca_util::sca_vector<double>& state, const sca_util::sca_vector<
				double>& input, double k, sca_core::sca_time tstep)
{

	if (pending_calculation)
	{
		std::ostringstream str;
		str
				<< "The previous ltf_zp calculation was performed without assigning"
					" the result for: " << ltf_object->name();
		SC_REPORT_ERROR("SystemC-AMS",str.str().c_str());
	}

	reinit_request = coeff_changed(zeros, poles);

	if(reinit_request)
	{
		calculate_nd_coefficients();
	}

	//mapp to nd method
	register_nd(num_old,den_old,ct_delay,state,input,k,tstep);

}


////////////////

void sca_ct_ltf_zp_proxy::register_zp(const sca_util::sca_vector<sca_util::sca_complex>& zeros,
		const sca_util::sca_vector<sca_util::sca_complex>& poles,
		sca_core::sca_time ct_delay,
		sca_util::sca_vector<double>& state,
		const sca_tdf::sca_in<double>& input, double k)
{
	if (pending_calculation)
	{
		std::ostringstream str;
		str
				<< "The previous ltf_zp calculation was performed without assigning"
					" the result for: " << ltf_object->name();
		SC_REPORT_ERROR("SystemC-AMS",str.str().c_str());
	}

	reinit_request = coeff_changed(zeros, poles);

	if(reinit_request)
	{
		calculate_nd_coefficients();
	}

	//mapp to nd method
	register_nd(num_old,den_old,ct_delay,state,input,k);

}

////////

void sca_ct_ltf_zp_proxy::register_zp(
		const sca_util::sca_vector<sca_util::sca_complex>& zeros,
		const sca_util::sca_vector<sca_util::sca_complex>& poles,
		sca_core::sca_time ct_delay,
		sca_util::sca_vector<double>& state,
		const sca_tdf::sca_de::sca_in<double>& input,
		double k)
{
	if (pending_calculation)
	{
		std::ostringstream str;
		str
				<< "The previous ltf_zp calculation was performed without assigning"
					" the result for: " << ltf_object->name();
		SC_REPORT_ERROR("SystemC-AMS",str.str().c_str());
	}

	reinit_request = coeff_changed(zeros, poles);

	if(reinit_request)
	{
		calculate_nd_coefficients();
	}

	//mapp to nd method
	register_nd(num_old,den_old,ct_delay,state,input,k);

}


////////////

void sca_ct_ltf_zp_proxy::register_zp(const sca_util::sca_vector<sca_util::sca_complex>& zeros,
		const sca_util::sca_vector<sca_util::sca_complex>& poles,
		sca_core::sca_time ct_delay,
		double input, double k, sca_core::sca_time tstep)
{
	if (pending_calculation)
	{
		std::ostringstream str;
		str
				<< "The previous ltf_zp calculation was performed without assigning"
					" the result for: " << ltf_object->name();
		SC_REPORT_ERROR("SystemC-AMS",str.str().c_str());
	}

	reinit_request = coeff_changed(zeros, poles);

	if(reinit_request)
	{
		calculate_nd_coefficients();
	}

	//mapp to nd method
	register_nd(num_old,den_old,ct_delay,input,k,tstep);
}

///////////////

void sca_ct_ltf_zp_proxy::register_zp(const sca_util::sca_vector<sca_util::sca_complex>& zeros,
		const sca_util::sca_vector<sca_util::sca_complex>& poles,
		sca_core::sca_time ct_delay,
		const sca_util::sca_vector<double>& input, double k,
		sca_core::sca_time tstep)
{
	if (pending_calculation)
	{
		std::ostringstream str;
		str
				<< "The previous ltf_zp calculation was performed without assigning"
					" the result for: " << ltf_object->name();
		SC_REPORT_ERROR("SystemC-AMS",str.str().c_str());
	}

	reinit_request = coeff_changed(zeros, poles);

	if(reinit_request)
	{
		calculate_nd_coefficients();
	}

	//mapp to nd method
	register_nd(num_old,den_old,ct_delay,input,k,tstep);
}


////////////

void sca_ct_ltf_zp_proxy::register_zp(const sca_util::sca_vector<sca_util::sca_complex>& zeros,
		const sca_util::sca_vector<sca_util::sca_complex>& poles,
		sca_core::sca_time ct_delay,
		const sca_tdf::sca_in<double>& input, double k)
{
	if (pending_calculation)
	{
		std::ostringstream str;
		str
				<< "The previous ltf_zp calculation was performed without assigning"
					" the result for: " << ltf_object->name();
		SC_REPORT_ERROR("SystemC-AMS",str.str().c_str());
	}

	reinit_request = coeff_changed(zeros, poles);

	if(reinit_request)
	{
		calculate_nd_coefficients();
	}

	//mapp to nd method
	register_nd(num_old,den_old,ct_delay,input,k);
}


//////////

void sca_ct_ltf_zp_proxy::register_zp(const sca_util::sca_vector<sca_util::sca_complex>& zeros,
		const sca_util::sca_vector<sca_util::sca_complex>& poles,
		sca_core::sca_time ct_delay,
		const sca_tdf::sca_de::sca_in<double>& input, double k)
{

	if (pending_calculation)
	{
		std::ostringstream str;
		str
				<< "The previous ltf_zp calculation was performed without assigning"
					" the result for: " << ltf_object->name();
		SC_REPORT_ERROR("SystemC-AMS",str.str().c_str());
	}

	reinit_request = coeff_changed(zeros, poles);

	if(reinit_request)
	{
		calculate_nd_coefficients();
	}

	//mapp to nd method
	register_nd(num_old,den_old,ct_delay,input,k);
}




} //namespace sca_implementation
} //namespace sca_tdf
