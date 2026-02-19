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

  sca_vcd_trace.h - description

  Original Author: Karsten Einwich Fraunhofer IIS/EAS Dresden

  Created on: Jan 19, 2010

   SVN Version       :  $Revision: 1265 $
   SVN last checkin  :  $Date: 2011-11-19 20:43:31 +0000 (Sat, 19 Nov 2011) $
   SVN checkin by    :  $Author: karsten $
   SVN Id            :  $Id: sca_vcd_trace.h 1265 2011-11-19 20:43:31Z karsten $

 *****************************************************************************/

/*****************************************************************************/


#ifndef SCA_VCD_TRACE_H_
#define SCA_VCD_TRACE_H_

namespace sca_util
{
namespace sca_implementation
{

class sca_vcd_trace : public sca_util::sca_trace_file
{
public:


    void close();

    sca_vcd_trace (const std::string& name);
    sca_vcd_trace (std::ostream & str);

    void write_ac_domain_init ();
    void write_ac_noise_domain_init(sca_util::sca_vector<std::string>& src_name);
    void write_ac_domain_stamp(double w, std::vector<sca_util::sca_complex>& tr_vec);
    void write_ac_noise_domain_stamp(double w,sca_util::sca_matrix<sca_util::sca_complex>& tr_matrix);

protected:

    void write_header ();
    void write_waves ();

    void write_value(sca_trace_value_handler_base* value);
    bool write_type_def(sca_type_explorer_base& typeinfo);

private:

    void reopen_impl(const std::string& name, std::ios_base::openmode m=std::ios_base::out |
                    std::ios_base::trunc );


    void construct ();

    bool warned_ac;

    std::string create_shortcut(unsigned long n);

    std::string integer2string(sc_dt::uint64 val,unsigned long wl);

    std::vector<std::string> name_map;

};


} // namespace sca_implementation
} // namespace sca_util



#endif /* SCA_VCD_TRACE_H_ */
