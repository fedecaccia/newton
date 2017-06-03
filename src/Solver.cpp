/*****************************************************************************\

NEWTON					|
						|
Implicit coupling 		|	CLASS
in nonlinear			|	SOLVER
calculations			|
						|

-------------------------------------------------------------------------------

Solver manages the three main stages in residual calculation:
initializing everything to start calculations,
administrates evolution parameters,
controls that solution to residual equations 
converge in each evolution step,
and ends everything safety.

Author: Federico A. Caccia
Date: 3 June 2017

\*****************************************************************************/

#include "Solver.h"

Solver::Solver()
{

	
}

int Solver::initialize()
{
	return 0;
}

int Solver::run()
{
	return 0;
}

int Solver::finish()
{
	return 0;
}

#endif
