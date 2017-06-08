/*****************************************************************************\

NEWTON					      |
                      |
Implicit coupling 		|	CLASS
in nonlinear			    |	CLIENT
calculations			    |
                      |

-------------------------------------------------------------------------------

In this class the user can programm specific routines to read output and write
inputs for a particular client code.

Author: Federico A. Caccia
Date: 7 June 2017

\*****************************************************************************/

#include "Client.h"

using namespace::std;

/* Client constructor
*/
Client::Client()
{
  // Initial error value
	error = NEWTON_SUCCESS;
}


/* Client::prepareInput
 
This function selects the appropiate input preparing routine based on code name. 
New code name switches have to be added here, also their definitions in global.h.

input: code ID and System
output: error

*/
int Client::prepareInput(int iCode, System* sys)
{ 
  switch (sys->code[iCode].type){
    
    case TEST:
      error = prepareTestClientInput(sys->code[iCode].id, 
                                     sys->code[iCode].nGuessesMapped, 
                                     sys->code[iCode].xValuesToSend, 
                                     sys->code[iCode].actualInput);    
      break;
    
    case USER_CODE:
      error = prepareUserClientInput(sys->code[iCode].id, 
                                     sys->code[iCode].nGuessesMapped, 
                                     sys->code[iCode].xValuesToSend, 
                                     sys->code[iCode].actualInput);    
      break;
      
    default:
      error = NEWTON_ERROR;
      cout<<"Client code name not founded - Client::prepareInput"<<endl;
      break; 
  }
 
 return error; 
}

/* Client::readOutput
 
This function selects the appropiate output reading routine based on code name. 
New code name switches have to be added here, also their definitions in global.h.

input: code ID and System
output: error

*/
int Client::readOutput(int iCode, System* sys)
{ 
  switch (sys->code[iCode].type){
    
    case TEST:
      error = readTestClientOutput(sys->code[iCode].id, 
                                   sys->code[iCode].nCalculationsWMap, 
                                   sys->code[iCode].yValuesReceived, 
                                   sys->code[iCode].actualOutput);    
      break;
    
    case USER_CODE:
      error = readUserClientOutput(sys->code[iCode].id, 
                                   sys->code[iCode].nCalculationsWMap, 
                                   sys->code[iCode].yValuesReceived, 
                                   sys->code[iCode].actualOutput);    
      break;
      
    default:
      error = NEWTON_ERROR;
      cout<<"Client code name not founded - Client::readOutput"<<endl;
      break; 
  } 
  
  return error;
}
