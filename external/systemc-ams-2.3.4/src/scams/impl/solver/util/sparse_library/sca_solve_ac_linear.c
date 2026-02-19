/*****************************************************************************

    Copyright 2010
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

  sca_solve_ac_linear.cpp - description

  Original Author: Karsten Einwich Fraunhofer / COSEDA Technologies

  Created on: 02.01.2010

   SVN Version       :  $Revision: 2021 $
   SVN last checkin  :  $Date: 2017-01-16 16:53:19 +0000 (Mon, 16 Jan 2017) $
   SVN checkin by    :  $Author: karsten $
   SVN Id            :  $Id: sca_solve_ac_linear.c 2021 2017-01-16 16:53:19Z karsten $

 *****************************************************************************/

/*****************************************************************************/


#include "sca_solve_ac_linear.h"

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#include "scams/impl/solver/util/sparse_library/ma_typedef.h"
#include "scams/impl/solver/util/sparse_library/ma_sparse.h"



struct sca_solve_ac_linear_data
{
    struct sparse* Spa;
    struct spcode  Cod;

    long critical_row;
    long critical_column;

};

void sca_solve_ac_get_error_position(struct sca_solve_ac_linear_data* data,long* row,long* column)
{
	(*row)=data->critical_row;
	(*column)=data->critical_column;
}

int sca_solve_ac_linear_init(  struct sparse* A,
                               struct sca_solve_ac_linear_data** data
                           )
{
	   struct sca_solve_ac_linear_data* sdata=NULL;

    if((*data)==NULL)
    {
        (*data)=malloc(sizeof(struct sca_solve_ac_linear_data));
        assert((*data)!=NULL);
        MA_InitCode(&((*data)->Cod));
    }
    else
    {
        MA_FreeCode(&((*data)->Cod));

        (*data)->critical_column=-1;
        (*data)->critical_row=-1;
    }

    (*data)->Spa=A;

    sdata=(*data);


    if (MA_LequSparseCodegen((struct sparse*)A, &(sdata->Cod)) != 0)
    {
        /*printf("solveLSG reports: Error in creating SparseCode!\n");*/

    	(*data)->critical_column=sdata->Cod.critical_column;
    	(*data)->critical_row=sdata->Cod.critical_line;


        return 2;
    }

    return 0;

}

int sca_solve_ac_linear(
		               double* B,
                       double* x,
                       struct sca_solve_ac_linear_data** data
                    )
{
	int errc=MA_LequSparseSolut((struct sparse*)((*data)->Spa), &((*data)->Cod), B, x);
	if(errc!=0)
	{
		(*data)->critical_row=(*data)->Cod.critical_line;
	}

    return errc;
}


void sca_solve_ac_linear_free(struct sca_solve_ac_linear_data** data)
{
    if((*data)!=NULL)
    {
        MA_FreeCode(&((*data)->Cod));
    }
    free (*data);
    (*data)=NULL;
}

