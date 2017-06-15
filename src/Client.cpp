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
  // Amount of Fermi codes;
  nFermi = 0;
  // Amount of RELAP codes;
  nRelap = 0;
  
  // Initial error value
	error = NEWTON_SUCCESS;
}

/* Client::allocate
Allocates necessary things in first step.

input: number of fermi & relap clients
output: -
*/
void Client::allocate1()
{
  // Client structures
  fermi = new fermiStruct[nFermi];
  relap = new relapStruct[nRelap];
  
  // Initial error value
	error = NEWTON_SUCCESS;
}

/* Client::allocate
Allocates necessary things in physical entitities.

input: -
output: -
*/
void Client::allocate2()
{
  // Fermi
  for(int iF=0; iF<nFermi; iF++){
    fermi[iF].pe = new fermiStruct::physicalEntity[fermi[iF].nPhysicalEntities];
    for(int ipe=0; ipe<fermi[iF].nPhysicalEntities; ipe++){
      fermi[iF].pe[ipe].xs = new double*[fermi[iF].nXS];
      for(int iXS=0; iXS<fermi[iF].nXS; iXS++){
        fermi->pe[ipe].xs[iXS] = new double[fermi[iF].nGroups];
      }
    }
  }
  
  // Relap
  for(int iR=0; iR<nFermi; iR++){
    relap[iR].hs = new string[relap[iR].nAxialZones];
    relap[iR].pipe = new string[relap[iR].nAxialZones];
    relap[iR].fpow = new double[relap[iR].nAxialZones];
    relap[iR].tempRef = new double[relap[iR].nAxialZones];
    relap[iR].rhomix = new double[relap[iR].nAxialZones];
    relap[iR].tempDoppFuel = new double[relap[iR].nAxialZones];
  }
  
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

/* Client::takeNextWord
Get the next word.

input: -
output: word string

*/
string Client::takeNextWord()
{
  // Next word
  if(inputModelFile.is_open()){
    inputModelFile >> word;
  }
  else if(outputFile.is_open()){
    outputFile >> word;
  }
  else{
    error=NEWTON_ERROR;
    checkError(error, "Neither model input nor output files are open - Client::takeNextWord");
  }
  
  return word;
}

/* Client::takeNextLine
Get the next line.

input: -
output: line string

*/
string Client::takeNextLine()
{
  // Next word
  if(inputModelFile.is_open()){
    getline(inputModelFile,line);
  }
  else if(outputFile.is_open()){
    getline(outputFile,line);
  }
  else{
    error=NEWTON_ERROR;
    checkError(error, "Neither model input nor output files are open - Client::takeNextLine");
  }
  
  return line;
}
