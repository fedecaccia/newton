/*****************************************************************************\

NEWTON					|
						|
Implicit coupling 		|	CLASS
in nonlinear			|	SOLVER
calculations			|
						|

-------------------------------------------------------------------------------

Solver computes the residual values and solves the nonlinear system with different kind of methods.

Author: Federico A. Caccia
Date: 4 June 2017

\*****************************************************************************/

#ifndef SOLVER_H
#define SOLVER_H

#include "global.h"

class Solver
{
	public:
		Solver();	
		
	private:
		int error;

};

#endif