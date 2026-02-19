/*****************************************************************************

    Copyright 2010-2013
    Fraunhofer-Gesellschaft zur Foerderung der angewandten Forschung e.V.

    Copyright 2015-2022
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

  sca_ac_domain_solver.cpp - description

  Original Author: Karsten Einwich Fraunhofer IIS Dresden / COSEDA Technologies

  Created on: 02.01.2010

   SVN Version       :  $Revision: 2320 $
   SVN last checkin  :  $Date: 2023-06-08 12:47:38 +0000 (Thu, 08 Jun 2023) $
   SVN checkin by    :  $Author: pehrlich $
   SVN Id            :  $Id: sca_ac_domain_solver.cpp 2320 2023-06-08 12:47:38Z pehrlich $

 *****************************************************************************/

/*****************************************************************************/

#include "systemc-ams"
#include "scams/impl/analysis/ac/sca_ac_domain_solver.h"
#include "scams/impl/analysis/ac/sca_ac_domain_db.h"
#include "scams/impl/core/sca_simcontext.h"
#include "scams/impl/solver/util/sparse_library/sca_solve_ac_linear.h"

#include "scams/predefined_moc/eln/sca_eln_module.h"
#include "scams/predefined_moc/lsf/sca_lsf_module.h"
#include "scams/impl/core/sca_solver_base.h"
#include "scams/impl/solver/linear/sca_linear_solver.h"
#include "scams/impl/analysis/ac/sca_ac_domain_globals.h"


