/*****************************************************************************\

NEWTON                |
                      |
Multiphysics          | CLASS
coupling              | MAPPER
maste code            |
                      |

-------------------------------------------------------------------------------

Mapper manages the preprocessing and / or postprocessing of the variables 
received and / or sended to the clients.

Date: 4 June 2017

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

#ifndef MAPPER_H
#define MAPPER_H

#include "global.h"
#include "Client.h"
#include "MathLib.h"
#include "Debugger.h"

#include <fstream>
#include <sstream>
#include <string>

class Mapper
{
	public:
		Mapper(Client*);
    void allocate1();
		void config(std::string);		
    int map(int, std::string, std::string, int, double*, int, double*);
    
    std::string xsFile;  
    int nMat;
    struct material{
      std::string name;
      int nBurnupPoints;
      double* burnup;
      int nDependencies;      
      double* refDepVal;
      double** deltaDepVal;      
      int* nPointsPerDependency;
      double***** xs;
      double*** xsRef;
      std::string* depName;
    };
    material* mat;
    int nGroups;
    int nXSTypes;
    double* energyPerFission;
    
    Debugger debug;

  private:
    Client* client;
    int testMap(int, double*, int, double*);
    int userMap(int, double*, int, double*);
    int th2xs(int, double*, int, double*, Client::fermiStruct*);
    double calculateDeltaXS(int, int, int, int, int, double);
    int pow2spow(int, double*, int, double*);
    int integratePow(int, double*, int, double*);
    int channel2FE(int, double*, int, double*);
    int spow2fpow(int, double*, int, double*);
    int spowk2fpowk(int, double*, int, double*);
    void loadXSfromCRM();
    std::string takeNextWord();
    std::string takeNextLine();
		
    int error;
    int materialReaded;
    MathLib* math;    
    std::ifstream crm;
    std::string word;

    // Neutronic: th 2 xs
/*    double* tRef;
    double* tFuel;
    double* nRef;
    double* burnup;
    double*** xs;
    double*** xsDown;
    double*** xsUp;*/
    double deltaXSi, deltaDepi, deltaDepVali, deltaXS, deltaInDependency, depRef;
    
};

#endif
