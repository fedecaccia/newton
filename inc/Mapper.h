/*****************************************************************************\

NEWTON					      |
                      |
Implicit coupling 		|	CLASS
in nonlinear			    |	MAPPER
calculations			    |
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
		void config(std::string);		
    int map(int, std::string, std::string, int, double*, int, double*);
    
  private:
    Client* client;
    int th2xs(int, double*, int, double*, Client::fermiStruct*);
    int pow2fpow(int, double*, int, double*);
    void loadXSfromCRM();
		
    int error;
    MathLib* math;    
    std::ifstream crm;
    std::string xsFile;
    
    struct material{
      std::string name;
      int nBurnupPoints;
      int nDependences;
      double***** xs;
    };
    material* mat;
    double* energyPerFission;
};

#endif
