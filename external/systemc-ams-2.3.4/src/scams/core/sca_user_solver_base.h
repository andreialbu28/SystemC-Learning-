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

  sca_generic_solver.h - description

  Original Author: Karsten Einwich Fraunhofer IIS/EAS Dresden

  Created on: 8.01.2014

   SVN Version       :  $Revision: 1620 $
   SVN last checkin  :  $Date: 2013-08-30 14:48:25 +0200 (Fri, 30 Aug 2013) $
   SVN checkin by    :  $Author: karsten $
   SVN Id            :  $Id: sca_solver_base.h 1620 2013-08-30 12:48:25Z karsten $

 *****************************************************************************/




/*****************************************************************************/

#ifndef SCA_GENERIC_SOLVER_H_
#define SCA_GENERIC_SOLVER_H_



namespace sca_core
{

class sca_module;

namespace sca_implementation
{
	class sca_generic_user_solver;
}

/**
  Abstract class for deriving a  generic (user defined) solver
*/
class sca_user_solver_base : public sc_core::sc_object
{

public:

	sca_user_solver_base();
	explicit sca_user_solver_base(const char*);
	virtual ~sca_user_solver_base();


    virtual void initialize();
    virtual void processing();

    //TODO not yet called from scheduler do it's not a TDF module
    virtual void change_attributes();
    virtual void reinitialize();


    virtual void end_of_simulation();

    /** Implements the pure virtual print_post_solve_statisitcs method of @ref #sca_solver_base. */
    virtual void print_post_solve_statisitcs();


    /** set implementation defined solver parameter */
    virtual void set_solver_parameter(
    		  sca_core::sca_module* mod,
    		  const std::string& par,
    		  const std::string& val);

    virtual const char* kind() const;

    //we can change the traceable object in solver data
    //we can call this function from add_solver_trace
    //with an id
    //returns true if succesful
    virtual bool trace_init(sca_util::sca_traceable_object*,int id);


    sca_util::sca_implementation::sca_trace_object_data& get_trace_obj_data(int id);

    //this function will be called by the solver to send a value to the trace file
    template<class T>
    void trace_from_user_solver(const sca_core::sca_time& ctime,const T& value,int user_solver_trace_id)
    {
    	sca_util::sca_implementation::sca_trace_object_data& data=get_trace_obj_data(user_solver_trace_id);
    	data.trace_base->store_time_stamp(data.id, ctime, value);
    }

    void send_trace_to_file();

    sca_core::sca_time get_time();
    sca_core::sca_time get_timestep();

    const std::vector<sca_core::sca_module*>&    get_associated_modules();
    const std::vector<sca_core::sca_interface*>& get_associated_channels();


    //** begin implementation specific */

	sca_core::sca_module* solver_requesting_module;
	sca_core::sca_implementation::sca_generic_user_solver* generic_solver;


};


namespace sca_implementation
{

class sca_user_solver_handler_base
{
public:

	virtual sca_core::sca_user_solver_base* create()                                  = 0;
	virtual sca_core::sca_user_solver_base* try_cast(sca_core::sca_user_solver_base*) = 0;

	virtual ~sca_user_solver_handler_base(){}
};

template<class T>
class sca_user_solver_handler : public sca_user_solver_handler_base
{
	sca_core::sca_user_solver_base* create()
	{
		return new T();
	}

	sca_core::sca_user_solver_base* try_cast(sca_core::sca_user_solver_base* s1)
	{
		return dynamic_cast<T*>(s1);
	}

};

} // namespace sca_implementation

} // namespace sca_core





#endif /* SCA_GENERALIZED_SOLVER_H_ */
