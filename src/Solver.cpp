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

#include "Solver.h"

using namespace::std;

/* Evolution constructor
*/
Solver::Solver()
{
	error = NEWTON_SUCCESS;
}

/* Solver iterateUntilConverge
This function controls the nonlinear iterations solving the residual equations.

input: -
output: -

*/
void Solver::iterateUntilConverge()
{

}