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
  // Math object
  math = new MathLib();  
  
  // Initial error value
	error = NEWTON_SUCCESS;
}

/* Solver::initialize

This function allocate space to numerical solution variables.

input: -
output: -

*/
void Solver::initialize(System* sys)
{
  // Guess
  x = new double[sys->nUnk];
  // Guess updated in serial calculations
  xStar = new double[sys->nUnk];
  // Calculation y=f(xStar)
  y = new double[sys->nUnk];
  // Residual vector
  resVector = new double[sys->nUnk];
  
  // Initialization
  math->zeros(x, sys->nUnk);
  math->zeros(xStar, sys->nUnk);
  math->zeros(y, sys->nUnk);
  math->zeros(resVector, sys->nUnk);
}


/* Solver:: setInitialCondition

This function sets the intial guess. In first evolution step it corresponds
to the initial condition loaded from configuration file, or, in case that
it couldn't be supported, every guess value is zero. In following steps
it is calculated interpolating previous values.

input: step value
output: -

*/
void Solver::setInitialCondition(int step)
{
  if(step!=0){
    // Interpolate previous values
    
  }
  else{
    // Set CI loaded or default zero values
    
  }
}


/* Solver iterateUntilConverge
This function controls the nonlinear iterations solving the residual equations.

input: -
output: -

*/
void Solver::iterateUntilConverge(System* sys, Mapper* map, Communicator* comm)
{
  calculateResiduals(sys, map, comm);
  rootPrints(" First guess: \t\t\t Residual: "+dou2str(residual));
  while(residual > nltol && iter < maxIter){
    
    calculateNewGuess();
    calculateResiduals(sys, map, comm);
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
void Solver::calculateResiduals(System* sys, Mapper* map, Communicator* comm)
{    
  /* Copy x values into xStar.
   * xStar is updated with another code calculations
   * in EXPLICIT_SERIAL method.
   */
  math->copyInVector(xStar, 0, x, 0, sys->nUnk);
  
  // Zeroing vector that receives calculation values
  math->zeros(y, sys->nUnk);
  
  /* Each iteration is composed by phases.
  * In each phase n codes run.
  * Each rank connect to just one client in each phase. 
  */
  for(int iPhase=0; iPhase<sys->nPhasesPerIter; iPhase++){
    // Code that runs each process
    iCode = sys->codeToConnectInPhase[iPhase];
    // The conenction is specific to process and code
    switch(sys->code[iCode].connection){
      case NEWTON_WAIT:
        break;       
      
      case NEWTON_SPAWN:
        // From xStar to iCode
        error = runCode(iCode);if(error>0)break;
        // From iCode to y
        error = readOutputFromCode(iCode);
        break;
      
      case NEWTON_MPI_COMMUNICATION:
        // From xStar to iCode
        error = sendDataToCode(iCode);if(error>0)break;
        // From iCode to y
        error = receiveDataFromCode(iCode);
        break;
        
      default:
        error = NEWTON_ERROR;
        break;
    }
    // All processes check errors. Barrier.
    checkError(error, "Error in residual calculation.");
    
    // In serial case, xStar must be updated after each phase calculation
    if(method==EXPLICIT_SERIAL){
      
      // Share calculations between process
      error = MPI_Allreduce(MPI_IN_PLACE, // const void *sendbuf
                            y, // void *recvbuf
                            sys->nUnk, // int count: number of elements in send buffer (integer) 
                            MPI_DOUBLE_PRECISION, // MPI_Datatype datatype
                            MPI_SUM, // MPI_Op op
                            MPI_COMM_WORLD); // MPI_Comm comm
      checkError(error, "Error sharing y between processes.");
      
      // Update xStar in the right possitions
      // y(local, pos) -> xStar(all, pos)
      error = NEWTON_ERROR;
      checkError(error, "xStar has to be updated after phase calculation.");
      
      // Zeroing y to allow new mpi_Allreduce in next phase
      math->zeros(y, sys->nUnk);      
    }
  }
  
  // In EXPLICIT_SERIAL, residuals are calculated as (xStar - x)
  if(method==EXPLICIT_SERIAL){
    resVector = math->differenceInVectors(xStar, x, sys->nUnk);
  }
  // In other cases, residuals are calculated as (y - x)
  else{
    // First update y in every process
    error = MPI_Allreduce(MPI_IN_PLACE, // const void *sendbuf
                          y, // void *recvbuf
                          sys->nUnk, // int count: number of elements in send buffer (integer) 
                          MPI_DOUBLE_PRECISION, // MPI_Datatype datatype
                          MPI_SUM, // MPI_Op op
                          MPI_COMM_WORLD); // MPI_Comm comm  
    checkError(error, "Error sharing y between processes.");
    // Compute residuals
    resVector = math->differenceInVectors(y, x, sys->nUnk);
  }
  
  // Calculate norm 2 of the residual vector
  residual = math->moduleAbs(resVector, sys->nUnk);  
}

/* Solver calculateNewGuess

Calculate new guess by different methods.

input: -
output: -

*/
void Solver::calculateNewGuess()
{
  switch(method){
    case EXPLICIT_SERIAL:
      break;
      
    case EXPLICIT_PARALLEL:
      break;   
      
    case NEWTON:
      break;   
      
    case SECANT:
      break;   
      
    case BROYDEN:
      break;   
      
    default:
      error = NEWTON_ERROR;
      checkError(error, "Non linear method has not been implemented yet");
  }
}

/* Solver::runCode

Spawn specific code.

input: code ID
output: error

*/
int Solver::runCode(int iCode)
{  
  error = NEWTON_SUCCESS;
  
  return error;
}

/* Solver::readOutputFromCode

Load variables from specific ouput file.

input: code ID
output: error

*/
int Solver::readOutputFromCode(int iCode)
{
  error = NEWTON_SUCCESS;
  
  return error;
}

/* Solver::sendDataToCode

Send data to a client via MPI functions.

input: code ID
output: error

*/
int Solver::sendDataToCode(int iCode)
{
  error = NEWTON_SUCCESS;
  
  return error;
}

/* Solver::receiveDataFromCode

Receive data from a client via MPI functions.

input: code ID
output: error

*/
int Solver::receiveDataFromCode(int iCode)
{
  error = NEWTON_SUCCESS;
  
  return error;
}
