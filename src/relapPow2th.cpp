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

/* Client::prepareRelapPow2thInput
 
In this function user can programm all necessary to write an input
for an specific client code.
Values in prepare(...)input are in the same order than the corresponding
mapper has saved them. If there are not mapper, values are in the same order 
that has been set in GUESSES in "newton.config".

input: code name, amount of values to send, vector with values to send 
and inputs file string (to write, and to read as model)
output: error: NEWTON_ERROR or NEWTON_SUCCESS

*/
int Client::prepareRelapPow2thInput(string codeName, int nValues, double* values, string input, string inputModel)
{ 
  // number of fermi code
  int iR=-1;
  for(int i=0; i<nRelap; i++){
    if(relap[i].name==codeName){
      iR = i;
      break;
    }
  }
  if(iR==-1){
    error=NEWTON_ERROR;
    cout<<"Bad Relap index name - Client::prepareRelapPow2thInput"<<endl;
    return error;
  }
  
  // Check amount of values received
  nValuesExpected = relap[iR].nAxialZones;
  if(nValues!=nValuesExpected){
    error=NEWTON_ERROR;
    cout<<"The amount of values received: "<<int2str(nValues)<<" is different from expected: "<<int2str(nValuesExpected)<<" - Client::prepareRelapPow2thInput";
    return error;
  }
  
  // Save values received as XS values in physical entities structures  
  int ivalue = 0;
  for(int iZone=0; iZone<relap[iR].nAxialZones; iZone++){
    relap[iR].fpow[iZone]=values[ivalue];
    ivalue++;
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
        
        if(line!="*NEWTON_POWER_CARD"){
          // Just copy input model
          inputFile <<line<<endl;
        }
        else{
          inputFile <<line<<endl;
          for(int iZone=0; iZone<relap[iR].nAxialZones; iZone++){
            // Same order as GUESSES in newton.config
            word=takeNextWord();
            inputFile<<word<<" ";
            word=takeNextWord();
            inputFile<<word<<" ";
            word=takeNextWord();
            inputFile<<relap[iR].fpow[iZone]<<" ";
            word=takeNextWord();
            inputFile<<word<<" ";
            word=takeNextWord();
            inputFile<<word<<" ";
            word=takeNextWord();
            inputFile<<word<<endl;
          }
          // Look for EndXs and continue copying
          while ( line!="*END_NEWTON_POWER_CARD"){
            line = takeNextLine();            
          }
          inputFile <<line<<endl;
        }
      }
    }
    else{
      cout<<"Error reading input model file: \""<<inputModel<<"\" for code: "<<codeName<<" - Client::prepareRelapPow2thInput"<<endl;
      error = NEWTON_ERROR;
    }
    inputModelFile.close();
    }
	else{
		cout<<"Error writing input file: \""<<input<<"\" for code: "<<codeName<<" - Client::prepareRelapPow2thInput"<<endl;
    error = NEWTON_ERROR;
	}
  inputFile.close();
 
 return error; 
}


