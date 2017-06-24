/*****************************************************************************\

NEWTON                |
                      |
Multiphysics          | CLASS
coupling              | CLIENT
maste code            |
                      |

-------------------------------------------------------------------------------

In this class user can programm specific routines to read output and write 
inputs for a particular client code.

Date: 7 June 2017

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
      fermi[iF].pe[ipe].EFissionRate = new double[fermi[iF].nGroups];
      fermi[iF].pe[ipe].xs = new double*[fermi[iF].nXS];
      for(int iXS=0; iXS<fermi[iF].nXS; iXS++){
        fermi->pe[ipe].xs[iXS] = new double[fermi[iF].nGroups];
      }
    }
  }
  
  // Initialization
  for(int iF=0; iF<nFermi; iF++){
    for(int ipe=0; ipe<fermi[iF].nPhysicalEntities; ipe++){
      fermi[iF].pe[ipe].burnup = 0;
      fermi[iF].pe[ipe].material = -1; // Secure initialization
    }
  }
  
  // Relap
  for(int iR=0; iR<nRelap; iR++){
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
    cout<<"Neither model input nor output files are open - Client::takeNextWord"<<endl;
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
    cout<<"Neither model input nor output files are open - Client::takeNextLine"<<endl;
  }
  
  return line;
}


/* Client::updateVars



*/
void Client::updateVars(double time)
{
  // Fermi updates
  for(int iF=0; iF<nFermi; iF++){
    for(int ipe=0; ipe<fermi[iF].nPhysicalEntities; ipe++){
      for(int ig=0; ig<fermi[iF].nGroups; ig++){
        fermi[iF].pe[ipe].burnup += fermi[iF].pe[ipe].EFissionRate[ig]*24*60*60*time;
      }
    }
  }
  
  debug.log("BURNUP values:\n");
  for(int iF=0; iF<nFermi; iF++){
    for(int ipe=0; ipe<fermi[iF].nPhysicalEntities; ipe++){
      debug.log(fermi[iF].pe[ipe].name+": "+dou2str(fermi[iF].pe[ipe].burnup)+"\n");
    }
  }
  debug.log("\n");
}
