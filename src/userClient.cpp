/*****************************************************************************\

NEWTON					      |
                      |
Implicit coupling 		|	CLASS
in nonlinear			    |	CLIENT
calculations			    |
                      |

-------------------------------------------------------------------------------

In this class user can programm specific routines to read output and write
inputs for a particular client code.

Author: Federico A. Caccia
Date: 7 June 2017

\*****************************************************************************/

#include "Client.h"

using namespace::std;

/* Client::prepareUserClientInput
 
In this function user can programm all necessary to write an input
for an specific client code.

input: code name, amount of values to send, vector with values to send 
and input file string
output: error: NEWTON_ERROR or NEWTON_SUCCESS

*/
int Client::prepareUserClientInput(string codeName, int nValues, double* values, string input)
{ 
  // Input file
  ofstream inputFile;
  
  inputFile.open(input.c_str());
	if (inputFile.is_open()){
		for(int i=0; i<nValues; i++){
      inputFile << values[i]<<endl;
    }
	}
	else{
		cout<<"Error writing input file for code: "<<codeName<<" - Client::prepareTestClientInput"<<endl;
    error = NEWTON_ERROR;
	}
  inputFile.close();
 
 return error; 
}

/* Client::readUserClientOutput
 
In this function user can programm all necessary to read an output
for an specific client code.

input: code name, amount of values to read, vector with values to read 
and output file string
output: error: NEWTON_ERROR or NEWTON_SUCCESS

*/
int Client::readUserClientOutput(string codeName, int nValues, double* values, string output)
{ 
  // Output file
  ifstream outputFile;
  
  outputFile.open(output.c_str());
	if (outputFile.is_open()){
		for(int i=0; i<nValues; i++){
      outputFile >> values[i];
    }
	}
	else{
		cout<<"Error reading output file from code: "<<codeName<<" - Client::readTestClientOutput"<<endl;
    error = NEWTON_ERROR;
	}
  outputFile.close();  
 
 return error; 
}
