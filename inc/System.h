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
#include "MathLib.h"

#include <string>
#include <math.h>

class System
{
	public:
		System();
    void allocate();
		void construct();
    int ToMap(int, double*);
      
    struct client{
      std::string name;
      int type;
      int connection;
      int map;
      int nProcs;
      int nGuesses;
      int nGuessesMapped;      
      int nCalculationsWMap;      
      int nCalculations;
      int* nCalculations2Code;
      int firstGuessPossition;      
      int* calculationPossitions;
      double* xValuesToMap;
      double* xValuesToSend;
      double* yValuesReceived;
      double* yValuesMapped;
      string inputName;
      string inputModel;
      string actualInput;
      string inputExt;
      string actualRestart;
      string restartExt;
      string restartPath;
      string actualOutput;
      string outputExt;
      string outputPath;
      string commandToRun;
    };
    
    int nCodes;
    int nPhasesPerIter;
    int* nCodesInPhase;
    int** codeToConnectInPhase;
    client* code;
    int nUnk;
		
	private:
		int error;
    MathLib* math;

};

#endif
