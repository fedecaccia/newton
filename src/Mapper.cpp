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

Date: 4 June 2017

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
along with Newton.  If not, see <http://www.gnu.org/licenses/>.

\*****************************************************************************/

#include "Mapper.h"

using namespace::std;

/* Mapper constructor
*/
Mapper::Mapper(Client* clientPtr)
{
  // Initialization
  nMat = 0;
  // Client Object
  client = clientPtr;
  // Math object
  math = new MathLib();
  
  // Initial error value
	error = NEWTON_SUCCESS;
}

/* Mapper::allocate1
Allocates in first stage.

input: -
output: -

*/
void Mapper::allocate1()
{
  mat = new material[nMat];
  energyPerFission = new double[nGroups];

  // Initialization
  for (int iMat=0; iMat<nMat; iMat++){
    mat[iMat].nDependencies = 0;
  }
}

/* Mapper::config
Set configuration parameters.

input: -
output: -

*/
void Mapper::config(string map)
{
  // Load data from external files?
  rootPrints("Setting \""+map+"\" map...");
  
  if(map=="th2xs"){
    loadXSfromCRM();
  }
  else if(map=="" || map=="none"){
    
  } 
  
	checkError(error,"Error configurating mapper.");
}

/* Mapper::map

Map one vector into another.

input: code, string of the mapper, number of elements to map, vector to map, 
number of elements of image, image vector.

* 
output: error

*/
int Mapper::map(int iCode, string codeName, string map, int nxToMap, double* xToMap, int nMapped, double* mapped)
{ 
  if(map == "" || map=="none"){
    if(nxToMap!=nMapped){
      error = NEWTON_ERROR;
      checkError(error, "Number of values to map are differtent to number of values mapped in code: "+int2str(iCode)+" - Mapper::map");
    }
    else{
      for(int iX=0; iX<nxToMap; iX++){
        mapped[iX] = xToMap[iX];
      }        
    }
  }

  else if(map == "map_test1"){
   error = testMap(nxToMap, xToMap, nMapped, mapped);
  }

  else if(map == "th2xs"){
    // Look which is the number of Fermi clients of Client
    int iFermi=-1;
    for(int iF=0; iF<client->nFermi; iF++){
      if(client->fermi[iF].name==codeName){
        iFermi=iF;
        break;
      }
    }
    if(iFermi==-1){
      error=NEWTON_ERROR;
      checkError(error, "Code "+codeName+" has not fermi structures data to map with "+map+" - Mapper::map");
      return error;
    }
    
    error = th2xs(nxToMap, xToMap, nMapped, mapped, &(client->fermi[iFermi]));
  }
  
  else if(map == "pow2spow"){
    error = pow2spow(nxToMap, xToMap, nMapped, mapped);
  }

  else if(map == "spow2fpow"){
    error = spow2fpow(nxToMap, xToMap, nMapped, mapped);
  }
  
  else if(map == "spowk2fpowk"){
    error = spowk2fpowk(nxToMap, xToMap, nMapped, mapped);
  }
  
  else if(map == "v2q"){

  }
  
  else{
    error = NEWTON_ERROR;
    checkError(error, "Mapper: "+map+" is not founded for code: "+int2str(iCode)+" - Mapper::map");
  }
  
  return error;
}
