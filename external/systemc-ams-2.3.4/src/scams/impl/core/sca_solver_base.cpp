/*****************************************************************************

    Copyright 2010-2013
    Fraunhofer-Gesellschaft zur Foerderung der angewandten Forschung e.V.

    Copyright 2015-2017
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

 sca_solver_base.cpp - description

 Original Author: Karsten Einwich Fraunhofer IIS Dresden / COSEDA Technologies

 Created on: 25.08.2009

 SVN Version       :  $Revision: 2320 $
 SVN last checkin  :  $Date: 2023-06-08 12:47:38 +0000 (Thu, 08 Jun 2023) $
 SVN checkin by    :  $Author: pehrlich $
 SVN Id            :  $Id: sca_solver_base.cpp 2320 2023-06-08 12:47:38Z pehrlich $

 *****************************************************************************/

/*****************************************************************************/

#define SC_INCLUDE_DYNAMIC_PROCESSES
#include "systemc-ams"
#include "scams/impl/core/sca_solver_base.h"
#include "scams/impl/core/sca_solver_manager.h"
#include "scams/impl/core/sca_object_manager.h"
#include "scams/impl/core/sca_simcontext.h"
#include "scams/core/sca_module.h"
#include "scams/impl/synchronization/sca_sync_value_handle.h"


