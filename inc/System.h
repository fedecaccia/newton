/*****************************************************************************\

NEWTON                 |
                       |
Implicit coupling      s|  CLASS
in nonlinear           |  SYSTEM
calculations           |
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
    void allocate1();
    void allocate2();
    void allocate3();
		void construct();
    int setFilesAndCommands(int);
    int extractToMap(int, double*);
    int setMappedOnY(int, double*);
    
    struct client{
      int id;
      //std::string name;
      int type;
      int connection;
      string* map;
      int nProcs;
      int nArgs;
      string* arg;
      int nGuesses;
      int nGuessesMapped;      
      int nCalculationsWMap;      
      int nCalculations;
      int* nCalculations2Code;
      int* nCalculationsFromCode;
      int firstGuessPossition;      
      int* calculationPossitions;
      double* xValuesToMap;
      double* xValuesToSend;
      double* yValuesReceived;
      double* yValuesMapped;
      string inputModelName;
      string actualInputName;      
      string actualInput;
      string inputExt;
      string restartName;
      string actualRestartName;
      string actualRestart;
      string restartExt;
      string restartPath;
      string outputName;
      string actualOutputName;
      string actualOutput;
      string outputExt;
      string outputPath;
      string binCommand;
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
