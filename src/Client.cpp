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

input: code type, code name, number of elements in array of values to send,
array of values to send, string of the input to write in the actual step
output: error

*/
int Client::prepareInput(int type, string name, int nDelta, double* delta, string actualInput, string inputModel)
{ 
  switch (type){
    
    case TEST:
      error = prepareTestClientInput(name, nDelta, delta, actualInput, inputModel);
      break;
    
    case USER_CODE:
      error = prepareUserClientInput(name, nDelta, delta, actualInput, inputModel);
      break;
      
    case RELAP_POW2TH:
      error = prepareRelapPow2thInput(name, nDelta, delta, actualInput, inputModel);
      break;
      
    case FERMI_XS2POW:
      error = prepareFermiXs2powInput(name, nDelta, delta, actualInput, inputModel);
      break;
      
    default:
      error = NEWTON_ERROR;
      cout<<"Client code type:\""<< type<<"\" not founded - Client::prepareInput"<<endl;
      break;
  }
 
 return error; 
}

/* Client::readOutput
 
This function selects the appropiate output reading routine based on code name. 
New code name switches have to be added here, also their definitions in global.h.

input: code type, code name, number of elements in array of values to receive,
array of values to receive, string of the output to read in the actual step
output: error

*/
int Client::readOutput(int type, string name, int nAlpha, double* alpha, string actualOutput)
{ 
  switch (type){
    
    case TEST:
      error = readTestClientOutput(name, nAlpha , alpha, actualOutput);
      break;
    
    case USER_CODE:
      error = readUserClientOutput(name, nAlpha , alpha, actualOutput);
      break;
    
    case RELAP_POW2TH:
      error = readRelapPow2thOutput(name, nAlpha , alpha, actualOutput);
      break;
    
    case FERMI_XS2POW:
      error = readFermiXs2powOutput(name, nAlpha , alpha, actualOutput);
      break;
      
    default:
      error = NEWTON_ERROR;
      cout<<"Client code type:\""<< type<<"\" not founded - Client::readOutput"<<endl;
      break; 
  } 
  
  return error;
}
