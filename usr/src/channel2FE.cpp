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
along with Newton.  If not, see <http://www.gnu.org/licenses/>.

\*****************************************************************************/

#include "Mapper.h"

using namespace::std;

/*Mapper::channel2FE
 
User map.

input: code, number of elements to map, vector to map, 
number of elements of image, image vector, & pointer to the current fermi 
struct
output: error

*/
int Mapper::channel2FE(int nxToMap, double* xToMap, int nMapped, double* mapped)
{
  // Check consistency
  
  int variablesIWantToReceive = 28*3;
  int variablesIWantToSend = 28*3*61;
  
  
  if(nMapped!=variablesIWantToSend || nxToMap!=variablesIWantToReceive){
    cout<<"Bad mapper formulation in test - Mapper::channel2FE"<<endl;
    error = NEWTON_ERROR;
  }

  // Just map
  int j=0;
  for(int ith=0; ith<3; ith++){
    for(int iz=0; iz<28; iz++){    
      for(int ich=0; ich<61; ich++){
        mapped[j] = xToMap[iz*3+ith];
        j++;
      }
    }
  }  
  
  debug.log("Variables received:\n", 0, 0);
  for(int ith=0; ith<3; ith++){
    if(ith==0) debug.log("T ref:\n"); else if(ith==1) debug.log("T fuel:\n"); else if(ith==2) debug.log("N ref:\n");
    for(int iz=0; iz<28; iz++){
      debug.log(dou2str(xToMap[iz*3+ith]));
    }
    debug.log("\n");
  } 
  debug.log("\n");
    
  debug.log("Input to puma:\n", 0, 0);
  for(int i=0; i<nMapped; i++){
    debug.log(dou2str(mapped[i])+"\n");
  }
  debug.log("\n");
     
  return error;
}
