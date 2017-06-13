/*****************************************************************************\

NEWTON					|
						|
Implicit coupling 		|	CLASS
in nonlinear			|	MAPPER
calculations			|
						|

-------------------------------------------------------------------------------

Mapper manages the preprocessing and / or postprocessing of the variables received and / or sended to the clients.

Author: Federico A. Caccia
Date: 4 June 2017

\*****************************************************************************/

#include "Mapper.h"

using namespace::std;

/* Mapper constructor
*/
Mapper::Mapper()
{
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
  rootPrints("Configurating mapper...");  
	checkError(error,"Error configurating mapper.");
}

/*
Map one vector into another.

input: code, string of the mapper, number of elements of image, image vector, 
number of elements to map, vector to map, 
* 
output: error

*/
int Mapper::map(int iCode, string map, int nXToMap, double* XToMap, int nMapped, double* mapped)
{
  if(map == ""){
    if(nXToMap!=nMapped){
      error = NEWTON_ERROR;
      cout<<"Number of values to map are differtent to number of values mapped in code: "<<iCode<<" - Mapper::map"<<endl;
    }
    else{
      for(int iX=0; iX<nXToMap; iX++){
        mapped[iX] = XToMap[iX];
      }        
    }
  }

  else if(map == "th2xs"){

  }
  
  else if(map == "v2q"){

  }
  
  else{
    error = NEWTON_ERROR;
    cout<<"Mapper: "<<map<<" is not founded for code: "<<iCode<<" - Mapper::map"<<endl;        
  }
  
  return error;
}
