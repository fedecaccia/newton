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

class Mapper
{
	public:
		Mapper();
		void config();		
		
	private:
		int error;

};

#endif