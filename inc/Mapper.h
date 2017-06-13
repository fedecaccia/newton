/*****************************************************************************\

NEWTON					|
						|
Implicit coupling 		|	CLASS
in nonlinear			|	MAPPER
calculations			|
						|

-------------------------------------------------------------------------------

Mapper manages the preprocessing and / or postprocessing of the variables received and / or sended to the clients.

Author: Federico A. Caccia
Date: 4 June 2017

\*****************************************************************************/

#ifndef MAPPER_H
#define MAPPER_H

#include "global.h"
#include "MathLib.h"

#include <string>

class Mapper
{
	public:
		Mapper();
		void config();		
    int map(int, string, int, double*, int, double*);
		
	private:
		int error;
    MathLib* math;
};

#endif
