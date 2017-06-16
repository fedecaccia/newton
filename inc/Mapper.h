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

#include <string>

class Mapper
{
	public:
		Mapper(Client*);
		void config();		
    int map(int, std::string, std::string, int, double*, int, double*);
		
	private:
    Client* client;
    int th2xs(int, double*, int, double*, Client::fermiStruct*);
    int pow2fpow(int, double*, int, double*);
		int error;
    MathLib* math;
    int nPhysicalEntities;
    int nXS;
    int nGroups;
    double* burnup;
};

#endif
