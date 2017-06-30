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

Date: 24 June 2017

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

/* Client::prepareNeutronicCr2kPInput
 
In this function user can programm all necessary to write an input
for an specific client code.
Values in prepare(...)input are in the same order than the corresponding
mapper has saved them. If there are not mapper, values are in the same order 
that has been set in GUESSES in "newton.config".

input: code name, amount of values to send, vector with values to send 
and inputs file string (to write, and to read as model)
output: error: NEWTON_ERROR or NEWTON_SUCCESS

*/
int Client::prepareNeutronicCr2kPInput(string codeName, int nValues, double* values, string input, string inputModel)
{ 
  // number of fermi code
  int iF=-1;
  for(int i=0; i<nFermi; i++){
    if(fermi[i].name==codeName){
      iF = i;
      break;
    }
  }
  if(iF==-1){
    error=NEWTON_ERROR;
    cout<<"Bad Fermi index name - Client::prepareNeutronicCr2kPInput"<<endl;
    return error;
  }
  
  // Check amount of values received
  nValuesExpected = fermi[iF].nCR;
  if(nValues!=nValuesExpected){
    error=NEWTON_ERROR;
    cout<<"The amount of values received: "
    <<int2str(nValues)<<" is different from expected: "
    <<int2str(nValuesExpected)<<" - Client::prepareNeutronicCr2kPInput"<<endl;
    return error;
  }
  
  // Save values received as CR position values in control rod structures  
  for(int icr=0; icr<fermi[iF].nCR; icr++){
    fermi[iF].cr[icr].pos = values[icr];
  }
      
  // Input file  
  inputFile.open(input.c_str());
  inputFile.precision(numeric_limits<double>::digits10 + 1);
  
  // Input model
  inputModelFile.open(inputModel.c_str());
  
	if(inputFile.is_open()){
    if(inputModelFile.is_open()){
    
      while(!inputModelFile.eof()){
            
        line = takeNextLine();
        
        if(line!="$Function"){
          // Just copy input model
          inputFile <<line<<endl;
        }
        else{
          // Print CR pos
          int icr=0;
          inputFile <<"$Function"<<endl;
          inputFile <<"  funcknd 1D"<<endl;
          inputFile <<"  funcnum "<<icr+1<<endl;
          inputFile <<"  funcint INTER1"<<endl;
          inputFile <<"  start"<<endl;
          inputFile <<"    0.0 "<<fermi[iF].cr[icr].pos<<endl;
          inputFile <<"    1.0 "<<fermi[iF].cr[icr].pos<<endl;
          inputFile <<"  end"<<endl;
          inputFile <<"$EndFunction"<<endl;
        
          // Look for EndXs and continue copying
          while ( line!="$EndFunction"){
            line = takeNextLine();
          }
        }
      }
    }
    else{
      cout<<"Error reading input model file: \""<<inputModel<<"\" for code: "<<codeName<<" - Client::prepareNeutronicCr2kPInput"<<endl;
      error = NEWTON_ERROR;
    }
    inputModelFile.close();
    }
	else{
		cout<<"Error writing input file: \""<<input<<"\" for code: "<<codeName<<" - Client::prepareNeutronicCr2kPInput"<<endl;
    error = NEWTON_ERROR;
	}
  inputFile.close();
 
 return error;
}

/* Client::readNeutronicCr2kPOutput
 
In this function user can programm all necessary to read an output
for an specific client code.
Values in read(...)output has to be saved in the same order than the 
corresponding mapper uses them. If there are not mapper, values has to be saved
in the same order that has been set in CALCS in "newton.config".

input: code name, amount of values to read, vector with values to read 
and output file string
output: error: NEWTON_ERROR or NEWTON_SUCCESS

*/
int Client::readNeutronicCr2kPOutput(string codeName, int nValues, double* values, string output)
{ 
  // Reading power distriution from "string output" and k eff from extra file
  
  // Output file: PHYSICAL ENTITIES POWER
  outputFile.open(output.c_str());
	if (outputFile.is_open()){
		for(int i=0; i<nValues; i++){
      // Same order as CALCS in newton.config
      outputFile >> values[i];
    }
	}
	else{
		cout<<"Error reading output file from code: "<<codeName<<" - Client::readNeutronicCr2kPOutput"<<endl;
    error = NEWTON_ERROR;
	}
  outputFile.close();

  outputFile.open("keff.out");
  if (outputFile.is_open()){
    // Same order as CALCS in newton.config
    outputFile >> values[nValues-1];
  }
  else{
    cout<<"Error reading output keff.out file from code: "<<codeName<<" - Client::readNeutronicCr2kPOutput"<<endl;
    error = NEWTON_ERROR;
  }
  outputFile.close();
  
  debug.log(codeName+"\n", 0, 0);
  debug.log("power:", 0, 0);
  for(int i=0; i<nValues-1; i++){
    debug.log(dou2str(values[i]));
  }
  debug.log("\n");
  debug.log("keff:", 0, 0);
  debug.log(dou2str(values[nValues-1]));
  debug.log("\n\n");
 
  return error; 
}
