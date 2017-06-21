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

Author: Federico A. Caccia
Date: 4 June 2017

\*****************************************************************************/

#ifndef MAPPER_H
#define MAPPER_H

#include "global.h"
#include "Client.h"
#include "MathLib.h"

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

  private:
    Client* client;
    int testMap(int, double*, int, double*);
    int th2xs(int, double*, int, double*, Client::fermiStruct*);
    int pow2fpow(int, double*, int, double*);
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
};

#endif
