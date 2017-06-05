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

#ifndef SYSTEM_H
#define SYSTEM_H

#include "global.h"

#include <string>

class System
{
	public:
		System();
		void construct();
      
    struct client{
      std::string name;
      int connection;
    };
    
    int nCodes;
    int nPhasesPerIter;
    int* codeToConnectInPhase;
    client* code;
    int nUnk;
    int* codeNGuesses;
    int* codeGuessesPossitions;
    int* codeNCalculations;
    int** codeNCalculations2Code;
    int** codeCalculationPossitions;    
		
	private:
		int error;

};

#endif
