/*****************************************************************************\

NEWTON					|
						|
Implicit coupling 		|	CLASS
in nonlinear			|	SYSTEM
calculations			|
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


/* System::allocate
Allocates main variables of the system.
Called by Parser.

input: -
output: -

*/
void System::allocate()
{ 
  // Client data
  code = new client[nCodes];
  
  // Initialization
  for(int iCode=0; iCode<nCodes; iCode++){
    // Code name
    code[iCode].name = "";
    // Code name in number
    code[iCode].type = USER_CODE;
    // Type of connection
    code[iCode].connection = NEWTON_UNKNOWN_CONNECTION;
    // Type of map of values: none, map variables after receive, map variables before send, or both?
    code[iCode].map = NEWTON_NO_MAP;
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
    // First possition of guesses in x vector
    code[iCode].firstGuessPossition = 0;
    // First possition of calculations in x vector, per code that receives
    code[iCode].calculationPossitions = new int[nCodes];
    for(int jCode=0; jCode<nCodes; jCode++){
      code[iCode].nCalculations2Code[jCode] = 0;
      code[iCode].calculationPossitions[jCode] = 0;
    }
    // File strings    
    code[iCode].inputName = "";
    code[iCode].inputModel = "";
    code[iCode].actualInput = "";
    code[iCode].inputExt = "";
    code[iCode].actualRestart = "";
    code[iCode].restartExt = "";
    code[iCode].restartPath = ".";
    code[iCode].actualOutput = "";
    code[iCode].outputExt = "";
    code[iCode].outputPath = ".";
    code[iCode].commandToRun = ".";
  }  

  /* Phases.
   * Set number of phases(rows) in wich codes ID(col) run in each phase 
   * (in explicit serial method).
   * After each phase, guesses are updated with previous calculations.
   */
  
  // Number of codes per phase
  nCodesInPhase = new int[nPhasesPerIter];
  // Code ID (col) to connect in each phase (row)
  codeToConnectInPhase = new int*[nPhasesPerIter];
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
   * nGuesses: number of guesses per code (row).
   * 
   * firstGuessPossition: possition in x of the first guess to the code (row).
   *  Guesses to the same code are contiguous.
   * 
   * nCalculations: amount of calculations per code (row).
   * 
   * nCalculations2Code: amount of calculations per code (row) that
   *  are inputs to another code (column). Diagonals should be with zeros.
   * 
   * nCalculationPossitions: possition in x of the first calculation
   *  per code (row) that is input to another code (column).
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
  
  // Seting amount of variables
  for(int iCode=0; iCode<nCodes; iCode++){
    for(int jCode=0; jCode<nCodes; jCode++){
      code[iCode].nGuesses += code[jCode].nCalculations2Code[iCode];
      code[iCode].nCalculations += code[jCode].nCalculations2Code[iCode];
    }
  }
  
  // Setting guess possitions
  int nPos = 0;
  for (int iCode=0; iCode<nCodes; iCode++){
    code[iCode].firstGuessPossition = nPos;
    nPos += code[iCode].nGuesses;
  }
  
  // Setting calculation possitions - should be TESTED
  nPos = 0;
  for (int iCode=0; iCode<nCodes; iCode++){
    for (int jCode=0; jCode<nCodes; jCode++){
      code[iCode].calculationPossitions[jCode] = nPos;
      nPos += code[iCode].nCalculations2Code[jCode];
    }
  }
  
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
  
  
	checkError(error,"Error setting system structure.");
}

/* System::ToMap()
Extracts from x all values of interest of code iCode.

input: code ID, x vector
output: error

*/
int System::ToMap(int iCode, double* x)
{
  math->copyInVector(code[iCode].xValuesToMap, 0, x, code[iCode].firstGuessPossition, code[iCode].nGuesses);
  error = NEWTON_SUCCESS;
  return error;
}
