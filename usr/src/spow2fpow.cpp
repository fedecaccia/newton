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

Date: 26 June 2017

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

/*Mapper::spow2fpow
 
Map vector of scaled power distribution into fractions of power distribution.

input: code, number of elements to map, vector to map, 
number of elements of image, image vector, & number of:
zones, physical entities, XS, energy groups
output: error

*/
int Mapper::spow2fpow(int nXToMap, double* xToMap, int nMapped, double* mapped)
{
  // Check consistency
  
  int variablesIWantToReceive = client->relap->nAxialZones;
  int variablesIWantToSend = client->relap->nAxialZones;
  
  if(nMapped!=variablesIWantToSend || nXToMap!=variablesIWantToReceive){
    error = NEWTON_ERROR;
    cout<<"Bad mapper formulation in test. xToMap: "<<nXToMap<<"; nMapped: "<<nMapped<<" - Mapper::spow2fpow"<<endl;
    return error;
  }
  
  // Just add all powers and calculate each fraction
  double power = 0;
  for(int ip=0; ip<nXToMap; ip++){
    power+=xToMap[ip];
  }
  
  for(int ifp=0; ifp<nXToMap; ifp++){
    mapped[ifp] = xToMap[ifp]/power;
  }
  
  debug.log("Power: " + dou2str(power) +"\n",0,0);
  debug.log("Power distribution scaled to fraction:\n",0,0);
  for(int ifp=0; ifp<nXToMap; ifp++){
    debug.log(dou2str(mapped[ifp]));
  }
  debug.log("\n");

  return error;
}
