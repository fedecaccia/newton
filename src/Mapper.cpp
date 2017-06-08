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

input: code ID, vector with data to map, vector in wich mapped values are saved
output: error

*/
int Mapper::map(System* sys, int iCode, int stage)
{
  if(sys->code[iCode].map[stage] == ""){ // id vacío
    // Just copy x2Map into xMapped
    math->copyInVector(sys->code[iCode].xValuesToSend, 0, 
                       sys->code[iCode].xValuesToMap, 0, 
                       sys->code[iCode].nGuesses);
  }
  else{
      // Map
      // IDEA
      // if(sys->code[iCode].map == function.id para alguna de las funciones de mapeo cargadas
      // (el usuario al escribir la función, guarda el id en un string)      
      
      // usar sys->code[iCode].xvaluesToMap
      // usar sys->code[iCode].xvaluesToSend
      // usar sys->code[iCode].yvaluesReceived
      // usar sys->code[iCode].yvaluesMapped
  }
  
  
  return error;
}
