/*****************************************************************************

    Copyright 2010-2014
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

  sca_solver_base.h - description

  Original Author: Karsten Einwich Fraunhofer IIS/EAS Dresden / COSEDA Technologies

  Created on: 14.05.2009

   SVN Version       :  $Revision: 2320 $
   SVN last checkin  :  $Date: 2023-06-08 12:47:38 +0000 (Thu, 08 Jun 2023) $
   SVN checkin by    :  $Author: pehrlich $
   SVN Id            :  $Id: sca_solver_base.h 2320 2023-06-08 12:47:38Z pehrlich $

 *****************************************************************************/

/*****************************************************************************/


#ifndef SCA_SOLVER_BASE_H_
#define SCA_SOLVER_BASE_H_


#include <systemc-ams>
#include "scams/impl/synchronization/sca_synchronization_obj_if.h"
#include "scams/impl/synchronization/sca_sync_value_handle.h"
#include "scams/impl/util/tracing/sca_trace_object_data.h"


namespace sca_core
{

class sca_module;
class sca_interface;

namespace sca_implementation
{

/**
  Solver base class. Implements the synchronization object interface of
  the synchronization layer, due a solver is a synchronization object
  ( @ref #sca_synchronization_obj_if ). A solver is an object. Thus a
  solver can have a method which will be called by the synchronization
  layer.
*/
class sca_solver_base :  public sca_synchronization_obj_if,
                         public ::sc_core::sc_object,
                         public sca_ac_analysis::sca_ac_object
{

  typedef  std::vector<sca_synchronization_port_data*>            sca_sync_portListT;
  typedef  std::vector<sca_synchronization_port_data*>::iterator  sca_sync_portListItT;


 public:

   //overloaded kind method of sc_object
   virtual const char* kind() const;

   virtual void print_post_solve_statisitcs(){}

  sca_solver_base(const char* solver_name,
		          std::vector<sca_core::sca_module*>& associated_modules,
		          std::vector<sca_core::sca_interface*>& associated_channels);

  virtual ~sca_solver_base();

  /** method for passing implementation defined solver parameter*/
  virtual void set_solver_parameter(
		  sca_core::sca_module* mod,
		  const std::string& par,
		  const std::string& val);


  long get_cluster_id();

  virtual void initialize()=0;

    /**
     * Adds trace which is activated (the trace method of the object is called)
     * after each cluster calculation by the synchronization layer
     */
    void add_cluster_trace(sca_util::sca_implementation::sca_trace_object_data& tr_obj);

    /**
     * Adds trace which is activated (the trace method of the object is called)
     * by the specific solver (traces usually signals/values inside a solver)
     * returns true if successful
     */
  virtual bool add_solver_trace(sca_util::sca_implementation::sca_trace_object_data& tr_obj);

  void push_back_sync_port(
                            sca_core::sca_implementation::sca_port_base*  port,
                            sca_core::sca_implementation::sca_port_direction_enum      dir,
                            sca_core::sca_interface*  ch,
                            unsigned long&  rate,
                            const unsigned long&  delay,
                            sca_core::sca_time&        T,
                            sc_dt::int64&              timestep_change_id
                          );


  void push_back_to_analog(
		                    sca_core::sca_implementation::sca_port_base*  port,
		                    sca_core::sca_interface*  sig,
                            unsigned long&  rate,
                            unsigned long&  delay,
                            sca_core::sca_time&        T,
                            sc_dt::int64&              timestep_change_id

                          );

  void push_back_from_analog(
		                    sca_core::sca_implementation::sca_port_base*  port,
                            sca_core::sca_interface*  sig,
                            unsigned long&  rate,
                            unsigned long&  delay,
                            sca_core::sca_time&        T,
                            sc_dt::int64&              timestep_change_id
                          );

  void push_back_to_systemc(
		                    sca_core::sca_implementation::sca_port_base*  port,
		                    sca_core::sca_interface*  sig,
                            unsigned long&  rate,
                            const unsigned long&  delay,
                            sca_core::sca_time&        T,
                            sc_dt::int64&              timestep_change_id
                          );

  void push_back_from_systemc(
		                    sca_core::sca_implementation::sca_port_base*  port,
                            sca_core::sca_interface*  sig,
                            unsigned long&  rate,
                            unsigned long&  delay,
                            sca_core::sca_time&        T,
                            sc_dt::int64&              timestep_change_id
                          );


  const long& get_calls_per_period()  { return   calls_per_period ? *calls_per_period : long_zero; }
  const long& get_last_calls_per_period()
  { return   last_calls_per_period ? *last_calls_per_period : long_zero; }
  const sca_core::sca_time& get_cluster_period() { return   csync_data   ? csync_data->cluster_period   : not_valid_time;}
  const sca_core::sca_time& get_last_cluster_period() { return   csync_data   ? csync_data->last_cluster_period   : not_valid_time;}
  const sca_core::sca_time& get_current_cluster_timestep() { return   csync_data   ? csync_data->current_cluster_timestep   : not_valid_time;}
  sc_dt::int64& get_call_counter() { return   solver_object_data.call_counter;     }
  unsigned long get_call_number_in_current_period() { return solver_object_data.call_counter%get_calls_per_period(); }
  bool*& get_allow_processing_access_flag_ref() { return   allow_processing_access_flag;     }

  sca_cluster_synchronization_data* get_cluster_synchronization_data()
  {
	  if(csync_data==NULL)
	  {
		  SC_REPORT_ERROR("SystemC-AMS",
				  "Internal error due a bug - Access to not initializes cluster synchronization data");
	  }
	  return csync_data;
  }


    /** registers method which is called after the event ev
     * will be used to trace an channel from SystemC-kernel
     * a handle of the concrete type has to be provided
     */
    void register_sc_value_trace( const ::sc_core::sc_event& ev,
                                  sca_sync_value_handle_base& handle);


    /** if SystemC has not reached time the sca process of the current cluster
     * suspended, after resume the corresponding value
     * (stored by the trace process) is returned
     */
    void get_sc_value_on_time(::sc_core::sc_time time,sca_sync_value_handle_base&  handle);



    /** writes with write_method a value to a channel at time ctime or maybe
     * timeless - the SystemC-kernel will suspend (and thus resume the sca -
     * cluster) not later then next_time - during this activation the mehtod
     * must be called to remove the old and schedule a new next_time -
     * otherwise it should give an exception, due we deadlocked
     * for timeless (ordered) synchronization next_time can be set to a large
     * value or SC_INVALID_TIME - in this case the sc_channel suspends if a
     * value required (we have to do nothing therefore)
     */
    void write_sc_value( ::sc_core::sc_time ctime, ::sc_core::sc_time next_time,
                         sca_sync_value_handle_base&  handle);

    /** registers synchronization from a sca domain to a sc domain -
     * limits the sc time to the first sca schedule time
     */
     void register_sca_schedule( ::sc_core::sc_time next_time,
                                 sca_sync_value_handle_base& handle);



    /** returns current cluster start time -> of the current period
     * calls of set_max_cluster_start_time of the current period have no
     * influence
     */
     sca_core::sca_time get_cluster_start_time();

    /** returns the current time when the object of the current cluster has
     *  been called
     */
     sca_core::sca_time get_current_time();

    /** returns the current  period of the object of the current cluster call
     * this time corresponds to cluster_period/(obj. calls per period)
     * a change in the current cluster call (using set_max_period) has not yet
     * influence
     */
     sca_core::sca_time get_current_period();

     sca_core::sca_time get_last_period(int offset=0);

 	/** returns the current  duration of the cluster execution -> equals
 	 * period-period/(calls of the module with the maximum executions)
 	 */
 	sca_core::sca_time get_cluster_duration();

    /** sets for the next cluster start the start maximal time - if the method
     *  called from different objects the minimum time will be choosen - the
     * method returns the number of object calls before the time will be
     * accepted (in a multirate system the number of object calls in the current
     * period - in single rate system this number will be always 0)- ntime is
     * setted to the time used if the return value >0
     * (ntime = ret_val * cluster_period/calls_per_period_timestep_ch_obj)
     */
     long set_max_start_time(const sca_core::sca_time& ntime);

     /** sets the maximum time period which will be valid up to the next cluster
      * start (the time is the cluster_period/(obj calls per cluster) ), if the
      * time is set from other objects of the clusters also the time resulting
      * from the minimum cluster_period will be used
      * the method returns the number of object calls in the current period
      * before the time will be accepted  (in a single rate system this number
      * will be always 0)
      */
      long set_max_period(const sca_core::sca_time& nperiod, sc_core::sc_object* obj);

      /** registers event for activation of the next cluster start
       * the method returns the time before the event can be
       * accepted (events before will be ignored) - for single rate always <
       * cluster period and if no t0 specified it should be equal to the
       * current time
       * after restarting the cluster the events will be removed from the list
       * -> thus the events has to be re-registered every cluster period
       */
      ::sc_core::sc_time register_activation_event(const sc_core::sc_event& ev);

      //method for new 1.x dtdf feature - sets the maximum start time of the cluster
      //abstime is the absolut time
      void request_next_activation_abs(const sca_core::sca_time& abstime);

      void request_next_activation(const sc_core::sc_event&);
      void request_next_activation(const sc_core::sc_event_or_list& evlist);
      void request_next_activation(const sc_core::sc_event_and_list& evlist);

      void change_timestep(const sca_core::sca_time&, sc_core::sc_object* req_object);

      sca_core::sca_time get_max_timestep();

      bool is_dynamic_tdf();
      bool are_attribute_changes_allowed();
      bool are_attribute_changes_may_allowed();
      bool are_attributes_changed();

      void attribute_change_flags_changed();

      virtual void reset();

      bool associated_module_deleted;

      /** implementation of sca_synchronization_obj_if methods, which returns
       * a pointer to the solver specific time data
       */
      sca_synchronization_object_data* get_synchronization_object_data();

      bool not_clustered();

      const std::string& get_name() const;

      virtual std::string get_name_associated_names(int max_num=-1) const;



  //Synchronization layer interface for synchronization objects


  /** Implements @ref #class sca_synchronization_obj_if method */
  unsigned long get_number_of_sync_ports(sca_port_direction_enum direction);

  /** Implements @ref #class sca_synchronization_obj_if method */
  sca_synchronization_port_data* get_sync_port(sca_port_direction_enum direction, unsigned long number);

 private:

  /** Implements @ref #class sca_synchronization_obj_if method */
  void get_initialization_method(sc_object*& obj,sc_object_method& fct);

  /** Implements @ref #class sca_synchronization_obj_if method */
  void get_reinitialization_method(sc_object*& obj,sc_object_method& fct);

  /** Implements @ref #class sca_synchronization_obj_if method */
  void get_change_attributes_method(sc_object*& obj,sc_object_method& fct);

  /** Implements @ref #class sca_synchronization_obj_if method */
  void get_processing_method(sc_object*& obj,sc_object_method& fct);

  /** Implements @ref #class sca_synchronization_obj_if method */
  void get_post_method(sc_object*& obj,sc_object_method& fct);

  /**mechanism to transfer datas from the sync-cluster to the solver obj */
  void set_sync_data_references(sca_cluster_synchronization_data& crefs);



  /**calls post proc methof if available */
  void terminate();

protected:

  sca_sync_portListT to_analog;
  sca_sync_portListT from_analog;

  sca_sync_portListT to_systemc;
  sca_sync_portListT from_systemc;

  sc_object_method   init_method;
  sc_object*         init_method_object;

  sc_object_method   reinit_method;
  sc_object*         reinit_method_object;

  sc_object_method   change_attributes_method;
  sc_object*         change_attributes_method_object;

  sc_object_method   processing_method;
  sc_object*         processing_method_object;

  sc_object_method   post_method;
  sc_object*         post_method_object;

  /** lists of channels and modules */
  std::vector<sca_core::sca_module*>    associated_module_list;
  std::vector<sca_core::sca_interface*> associated_channel_list;

  std::vector<sca_util::sca_implementation::sca_trace_object_data*>  solver_traces;

  sca_synchronization_object_data solver_object_data;

private:

    const long    long_zero;
    const ::sc_core::sc_time not_valid_time;
    //refernces to sync-layer datas
    sca_cluster_synchronization_data* csync_data;

    const sc_dt::int64* call_counter;
    bool* allow_processing_access_flag;
    const long*      calls_per_period;
    const long*      last_calls_per_period;


    class sca_sync_trace_handle
    {
     public:
        sca_core::sca_implementation::sca_sync_value_handle_base*    value_handle;
        const ::sc_core::sc_event*                activation_event;
        std::vector< ::sc_core::sc_time>                time_points;
        sc_core::sc_process_handle      method_process_handle;

        long id;               //position in vector
        long size, index_start, count; //variables for ringbuffer handling

        void resize(long n)
        {
            long old_size=(long)(time_points.size());

          time_points.resize(n);
          value_handle->resize(n);
          size = n;

          //correct ringbuffer
          for(long j=0, k=old_size; (j<index_start)&&(k<size); ++j, ++k)
          {
           time_points[k]=time_points[j];
           value_handle->set_index(j);
           value_handle->read_tmp();
           value_handle->set_index(k);
           value_handle->store_tmp();
          }

          for(long j=size-old_size, k=0; j<index_start; ++j, ++k)
          {
           time_points[k]=time_points[j];
           value_handle->set_index(j);
           value_handle->read_tmp();
           value_handle->set_index(k);
           value_handle->store_tmp();
          }
        }

       sca_sync_trace_handle()
       {
          value_handle=NULL;
          activation_event=NULL;
          count=0;
          size=0;
          id=-1;
          index_start=0;
       }
    };
    std::vector<sca_sync_trace_handle> sync_trace_handles;

    class sca_sync_write_handle
    {
     public:
       sca_core::sca_implementation::sca_sync_value_handle_base* value_handle;
       std::vector< ::sc_core::sc_time>             time_points;
       ::sc_core::sc_event*                   activation_event;
       sc_core::sc_process_handle             method_process_handle;

       long id;               //position in vector
       long size, index_start, count; //variables for ringbuffer handling

       void resize(long n)
       {
          long old_size=(long)time_points.size();

          size=n;
          time_points.resize(n);
          value_handle->resize(n);

          value_handle->backup_tmp();

          //correct ringbuffer
          //copy elements from 0 to index_start to the new allocated
          //elements
          // 2 3 0 1 -> x 3 0 1 2
          for(long j=0, k=old_size; (j<index_start)&&(k<size); ++j, ++k)
          {
           time_points[k]=time_points[j];
           value_handle->set_index(j);
           value_handle->read_tmp();
           value_handle->set_index(k);
           value_handle->store_tmp();
          }

          // x 3 0 1 2 -> 3 x 0 1 2
          for(long j=size-old_size, k=0; j<index_start; ++j, ++k)
          {
           time_points[k]=time_points[j];
           value_handle->set_index(j);
           value_handle->read_tmp();
           value_handle->set_index(k);
           value_handle->store_tmp();
          }

          value_handle->restore_tmp();
       }

       sca_sync_write_handle()
       {
          value_handle=NULL;
          activation_event=NULL;
          count=0;
          size=0;
          id=-1;
          index_start=0;
       }
    };

    std::vector<sca_sync_write_handle>    sca_sync_write_handles;

    void sc_value_trace(int id);
    void sc_write_value_process(int id);
};

} //namespace sca_implementation
} //namespace sca_core

#endif /* SCA_SOLVER_BASE_H_ */
