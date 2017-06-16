/*****************************************************************************\

NEWTON					      |
                      |
Implicit coupling 		|	CLASS
in nonlinear			    |	MAPPER
calculations			    |
                      |

-------------------------------------------------------------------------------

Mapper manages the preprocessing and / or postprocessing of the variables 
received and / or sended to the clients.

Author: Federico A. Caccia
Date: 15 June 2017

\*****************************************************************************/

#include "Mapper.h"

using namespace::std;

/*Mapper::th2xs
 
Map vector of thermal-hydraulic values into another of cross sections.

input: code, number of elements to map, vector to map, 
number of elements of image, image vector, & number of:
zones, physical entities, XS, energy groups
output: error

*/
int Mapper::th2xs(int nXToMap, double* xToMap, int nMapped, double* mapped, int nPhysicalEntities, int nXS, int nGroups)
{
  // Check consistency

  // Organizing data
  static double* tRef = new double [nPhysicalEntities];
  static double* tFuel = new double [nPhysicalEntities];
  static double* nRef = new double [nPhysicalEntities];
  static double*** xs = new double** [nPhysicalEntities];
  for(int ipe=0; ipe<nPhysicalEntities; ipe++){
    xs[ipe] = new double*[nXS];
    for(int ixs=0; ixs<nXS; ixs++){
      xs[ipe][ixs] = new double[nGroups];
    }
  }
  
  /* Assuming that in xToMap values are:
   * tRef, tFuel, nRef, (phys ent 1)
   * tRef, tFuel, nRef, (phys ent 2),
   * ...*/
  
  int ivalue=0;
  for (int ipe=0; ipe<nPhysicalEntities; ipe++){
    tRef[ipe] = xToMap[ivalue];
    tFuel[ipe] = xToMap[ivalue+1];
    nRef[ipe] = xToMap[ivalue+2];
    ivalue+=3;
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
  // XS_D          XS_abs                           XS_eXF           chi
  xs[0][0][0]=1.5; xs[0][1][0]=0.200-nRef[0]/50000; xs[0][2][0]=0.5; xs[0][3][0]=0.4;
  xs[1][0][0]=1.5; xs[1][1][0]=0.198-nRef[1]/50000; xs[1][2][0]=0.5; xs[1][3][0]=0.4;
  xs[2][0][0]=1.5; xs[2][1][0]=0.197-nRef[2]/50000; xs[2][2][0]=0.5; xs[2][3][0]=0.4;
  xs[3][0][0]=1.5; xs[3][1][0]=0.197-nRef[3]/50000; xs[3][2][0]=0.5; xs[3][3][0]=0.4;
  xs[4][0][0]=1.5; xs[4][1][0]=0.196-nRef[4]/50000; xs[4][2][0]=0.5; xs[4][3][0]=0.4;

  // Refrigerant
  xs[5][0][0]=1.5; xs[5][1][0]=0.005;               xs[5][2][0]=0; xs[5][3][0]=0;
  xs[6][0][0]=1.5; xs[6][1][0]=0.005;               xs[6][2][0]=0; xs[6][3][0]=0;
  xs[7][0][0]=1.5; xs[7][1][0]=0.005;               xs[7][2][0]=0; xs[7][3][0]=0;
  xs[8][0][0]=1.5; xs[8][1][0]=0.004;               xs[8][2][0]=0; xs[8][3][0]=0;
  xs[9][0][0]=1.5; xs[9][1][0]=0.003;               xs[9][2][0]=0; xs[9][3][0]=0;

  /* Map values in the order that are going to be send to the client.
   * We are setting deltas to the client here. */
   ivalue=0;
    for(int ipe=0; ipe<nPhysicalEntities; ipe++){
      for(int ixs=0; ixs<nXS; ixs++){
        for(int ig=0; ig<nGroups; ig++){
          mapped[ivalue] = xs[ipe][ixs][ig];
          ivalue++;
        }
      } 
    }
     
  return error;
}
