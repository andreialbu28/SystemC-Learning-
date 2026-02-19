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

  systemc-ams.h - Top-level include file for the SystemC AMS library with usings.

  Original Author: Karsten Einwich Fraunhofer IIS/EAS Dresden

  Created on: 03.03.2009

   SVN Version       :  $Revision: 1914 $
   SVN last checkin  :  $Date: 2016-02-23 18:06:06 +0000 (Tue, 23 Feb 2016) $
   SVN checkin by    :  $Author: karsten $
   SVN Id            :  $Id: systemc-ams.h 1914 2016-02-23 18:06:06Z karsten $

 *****************************************************************************/
/*
     The header file named systemc-ams.h shall add names from the
     namespace sca_core, sca_ac and sca_util as defined in this subclause to
     the declarative region in which it is included. It is recommended that an
     implementation keep to a minimum the number of additional implementation-specific
     names introduced by this header file.

*/
/*****************************************************************************/


#ifndef SYSTEMCAMS_H_
#define SYSTEMCAMS_H_

//begin LRM definition

#include "systemc.h"
#include "systemc-ams"

// Using declarations for the following names in the sca_ac namespace
using sca_ac_analysis::sca_ac_start;
using sca_ac_analysis::sca_ac_noise_start;
using sca_ac_analysis::sca_ac;
using sca_ac_analysis::sca_ac_is_running;
using sca_ac_analysis::sca_ac_noise;
using sca_ac_analysis::sca_ac_noise_is_running;
using sca_ac_analysis::sca_ac_f;
using sca_ac_analysis::sca_ac_w;
using sca_ac_analysis::sca_ac_s;
using sca_ac_analysis::sca_ac_z;
using sca_ac_analysis::sca_ac_delay;
using sca_ac_analysis::sca_ac_ltf_nd;
using sca_ac_analysis::sca_ac_ltf_zp;
using sca_ac_analysis::sca_ac_ss;
using sca_ac_analysis::SCA_LOG;
using sca_ac_analysis::SCA_LIN;


// Using declarations for the following names in the sca_util namespace
using sca_util::sca_trace_file;
using sca_util::sca_trace;
using sca_util::sca_create_tabular_trace_file;
using sca_util::sca_close_tabular_trace_file;
using sca_util::sca_create_vcd_trace_file;
using sca_util::sca_close_vcd_trace_file;
using sca_util::sca_write_comment;
using sca_util::sca_complex;
using sca_util::sca_matrix;
using sca_util::sca_vector;
using sca_util::sca_create_vector;
using sca_util::sca_information_on;
using sca_util::sca_information_off;
using sca_util::SCA_AC_REAL_IMAG;
using sca_util::SCA_AC_MAG_RAD;
using sca_util::SCA_AC_DB_DEG;
using sca_util::SCA_NOISE_SUM;
using sca_util::SCA_NOISE_ALL;
using sca_util::SCA_INTERPOLATE;
using sca_util::SCA_DONT_INTERPOLATE;
using sca_util::SCA_HOLD_SAMPLE;
using sca_util::sca_ac_format;
using sca_util::sca_noise_format;
using sca_util::sca_decimation;
using sca_util::sca_sampling;
using sca_util::sca_multirate;
using sca_util::SCA_COMPLEX_J;
using sca_util::SCA_INFINITY;
using sca_util::SCA_UNDEFINED;
namespace sca_info = sca_util::sca_info;

// Using declarations for the following names in the sca_core namespace
using sca_core::sca_parameter;
using sca_core::sca_time;
using sca_core::sca_max_time;
using sca_core::sca_copyright;
using sca_core::sca_version;
using sca_core::sca_release;

//new SystemC-AMS 2.0 feature
using sca_tdf::SCA_CT_CUT;
using sca_tdf::SCA_DT_CUT;
using sca_tdf::SCA_NO_CUT;


//end LRM definition

//pre C++11 convienient function
using sca_util::sca_create_pair_vector;


#endif /* SYSTEMCAMS_H_ */
