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

/* Evolution constructor
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
  rootPrints("Configuration mapper...");  
	checkError(error,"Error configuration mapper.");
}

/*
Map one vector into another.

input: code ID, vector with data to map, vector in wich mapped values are saved
output: error

*/
int Mapper::map(System* sys, int iCode, int stage)
{
  switch (sys->code[iCode].map){
    case NEWTON_NO_MAP:
      // Just copy x2Map into xMapped
      math->copyInVector(sys->code[iCode].xValuesToSend, 0, 
                         sys->code[iCode].xValuesToMap, 0, 
                         sys->code[iCode].nGuesses);
      break;
      
    case NEWTON_PRE_MAP:
      if(stage==NEWTON_PRE_SEND){
        // Map
        
      }
      else{
      // Just copy x2Map into xMapped
      math->copyInVector(sys->code[iCode].xValuesToSend, 0, 
                         sys->code[iCode].xValuesToMap, 0, 
                         sys->code[iCode].nGuesses);        
      }
      break;
      
    case NEWTON_POST_MAP:
      if(stage==NEWTON_POST_RECV){
        // Map
        
      }
      else{
      // Just copy x2Map into xMapped
      math->copyInVector(sys->code[iCode].xValuesToSend, 0, 
                         sys->code[iCode].xValuesToMap, 0, 
                         sys->code[iCode].nGuesses);        
      }
      break;
      
    case NEWTON_BOTH_MAP:
      // Map
      
      break;
  }
  
  
  return error;
}
