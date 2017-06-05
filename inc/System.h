/*****************************************************************************\

NEWTON					|
						|
Implicit coupling 		|	CLASS
in nonlinear			|	SYSTEM
calculations			|
						|

-------------------------------------------------------------------------------

System contains all the usefull information of the strucure of the problem, like number of unknowns, connectivities, etc.

Author: Federico A. Caccia
Date: 4 June 2017

\*****************************************************************************/

#ifndef SYSTEM_H
#define SYSTEM_H

#include "global.h"

class System
{
	public:
		System();
		void construct();
    
    int nCodes;
		
	private:
		int error;

};

#endif
