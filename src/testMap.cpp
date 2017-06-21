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

Author: Federico A. Caccia
Date: 15 June 2017

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
