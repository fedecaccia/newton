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
Date: 15 June 2017

\*****************************************************************************/

#include "Client.h"

using namespace::std;

/* Client::prepareFermiXs2powInput
 
In this function user can programm all necessary to write an input
for an specific client code.
Values in prepare(...)input are in the same order than the corresponding
mapper has saved them. If there are not mapper, values are in the same order 
that has been set in GUESSES in "newton.config".

input: code name, amount of values to send, vector with values to send 
and inputs file string (to write, and to read as model)
output: error: NEWTON_ERROR or NEWTON_SUCCESS

*/
int Client::prepareFermiXs2powInput(string codeName, int nValues, double* values, string input, string inputModel)
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
    cout<<"Bad Fermi index name - Client::prepareFermiXs2powInput"<<endl;
    return error;
  }
  
  // Check amount of values received
  nValuesExpected = fermi[iF].nXS*fermi[iF].nGroups*fermi[iF].nPhysicalEntities;
  if(nValues!=nValuesExpected){
    error=NEWTON_ERROR;
    cout<<"The amount of values received: "
    <<int2str(nValues)<<" is different from expected: "
    <<int2str(nValuesExpected)<<" - Client::prepareFermiXs2powInput"<<endl;
    return error;
  }
  
  // Save values received as XS values in physical entities structures  
  int ivalue = 0;
  for(int ipe=0; ipe<fermi[iF].nPhysicalEntities; ipe++){
    for(int ixs=0; ixs<fermi[iF].nXS; ixs++){
      for(int ig=0; ig<fermi[iF].nGroups; ig++){
        fermi[iF].pe[ipe].xs[ixs][ig] = values[ivalue];
        ivalue++;
      }
    }
  }
  
  // Save Energy multipled by fission rate in each physical entity - THIS DEPENDS ON XS FISSION POSITION
  for(int ipe=0; ipe<fermi[iF].nPhysicalEntities; ipe++){
    for(int ig=0; ig<fermi[iF].nGroups; ig++){
      fermi[iF].pe[ipe].EFissionRate[ig] = fermi[iF].pe[ipe].xs[2][ig]*fermi[iF].pe[ipe].xs[3][ig];
    }
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
        
        if(line!="$Xs"){
          // Just copy input model
          inputFile <<line<<endl;
        }
        else{
          // Print XS
          inputFile <<"$Xs"<<endl;
          inputFile <<"egn "<<fermi[iF].nGroups<<endl;
          
          for(int ipe=0; ipe<fermi[iF].nPhysicalEntities; ipe++){
            inputFile<<fermi[iF].pe[ipe].name<<" 0 ";
            for(int ixs=0; ixs<fermi[iF].nXS; ixs++){
              for(int ig=0; ig<fermi[iF].nGroups; ig++){
                inputFile<<fermi[iF].pe[ipe].xs[ixs][ig]<<" ";
              }
            }
            inputFile<<" 1.0"<<endl;
          }
          
          inputFile <<"$EndXs"<<endl;
        
          // Look for EndXs and continue copying
          while ( line!="$EndXs"){
            line = takeNextLine();
          }
        }
      }
    }
    else{
      cout<<"Error reading input model file: \""<<inputModel<<"\" for code: "<<codeName<<" - Client::prepareFermiXs2powInput"<<endl;
      error = NEWTON_ERROR;
    }
    inputModelFile.close();
    }
	else{
		cout<<"Error writing input file: \""<<input<<"\" for code: "<<codeName<<" - Client::prepareFermiXs2powInput"<<endl;
    error = NEWTON_ERROR;
	}
  inputFile.close();
 
 return error; 
}

/* Client::readFermiXs2powOutput
 
In this function user can programm all necessary to read an output
for an specific client code.
Values in read(...)output has to be saved in the same order than the 
corresponding mapper uses them. If there are not mapper, values has to be saved
in the same order that has been set in CALCS in "newton.config".

input: code name, amount of values to read, vector with values to read 
and output file string
output: error: NEWTON_ERROR or NEWTON_SUCCESS

*/
int Client::readFermiXs2powOutput(string codeName, int nValues, double* values, string output)
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
    cout<<"Bad Fermi index name - Client::prepareFermiXs2powInput"<<endl;
    return error;
  }
  
  // Output file
  outputFile.open(output.c_str());
	if (outputFile.is_open()){
		for(int i=0; i<nValues; i++){
      // Same order as CALCS in newton.config
      outputFile >> values[i];
    }
	}
	else{
		cout<<"Error reading output file from code: "<<codeName<<" - Client::readFermiXs2powOutput"<<endl;
    error = NEWTON_ERROR;
	}
  outputFile.close(); 
  
  // TEST
  //~ cout<<"power"<<endl;
  //~ for(int i=0; i<nValues; i++){
    //~ cout<<values[i]<<endl;
  //~ }
 
  return error; 
}
