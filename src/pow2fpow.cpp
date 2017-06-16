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
int Mapper::pow2fpow(int nXToMap, double* xToMap, int nMapped, double* mapped)
{
  // Check consistency
  if(nXToMap!=nMapped){
    error = NEWTON_ERROR;
    cout<<"Different amount of values to analyze - Mapper::pow2fpow"<<endl;
    return error;
  }
  
  // Just sum all powers and calculate each fraction
  int power = 0;
  for(int ip=0; ip<nXToMap; ip++){
    power+=xToMap[ip];
  }  
  
  for(int ifp=0; ifp<nXToMap; ifp++){
    mapped[ifp] = xToMap[ifp]/power;
  }  
     
  return error;
}
