/*****************************************************************************\

NEWTON					       |
						           |
Implicit coupling 		 |	CLASS
in nonlinear			     |	SYSTEM
calculations			     |
						           |

-------------------------------------------------------------------------------

System contains all the usefull information of the strucure of the problem, like number of unknowns, connectivities, etc.

Author: Federico A. Caccia
Date: 4 June 2017

\*****************************************************************************/

#include "System.h"

using namespace::std;

/* System constructor
*/
System::System()
{
  // Number of client codes
  nCodes = 0;
  // Number of unknowns
  nUnk = 0;
  // Number of phases in explicit serial method
  nPhasesPerIter = 0;
  // Math object
  math = new MathLib();
  // Initial error value
	error = NEWTON_SUCCESS;
}


/* System::allocate1
Allocates main variables of the system.
Called by Parser.

input: -
output: -

*/
void System::allocate1()
{ 
  // Client data
  code = new client[nCodes];
  
  // Initialization
  for(int iCode=0; iCode<nCodes; iCode++){
    // Code name in number
    code[iCode].type = USER_CODE;
    // Type of connection
    code[iCode].connection = NEWTON_UNKNOWN_CONNECTION;
    // Mapper ID of values in each stage(pre_send and post_receive)
    code[iCode].map = new string[2];
    code[iCode].map[NEWTON_PRE_SEND] = "";
    code[iCode].map[NEWTON_POST_RECV] = "";
    // Number of arguments that bin has to take
    code[iCode].nArgs = 0;
    // Number of processes to spawn
    code[iCode].nProcs = 0;
    // Number of unknowns guesses
    code[iCode].nGuesses = 0;
    // Number of unknowns guesses mapped before send
    code[iCode].nGuessesMapped = 0;
    // Number of unknowns received without map
    code[iCode].nCalculationsWMap = 0;
    // Number of unknowns received and mapped
    code[iCode].nCalculations = 0;
    // Numer of unknowns wich are guesses to each code
    code[iCode].nCalculations2Code = new int[nCodes];
    // Numer of unknowns wich are guesses from another codes
    code[iCode].nCalculationsFromCode = new int[nCodes];
    // First possition of guesses in x vector
    code[iCode].firstGuessPossition = 0;
    // First possition of calculations in x vector, per code that receives
    code[iCode].calculationPossitions = new int[nCodes];
    for(int jCode=0; jCode<nCodes; jCode++){
      code[iCode].nCalculations2Code[jCode] = 0;
      code[iCode].nCalculationsFromCode[jCode] = 0;
      code[iCode].calculationPossitions[jCode] = 0;
    }
    // File strings    
    code[iCode].id = 0;    
    code[iCode].inputModelName = ""; // string model file input without extension
    code[iCode].actualInputName = ""; // string actual file input without extension
    code[iCode].actualInput = ""; // string actual file input with extension
    code[iCode].inputExt = ""; // string input extension
    code[iCode].restartName = "";
    code[iCode].actualRestartName = "";
    code[iCode].actualRestart = "";
    code[iCode].restartExt = "";
    code[iCode].restartPath = ".";
    code[iCode].actualOutputName = "";
    code[iCode].actualOutput = "";
    code[iCode].outputExt = "";
    code[iCode].outputPath = ".";
    code[iCode].binCommand = "";
    code[iCode].commandToRun = "";
  }  

}

/* System::allocate2
Set number of phases(rows) in wich codes ID(col) run in each phase 
(in explicit serial method).
After each phase, guesses are updated with previous calculations.
Also, set number of arguments that each bin has to take.

input: -
output: -

*/

void System::allocate2()
{  
  // Number of codes per phase
  nCodesInPhase = new int[nPhasesPerIter];
  // Code ID (col) to connect in each phase (row)
  codeToConnectInPhase = new int*[nPhasesPerIter];
  // Initialization
  for(int iPhase=0; iPhase<nPhasesPerIter; iPhase++){
    nCodesInPhase[iPhase] = 0;    
  } 
  
  // Number of arguments of each client code
  for(int iCode=0; iCode<nCodes; iCode++){
    code[iCode].arg = new string[code[iCode].nArgs];
    // Initialization
    for(int iArg=0; iArg<code[iCode].nArgs; iArg++){
      code[iCode].arg[iArg] = "";
    }
  }
}

