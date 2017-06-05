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
  // Default values
  
  // Big residual
  residual = 1000;
  // Non linear tolerance
  nltol = 1e-07;
  // Non linear iterations
  iter = 0;
  // Maximum amount of non linear iterations allowed in each evolution step
  maxIter = 10;
	error = NEWTON_SUCCESS;
}

/* Solver iterateUntilConverge
This function controls the nonlinear iterations solving the residual equations.

input: -
output: -

*/
void Solver::iterateUntilConverge()
{
  calculateResiduals();
  rootPrints(" First guess: \t\t\t Residual: "+dou2str(residual));
  while(residual > nltol && iter < maxIter){
    
    calculateNewGuess();
    calculateResiduals();
    residual = 0;
    iter++;
    
    rootPrints(" Non linear iteration: "+int2str(iter)+"\t Residual: "+dou2str(residual));
  }
  if(residual>nltol){
    error = NEWTON_ERROR;
    checkError(error, "Maximum non linear iterations reached.");      
  }
}
  
/* Solver calculateResiduals

Calls particular solvers and compute the difference between guesses and calculations.

input: -
output: -

*/
void Solver::calculateResiduals()
{
  switch(method){
    case D2N_SERIAL:
      break;
      
    case D2N_PARALLEL:
      break;   
  }
}

/* Solver calculateNewGuess

Calculate new guess by different methods.

input: -
output: -

*/
void Solver::calculateNewGuess()
{
  
}  
