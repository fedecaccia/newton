/*****************************************************************************\

NEWTON					|
						|
Implicit coupling 		|	CLASS
in nonlinear			|	COMMUNICATOR
calculations			|
						|

-------------------------------------------------------------------------------

Evolution updates the evolution parameter and other problem dependent variables and configurations.

Author: Federico A. Caccia
Date: 4 June 2017

\*****************************************************************************/

#ifndef COMMUNICATOR_H
#define COMMUNICATOR_H

#include "global.h"

class Communicator
{
	public:
		Communicator();
		void initialize();		
		
	private:
		int error;

};

#endif