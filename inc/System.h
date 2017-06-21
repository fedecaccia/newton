/*****************************************************************************\

NEWTON                |
                      |
Multiphysics          | CLASS
coupling              | SYSTEM
maste code            |
                      |

-------------------------------------------------------------------------------

System contains all the usefull information of the strucure of the problem, 
like number of unknowns, connectivities, etc.

Date: 9 June 2017

-------------------------------------------------------------------------------

Copyright (C) 2017 Federico A. Caccia

This file is part of Newton.

Newton is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

Newton is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with Newton.  If not, see <http://www.gnu.org/licenses/>.

\*****************************************************************************/

#ifndef SYSTEM_H
#define SYSTEM_H

#include "global.h"
#include "Mapper.h"
#include "MathLib.h"

#include <string>
#include <algorithm>
#include <math.h>

class System
{
	public:
		System();
    void allocate1(int);
    void allocate3();
		void construct(Mapper*);
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
    std::string* betaName;
    double* gamma;
    int nGamma;
    std::string* gammaName;
    int* beta2linkInRes;
    int* beta2linkInX;
    int* x2linkInRes;
    int* x2linkInGamma;
    std::string* xName;
    std::string* resName;
    int* beta2linkInGamma;
    struct client{
      std::string name;
      int id;
      int type;
      int connection;
      std::string spawnType;
      int nProcs;
      int nArgs;
      std::string* arg;
      int nAlpha;
      double* alpha;
      int nBeta;
      int betaFirstValuePos;
      int nGamma;
      int gammaFirstValuePos;
      int nDelta;
      double* delta;
      std::string alphaMap;
      std::string gammaMap;
      
      std::string inputModelName;
      std::string inputModel;
      std::string actualInputName;      
      std::string actualInput;
      std::string inputExt;
      std::string restartName;
      std::string actualRestartName;
      std::string actualRestart;
      std::string restartExt;
      std::string restartPath;
      std::string outputName;
      std::string actualOutputName;
      std::string actualOutput;
      std::string outputExt;
      std::string outputPath;
      std::string binCommand;
      std::string commandToRun;      
    };
    client* code;    
		
	private:
		int error;
    int nMap;
    std::string* listOfMaps;    
    MathLib* math;
    Mapper* NewtonMap;

};

#endif
