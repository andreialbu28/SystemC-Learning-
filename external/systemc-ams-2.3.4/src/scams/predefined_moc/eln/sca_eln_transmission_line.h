/*****************************************************************************

    Copyright 2010-2012
    Fraunhofer-Gesellschaft zur Foerderung der angewandten Forschung e.V.

    Copyright 2015-2020
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

 sca_eln_transmission_line.h - electrical linear net transmission line

 Original Author: Karsten Einwich Fraunhofer IIS/EAS Dresden

 Created on: 07.03.2009

 SVN Version       :  $Revision: 2106 $
 SVN last checkin  :  $Date: 2020-02-26 15:58:39 +0000 (Wed, 26 Feb 2020) $ (UTC)
 SVN checkin by    :  $Author: karsten $
 SVN Id            :  $Id: sca_eln_transmission_line.h 2106 2020-02-26 15:58:39Z karsten $

 *****************************************************************************/
/*
 * LRM clause 4.3.1.15.
 * The class sca_eln::sca_transmission_line shall implement a primitive module
 * for the ELN MoC that represents a transmission line. The primitive shall
 * contribute the following equations to the equation system:
 *
 * v(a1,b1)(t) = z0 * i(a1,b1)(t)                                    t<delay
 * v(a1,b1)(t) = e**(-delta0*delay) * v(a2,b2)(t-delay)+z0*i(a2,b2)(t-delay)) +
 *               z0 * i(a1,b1)(t)                                    t>=delay
 *
 * v(a2,b2)(t) = z0 * i(a2,b2)(t)                                    t<delay
 * v(a2,b2)(t) = e**(-delta0*delay) * v(a1,b1)(t-delay)+z0*i(a1,b1)(t-delay)) +
 *               z0 * i(a2,b2)(t)                                    t>=delay
 *
 * where z0 is the characteristic impedance of the transmission line in ohm,
 * delay is the transmission delay in second and delta0 is the dissipation
 * factor in 1/seconds.v(a1,b1)(t) is the voltage across terminals a1 and b1,
 * v(a2,b2)(t) is the voltage across terminals a2 and b2,i(a1,b1)(t)is the
 * current flowing through the primitive from terminal a1 to terminal b1, and
 * i(a2,b2)(t) is the current flowing through the primitive from terminal
 * a2 to terminal b2.
 */

/*****************************************************************************/

#ifndef SCA_ELN_TRANSMISSION_LINE_H_
#define SCA_ELN_TRANSMISSION_LINE_H_

namespace sca_eln
{

//  class sca_transmission_line : public implementation-derived-from sca_core::sca_module
class sca_transmission_line: public sca_eln::sca_module
{
public:
	sca_eln::sca_terminal a1;
	sca_eln::sca_terminal b1;

	sca_eln::sca_terminal a2;
	sca_eln::sca_terminal b2;

	sca_core::sca_parameter<double> z0;
	sca_core::sca_parameter<sca_core::sca_time> delay;
	sca_core::sca_parameter<double> delta0;

	virtual const char* kind() const;

	explicit sca_transmission_line(sc_core::sc_module_name, double z0_ = 100.0,
			const sca_core::sca_time& delay_ = sc_core::SC_ZERO_TIME,
			double delta0_ = 0.0);

    //begin implementation specific

private:

	virtual void matrix_stamps();
	long nadd1, nadd2;

    double v_line1();
    double v_line2();
    void   pre_solve();
    void   post_solve();
    void   init_delays();

    double* delay_buffer_va1b1;
    double* delay_buffer_va2b2;

    double last_sample_va1b1;
    double last_sample_va2b2;

    unsigned long delay_buffer_size;
    unsigned long delay_cnt;
    unsigned long nsample;
    bool          initialized;
    double        delay_sec, dt_sec, dt_delay;

    double        k_diss;
    double        scale_last_sample;







    //end implementation specific
};

} // namespace sca_eln


#endif /* SCA_ELN_TRANSMISSION_LINE_H_ */