/* System::allocate3

Allocate number of codes (col) in each phase in codeToConnectInPhase.

input: -
output: -

*/

void System::allocate3()
{  
  // Code ID (col) to connect in each phase (row)
  for(int iPhase=0; iPhase<nPhasesPerIter; iPhase++){
    codeToConnectInPhase[iPhase] = new int[nCodesInPhase[iPhase]];
    for(int iCode=0; iCode<nCodesInPhase[iPhase]; iCode++){
      codeToConnectInPhase[iPhase][iCode] = 0;
    }
  }
}

/* System::construct
Sets the configuration of the system.

input: -
output: -

*/
void System::construct()
{
  
  rootPrints("Building system...");
  
  /* Main variables in client struct:
   * 
   * nCalculations2Code: amount of variables that each code (row)
   *  send to each code (column). Diagonals should be with zeros.
   * 
   * nGuesses: number of guesses per code, which are solved by coupling.
   * nGuessesMapped: number of guesses per code, which are effectively sended.
   * 
   * firstGuessPossition: possition in x of the first guess to the code.
   *  Guesses to the same code are contiguous.
   * 
   * nCalculationsWMap: amount of calculations received from the code.
   * nCalculations: amount of calculations mapped, which are used in coupling.
   * 
   * nCalculations2Code: amount of calculations by the code that
   *  are inputs to another code (row). Zeros t the same code.
   * 
   * nCalculationPossitions: possition in x of the first calculation
   *  of the code that is input to another code (row).
   * 
   * x guess vector is organized as following:
   *  first all guesses of code with ID 0, and inside:
   *    first all guesses that come from code with ID 1,
   *    then guesses that come from code with ID 2,
   *    ...
   *    at the end guesses that come from the last code,
   *  second all guesses of code with ID 1, and inside:
   *    first all guesses that come from code with ID 0,
   *    then guesses that come from code with ID 2, 
   *    ...
   *    at the end guesses that come from the last code,
   *  ...
   *  at last all guesses of code with the greatest ID, and inside:
   *    first all guesses that come from code with ID 0,
   *    ...
   */
  
  // Supossing that code[x].nCalculations2Code[...] was loaded by Parser 
  
  /* Seting amount of variables:
   * 
   * 
   * 
   * 
   * 
   */
   
  for(int iCode=0; iCode<nCodes; iCode++){
    for(int jCode=0; jCode<nCodes; jCode++){
      code[iCode].nGuesses += code[iCode].nCalculationsFromCode[jCode];
      code[iCode].nGuessesMapped += code[iCode].nCalculationsFromCode[jCode];
      code[iCode].nCalculations += code[iCode].nCalculations2Code[jCode];
      code[iCode].nCalculationsWMap += code[iCode].nCalculations2Code[jCode];
    }
  }
  
  // Setting guess possitions
  int nPos = 0;
  for (int iCode=0; iCode<nCodes; iCode++){
    code[iCode].firstGuessPossition = nPos;
    nPos += code[iCode].nGuesses;
  }
  
  // Setting calculation possitions
  nPos = 0;
  for (int iCode=0; iCode<nCodes; iCode++){
    for (int jCode=0; jCode<nCodes; jCode++){
      code[jCode].calculationPossitions[iCode] = nPos;
      if(iCode==jCode){
        // This element is avoided. -1 to show errors if it is tried to be used!
        code[jCode].calculationPossitions[iCode] = -1;
      }
      nPos += code[jCode].nCalculations2Code[iCode];
    }
  }
  
  // Total unknowns
  for(int iCode=0; iCode<nCodes; iCode++){
    for(int jCode=0; jCode<nCodes; jCode++){
      nUnk += code[iCode].nCalculations2Code[jCode]; 
    }
  }
  
  
  //~ // TEST
  //~ cout<<"nGuesses"<<endl;
  //~ for(int i=0; i<nCodes; i++){
    //~ cout<<i<<" "<<code[i].nGuesses<<endl;
  //~ }
  //~ cout<<"nGuessesMapped"<<endl;
  //~ for(int i=0; i<nCodes; i++){
    //~ cout<<i<<" "<<code[i].nGuessesMapped<<endl;
  //~ }
  //~ cout<<"nCalculationsWMap"<<endl;
  //~ for(int i=0; i<nCodes; i++){
    //~ cout<<i<<" "<<code[i].nCalculationsWMap<<endl;
  //~ }
  //~ cout<<"nCalculations"<<endl;
  //~ for(int i=0; i<nCodes; i++){
    //~ cout<<i<<" "<<code[i].nCalculations<<endl;
  //~ }
  //~ cout<<"first guess"<<endl;
  //~ for(int i=0; i<nCodes; i++){
    //~ cout<<i<<" "<<code[i].firstGuessPossition<<endl;
  //~ }
  //~ cout<<"n calc 2 code"<<endl;
  //~ for(int i=0; i<nCodes; i++){
    //~ for(int j=0; j<nCodes; j++){
      //~ cout<<i<<" "<<j<<" "<<code[i].nCalculations2Code[j]<<endl;
    //~ }
  //~ }
  //~ cout<<"n calc from code"<<endl;
  //~ for(int i=0; i<nCodes; i++){
    //~ for(int j=0; j<nCodes; j++){
      //~ cout<<i<<" "<<j<<" "<<code[i].nCalculationsFromCode[j]<<endl;
    //~ }
  //~ }  
  //~ cout<<"first calc per code"<<endl;
  //~ for(int i=0; i<nCodes; i++){
    //~ for(int j=0; j<nCodes; j++){
      //~ cout<<i<<" "<<j <<" "<<code[i].calculationPossitions[j]<<endl;
    //~ }
  //~ }
  //~ cout<<"Total unknowns: "<<nUnk<<endl;
  //~ exit(1);
  
  // Vector of values of x that are guesses to a particular code, without mapping
  for(int iCode=0; iCode<nCodes; iCode++){
    code[iCode].xValuesToMap = new double[code[iCode].nGuesses];
  }
  
  // Vector of values of x that are guesses to a particular code, mapped
  for(int iCode=0; iCode<nCodes; iCode++){
    code[iCode].xValuesToSend = new double[code[iCode].nGuessesMapped];
  }
  
  // Vector of values of y that are received from a particular code
  for(int iCode=0; iCode<nCodes; iCode++){
    code[iCode].yValuesReceived = new double[code[iCode].nCalculationsWMap];
  }
  
  // Vector of values of y that are received from a particular code, mapped
  for(int iCode=0; iCode<nCodes; iCode++){
    code[iCode].yValuesMapped = new double[code[iCode].nCalculations];
  }
  
  for(int iCode=0; iCode<nCodes; iCode++){
    if(code[iCode].map[NEWTON_PRE_SEND]!=""){
      error = NEWTON_ERROR;
      rootPrints("You have to change amount of nGuessesMapped & nCalculationsWMap");      
    }    
    if(code[iCode].map[NEWTON_POST_RECV]!=""){
      error = NEWTON_ERROR;
      rootPrints("You have to change amount of nGuessesMapped & nCalculationsWMap");      
    }    
  }
  
  
  
  
  
  
  
  
  
  
  
  
  
  
  
  
  
  
  
  
  
  
  
  
  
  // Initialize file names and command args in I/O case
  error = setFilesAndCommands(0);
    
	checkError(error,"Error setting system structure.");
}

