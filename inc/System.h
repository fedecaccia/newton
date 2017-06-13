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
#include "Mapper.h"
#include "MathLib.h"

#include <string>
#include <math.h>

class System
{
	public:
		System();
    void allocate1(int);
    void allocate3();
		void construct();
    int setFilesAndCommands(int);
    void alpha2beta(int);
    void computeResiduals(double*, double*);
    void x2gamma(double*);
    void beta2x(double*);
    void beta2gamma();
    void gamma2delta(int);
    int findCodeInPhase(int, int);
    
    int nCodes;
    int nPhasesPerIter;
    int* nCodesInPhase;
    std::string** codeToConnectInPhase;
    //~ client* code;
    int nUnk;
    int nRes;
    
    double* beta;
    int nBeta;
    string* betaName;
    double* gamma;
    int nGamma;
    string* gammaName;
    int* beta2linkInRes;
    int* beta2linkInX;
    int* x2linkInRes;
    int* x2linkInGamma;
    string* xName;
    string* resName;
    int* beta2linkInGamma;
    struct client{
      string name;
      int id;
      int type;
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
    client* code;    
		
	private:
		int error;    
    MathLib* math;
    Mapper* NewtonMap;

};

#endif
