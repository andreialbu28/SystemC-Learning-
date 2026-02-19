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

 sca_lsf_tdf_sink.h - linear signal flow converter to tdf

 Original Author: Karsten Einwich Fraunhofer IIS/EAS Dresden

 Created on: 05.03.2009

 SVN Version       :  $Revision: 1265 $
 SVN last checkin  :  $Date: 2011-11-19 20:43:31 +0000 (Sat, 19 Nov 2011) $
 SVN checkin by    :  $Author: karsten $
 SVN Id            :  $Id: sca_lsf_tdf_sink.h 1265 2011-11-19 20:43:31Z karsten $

 *****************************************************************************/
/*
 * LRM clause 4.2.1.17.
 * The class sca_lsf::sca_tdf::sca_sink shall implement a primitive module for
 * the LSF MoC that realizes a scaled conversion from an LSF signal to a
 * TDF signal. The value of the LSF input signal x(t) shall be scaled with
 * coefficient scale and written to the TDF output port outp. The primitive
 * shall not contribute any equation to the equation system.
 */

/*****************************************************************************/

#ifndef SCA_LSF_TDF_SINK_H_
#define SCA_LSF_TDF_SINK_H_

namespace sca_lsf
{

namespace sca_tdf
{

//class sca_sink : public implementation-derived-from sca_core::sca_module
class sca_sink: public sca_lsf::sca_module
{
public:
	sca_lsf::sca_in x; // LSF input

	::sca_tdf::sca_out<double> outp; // TDF output

	sca_core::sca_parameter<double> scale; // scale coefficient

	virtual const char* kind() const;

	explicit sca_sink(sc_core::sc_module_name, double scale_ = 1.0);

	//begin implementation specific

private:
	virtual void matrix_stamps();
	void assign_result();

		//end implementation specific

};

} // namespace sca_tdf

typedef sca_lsf::sca_tdf::sca_sink sca_tdf_sink;

} // namespace sca_lsf

#endif /* SCA_LSF_TDF_SINK_H_ */
