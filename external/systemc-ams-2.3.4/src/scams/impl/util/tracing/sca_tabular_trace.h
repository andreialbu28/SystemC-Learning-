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

  sca_tabular_trace.h - description

  Original Author: Karsten Einwich Fraunhofer IIS/EAS Dresden

  Created on: 16.11.2009

   SVN Version       :  $Revision: 2016 $
   SVN last checkin  :  $Date: 2016-11-25 10:15:05 +0000 (Fri, 25 Nov 2016) $
   SVN checkin by    :  $Author: karsten $
   SVN Id            :  $Id: sca_tabular_trace.h 2016 2016-11-25 10:15:05Z karsten $

 *****************************************************************************/

/*****************************************************************************/


#ifndef SCA_TABULAR_TRACE_H_
#define SCA_TABULAR_TRACE_H_

namespace sca_util
{
namespace sca_implementation
{

class sca_tabular_trace : public sca_util::sca_trace_file
{
public:


    sca_tabular_trace (const std::string& name);
    sca_tabular_trace (std::ostream & str);

    void write_ac_domain_init ();
    void write_ac_noise_domain_init(sca_util::sca_vector<std::string>& src_name);

protected:

    //used to set trace type specific mode manipulations
    void set_mode_impl (const sca_util::sca_trace_mode_base & mode);

    enum ac_format_enum
    {
        real_imag = 0,
        db_deg    = 1,
        mag_rad   = 2
    } ac_format ;


    virtual void write_ac_domain_stamp (double w,
                                        std::vector <sca_util::sca_complex >&tr_vec);


    void write_ac_noise_domain_stamp(
        double w,
        sca_util::sca_matrix<sca_util::sca_complex >& tr_matrix);


    void write_header ();
    void write_waves ();

    unsigned long decimation_factor;

    bool enable_sampling;
    bool no_interpolation;
    bool hold_sample;
    bool noise_all_flag;
    sca_core::sca_time sample_time;

private:

    void reopen_impl(const std::string& name, std::ios_base::openmode m=std::ios_base::out |
                    std::ios_base::trunc );


    void construct ();

    void write_waves_sampled ();

    void write_domain_unit(sca_util::sca_traceable_object*);

    long dec_cnt;
    sca_core::sca_time next_sample_time;

};


} // namespace sca_implementation
} // namespace sca_util



#endif /* SCA_TABULAR_TRACE_H_ */