namespace sca_ac_analysis
{
namespace sca_implementation
{




int solve_linear_complex_eq_system::solve(sca_util::sca_matrix<sca_util::sca_complex >& Ac,
        sca_util::sca_vector<sca_util::sca_complex >& Bc,
        sca_util::sca_vector<sca_util::sca_complex >& Xc)
{
	critical_row=-1;
	critical_column=-1;

    unsigned long dimb=Bc.length();
    sca_util::sca_vector<double> b(2*dimb);

    for(unsigned long i=0; i<dimb; i++)
    {
        b(2*i)     =Bc(i).real();      // |  Real(B) |
        b(2*i+1)   =Bc(i).imag();      // |  Imag(B) |
    }


    sca_util::sca_vector<double> sigs(2*dimb);
    sca_solve_ac_linear_data* sdata = NULL;
    int errc=sca_solve_ac_linear_init((sparse*)Ac.get_sparse_matrix(),&sdata);

    if(errc!=0)
    {
    	if(errc==2)
    	{
    		long row=-1, column=-1;
    		sca_solve_ac_get_error_position(sdata,&row,&column);

    		//if error in imaginary part -> normalize
    		if(row>=long(dimb))    row-=dimb;
    		if(column>=long(dimb)) column-=dimb;

    		critical_row=row;
    		critical_column=column;
    	}


        sca_solve_ac_linear_free(&sdata);
    	return errc;
    }

    //solving the real equation system
    errc=sca_solve_ac_linear(b.get_flat(), sigs.get_flat(),&sdata);

    //an error at this stage means the result is may inaccurate
	if(errc==5)
	{
		long row=-1, column=-1;
		sca_solve_ac_get_error_position(sdata,&row,&column);

		//if error in imaginary part -> normalize
		if(row>=long(dimb))    row-=dimb;
		if(column>=long(dimb)) column-=dimb;

		critical_row=row;
		critical_column=column;
	}


    //converting result to complex
    for(unsigned long i=0; i<dimb; i++)
    {
        Xc(i)=sca_util::sca_complex(sigs(2*i),sigs(2*i+1));
    }

    sca_solve_ac_linear_free(&sdata);
    return errc;
}


/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////

//constructor
sca_ac_domain_solver::sca_ac_domain_solver(sca_ac_domain_db& ac_db,bool noise_domain) :
        ac_data(ac_db),
        equations(ac_db)
{

    if(!(sca_core::sca_implementation::sca_get_curr_simcontext()->initialized()))
    {
    	sca_util::sca_implementation::sca_disable_all_traces();

        this->init_systemc();

    	sca_util::sca_implementation::sca_restore_enable_all_traces();

    }

    if(sc_core::sc_get_status() & sc_core::SC_STOP)
    {
    	SC_REPORT_ERROR("SystemC AMS","AC simulation can't be executed after sc_core::sc_stop has been called");
    }

    ac_data.ac_domain    = true;
    ac_data.noise_domain = noise_domain;
    ac_data.initialized  = false;


   //disable time domain simulation -> prevent wait and thus time progress,
   //which may happens during equation setup due e.g. converter port access
   sca_core::sca_implementation::sca_get_curr_simcontext()->set_no_time_domain_simulation();


    this->ac_elaborate();

    equations.initialize_equation_system();

    ac_data.ac_domain    = false;

    ac_data.initialized=true;

    sca_core::sca_implementation::sca_get_curr_simcontext()->set_time_domain_simulation();

}

/////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////

void sca_ac_domain_solver::solve_complex_eq_system(
    sca_util::sca_matrix<sca_util::sca_complex >& Ac,
    sca_util::sca_vector<sca_util::sca_complex >& Bc,
    sca_util::sca_vector<sca_util::sca_complex >& Xc
)
{
	solve_linear_complex_eq_system solver;

	int err=solver.solve(Ac,Bc,Xc);

    if(err!=0)
    {
        std::ostringstream str;
        if(err==1)
        {
            str << " Creation of sparse matrix failed (can't allocate memory) for frequency: "
            << sca_ac_analysis::sca_ac_f() << " Hz"<< std::endl;
        }
        else if((err==2) || (err==5))
        {
        	//currently only for signals and nodes
        	sc_core::sc_object* row_obj=this->ac_data.find_object_for_id(solver.critical_row);
        	sc_core::sc_object* col_obj=this->ac_data.find_object_for_id(solver.critical_column);

        	if(err==2)
        	{
        		str << " Creation of SparseCode failed for frequency: ";
        	}
        	else
        	{
        		str << " Equation solver has accuracy problems for frequency: ";
        	}
            str << sca_ac_analysis::sca_ac_f() << " Hz"<< std::endl;

            if((row_obj!=NULL) || (col_obj!=NULL))
            {
            	str << "The error is maybe near: " << std::endl << "\t\t";
            	if(row_obj!=NULL)
            	{
            		str << row_obj->name();

            		if( (col_obj!=NULL)&&(row_obj!=col_obj) )
            		{
            			str << " and "  << std::endl << "\t\t";
            		}
            	}

            	if( (col_obj!=NULL)&&(row_obj!=col_obj) )
            	{
            		str << col_obj->name();
            	}
            	str << std::endl;
            }
        }
        else
        {
        	  std::ostringstream str;
        	  str << "Equation solver failed for frequency: "
        	      << sca_ac_analysis::sca_ac_f() << " Hz"<< std::endl;
        }
        SC_REPORT_WARNING("SystemC-AMS",str.str().c_str());
    }
}

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////

void sca_ac_domain_solver::ac_noise_solver(
    sca_util::sca_matrix<sca_util::sca_complex >& Ac,
    sca_util::sca_vector<sca_util::sca_complex >& Bc,
    std::vector<bool>&                 noise_src_flags,
    sca_util::sca_matrix<sca_util::sca_complex >& result
)
{
    //noise analysis - assumption all b!=0 are independent noise sources
    //for those sources there contribution has to be calculated seperately
    //and the contribution has to be added arithmeticly

    unsigned long dima=Ac.n_cols();
    unsigned long n_equations=dima;

    //reset overall noise vector
    for(unsigned long i=0;i<n_equations;i++)
        result(i,0)=0.0;

    sca_solve_ac_linear_data* sdata = NULL;
    int errc=sca_solve_ac_linear_init((sparse*)Ac.get_sparse_matrix(),&sdata);
    if(errc!=0)
    {
        std::ostringstream str;
        if(errc==1)
        {
            str << " Creation of sparse matrix failed (can't allocate memory) for frequency: "
            << sca_ac_analysis::sca_ac_f() << " Hz"<< std::endl;
        }
        else
        {
            str << " Creation of SparseCode failed for frequency: "
            << sca_ac_analysis::sca_ac_f() << " Hz"<< std::endl;

        	long row=-1, column=-1;
        	sca_solve_ac_get_error_position(sdata,&row,&column);

        	//if error in imaginary part -> normalize
        	if(row>=long(dima))    row-=dima;
        	if(column>=long(dima)) column-=dima;

        	//currently only for signals and nodes
        	sc_core::sc_object* row_obj=this->ac_data.find_object_for_id(row);
        	sc_core::sc_object* col_obj=this->ac_data.find_object_for_id(column);

            str << " Creation of SparseCode failed for frequency: "
            << sca_ac_analysis::sca_ac_f() << " Hz"<< std::endl;

            if((row_obj!=NULL) || (col_obj!=NULL))
            {
            	str << "The error is maybe near: " << std::endl << "\t\t";
            	if(row_obj!=NULL)
            	{
            		str << row_obj->name();

            		if( (col_obj!=NULL)&&(row_obj!=col_obj) )
            		{
            			str << " and "  << std::endl << "\t\t";
            		}
            	}

            	if( (col_obj!=NULL)&&(row_obj!=col_obj) )
            	{
            		str << col_obj->name();
            	}
            	str << std::endl;
            }
        }
        SC_REPORT_WARNING("SystemC-AMS",str.str().c_str());
    }
    else
    {
        sca_util::sca_vector<double> b;
        sca_util::sca_vector<double> xnoise;


        b.resize(2*n_equations);
        xnoise.resize(2*n_equations);

        long n_src=1; //start with row 1 (the first is used for the sum)
        for(unsigned long i=0; i<n_equations; ++i)
        {
        	//solve for each noise source
            if((noise_src_flags[i]) )
            {
                b.reset(); //sca_solvelinearc destroys the right-hand-vector !!!!
                b(2*i)      = Bc(i).real();
                b(2*i+1)    = Bc(i).imag();

                errc=sca_solve_ac_linear(b.get_flat(), xnoise.get_flat(),&sdata);

                if(errc!=0)
                {
                    std::ostringstream str;
                    str << "Equation solver failed for frequency: "
                    << sca_ac_analysis::sca_ac_f() << " Hz"<< std::endl;
                    SC_REPORT_WARNING("SystemC-AMS",str.str().c_str());
                }



                for(unsigned long k=0; k<n_equations; ++k)
                {
                    sca_util::sca_complex res(xnoise(2*k),xnoise(2*k+1));
                    result(k,0)+=norm(res); //the first row is used for the sum
                    result(k,n_src)=res;
                }
                n_src++;
            }
        }

        //square root of arithmetic sum
        for(unsigned long j=0; j<n_equations; ++j)
        {
            result(j,0)=sqrt(result(j,0).real());
        }
    }
}

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////

void sca_ac_domain_solver::calculate(std::vector<double> omegas)
{
    ac_data.ac_domain    = true;
    ac_data.noise_domain = false;

    sca_core::sca_implementation::sca_get_curr_simcontext()->set_no_time_domain_simulation();

    sca_util::sca_vector<sca_util::sca_complex >& result(ac_data.get_result_vector_ref());
    result.reset();

    if(result.length()<=0)
    {
    	SC_REPORT_WARNING("SystemC-AMS","No AC equations in System - skip AC analysis");
    	return;
    }

    trace_init();

    for(std::vector<double>::iterator wi=omegas.begin(); wi<omegas.end(); ++wi)
    {
        equations.setup_equations(*wi);

#ifdef SCA_IMPLEMENTATION_DEBUG

        std::cout << "Ag: " << equations.get_Ag().dimx() << std::endl
        << equations.get_Ag() << std::endl << std::endl;
        std::cout << "Bg: " << equations.get_Bg().dimx() << std::endl
        << equations.get_Bg() << std::endl << std::endl;
#endif

        solve_complex_eq_system(equations.get_Ag(),equations.get_Bg(),result);

#ifdef SCA_IMPLEMENTATION_DEBUG

        std::cout << "result: " << result.dimx() << std::endl << result << std::endl << std::endl;
#endif

        //////////////// tracing ////////////////////
        trace(*wi,result);
    }

    sca_core::sca_implementation::sca_get_curr_simcontext()->set_time_domain_simulation();

    ac_data.ac_domain = false;
}

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////

void sca_ac_domain_solver::calculate_noise(std::vector<double> omegas)
{
    ac_data.ac_domain    = true;
    ac_data.noise_domain = true;
    ac_data.equations=&equations; //set reference to be able to access
    //from global routines->required for noise

    sca_core::sca_implementation::sca_get_curr_simcontext()->set_no_time_domain_simulation();

    sca_util::sca_vector<sca_util::sca_complex >& result(ac_data.get_result_vector_ref());
    result.reset();

    if(omegas.size()>0)
        equations.setup_equations(omegas[0]); //get noise sources

    //detect noise_src, store names
    sca_util::sca_vector<std::string> src_names;
    std::vector<bool>   noise_src_flags(equations.get_noise_src_names().length());
    long n_src=0;
    for(unsigned int i=0;i<equations.get_noise_src_names().length();i++)
    {
        if(equations.get_noise_src_names()(i)!="")
        {
        	std::string nsrc=equations.get_noise_src_names()(i);

        	//replace . by / to distingush from path specifier
        	size_t pos=nsrc.find(".");
        	while(pos!=std::string::npos)
        	{
        		nsrc[pos]='/';
        		pos=nsrc.find(".",pos+1);
        	}

            src_names(n_src++)=nsrc;
            noise_src_flags[i]=true;
        }
        else
        {
            noise_src_flags[i]=false;
        }
    }

    trace_init_noise(src_names);


    sca_util::sca_matrix<sca_util::sca_complex > results;
    results.resize(result.length(),n_src+1);
    results.unset_auto_resizable();
    for(std::vector<double>::iterator wi=omegas.begin(); wi<omegas.end(); ++wi)
    {
        equations.get_Bgnoise().reset();
        equations.setup_equations(*wi);

        ac_noise_solver(equations.get_Ag(),equations.get_Bgnoise(),noise_src_flags,results);

        trace_noise(*wi,results);
    }

    sca_core::sca_implementation::sca_get_curr_simcontext()->set_time_domain_simulation();

    ac_data.ac_domain    = false;
    ac_data.noise_domain = false;
}

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////

void sca_ac_domain_solver::trace_init_noise(sca_util::sca_vector<std::string>& src_names)
{
    ac_data.init_arc_map_for_add_eqs();

    std::vector<sca_util::sca_implementation::sca_trace_file_base*>* tr_list=
    	sca_core::sca_implementation::sca_get_curr_simcontext()->get_trace_list();

    for(std::vector<sca_util::sca_implementation::sca_trace_file_base*>::iterator tr_it  = tr_list->
            begin();
            tr_it != tr_list->end();
            ++tr_it )
    {
        if((*tr_it)->trace_disabled())
            continue;

        if((*tr_it)->is_ac_enabled())
        {
        	(*tr_it)->write_ac_noise_domain_init(src_names);
        }
    }

}

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////

void sca_ac_domain_solver::trace_init()
{
    ac_data.init_arc_map_for_add_eqs();

    std::vector<sca_util::sca_implementation::sca_trace_file_base*>* tr_list=
    	sca_core::sca_implementation::sca_get_curr_simcontext()->get_trace_list();

    for(std::vector<sca_util::sca_implementation::sca_trace_file_base*>::iterator tr_it  = tr_list->
            begin();
            tr_it != tr_list->end();
            ++tr_it )
    {
        if((*tr_it)->trace_disabled())
            continue;

        if((*tr_it)->is_ac_enabled())
        {
        	(*tr_it)->write_ac_domain_init();
        }
    }

}


/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////

void sca_ac_domain_solver::trace_noise(double w, sca_util::sca_matrix<sca_util::sca_complex >& result)
{
    std::vector<sca_util::sca_implementation::sca_trace_file_base*>* tr_list=
    	sca_core::sca_implementation::sca_get_curr_simcontext()->get_trace_list();
    result.unset_auto_resizable();

    for(std::vector<sca_util::sca_implementation::sca_trace_file_base*>::iterator tr_it  = tr_list->
            begin();
            tr_it != tr_list->end();
            ++tr_it )
    {
        if((*tr_it)->trace_disabled()) continue;

        if(!(*tr_it)->is_ac_enabled()) continue;

        long n_src=result.n_cols();
        sca_util::sca_matrix<sca_util::sca_complex > tr_matrix((unsigned long)((*tr_it)->traces.size()),n_src);
        tr_matrix.unset_auto_resizable();

        sc_core::sc_interface* sca_if;

        long npoint=-1;
        for(std::vector<sca_util::sca_implementation::sca_trace_object_data>::iterator
                tr_objit =  (*tr_it)->
                            traces.begin();
                tr_objit != (*tr_it)->traces.end();
                ++tr_objit
           )
        {
        	sca_util::sca_traceable_object* tobj = (*tr_objit).trace_object;

            if(tobj)
            {
            	npoint++;

                sc_core::sc_port_base* port = dynamic_cast<sc_core::sc_port_base*>(tobj);
                if(port)
                    sca_if  = dynamic_cast<sc_core::sc_interface*>(port->get_interface());
                else
                    sca_if  = dynamic_cast<sc_core::sc_interface*>(tobj);

                if(sca_if)
                {
                    long id = ac_data.get_arc_id(sca_if);
                    if(id<0)
                        tr_matrix(npoint,0)=0.0;
                    else
                        for(long i=0;i<n_src;i++)
                            tr_matrix(npoint,i)=result(id,i);
                }
                else //tracing for modules (may current tracing for networks)
                {
                    sca_core::sca_module* mod;
                    mod = dynamic_cast<sca_core::sca_module*>(tobj);
                    if(mod==NULL)
                    {
                        tr_matrix(npoint,0)=0.0;
                        continue;
                    }

                    sca_core::sca_implementation::sca_solver_base* solv=mod->get_sync_domain();

                    if(solv==NULL)
                    {
                        tr_matrix(npoint,0)=0.0;
                        continue;
                    }

                    long adr_start=ac_data.get_start_of_add_equations(solv);
                    if(adr_start>=0)
                    {
                    	for(long i=0;i<n_src;i++)
                    	{
                        	sca_util::sca_complex* res=&(result(0,i)) + adr_start;
                        	tr_matrix(npoint,i)=tobj->calculate_ac_result(res);
                    	}
                    }
                    else
                    {
                        tr_matrix(npoint,0)=0.0;
                    }

                }
            }
        }
        (*tr_it)->write_ac_noise_domain_stamp(w,tr_matrix);
    }
}

/////////////////////////////////


/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////

void sca_ac_domain_solver::trace(double w, sca_util::sca_vector<sca_util::sca_complex >& result)
{
    std::vector<sca_util::sca_implementation::sca_trace_file_base*>* tr_list=
    	sca_core::sca_implementation::sca_get_curr_simcontext()->get_trace_list();

    for(std::vector<sca_util::sca_implementation::sca_trace_file_base*>::iterator tr_it  =
    	tr_list->begin();
            tr_it != tr_list->end();
            ++tr_it )
    {
        if((*tr_it)->trace_disabled()) continue;

        if(!(*tr_it)->is_ac_enabled()) continue;


        std::vector<sca_util::sca_complex > tr_vec;
        const sc_core::sc_interface* sca_if(NULL);

        for(std::vector<sca_util::sca_implementation::sca_trace_object_data>::iterator
                tr_objit =  (*tr_it)->
                            traces.begin();
                tr_objit != (*tr_it)->traces.end();
                ++tr_objit
           )
        {
            sca_util::sca_traceable_object* tobj = (*tr_objit).trace_object;

            if(tobj)
            {
                sc_core::sc_port_base* port = dynamic_cast<sc_core::sc_port_base*>(tobj);
                if(port)
                    sca_if  = dynamic_cast<sc_core::sc_interface*>(port->get_interface());
                else
                    sca_if  = dynamic_cast<sc_core::sc_interface*>(tobj);

                if(sca_if==NULL)
                {
                	sca_util::sca_implementation::sca_sc_trace_base* sct;
                	sct=dynamic_cast<sca_util::sca_implementation::sca_sc_trace_base*>(tobj);
                	if(sct)
                	{
                		sca_if=sct->get_sc_interface();
                	}
                }

                if(sca_if)
                {
                    long id = ac_data.get_arc_id(sca_if);
                    if(id<0)
                        tr_vec.push_back(0.0);
                    else
                        tr_vec.push_back(result(id));
                }
                else //tracing for modules (may current tracing for networks)
                {
                    sca_core::sca_module* mod;
                    mod = dynamic_cast<sca_core::sca_module*>(tobj);
                    if(mod==NULL)
                    {
                        tr_vec.push_back(0.0);
                        continue;
                    }

                    sca_core::sca_implementation::sca_solver_base* solv=mod->get_sync_domain();

                    if(solv==NULL)
                    {
                        tr_vec.push_back(0.0);
                        continue;
                    }

                    long adr_start=ac_data.get_start_of_add_equations(solv);
                    if(adr_start>=0)
                    {
                        sca_util::sca_complex* res=result.get_flat() + adr_start;
                        tr_vec.push_back(tobj->calculate_ac_result(res));
                    }
                    else
                    {
                        tr_vec.push_back(0.0);
                    }

                }
            }
        }
        (*tr_it)->write_ac_domain_stamp(w,tr_vec);
    }
}
/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////

void sca_ac_domain_solver::init_systemc()
{
    sc_core::sc_start(sc_core::SC_ZERO_TIME); //Unfortunately all initial deltas will be performed - also
}




//disables all eln/lsf solver which have modules below in hierarchy of obj
void sca_ac_domain_solver::hierarchic_disable_eln_lsf_solver(sc_core::sc_object* obj)
{
	std::vector<sc_core::sc_object*> chobjs;
	chobjs=obj->get_child_objects();

	//go recursivily through hierarchy
	for(std::size_t i=0;i<chobjs.size();++i)
	{
		sca_eln::sca_module* elnm=dynamic_cast<sca_eln::sca_module*>(chobjs[i]);
		if(elnm!=NULL)
		{
			elnm->get_sync_domain()->ac_disable();
			continue;
		}

		sca_lsf::sca_module* lsfm=dynamic_cast<sca_lsf::sca_module*>(chobjs[i]);
		if(elnm!=NULL)
		{
			lsfm->get_sync_domain()->ac_disable();
			continue;
		}

		this->hierarchic_disable_eln_lsf_solver(chobjs[i]);
	}
}



void sca_ac_domain_solver::register_ac_entities(sc_core::sc_object *obj)
{
    if (obj == NULL)
    {
        SC_REPORT_ERROR("sca_ac_domain_solver", "Internal not possible error");
        return;
    }

    sca_ac_analysis::sca_ac_object *ac_obj;
    ac_obj = dynamic_cast<sca_ac_analysis::sca_ac_object*>(obj);

    if ((ac_obj != NULL) && (ac_obj->is_ac_enabled()))
    {
        sca_core::sca_implementation::sca_solver_base *solv;
        solv =
                dynamic_cast<sca_core::sca_implementation::sca_solver_base*>(ac_obj);

        if (solv != NULL) //linear solver registration -> eventually generalize to solver base
        {
            //register only for registration due it will be maybe disabled
            to_reg_ac_solver.push_back(solv);

            //a solver object has no hierarchy so we can stop here
            return;
        }

        sca_ac_analysis::sca_ac_module *ac_mod;
        ac_mod = dynamic_cast<sca_ac_analysis::sca_ac_module*>(ac_obj);
        if (ac_mod != NULL)
        {

            sca_ac_analysis::sca_implementation::sca_ac_domain_register_entity(
                    dynamic_cast<sc_core::sc_module*>(obj),
                    static_cast<sca_ac_analysis::sca_implementation::sca_ac_domain_method>(&sca_ac_module::ac_processing));

            //register arcs for modules
            //an interface is registered as arc if it connected to an outport of an AC entity
            //we detect an outport via the kind string - if the kind string of the port contains out and not in
            //we assume it is an outport

            std::vector<sc_core::sc_object*> chobjs;
            chobjs = obj->get_child_objects();

            for (std::size_t i = 0; i < chobjs.size(); ++i)  //we search for ports inside the child objects
            {
                sc_core::sc_port_base *pb = dynamic_cast<sc_core::sc_port_base*>(chobjs[i]);

                if ((pb != NULL)  &&(sca_ac_port_direction(pb) == 1)) // we found an outport
                {
                    sc_core::sc_interface *scif = pb->get_interface();
                    if (scif != NULL) //if an interface connected we register the interface as ac_arc
                    {
                             sca_ac_register_arc(scif);
                    }
                }
            }
        }

        //ac-module found go no deeper in hierarchy

        //however disable all eln and lsf solver
        this->hierarchic_disable_eln_lsf_solver(obj);

        return;
    }

    std::vector<sc_core::sc_object*> chobjs;
    chobjs = obj->get_child_objects();

    //go recursively through hierarchy
    for (std::size_t i = 0; i < chobjs.size(); ++i)
    {
        this->register_ac_entities(chobjs[i]);
    }

}



void sca_ac_domain_solver::ac_elaborate()
{
    if (get_ac_database().is_elaborated())
        return;

    std::vector<sc_core::sc_object*> top_objects;

    top_objects = sc_core::sc_get_top_level_objects();

    //determine all used entities
    for (std::size_t i = 0; i < top_objects.size(); ++i)
    {
        this->register_ac_entities(top_objects[i]);
    }

    //register active solver
    for (std::size_t i = 0; i < to_reg_ac_solver.size(); ++i)
    {
        sca_core::sca_implementation::sca_solver_base *solv =
                to_reg_ac_solver[i];

        if (solv->is_ac_enabled())
        {
            //can be generalized for sca_solver_base
            sca_core::sca_implementation::sca_linear_solver *lsolv;
            lsolv = dynamic_cast<sca_core::sca_implementation::sca_linear_solver*>(solv);

            if (lsolv != NULL)
            {
                sca_ac_analysis::sca_implementation::sca_ac_domain_register_entity(
                        lsolv,
                        NULL,
                        static_cast<sca_ac_analysis::sca_implementation::sca_add_ac_domain_eq_method>(&sca_core::sca_implementation::sca_linear_solver::ac_domain_eq_method),
                        static_cast<sca_ac_analysis::sca_implementation::sca_calc_add_eq_cons_method>(&sca_core::sca_implementation::sca_linear_solver::ac_add_eq_cons_method));


                //register arcs for solver interface
                //we analyze all outports - an out port is always an arc
                 for(long i = 0,
                         nout = solv->get_number_of_sync_ports(sca_core::sca_implementation::SCA_TO_ANALOG);
                         i < nout;
                         ++i)
                 {
                     sca_core::sca_implementation::sca_synchronization_port_data* sync_port =
                         solv->get_sync_port(sca_core::sca_implementation::SCA_TO_ANALOG,i);
                     auto sca_if = dynamic_cast<sc_core::sc_interface*>(sync_port->channel);

                     //ignore non ac arcs
                     if(sca_if!=NULL)
                     {
                         sca_ac_analysis::sca_implementation::sca_ac_register_arc(sca_if);
                         //continue;
                     }
                 }

                 for(long i = 0,
                         nout = solv->get_number_of_sync_ports(sca_core::sca_implementation::SCA_TO_SYSTEMC);
                         i < nout;
                         ++i)
                 {
                     sca_core::sca_implementation::sca_synchronization_port_data* sync_port =
                         solv->get_sync_port(sca_core::sca_implementation::SCA_TO_SYSTEMC,i);
                     auto sca_if = sync_port->port->sc_get_interface();

                     //ignore non ac arcs
                     if(sca_if!=NULL)
                     {
                         sca_ac_analysis::sca_implementation::sca_ac_register_arc(sca_if);
                         //continue;
                     }
                 }
            } //if (lsolv != NULL)
        } //if (solv->is_ac_enabled())
    } //for (std::size_t i = 0; i < to_reg_ac_solver.size(); ++i) register active solver

    to_reg_ac_solver.clear();

    sca_ac_analysis::sca_implementation::get_ac_database().set_elaborated();
}

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////


} // namespace sca_implementation
} // namespace sca_ac_analysis

