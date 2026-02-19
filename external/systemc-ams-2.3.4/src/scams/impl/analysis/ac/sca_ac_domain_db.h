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

  sca_ac_domain_db.h - description

  Original Author: Karsten Einwich Fraunhofer IIS/EAS Dresden

  Created on: 02.01.2010

   SVN Version       :  $Revision: 2046 $
   SVN last checkin  :  $Date: 2017-09-20 12:41:42 +0000 (Wed, 20 Sep 2017) $
   SVN checkin by    :  $Author: karsten $
   SVN Id            :  $Id: sca_ac_domain_db.h 2046 2017-09-20 12:41:42Z karsten $

 *****************************************************************************/

/*****************************************************************************/


#ifndef SCA_AC_DOMAIN_DB_H_
#define SCA_AC_DOMAIN_DB_H_

namespace sca_core
{
namespace sca_implementation
{
class sca_solver_base;
}
}


namespace sca_ac_analysis
{
namespace sca_implementation
{


class sca_ac_domain_entity;
class sca_ac_domain_eq;

class sca_ac_domain_db
{
public:

	sca_ac_domain_db();
	~sca_ac_domain_db();

    void register_entity(sca_ac_domain_entity* entity);
    void register_arc(sc_core::sc_interface* arc);
    void register_add_eq_arc(sc_core::sc_interface* arc,
                             long* rel_pos,
                             bool* ignore_flag = NULL);

    void set_current_node_data(
                      sca_util::sca_vector<sca_util::sca_complex >* node_x,
                      sca_util::sca_vector<sca_util::sca_complex >* node_y,
                      double                        w,
                      bool                          bflag
                               );

    sca_util::sca_vector<sca_util::sca_complex >& get_inp_vector_ref()
    {
        return x_in;
    }

    sca_util::sca_vector<sca_util::sca_complex >& get_result_vector_ref()
    {
        return y_result;
    }

    sca_util::sca_vector<sca_util::sca_complex >& get_current_inp_vector()
    {
        return *current_x;
    }

    sca_util::sca_vector<sca_util::sca_complex >& get_current_outp_vector()
    {
        return *current_y;
    }

    inline double get_current_freq()
    {
        return current_w/(2.0*M_PI);
    }

    inline double get_current_w()
    {
        return current_w;
    }

    inline bool is_ac_domain()
    {
        return ac_domain;
    }

    inline bool is_noise_domain()
    {
        return noise_domain;
    }

    inline bool set_src_value()
    {
    	return bflag;
    }

    inline bool is_initialized() { return initialized;}


    sca_ac_domain_eq& get_equations()
    {
    	return *equations;
    }

    long get_start_of_add_equations(sca_core::sca_implementation::sca_solver_base* solv)
    {
    	std::map<sca_core::sca_implementation::sca_solver_base*,long>::iterator it;
    	it=add_eq_start_map.find(solv);
    	if(it!=add_eq_start_map.end()) return it->second;
    	else return -1;
    }

    sc_core::sc_object* get_active_object()
    {
    	return dynamic_cast<sc_core::sc_object*>(active_object);
    }

    bool is_elaborated()
    {
    	return elaborated;
    }

    void set_elaborated()
    {
    	elaborated=true;
    }


    long get_arc_id(const sc_core::sc_interface* arc) const;
    long get_port_id(const sc_core::sc_port_base* port);
    long get_inport_id(const sc_core::sc_port_base* port);
    long get_outport_id(const sc_core::sc_port_base* port);

    //currently only for sc_interface
    sc_core::sc_object* find_object_for_id(long id);

private:

    double current_w;

    sca_util::sca_vector<sca_util::sca_complex >* current_x;
    sca_util::sca_vector<sca_util::sca_complex >* current_y;


    friend class sca_ac_domain_entity;
    friend class sca_ac_domain_eq;

    sca_util::sca_vector<sca_util::sca_complex >  x_in;
    sca_util::sca_vector<sca_util::sca_complex >  y_result;

    sca_ac_domain_eq* equations;

    struct add_eq_arc_data_t
    {
        sc_core::sc_interface*    arc;
        long*            rel_pos;
        bool*            ignore_flag;

        add_eq_arc_data_t():arc(NULL),rel_pos(NULL),ignore_flag(NULL){}
    };

    std::vector<sca_ac_domain_entity*>       entities;
    std::set<sc_core::sc_interface*>         arcs;
    std::vector<add_eq_arc_data_t>           add_eq_arcs;

    std::map<sc_core::sc_interface*,long>     arc_map;
    std::map<const sc_core::sc_port_base*,long>    inport_map;
    std::map<const sc_core::sc_port_base*,long>    outport_map;
    std::map<sca_core::sca_implementation::sca_solver_base*,long> add_eq_start_map;

    friend class sca_ac_domain_solver;

    void init_arc_map_for_add_eqs();

    sca_ac_analysis::sca_ac_object* active_object;
    bool ac_domain;
    bool noise_domain;
    bool bflag;   //signs that all inports set to zero -> B calculation
                  //is used for noise domain to ensure an independend value
    bool initialized;

    bool elaborated;
};


} // namespace sca_ac_analysis
} // namespace sca_ac_analysis


#endif /* SCA_AC_DOMAIN_DB_H_ */
