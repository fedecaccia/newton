/*****************************************************************************\

NEWTON                 |
                       |
Implicit coupling      |  CLASS
in nonlinear           |  SYSTEM
calculations           |
                       |

-------------------------------------------------------------------------------

System contains all the usefull information of the strucure of the problem, 
like number of unknowns, connectivities, etc.

Author: Federico A. Caccia
Date: 9 June 2017

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
    std::string** codeToConnectInPhase2;
    client* code;
    int nUnk;
    int nRes;
    
    double* beta;
    int nBeta;
    string* betaName;
    double* gamma;
    int nGamma;
    string* gammaName;
    int* beta2linkInRes;
    int* x2linkInRes;
    string* xName;
    string* resName;
    int* beta2linkInGamma;
    struct client2{
      string name;
      int connection;
      int nProcs;
      int nArgs;
      string* arg;
      int nAlpha;
      double* alpha;
      int nBeta;
      int betaFirstValuePos;
      int nGamma;
      int gammaFirstValuePos;
      int nDelta;
      double* delta;
      string alphaMap;
      string gammaMap;
      
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
    client2* code2;
		
	private:
		int error;
    MathLib* math;

};

#endif