/* System::setFilesAndCommands

Set input, output, restart file and command to run strings.
This only matters to codes wich are connected by I/O.

input: step integer
output: error

*/
int System::setFilesAndCommands(int step)
{
  
  for(int iCode=0; iCode<nCodes; iCode++){
    // Input file
    code[iCode].actualInputName = code[iCode].inputModelName+"_step"+int2str(step);
    code[iCode].actualInput = code[iCode].actualInputName+"."+code[iCode].inputExt;
    // Ouput file
    if(code[iCode].outputName=="FROM_INPUT" || 
       code[iCode].outputName=="from_input" || 
       code[iCode].outputName=="From_input" ||
       code[iCode].outputName==""){
      code[iCode].actualOutputName = code[iCode].actualInputName;
      code[iCode].actualOutput = code[iCode].actualOutputName+"."+code[iCode].outputExt;
    }
    else{
      code[iCode].actualOutputName = code[iCode].outputName+"_step"+int2str(step);
      code[iCode].actualOutput = code[iCode].actualOutputName+"."+code[iCode].outputExt;
    }
    // Restart file
    if(code[iCode].restartName=="FROM_INPUT" || 
       code[iCode].restartName=="from_input" || 
       code[iCode].restartName=="From_input" ||
       code[iCode].restartName==""){
      code[iCode].actualRestartName = code[iCode].actualInputName;
      code[iCode].actualRestart = code[iCode].actualRestartName+code[iCode].restartExt;
    }
    else{
      code[iCode].actualRestartName = code[iCode].restartName+"_step"+int2str(step);
      code[iCode].actualRestart = code[iCode].actualRestartName+"."+code[iCode].restartExt;
    }
    // Command to run
    code[iCode].commandToRun = code[iCode].binCommand;
    for(int iArg=0; iArg<code[iCode].nArgs; iArg++){
      // Code ID
      if(code[iCode].arg[iArg]=="CODE_ID" || 
         code[iCode].arg[iArg]=="code_id" ||
         code[iCode].arg[iArg]=="Code_id"){
        code[iCode].commandToRun +=" "+int2str(code[iCode].id);
      }
      // Actual input with extension
      else if(code[iCode].arg[iArg]=="INPUT" || 
              code[iCode].arg[iArg]=="input" ||
              code[iCode].arg[iArg]=="Input"){
        code[iCode].commandToRun +=" "+code[iCode].actualInput;
      }
      // Actual input without extension
      else if(code[iCode].arg[iArg]=="INPUT_NAME" || 
              code[iCode].arg[iArg]=="input_name" ||
              code[iCode].arg[iArg]=="Input_name"){
        code[iCode].commandToRun +=" "+code[iCode].actualInputName;
      }
      // Actual output with extension
      else if(code[iCode].arg[iArg]=="OUTPUT" || 
              code[iCode].arg[iArg]=="ouput" ||
              code[iCode].arg[iArg]=="Ouput"){
        code[iCode].commandToRun +=" "+code[iCode].actualOutput;
      }
      // Actual output without extension
      else if(code[iCode].arg[iArg]=="OUTPUT_NAME" || 
              code[iCode].arg[iArg]=="output_name" ||
              code[iCode].arg[iArg]=="Output_name"){
        code[iCode].commandToRun +=" "+code[iCode].actualOutputName;
      }
      // Else, just use the string readed as argument
      else{
        code[iCode].commandToRun += " "+code[iCode].arg[iArg];
      }
    }
  }  
  return error;
}


























/* System::extractToMap()
Extracts from x all values of interest of code iCode.

input: code ID, x vector
output: error

*/
int System::extractToMap(int iCode, double* x)
{
  math->copyInVector(code[iCode].xValuesToMap, 0, x, code[iCode].firstGuessPossition, code[iCode].nGuesses);
  error = NEWTON_SUCCESS;
  return error;
}

/* System::setMapped()
Set on y all values mapped calculated by code iCode.

input: code ID, y vector
output: error

*/
int System::setMappedOnY(int iCode, double* y)
{
  // Lecture possition in yValuesMapped
  int pos = 0;
  for(int jCode=0; jCode<nCodes; jCode++){
    math->copyInVector(y, code[iCode].calculationPossitions[jCode], // vector to save the copy, possition of first element
                       code[iCode].yValuesMapped, pos, // vector to copy, possition of first element
                       code[iCode].nCalculations2Code[jCode]); // number of elements to copy
    // Lecture possition incremented
    pos+=code[iCode].nCalculations2Code[jCode];
  }  
  
  error = NEWTON_SUCCESS;
  return error;
}
