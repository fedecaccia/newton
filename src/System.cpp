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
  // Connection mode by process and code
  codeToConnectInPhase = new int[nCodes];
  // Number of guesses per code
  codeNGuesses = new int[nCodes];
  // Possition of first guess per code
  codeGuessesPossitions = new int[nCodes];
  // Number of calculations per code
  codeNCalculations = new int[nCodes];
  // Number of calculations per code that are input to another code
  codeNCalculations2Code = new int*[nCodes];
  // Possition of first calculations per code that are input to another code
  codeCalculationPossitions = new int*[nCodes];
  
  // Initialization
  for(int iCode=0; iCode<nCodes; iCode++){
    codeToConnectInPhase[iCode] = -1;
    code[iCode].connection = NEWTON_WAIT;
    codeNGuesses[iCode] = 0;
    codeGuessesPossitions[iCode] = 0;
    codeNCalculations[iCode] = 0;
    codeNCalculations2Code[iCode] = new int[nCodes];
    codeCalculationPossitions[iCode] = new int[nCodes];
    for(int jCode=0; jCode<nCodes; jCode++){
      codeNCalculations2Code[iCode][jCode] = 0;
      codeCalculationPossitions[iCode][jCode] = 0;
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
  
  /* Main variables:
   * 
   * codeNCalculations2Code: amount of variables that each code (row)
   *  send to each code (column). Diagonals should be with zeros.
   * 
   * codeNGuesses: number of guesses per code (row).
   * 
   * codeGuessesPossitions: possition in x of the first guess to the code (row).
   *  Guesses to the same code are contiguous.
   * 
   * codeNCalculations: amount of calculations per code (row).
   * 
   * codeNCalculations2Code: amount of calculations per code (row) that
   *  are inputs to another code (column). Diagonals should be with zeros.
   * 
   * codeCalculationPossitions: possition in x of the first calculation
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
  
  // Supossing that codeNCalculations2Code was loaded by Parser 
  
  // Seting amount of variables
  for(int iCode=0; iCode<nCodes; iCode++){
    for(int jCode=0; jCode<nCodes; jCode++){
      codeNGuesses[iCode] += codeNCalculations2Code[jCode][iCode];
      codeNCalculations[iCode] += codeNCalculations2Code[iCode][jCode];
    }
  }
  
  // Setting guess possitions
  int nPos = 0;
  for (int iCode=0; iCode<nCodes; iCode++){
    codeGuessesPossitions[iCode] = nPos;
    nPos += codeNGuesses[iCode];
  } 
  
  // Setting calculation possitions - should be TESTED
  nPos = 0;
  for (int iCode=0; iCode<nCodes; iCode++){
    for (int jCode=0; jCode<nCodes; jCode++){
      codeCalculationPossitions[jCode][iCode] = nPos;
      nPos += codeNCalculations2Code[jCode][iCode];
    }
  } 
  
	checkError(error,"Error setting system structure.");
}