/* Client::readRelapPow2thOutput
 
In this function user can programm all necessary to read an output
for an specific client code.
Values in read(...)output has to be saved in the same order than the 
corresponding mapper uses them. If there are not mapper, values has to be saved
in the same order that has been set in CALCS in "newton.config".

input: code name, amount of values to read, vector with values to read 
and output file string
output: error: NEWTON_ERROR or NEWTON_SUCCESS

*/
int Client::readRelapPow2thOutput(string codeName, int nValues, double* values, string output)
{ 
  // number of fermi code
  int iR=-1;
  for(int i=0; i<nRelap; i++){
    if(relap[i].name==codeName){
      iR = i;
      break;
    }
  }
  if(iR==-1){
    error=NEWTON_ERROR;
    cout<<"Bad Relap index name - Client::prepareRelapPow2thInput"<<endl;
    return error;
  }
  
  // Output file
  outputFile.open(output.c_str());
	if (outputFile.is_open()){

    while(!outputFile.eof()){
      word = takeNextWord();
      
      // Standing at the beginning of the interest time
      if(word=="0MAJOR"){
        // dummies
        word = takeNextWord();
        word = takeNextWord();
        // time
        word = takeNextWord();
        stringstream(word) >> relap[iR].t;

        if( relap[iR].t<=(relap[iR].tf + relap[iR].maxDt) && relap[iR].t>=(relap[iR].tf - relap[iR].maxDt)){
          
          while(!outputFile.eof()){
            
            
            // Refrigerant temperature
            if(word=="voidf"){
              
              // Standing at the beggining of the interest pipe
              while(word!=relap[iR].pipe[0]){
                word = takeNextWord();
              }
              
              for(int iZone=0; iZone<relap[iR].nAxialZones; iZone++){
                // pressure
                word = takeNextWord();
                // voidf
                word = takeNextWord();
                stringstream(word) >> relap[iR].voidf;
                // voidg
                word = takeNextWord();
                stringstream(word) >> relap[iR].voidg;
                // voidgo
                word = takeNextWord();
                // tempf
                word = takeNextWord();
                stringstream(word) >> relap[iR].tempf;
                // tempg
                word = takeNextWord();
                stringstream(word) >> relap[iR].tempg;
                
                // Mean temperature
                relap[iR].tempRef[iZone] = relap[iR].voidf*relap[iR].tempf + relap[iR].voidg*relap[iR].tempg;          
                
                // Taking rest of line
                word = takeNextLine();
                // Taking next pipe
                word = takeNextWord();           
              }
              
            }
            
            // Refrigerant density
            if(word=="rho-mix"){
              
              // Standing at the beggining of the interest pipe
              while(word!=relap[iR].pipe[0]){
                word = takeNextWord();
              }              
              
              for(int iZone=0; iZone<relap[iR].nAxialZones; iZone++){
                // rhof
                word = takeNextWord();
                // rhog
                word = takeNextWord();
                // rho-mix
                word = takeNextWord();
                stringstream(word) >> relap[iR].rhomix[iZone];
                
                // Taking rest of line
                word = takeNextLine();
                // Taking next pipe
                word = takeNextWord();            
              }
              
            }      
          
            // Fuel temperature
            if(word=="HEAT"){
              word = takeNextWord();
              if(word=="STRUCTURE"){
              
                // Looking for position
                while(word!="temperatures"){
                  word = takeNextWord();
                }
                
                // Standing at the beggining of the interest HS
                while(word!=relap[iR].hs[0]){
                  word = takeNextWord();  
                }              
                
                for(int iZone=0; iZone<relap[iR].nAxialZones; iZone++){
                  
                  // Temperature at the center
                  word = takeNextWord();
                  stringstream(word) >> relap[iR].tempFuelC;
                  
                  // Temperature at the cladding
                  for(int iMPoint=0; iMPoint<relap[iR].nRadialPoints-1; iMPoint++){
                    word = takeNextWord();
                  }
                  stringstream(word) >> relap[iR].tempFuelV;
                  
                  relap[iR].tempDoppFuel[iZone] = 0.3*relap[iR].tempFuelC + 0.7*relap[iR].tempFuelV;
                  
                  // Taking rest of line
                  word = takeNextLine();
                  // Taking next HS
                  word = takeNextWord();
                }
                
              }
            } 
            
            word = takeNextWord();
          }
        }      
      }
      
      word = takeNextWord();
    }


	}
	else{
		cout<<"Error reading output file from code: "<<codeName<<" - Client::readRelapPow2thOutput"<<endl;
    error = NEWTON_ERROR;
	}
  outputFile.close();
  
  // Saving data
  int ivalue=0;
  for(int iZone=0; iZone<relap[iR].nAxialZones; iZone++){
    // Same order as CALCS in newton.config
    values[ivalue] = relap[iR].tempRef[iZone];
    //cout<<iZone<<" T ref: "<<values[ivalue]<<" ";
    ivalue++;
    values[ivalue] = relap[iR].tempDoppFuel[iZone];
    //cout<<" T fuel: "<<values[ivalue]<<" ";
    ivalue++;
    values[ivalue] = relap[iR].rhomix[iZone];
    //cout<<" N ref: "<<values[ivalue]<<endl;
    ivalue++;
  }
 
  return error; 
}
