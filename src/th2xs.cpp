/*****************************************************************************\

NEWTON                |
                      |
Multiphysics          | CLASS
coupling              | MAPPER
maste code            |
                      |

-------------------------------------------------------------------------------

Mapper manages the preprocessing and / or postprocessing of the variables 
received and / or sended to the clients.

Date: 15 June 2017

-------------------------------------------------------------------------------

Copyright (C) 2017 Federico A. Caccia

This file is part of Newton.

Newton is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

Newton is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with Foobar.  If not, see <http://www.gnu.org/licenses/>.

\*****************************************************************************/

#include "Mapper.h"

using namespace::std;

/*Mapper::th2xs
 
Map vector of thermal-hydraulic values into another of cross sections.

input: code, number of elements to map, vector to map, 
number of elements of image, image vector, & pointer to the current fermi 
struct
output: error

*/
int Mapper::th2xs(int nXToMap, double* xToMap, int nMapped, double* mapped, Client::fermiStruct* fermi)
{
  // Check consistency

  // Organizing data
  double* tRef = new double [fermi->nPhysicalEntities];
  double* tFuel = new double [fermi->nPhysicalEntities];
  double* nRef = new double [fermi->nPhysicalEntities];
  double* burnup = new double [fermi->nPhysicalEntities];
  double*** xs = new double** [fermi->nPhysicalEntities];
  double*** xsDown = new double** [fermi->nPhysicalEntities];
  double*** xsUp = new double** [fermi->nPhysicalEntities];
  for(int ipe=0; ipe<fermi->nPhysicalEntities; ipe++){
    xs[ipe] = new double*[fermi->nXS];
    xsDown[ipe] = new double*[fermi->nXS];
    xsUp[ipe] = new double*[fermi->nXS];
    for(int ixs=0; ixs<fermi->nXS; ixs++){
      xs[ipe][ixs] = new double[fermi->nGroups];
      xsDown[ipe][ixs] = new double[fermi->nGroups];
      xsUp[ipe][ixs] = new double[fermi->nGroups];
    }
  }
  
  /* Assuming that in xToMap values are:
   * tRef, tFuel, nRef, (phys ent 1)
   * tRef, tFuel, nRef, (phys ent 2),
   * ...
  (this is how I set in newton.config)*/
  
  int ivalue=0;
  for (int ipe=0; ipe<fermi->nPhysicalEntities; ipe++){
    tRef[ipe] = xToMap[ivalue];
    tFuel[ipe] = xToMap[ivalue+1];
    nRef[ipe] = xToMap[ivalue+2];
    ivalue+=3;
    burnup[ipe] = fermi->pe[ipe].burnup;
  }  

  // TEST
  //~ for(int ipe=0; ipe<nPhysicalEntities; ipe++){
    //~ for(int ixs=0; ixs<nXS; ixs++){
      //~ for(int ig=0; ig<nGroups; ig++){
        //~ xs[ipe][ixs][ig] = 0.1+(tRef[ipe]+tFuel[ipe]+nRef[ipe])*0.001;
      //~ }
    //~ }
  //~ }
  
  /*#            F D   XA  nXF  eXF CHI
  "fuel_z1"  0 1.5 0.2 0.5 0.4 1.0
  "fuel_z2"  0 1.5 0.198 0.5 0.4 1.0
  "fuel_z3"  0 1.5 0.197 0.5 0.4 1.0
  "fuel_z4"  0 1.5 0.197 0.5 0.4 1.0
  "fuel_z5"  0 1.5 0.196 0.5 0.4 1.0
  "fluid_z1" 0 1.5 0.005 0.0  0.0 1.0
  "fluid_z2" 0 1.5 0.005 0.0  0.0 1.0
  "fluid_z3" 0 1.5 0.005 0.0  0.0 1.0
  "fluid_z4" 0 1.5 0.004 0.0  0.0 1.0
  "fluid_z5" 0 1.5 0.003 0.0  0.0 1.0 */
  
  // Fuel
  // XS_D          XS_abs                                               XS_eXF           chi
  xs[0][0][0]=1.5; xs[0][1][0]=0.200-nRef[0]/50000-burnup[0]/864000000; xs[0][2][0]=0.5; xs[0][3][0]=0.4;
  xs[1][0][0]=1.5; xs[1][1][0]=0.198-nRef[1]/50000-burnup[0]/864000000; xs[1][2][0]=0.5; xs[1][3][0]=0.4;
  xs[2][0][0]=1.5; xs[2][1][0]=0.197-nRef[2]/50000-burnup[0]/864000000; xs[2][2][0]=0.5; xs[2][3][0]=0.4;
  xs[3][0][0]=1.5; xs[3][1][0]=0.197-nRef[3]/50000-burnup[0]/864000000; xs[3][2][0]=0.5; xs[3][3][0]=0.4;
  xs[4][0][0]=1.5; xs[4][1][0]=0.196-nRef[4]/50000-burnup[0]/864000000; xs[4][2][0]=0.5; xs[4][3][0]=0.4;

  // Refrigerant
  xs[5][0][0]=1.5; xs[5][1][0]=0.005;                                   xs[5][2][0]=0;  xs[5][3][0]=0;
  xs[6][0][0]=1.5; xs[6][1][0]=0.005;                                   xs[6][2][0]=0;  xs[6][3][0]=0;
  xs[7][0][0]=1.5; xs[7][1][0]=0.005;                                   xs[7][2][0]=0;  xs[7][3][0]=0;
  xs[8][0][0]=1.5; xs[8][1][0]=0.004;                                   xs[8][2][0]=0;  xs[8][3][0]=0;
  xs[9][0][0]=1.5; xs[9][1][0]=0.003;                                   xs[9][2][0]=0;  xs[9][3][0]=0;

  // Interpolating with crm data
  for(int ipe=0; ipe<fermi->nPhysicalEntities; ipe++){
    for(int ibp=0; ibp<mat[fermi->pe[ipe].material].nBurnupPoints-1; ibp++){
      if(burnup[ipe]>=mat[fermi->pe[ipe].material].burnup[ibp]){
        error = NEWTON_ERROR;
        checkError(error, "XS interpolation not implemented yet - Mapper::th2xs");
      }
    }
  }






  /* Map values in the order that are going to be send to the client.
   * We are setting deltas to the client here. */
   ivalue=0;
    for(int ipe=0; ipe<fermi->nPhysicalEntities; ipe++){
      for(int ixs=0; ixs<fermi->nXS; ixs++){
        for(int ig=0; ig<fermi->nGroups; ig++){
          mapped[ivalue] = xs[ipe][ixs][ig];
          ivalue++;
        }
      } 
    }
     
  return error;
}
