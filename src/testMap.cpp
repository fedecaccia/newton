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

/*Mapper::testMap
 
Testing map.

input: code, number of elements to map, vector to map, 
number of elements of image, image vector, & pointer to the current fermi 
struct
output: error

*/
int Mapper::testMap(int nxToMap, double* xToMap, int nMapped, double* mapped)
{
  // Check consistency
  
  // Make alpha (mapped variable) = x + w (received variables)
  if(nMapped!=1 || nxToMap!=2){
    cout<<"Bad mapper formulation in test - Mapper::map"<<endl;
    error = NEWTON_ERROR;
  }

  // Just map
  mapped[0] = xToMap[0] + xToMap[1];
     
  return error;
}
