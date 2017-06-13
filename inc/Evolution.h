/*****************************************************************************\

NEWTON					      |
                      |
Implicit coupling 		|	CLASS
in nonlinear			    |	EVOLUTION
calculations			    |
                      |

-------------------------------------------------------------------------------

Evolution updates the evolution parameter and other problem dependent variables 
and configurations.

Author: Federico A. Caccia
Date: 4 June 2017

\*****************************************************************************/

#ifndef EVOLUTION_H
#define EVOLUTION_H

#include "global.h"
#include "System.h"

class Evolution
{
	public:
		Evolution();
		void update(System*);

		int status;
		int step;
		int nSteps;
    double deltaStep;
		
	private:
		int error;

};

#endif
