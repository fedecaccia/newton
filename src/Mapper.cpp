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
Date: 4 June 2017

\*****************************************************************************/

#include "Mapper.h"

using namespace::std;

/* Mapper constructor
*/
Mapper::Mapper(Client* clientPtr)
{
  // Client Object
  client = clientPtr;
  // Math object
  math = new MathLib();
  // Initial error value
	error = NEWTON_SUCCESS;
}

/* Mapper::config
Set configuration parameters.

input: -
output: -

*/
void Mapper::config()
{
  rootPrints("Checking for mappers...");
  
  // Load data from external files?
  
  
  
  
  
  
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
      cout<<"Number of values to map are differtent to number of values mapped in code: "<<iCode<<" - Mapper::map"<<endl;
    }
    else{
      for(int iX=0; iX<nxToMap; iX++){
        mapped[iX] = xToMap[iX];
      }        
    }
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
      cout<<"Code "<<codeName<<" has not fermi structures data to map with "<<map<<" - Mapper::map"<<endl;
      return error;
    }
    
    error = th2xs(nxToMap, xToMap, nMapped, mapped, &(client->fermi[iFermi]));
  }
  
  else if(map == "pow2fpow"){
    error = pow2fpow(nxToMap, xToMap, nMapped, mapped);
  }
  
  else if(map == "v2q"){

  }
  
  else{
    error = NEWTON_ERROR;
    cout<<"Mapper: "<<map<<" is not founded for code: "<<iCode<<" - Mapper::map"<<endl;        
  }
  
  return error;
}
