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

 sca_ac_noise_start.h - functions for starting small signal frequency domain noise analysis

 Original Author: Karsten Einwich Fraunhofer IIS/EAS Dresden

 Created on: 08.03.2009

 SVN Version       :  $Revision: 1265 $
 SVN last checkin  :  $Date: 2011-11-19 20:43:31 +0000 (Sat, 19 Nov 2011) $ (UTC)
 SVN checkin by    :  $Author: karsten $
 SVN Id            :  $Id: sca_ac_noise_start.h 1265 2011-11-19 20:43:31Z karsten $

 *****************************************************************************/
/*
 * LRM clause 5.2.1.3.2.
 * The functions sca_ac::sca_ac_noise_start shall perform a small-signal
 * frequency-domain noise simulation. The first function shall calculate the
 * frequency-domain noise behavior at npoints frequencies. If npoints is greater
 * than zero, the first frequency point in hertz shall be start_freq. If npoints
 * is greater than one, the last frequency point in hertz shall be stop_freq.
 * If scale is sca_ac::SCA_LOG, the remaining frequency points shall be
 * distributed logarithmically and if scale is sca_ac::SCA_LIN, the remaining
 * points shall be distributed linear.
 * The second function shall calculate the frequency-domain noise behavior at
 * the frequency points given by the vector frequencies.
 */

/*****************************************************************************/

#ifndef SCA_AC_NOISE_START_H_
#define SCA_AC_NOISE_START_H_

namespace sca_ac_analysis
{

void sca_ac_noise_start(double start_freq, double stop_freq,
		unsigned long npoints, sca_ac_analysis::sca_ac_scale scale = sca_ac_analysis::SCA_LOG);

void sca_ac_noise_start(const sca_util::sca_vector<double>& frequencies);

} // namespace sca_ac

#endif /* SCA_AC_NOISE_START_H_ */