namespace sca_core
{
namespace sca_implementation
{


////////////////////////////////////////////////////

// object kind for sc_object base class
static char kind_string[]="sca_solver";

const char* sca_solver_base::kind() const
{
	return kind_string;
}

////////////////////////////////////////////////////


sca_solver_base::sca_solver_base(const char* solver_name,
		std::vector<sca_core::sca_module*>& associated_modules,
        std::vector<sca_core::sca_interface*>& associated_channels) :
	::sc_core::sc_object(solver_name),
	         associated_module_list(associated_modules),
	         associated_channel_list(associated_channels),
	        long_zero(0),
	        not_valid_time(NOT_VALID_SCA_TIME()),
	        csync_data(NULL),
	        call_counter(NULL),
	        allow_processing_access_flag(NULL),
	        calls_per_period(NULL),
	        last_calls_per_period(NULL)

{
	if (solver_name == NULL)
		get_synchronization_object_data()->sync_obj_name = "unnamed_solver";
	else
		get_synchronization_object_data()->sync_obj_name = solver_name;

	for (std::vector<sca_core::sca_module*>::iterator it = associated_module_list.begin(); it
			!= associated_module_list.end(); ++it)
	{
		(*it)->sync_domain = this;
	}

	sca_solver_manager *solver_manager;
	solver_manager = sca_get_curr_simcontext()->get_sca_solver_manager();

	solver_manager->push_back_solver(this);


	init_method=NULL;
	init_method_object=NULL;

	reinit_method=NULL;
	reinit_method_object=NULL;

	change_attributes_method=NULL;
	change_attributes_method_object=NULL;

	processing_method=NULL;
	processing_method_object=NULL;

	post_method=NULL;
	post_method_object=NULL;

	associated_module_deleted=false;

	//ac per default disabled
	this->ac_disable();
}


///////////////////////////////////////////////////

void sca_solver_base::reset()
{
	this->get_synchronization_object_data()->cluster_id = -1;
}

////////////////////////////////////////////////////

sca_solver_base::~sca_solver_base()
{
	for (sca_sync_portListItT it = to_analog.begin(); it != to_analog.end(); ++it)
	{
		delete *it;
	}
	for (sca_sync_portListItT it = from_analog.begin(); it != from_analog.end(); ++it)
	{
		delete *it;
	}
	for (sca_sync_portListItT it = to_systemc.begin(); it != to_systemc.end(); ++it)
	{
		delete *it;
	}
	for (sca_sync_portListItT it = from_systemc.begin(); it
			!= from_systemc.end(); ++it)
	{
		delete *it;
	}

	for (std::vector<sca_sync_write_handle>::iterator it =
			sca_sync_write_handles.begin(); it != sca_sync_write_handles.end(); ++it)
	{
		if(it->method_process_handle.valid())
		{
			it->method_process_handle.kill();
		}


		if(it->activation_event!=NULL)
		{
			delete it->activation_event;
			it->activation_event=NULL;
		}
	}
}



void sca_solver_base::set_solver_parameter(
		  sca_core::sca_module* mod,
		  const std::string& par,
		  const std::string& val)
{
	std::ostringstream str;
	str << "Solver parameter: " << par << " value: " << val;
	str << " not supported for the solver: " << this->kind();
	str << " associated with module: " << mod->name();
	str << " of type: " << mod->kind();
	SC_REPORT_WARNING("SystemC-AMS",str.str().c_str());
}

long sca_solver_base::get_cluster_id()
{
	return solver_object_data.cluster_id;
}


void sca_solver_base::terminate()
{
	if(associated_module_deleted)
	{
		SC_REPORT_ERROR("SystemC-AMS",
				"It is not allowed to destroy modules before "
				"end_of_simulation (see SystemC LRM) "
				"e.g. before calling sc_core::sc_stop()");
		return;
	}
	else
	{
		if (post_method_object != NULL)
								(post_method_object->*post_method)();
	}
}

///////////////////////////////////////////////////


void sca_solver_base::push_back_sync_port(sca_port_base* port, sca_port_direction_enum dir,
		sca_interface* ch, unsigned long& rate,const unsigned long& delay,
		::sc_core::sc_time& T,sc_dt::int64& timestep_change_id)
{
	sca_core::sca_implementation::sca_synchronization_port_data* con =
			new sca_core::sca_implementation::sca_synchronization_port_data;

	con->parent_obj = this;
	con->port = port;

	if ((ch == NULL)&& ((dir==SCA_TO_ANALOG) | (dir==SCA_FROM_ANALOG)))
	{
		std::ostringstream str;
		//error should not be possible, due it has to be checked earlier ???
		str
				<< "Internal error No channel is connected to a solver connection in : "
				<< __FILE__ << "  line: " << __LINE__ << std::endl;
		SC_REPORT_ERROR("SystemC-AMS",str.str().c_str());
	}

	con->channel = dynamic_cast<sca_core::sca_prim_channel*> (ch);
	if( (con->channel == NULL) && ((dir==SCA_TO_ANALOG) | (dir==SCA_FROM_ANALOG)))
	{
		std::ostringstream str;
		//error should not be possible, due it has to be checked earlier ???
		str
				<< "Internal error interface is no sca_channel  connected to a solver connection in : "
				<< __FILE__ << "  line: " << __LINE__ << std::endl;
		SC_REPORT_ERROR("SystemC-AMS",str.str().c_str());
	}

	con->dir = dir;

	con->rate = &rate;
	con->delay = &delay;
	con->T_set = &T;
	con->timestep_change_id=&timestep_change_id;

	if(con->channel!=NULL) con->channel->register_connected_solver(con);

	switch (dir)
	{
	case SCA_TO_ANALOG:
		con->sync_port_number = (unsigned long)(to_analog.size());
		to_analog.push_back(con);
		break;
	case SCA_FROM_ANALOG:
		con->sync_port_number = (unsigned long)(from_analog.size());
		from_analog.push_back(con);
		break;
	case SCA_TO_SYSTEMC:
		con->sync_port_number = (unsigned long)(to_systemc.size());
		to_systemc.push_back(con);
		break;
	case SCA_FROM_SYSTEMC:
		con->sync_port_number = (unsigned long)(from_systemc.size());
		from_systemc.push_back(con);
		break;
	default:
		std::ostringstream str;
		// Error should not be possible
		str << "Internal error undefined channel direction: " << __FILE__
				<< "  line: " << __LINE__ << std::endl;
		SC_REPORT_ERROR("SystemC-AMS",str.str().c_str());
	}

}

///////////////////////////////////////////////////

unsigned long sca_solver_base::get_number_of_sync_ports(sca_port_direction_enum direction)
{
	switch (direction)
	{
	case SCA_TO_ANALOG:
		return static_cast<unsigned long>(to_analog.size());
	case SCA_FROM_ANALOG:
		return static_cast<unsigned long>(from_analog.size());
	case SCA_TO_SYSTEMC:
		return static_cast<unsigned long>(to_systemc.size());
	case SCA_FROM_SYSTEMC:
		return static_cast<unsigned long>(from_systemc.size());
	default:
		std::ostringstream str;
		str << "Internal error undefined channel direction: " << __FILE__
				<< "  line: " << __LINE__ << std::endl;
		SC_REPORT_ERROR("SystemC-AMS",str.str().c_str());
		return 0; // due compiler warning
	}
}

////////////////////////////////////////////////////

sca_synchronization_port_data*
sca_solver_base::get_sync_port(sca_port_direction_enum direction, unsigned long number)
{
	if (number < get_number_of_sync_ports(direction))
	{
		switch (direction)
		{
		case SCA_TO_ANALOG:
			return to_analog[number];
		case SCA_FROM_ANALOG:
			return from_analog[number];
		case SCA_TO_SYSTEMC:
			return to_systemc[number];
		case SCA_FROM_SYSTEMC:
			return from_systemc[number];
		default:
			std::ostringstream str;
			str << "Internal error undefined channel direction: " << __FILE__
					<< "  line: " << __LINE__ << std::endl;
			SC_REPORT_ERROR("SystemC-AMS",str.str().c_str());
		}
	}
	else
	{
		std::ostringstream str;
		str << "Channel number is greater than available channels in "
				<< "sca_solver_base::get_sync_port " << __FILE__ << "  line: "
				<< __LINE__ << std::endl;
		SC_REPORT_ERROR("SystemC-AMS",str.str().c_str());
	}

	return NULL; //due compiler Warning
}

///////////////////////////////////////////////////

void sca_solver_base::get_initialization_method(sc_object*& obj,
		sc_object_method& fct)
{
	obj = init_method_object;
	fct = init_method;
}

///////////////////////////////////////////////////

void sca_solver_base::get_reinitialization_method(sc_object*& obj,
		sc_object_method& fct)
{
	obj = reinit_method_object;
	fct = reinit_method;
}

///////////////////////////////////////////////////

// new SystemC-AMS 1.x dtdf extension
void sca_solver_base::get_change_attributes_method(sc_object*& obj,
		sc_object_method& fct)
{
	obj = change_attributes_method_object;
	fct = change_attributes_method;
}



///////////////////////////////////////////////////


void sca_solver_base::get_processing_method(sc_object*& obj,
		sc_object_method& fct)
{
	obj = processing_method_object;
	fct = processing_method;
}

///////////////////////////////////////////////////

void sca_solver_base::get_post_method(sc_object*& obj, sc_object_method& fct)
{
	obj = post_method_object;
	fct = post_method;
}

///////////////////////////////////////////////////

void sca_solver_base::push_back_to_analog(sca_port_base* port,
		sca_interface* sig, unsigned long& rate, unsigned long& delay,
		::sc_core::sc_time& T,
         sc_dt::int64&              timestep_change_id)
{
	push_back_sync_port(port, SCA_TO_ANALOG, sig, rate, delay, T,timestep_change_id);
}

///////////////////////////////////////////////////

void sca_solver_base::push_back_from_analog(sca_port_base* port,
		sca_interface* sig, unsigned long& rate, unsigned long& delay,
		::sc_core::sc_time& T,
         sc_dt::int64&              timestep_change_id)
{
	push_back_sync_port(port, SCA_FROM_ANALOG, sig, rate, delay, T,timestep_change_id);
}

///////////////////////////////////////////////////

void sca_solver_base::push_back_to_systemc(sca_port_base* port,
		sca_interface* sig, unsigned long& rate,const unsigned long& delay,
		::sc_core::sc_time& T,
         sc_dt::int64&              timestep_change_id)
{
	push_back_sync_port(port, SCA_TO_SYSTEMC, sig, rate, delay, T,timestep_change_id);
}

///////////////////////////////////////////////////

void sca_solver_base::push_back_from_systemc(sca_port_base* port,
		sca_interface* sig, unsigned long& rate, unsigned long& delay,
		::sc_core::sc_time& T,
         sc_dt::int64&              timestep_change_id)
{
	push_back_sync_port(port, SCA_FROM_SYSTEMC, sig, rate, delay, T, timestep_change_id);
}

///////////////////////////////////////////////////
// registers method which is called after the event ev
// will be used to trace a channel from SystemC-kernel
// a handle of the concrete type has to be provided
void sca_solver_base::register_sc_value_trace(const ::sc_core::sc_event& ev,
		sca_sync_value_handle_base& handle)
{
	long idl = (long)(sync_trace_handles.size());

	sync_trace_handles.resize(idl + 1);

	handle.set_index(0);
	handle.set_id(idl);

	sync_trace_handles[idl].id = idl; //identification
	sync_trace_handles[idl].value_handle = &handle; //handle for sc-reading
	sync_trace_handles[idl].activation_event = &ev; //event for activation
	sync_trace_handles[idl].resize(1); //set buffer size to 1


	sc_core::sc_spawn_options opt;
	opt.spawn_method();
	opt.set_sensitivity(&ev);

	sync_trace_handles[idl].method_process_handle=
			sc_core::sc_spawn(
								sc_bind(&sca_solver_base::sc_value_trace,this,idl),
								sc_core::sc_gen_unique_name("sca_implementation_method"),&opt);
}

//Method with is called by the trace process
void sca_solver_base::sc_value_trace(int idl)
{
	sca_sync_trace_handle& handle(sync_trace_handles[idl]); //get handle
	::sc_core::sc_time ctime = ::sc_core::sc_time_stamp(); //current sc-time

	long csize = handle.count; //events yet stored in buffer

	if (csize >= handle.size)
	{
		//for the case the port is not read, we check whether we can remove values
		//before we resize to prevent a memory leak
		const sca_core::sca_time& cstime=this->csync_data->cluster_start_time;


		while((handle.count>1) && (handle.time_points[handle.index_start]<cstime))
		{
			handle.index_start=(handle.index_start+1)%handle.size;
			handle.count--;
			csize=handle.count;
		}

		if (csize >= handle.size)
		{
			handle.resize(csize + 1); //if required resize buffer
		}
	}

	long pos = (handle.index_start + csize) % handle.size; //ringb. pos for next ev.
	handle.count++; //increase ev. counter

	handle.time_points[pos] = ctime; //store time of event
	handle.value_handle->set_index(pos); //store concrete value in vector
	handle.value_handle->call_method(); //at position pos

#ifdef SCA_IMPLEMENTATION_DEBUG

	std::cout << "?????????????? >>>>>>>>> ";
	std::cout << sc_time_stamp() << " stored event at time: "
	<< handle.time_points[pos] << " value: ";
	handle.value_handle->dump(cout);
	std::cout << " on index: " << handle.value_handle->index << " val_handle: " <<
	handle.value_handle << " size: " << handle.size << std::endl;

#endif

}

///////////////////////////////////////////////////////////////////////////////

//method process for writting values from sca to sc
void sca_solver_base::sc_write_value_process(int idl)
{
	sca_sync_write_handle& handle(sca_sync_write_handles[idl]); //get handle

	//trigger next activation
	next_trigger(*(handle.activation_event));

	//due process is called during elaboration
	if (handle.count <= 0)
		return;

	//use first value from ringb. (fifo)
	handle.value_handle->set_index(handle.index_start);
	handle.value_handle->call_method(); //schedule event/value at current time

#ifdef SCA_IMPLEMENTATION_DEBUG

	std::cout << "ooooooooooo <<<<<<<<< ";
	std::cout << sc_core::sc_time_stamp() << " read at time: "
	<< handle.time_points[handle.index_start] << " value: ";
	handle.value_handle->dump(std::cout);
	std::cout << " on index: " << handle.index_start
	<< " size: " << handle.size << std::endl; //handle.value_handle->index << std::endl;
#endif

	handle.count--; //remove value
	if (handle.count > 0) //if yet events in ringb. schedule process at next time
	{
		handle.index_start = (handle.index_start + 1) % handle.size; //next ev. in buffer
		//notify next activation event
		handle.activation_event->notify(handle.time_points[handle.index_start]
				- ::sc_core::sc_time_stamp());

#ifdef SCA_IMPLEMENTATION_DEBUG

		std::cout << "Next activation scheduled: "
		<< handle.time_points[handle.index_start] << std::endl;
#endif

	}
}

////////////////////////////////////////////////////////////////////////
// writes with write_method a value to a channel at time ctime or maybe
//     timeless - the SystemC-kernel will suspend (and thus resume the sca -
//     cluster) not later then next_time - during this activation the mehtod
//     must be called to remove the old and schedule a new next_time -
//     otherwise it should give an exception, due we deadlocked
//     for timeless (ordered) synchronization next_time can be set to a large
//     value or SC_INVALID_TIME - in this case the sc_channel suspends if a
//     value required (we have to do nothing therefore)
//
//          write sc - value from sca - domain
void sca_solver_base::write_sc_value(::sc_core::sc_time ctime,
		::sc_core::sc_time next_time, sca_sync_value_handle_base& val_handle)
{
	if (ctime < (::sc_core::sc_time_stamp()))
	{
		std::ostringstream str;
		str << "sca-de synchronization failed in: "
				<< this->get_synchronization_object_data()->cluster_id
				<< " " << __FILE__ << " line: " << __LINE__
				<< " current sca-time: " << ctime << " current sc-time: "
				<< ::sc_core::sc_time_stamp() << " sca-next-time:   "
				<< next_time << " insert a delay of at least: "
				<< sc_core::sc_time_stamp()-ctime ;

		sca_core::sca_prim_channel* ch =
				dynamic_cast<sca_prim_channel*> (val_handle.get_object());
		if (ch != NULL)
		{
			str << " near: " << std::endl << "\t";
			ch->print_connected_module_names(str);
			str << std::endl;
		}
		else
		{
			str << " in: " << val_handle.get_object()->name() << std::endl;
		}
		SC_REPORT_ERROR("SystemC-AMS",str.str().c_str());
	}

	//get handle
	sca_sync_write_handle& handle(sca_sync_write_handles[val_handle.get_id()]);

	//if no previous event schedule process for value scheduling
	if (handle.count <= 0)
	{
		if(ctime == ::sc_core::sc_time_stamp())
		{
			//activate in same delta to prevent additional delta delay
			handle.activation_event->notify();
		}
		else
		{
			handle.activation_event->notify(ctime - ::sc_core::sc_time_stamp());
		}
	}

	//if required enlarge ringbuffer
	if (handle.size <= handle.count)
		handle.resize(handle.count + 1);

	//calculate position for storing next event
	long windex=(handle.index_start + handle.count) % handle.size;
	val_handle.set_index(windex);
	handle.value_handle->store_tmp(); //store concrete value in handle
	handle.time_points[val_handle.get_index()] = ctime; //store time
	handle.count++; //increase event counter



#ifdef SCA_IMPLEMENTATION_DEBUG
	std::cout << "ooooooooooo >>>>>: " << cluster_id << " ";
	std::cout << sc_core::sc_time_stamp() << " stored at time: " << ctime << " value: ";
	val_handle.dump(std::cout);
	std::cout << " on index: " << windex << " id: " << val_handle.get_id()
	<< " &val_handle: " << &val_handle<< std::endl;

	std::cout << sc_core::sc_time_stamp() << " xxxxxxxx next time step is: " << next_time
	<< " limit time step to: " << (*cluster_resume_event_time) << std::endl;
#endif

}

////////////////////////////////////////////////////////////////////////
// if SystemC has not reached time the sca process of the current cluster
// suspended, after resume the corresponding value (stored by the trace process) is returned
//
//         get sc-value from sca - domain on time
void sca_solver_base::get_sc_value_on_time(::sc_core::sc_time ctime,
		sca_sync_value_handle_base& val_handle)
{

	//do not synchronize, read most recent value
	if (!sca_get_curr_simcontext()->time_domain_simulation())
	{
		sca_sync_trace_handle& handle(sync_trace_handles[val_handle.get_id()]);//get handle
		val_handle.set_index((handle.index_start + handle.count - 1)
				% handle.size);
		val_handle.call_method(); //reading value from handle
		return;
	}

	if (ctime > ::sc_core::sc_time_stamp())
	{
		//cout << endl << "Suspend :" << cluster_id <<" at: " << sc_time_stamp() << " wait on: " << ctime << endl;

		//this should be sure - however causes maybe some additional context switches
		::sc_core::wait(ctime - ::sc_core::sc_time_stamp());

		//wait(); //works sometimes not properly due time step limitation is not for
		//all cases correct (cluster_resume_event_time) - if we have slight non-causalities
		//it will be difficult to remove an obsolete limitation - the limitation will
		//also not work properly if not always a sample written to the sync-port
		//cout << "Resume: " << cluster_id << " at: " << sc_time_stamp() << endl << endl;

		if (ctime > (::sc_core::sc_time_stamp()))
		{
			std::ostringstream str;
			str << "Internal error de-sca synchronization failed in: "
					<< __FILE__ << " line: " << __LINE__
					<< " current sca time: " << ctime << " sc-time: "
					<< ::sc_core::sc_time_stamp() << std::endl;
			SC_REPORT_ERROR("SystemC-AMS",str.str().c_str());
		}
	}

	sca_sync_trace_handle& handle(sync_trace_handles[val_handle.get_id()]); //get handle

	//search the current value
	while (handle.count > 1) //remove all out of date events
	{
		long index = (handle.index_start + 1) % handle.size;
		if (handle.time_points[index] > ctime)
			break;
		handle.index_start = index;
		handle.count--;
	}

	handle.value_handle->set_index(handle.index_start);
	//handle.value_handle->call_method(); //reading value from handle
	handle.value_handle->read_tmp();

#ifdef SCA_IMPLEMENTATION_DEBUG

	std::cout << "?????????????? get sc-value ";
	std::cout << sc_time_stamp() << " read event for time: " << ctime << " at time: "
	<< handle.time_points[handle.index_start] << " value: ";
	handle.value_handle->dump(cout);
	std::cout << " on index: " << handle.value_handle->index << " val_handle: " <<
	handle.value_handle << std::endl;
#endif

}


////////////////////////////////////////////////////////////////////

void sca_solver_base::set_sync_data_references(sca_cluster_synchronization_data& crefs)
{
	csync_data=&crefs;

	call_counter = &(solver_object_data.call_counter);
	allow_processing_access_flag=&(solver_object_data.allow_processing_access_flag);
	calls_per_period = &(solver_object_data.calls_per_period);
	last_calls_per_period=&(solver_object_data.last_calls_per_period);

	//cout << "----- Sync data references setted ---------" << endl;
}


////////////////////////////////////////////////////////////////////

sca_synchronization_object_data* sca_solver_base::get_synchronization_object_data()
{
	  return &solver_object_data;
}

bool sca_solver_base::not_clustered()
{
	return (solver_object_data.cluster_id<0);
}


const std::string& sca_solver_base::get_name() const
{
	return solver_object_data.sync_obj_name;
}



std::string sca_solver_base::get_name_associated_names(int max_num) const
{
	return get_name();
}


////////////////////////////////////////////////////////////////////

// registers synchronization from a sca domain to a sc domain -
// limits the sc time to the first sca schedule time
void sca_solver_base::register_sca_schedule(::sc_core::sc_time next_time,
		sca_sync_value_handle_base& handle)
{
	handle.set_id((long)(sca_sync_write_handles.size()));
	sca_sync_write_handles.resize(handle.get_id() + 1);
	sca_sync_write_handles[handle.get_id()].value_handle = &handle;
	sca_sync_write_handles[handle.get_id()].id = handle.get_id();

	::sc_core::sc_event* ev = new ::sc_core::sc_event;
	sca_sync_write_handles[handle.get_id()].activation_event = ev;


	sc_core::sc_spawn_options opt;
	opt.spawn_method();

	sca_sync_write_handles[handle.get_id()].method_process_handle=
			sc_core::sc_spawn(
					sc_bind(&sca_solver_base::sc_write_value_process,this,handle.get_id()),
					sc_core::sc_gen_unique_name("sca_implementation_method"),&opt);

}

//////////////////////////////////////


sca_core::sca_time sca_solver_base::get_cluster_start_time()
{
	return csync_data->cluster_start_time;
}

//////////////////////////////////////

sca_core::sca_time sca_solver_base::get_current_time()
{
	if (call_counter == NULL)
		return sc_core::SC_ZERO_TIME;
	else if (*call_counter == -1)
	{
		if(csync_data!=NULL) return  csync_data->cluster_start_time;
		else return sc_core::SC_ZERO_TIME;
	}
	else
	{
		return csync_data->cluster_start_time + (((*call_counter) % (*calls_per_period))
				* (csync_data->cluster_period) / (*calls_per_period));
	}
}


//////////////////////////////////////

sca_core::sca_time sca_solver_base::get_last_period(int offset)
{
	if ((csync_data == NULL) || (calls_per_period == NULL) ||
			(last_calls_per_period == NULL))
		return NOT_VALID_SCA_TIME();

	sc_dt::int64 ccnt=*call_counter+offset;

	if(ccnt<2)
	{
		//if(ccnt<=0) return sca_core::sca_max_time();
		//if no last period available we use the propagated one
		return csync_data->last_cluster_period/(*last_calls_per_period);
	}

	sca_core::sca_time result;

	 //first cluster call
	if((ccnt%(*calls_per_period)==0))
	{
		//we must refer two periods back
		if((*last_calls_per_period)==1)
		{
			//last cluster start time equals to last module execution
			result=csync_data->last_cluster_start_time -
					//time of the last last module execution
				  (csync_data->last_last_cluster_start_time +
				  csync_data->last_last_cluster_period/solver_object_data.last_last_calls_per_period *
				  (solver_object_data.last_last_calls_per_period-1));

/*
std::cout << "last_cluster_start_time " << csync_data->last_cluster_start_time
		<< " last_last_cluster_period " << csync_data->last_last_cluster_period
		<< " last_last_cluster_start_time " << csync_data->last_last_cluster_start_time
		<< " last_last_calls_per_period " << solver_object_data.last_last_calls_per_period
		<< std::endl;
*/

		}
		else
		{
			//otherwise it's the last cluster period divided by the number of calls
			result=csync_data->last_cluster_period/(*last_calls_per_period);
		}
	} //second call during cluster execution
	else if((ccnt%(*calls_per_period)==1))
	{
		//IMPROVE: check for simplification
		result=csync_data->cluster_start_time -
				(csync_data->last_cluster_start_time +
				  (csync_data->last_cluster_period * ((*last_calls_per_period)-1))
				    /(*last_calls_per_period));
	}
	else
	{
		result=csync_data->cluster_period / (*calls_per_period);
	}

	//the last time step is the last non-zero step, or if not available
	//the propagated timestep
	if(result==sc_core::SC_ZERO_TIME)
	{
		return csync_data->last_cluster_period/(*last_calls_per_period);
	}

	return result;
}


//////////////////////////////////////

sca_core::sca_time sca_solver_base::get_current_period()
{
	if ((csync_data == NULL) || (calls_per_period == NULL))
		return NOT_VALID_SCA_TIME();

	 //first cluster call has maybe different timestep - if request_next_activation was called
	if((((*call_counter)%(*calls_per_period)==0)&&(*call_counter)>0) &&
			!sca_ac_analysis::sca_ac_is_running())
	{
		if((*last_calls_per_period)==1)
			return csync_data->cluster_start_time - csync_data->last_cluster_start_time;


		//IMPROVE: check for simplification
		return csync_data->cluster_start_time -
				(csync_data->last_cluster_start_time +
				    (csync_data->last_cluster_period * ((*last_calls_per_period)-1))
				    /(*last_calls_per_period));
	}

	//all other calls are simply the period divided by calls_per_period_timestep_ch_obj
	return csync_data->cluster_period / (*calls_per_period);
}

//////////////////////////////////////

sca_core::sca_time sca_solver_base::get_max_timestep()
{
//	std::cout<< "csync_data->cluster_max_time_step=" << csync_data->cluster_max_time_step<< "\n";
//	std::cout<< "*calls_per_period_timestep_ch_obj=" << (*calls_per_period_timestep_ch_obj) << "\n";
	if ((csync_data == NULL) || (calls_per_period == NULL))
		return NOT_VALID_SCA_TIME();

	if(csync_data->cluster_max_time_step==sca_core::sca_max_time())
											return sca_core::sca_max_time();

	//all other calls are simply the period divided by calls_per_period_timestep_ch_obj
	return csync_data->cluster_max_time_step / (*calls_per_period);

}

//////////////////////////////////////

sca_core::sca_time sca_solver_base::get_cluster_duration()
{
	if (csync_data == NULL)
		return NOT_VALID_SCA_TIME();

	return csync_data->cluster_end_time - csync_data->cluster_start_time;
}
//////////////////////////////////////

long sca_solver_base::set_max_start_time(const sca_core::sca_time& st)
{
	sca_core::sca_time ntime=st;

	if (ntime < (::sc_core::sc_time_stamp()))
	{
		std::ostringstream str;
		str << "Cluster cannot be scheduled to elapsed time in: " << __FILE__
				<< " line: " << __LINE__ << std::endl;
		SC_REPORT_ERROR("SystemC-AMS",str.str().c_str());
	}

	long shift = ((*call_counter) + (*calls_per_period) - 1)
			% (*calls_per_period);
	if (shift)
		ntime = sc_core::sc_time_stamp() + (shift * csync_data->cluster_period)
				/ (*calls_per_period);

	if (!csync_data->request_new_start_time || (csync_data->requested_next_cluster_start_time > ntime))
	{
		csync_data->request_new_start_time = true;
		csync_data->requested_next_cluster_start_time = ntime;
	}

	return shift;
}


//method for new 1.x dtdf feature - its execution at cluster end time is guaranteed
void sca_solver_base::request_next_activation_abs(const sca_core::sca_time& abstime)
{
	if (!csync_data->request_new_start_time || (csync_data->requested_next_cluster_start_time > abstime))
	{
		csync_data->request_new_start_time = true;
		csync_data->requested_next_cluster_start_time = abstime;
	}
}

//////////////////////////////////////

long sca_solver_base::set_max_period(const sca_core::sca_time& nperiod,sc_core::sc_object* obj)
{
	long shift = (*call_counter + (*calls_per_period) - 1)
			% (*calls_per_period);

	if (!csync_data->request_new_max_timestep || (csync_data->new_max_timestep > nperiod*(*calls_per_period)))
	{
		csync_data->max_timestep_obj=obj;
		csync_data->request_new_max_timestep = true;
		csync_data->new_max_timestep = nperiod*(*calls_per_period);
	}

	return shift;
}

//////////////////////////////////////


sc_core::sc_time sca_solver_base::register_activation_event(
		const ::sc_core::sc_event& ev)
{
	::sc_core::sc_time earliest_time;
	long shift = ((*call_counter) + (*calls_per_period) - 1)
			% (*calls_per_period);
	if (shift)
		earliest_time = ::sc_core::sc_time_stamp()
				+ (shift * csync_data->cluster_period) / (*calls_per_period);

	csync_data->reactivity_events.push_back(&ev);

	return earliest_time;
}

void sca_solver_base::request_next_activation(const sc_core::sc_event& ev)
{
	csync_data->reactivity_events.push_back(&ev);
}

void sca_solver_base::request_next_activation(const sc_core::sc_event_or_list& evlist)
{
	csync_data->reactivity_event_or_lists.push_back(evlist);
}


void sca_solver_base::request_next_activation(const sc_core::sc_event_and_list& evlist)
{
	csync_data->reactivity_event_and_lists.push_back(new event_and_list2ev(evlist));
}


void sca_solver_base::change_timestep(const sca_core::sca_time& ts, sc_core::sc_object* req_object)
{
	if(csync_data->change_timestep)
	{

		bool err=false;
		if((*calls_per_period)<csync_data->calls_per_period_timestep_ch_obj)
		{
			if( (ts - (csync_data->new_timestep/(*calls_per_period))) >
			                                    sc_core::sc_get_time_resolution())
			{
				err=true;
			}
			else
			{
				csync_data->new_timestep=ts*(*calls_per_period);
				csync_data->calls_per_period_timestep_ch_obj=(*calls_per_period);
				csync_data->new_timestep_obj=req_object;
			}
		}
		else
		{
			if( (ts/csync_data->calls_per_period_timestep_ch_obj -
					(csync_data->new_timestep/(*calls_per_period))) >
						                                    sc_core::sc_get_time_resolution())
			{
				err=true;
			}
		}

		if(err)
		{
			std::ostringstream str;
			str << " timestep change is not consistent: " << std::endl;
			str << "    object: " << req_object->name() << " sets the timestep to: ";
			str << ts << " the object is called: " << (*calls_per_period) << " per cluster execution";
			str << " this corresponds to a period: " << (*calls_per_period)*ts << std::endl;
			str << "    the object: " << csync_data->new_timestep_obj->name();
			str << " set the timestep to: " << csync_data->new_timestep/csync_data->calls_per_period_timestep_ch_obj;
			str << " this object is called: " << csync_data->calls_per_period_timestep_ch_obj;
			str << " per cluster execution, which corresponds to a period: ";
			str << csync_data->new_timestep;
			SC_REPORT_ERROR("SystemC-AMS",str.str().c_str());
		}

	}
	else
	{
		csync_data->change_timestep=true;
		csync_data->new_timestep=ts*(*calls_per_period);
		csync_data->calls_per_period_timestep_ch_obj=(*calls_per_period);
		csync_data->new_timestep_obj=req_object;
	}
}



//////////////////////////////////////

void sca_solver_base::add_cluster_trace(sca_util::sca_implementation::sca_trace_object_data& tr_obj)
{
	csync_data->traces.push_back(&tr_obj);
}

//////////////////////////////////////

bool sca_solver_base::add_solver_trace(sca_util::sca_implementation::sca_trace_object_data& tr_obj)
{
	solver_traces.push_back(&tr_obj);
	return true;
}

//////////////////////////////////////


bool sca_solver_base::is_dynamic_tdf()
{
	return csync_data->is_dynamic_tdf;
}

bool sca_solver_base::are_attribute_changes_allowed()
{
	return csync_data->attribute_changes_allowed;
}

bool sca_solver_base::are_attribute_changes_may_allowed()
{
	return csync_data->attribute_changes_may_allowed;
}

bool sca_solver_base::are_attributes_changed()
{
	return csync_data->attributes_changed;
}

void sca_solver_base::attribute_change_flags_changed()
{
	if(csync_data!=NULL) csync_data->attribute_change_flags_changed=true;
}

}
}

