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
  
  // Non linear method
  method = NO_METHOD;
  // Non linear tolerance
  nltol = 1e-07;
  // Non linear iterations
  iter = 0;
  // Maximum amount of non linear iterations allowed in each evolution step
  maxIter = 10;
  // Delta x in Jacobian calculation by finitte difference method
  dxJacCalc = 0.1;
  // Steps between Jacobian calculation by finite difference method
  sJacCalc = 0;    
  // Math object
  math = new MathLib();
  // client code object
  NewtonClient = new Client();
  
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
  // Calculations, in order of interest
  y = new double[sys->nUnk];
  // Residual vector
  resVector = new double[sys->nUnk];
  // Matrices
  
  
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
    iter++;
    
    rootPrints(" Non linear iteration: "+int2str(iter)+"\t Residual: "+dou2str(residual));
  }
  if(residual>nltol){
    error = NEWTON_ERROR;
    checkError(error, "Maximum non linear iterations reached - Solver-iterateUntilConverge");      
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
    
  // In serial case, xStar must be updated after certain calculations
  if(method==EXPLICIT_SERIAL){
    
    /* Each iteration is composed by phases.
     * After each phase, xStar is updated with previous calculations.
     */    
    for(int iPhase=0; iPhase<sys->nPhasesPerIter; iPhase++){
            
    
      // Send variables to all clients that connect by MPI in this phase
      for(int iPhaseCode = 0; iPhaseCode<sys->nCodesInPhase[iPhase]; iPhaseCode++){
        int iCode = sys->codeToConnectInPhase[iPhase][iPhaseCode];
        if(sys->code[iCode].connection==NEWTON_MPI_COMMUNICATION){
          if(irank==0){ // only root works
            error = sendDataToCode(iCode);
          }
          // All processes check
          checkError(irank, "Error sending data to code - Solver::calculateResiduals");
        }
      }
      // Run codes by script in this phase
      freeRank = 0;
      for(int iPhaseCode = 0; iPhaseCode<sys->nCodesInPhase[iPhase]; iPhaseCode++){
        int iCode = sys->codeToConnectInPhase[iPhase][iPhaseCode];
        if(sys->code[iCode].connection==NEWTON_SPAWN){
          if(irank==freeRank){
            
            error = runCode(iCode, sys, map);
          }
          freeRank++;
          // When all processes run particular codes, 
          // check errors and give them more work
          if(freeRank==world_size){
            checkError(error, "Error running code - Solver::calculateResiduals");
            freeRank = 0;
          }
        }
      }
      // Other communication types  in this phase?
      
      
      // Read output from codes that run by script in this phase
      freeRank = 0;
      for(int iPhaseCode = 0; iPhaseCode<sys->nCodesInPhase[iPhase]; iPhaseCode++){
        int iCode = sys->codeToConnectInPhase[iPhase][iPhaseCode];
        if(sys->code[iCode].connection==NEWTON_SPAWN){
          if(irank==freeRank){
            error = readOutputFromCode(iCode, sys, map);
          }
          freeRank++;
          // When all processes run particular codes, 
          // check errors and give them more work
          if(freeRank==world_size){
            checkError(error, "Error reading output from code - Solver::calculateResiduals");
            freeRank = 0;
          }
        }
      }
      // Receive variables from all clients  in this phase that connect by MPI
      for(int iPhaseCode = 0; iPhaseCode<sys->nCodesInPhase[iPhase]; iPhaseCode++){
        int iCode = sys->codeToConnectInPhase[iPhase][iPhaseCode];
        if(sys->code[iCode].connection==NEWTON_MPI_COMMUNICATION){
          if(irank==0){ // only root works
            error = receiveDataFromCode(iCode);
          }
          // All processes check
          checkError(irank, "Error receiving data from code - Solver::calculateResiduals");
        }
      }
      
      // Update y in every process
      error = MPI_Allreduce(MPI_IN_PLACE, // const void *sendbuf
                            y, // void *recvbuf
                            sys->nUnk, // int count: number of elements in send buffer (integer) 
                            MPI_DOUBLE_PRECISION, // MPI_Datatype datatype
                            MPI_SUM, // MPI_Op op
                            MPI_COMM_WORLD); // MPI_Comm comm  
      checkError(error, "Error sharing (y) between processes - Solver::calculateResiduals");
      
      // In serial case, xStar must be updated after each phase calculation       
      // Update xStar in the right possitions
      // Browsing codes that had been running in phase
      for(int iPhaseCode = 0; iPhaseCode<sys->nCodesInPhase[iPhase]; iPhaseCode++){
        int iCode = sys->codeToConnectInPhase[iPhase][iPhaseCode];
        // Browsing calculations
        for(int jCode=0; jCode<sys->nCodes; jCode++){
          math->copyInVector(xStar, sys->code[iCode].calculationPossitions[jCode], // vector to save the copy, possition of first element
                             y, sys->code[iCode].calculationPossitions[jCode], // vector to copy, possition of first element
                             sys->code[iCode].nCalculations2Code[jCode]); // number of elements to copy
        }
      }
      checkError(error, "x* has to be updated after phase calculation - Solver::calculateResiduals");
      
      // Zeroing y to allow new mpi_Allreduce in next phase
      math->zeros(y, sys->nUnk);      
    }
    
    // Compute residuals as (xStar - x)
    resVector = math->differenceInVectors(xStar, x, sys->nUnk);
    
  }
  // Other methods only update data after all calculations
  else{
    // Send variables to all clients that connect by MPI
    for(int iCode = 0; iCode<sys->nCodes; iCode++){
      if(sys->code[iCode].connection==NEWTON_MPI_COMMUNICATION){
        if(irank==0){ // only root works
          error = sendDataToCode(iCode);
        }
        // All processes check
        checkError(irank, "Error sending data to code  - Solver::calculateResiduals");
      }
    }
    // Run codes by script
    freeRank = 0;
    for(int iCode = 0; iCode<sys->nCodes; iCode++){
      if(sys->code[iCode].connection==NEWTON_SPAWN){
        if(irank==freeRank){
          error = runCode(iCode, sys, map);
        }
        freeRank++;
        // When all processes run particular codes, 
        // check errors and give them more work
        if(freeRank==world_size){
          checkError(error, "Error running code  - Solver::calculateResiduals");
          freeRank = 0;
        }
      }
    }
    // Other communication types?
    
    
    // Read output from codes that run by script
    freeRank = 0;
    for(int iCode = 0; iCode<sys->nCodes; iCode++){
      if(sys->code[iCode].connection==NEWTON_SPAWN){
        if(irank==freeRank){
          error = readOutputFromCode(iCode, sys, map);
        }
        freeRank++;
        // When all processes run particular codes, 
        // check errors and give them more work
        if(freeRank==world_size){
          checkError(error, "Error reading output from code  - Solver::calculateResiduals");
          freeRank = 0;
        }
      }
    }
    // Receive variables from all clients that connect by MPI
    for(int iCode = 0; iCode<sys->nCodes; iCode++){
      if(sys->code[iCode].connection==NEWTON_MPI_COMMUNICATION){
        if(irank==0){ // only root works
          error = receiveDataFromCode(iCode);
        }
        // All processes check
        checkError(irank, "Error receiving data from code  - Solver::calculateResiduals");
      }
    }
    
    // Update y in every process
    error = MPI_Allreduce(MPI_IN_PLACE, // const void *sendbuf
                          y, // void *recvbuf
                          sys->nUnk, // int count: number of elements in send buffer (integer) 
                          MPI_DOUBLE_PRECISION, // MPI_Datatype datatype
                          MPI_SUM, // MPI_Op op
                          MPI_COMM_WORLD); // MPI_Comm comm  
    checkError(error, "Error sharing y between processes  - Solver::calculateResiduals");
    // Compute residuals as (y - x)
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
      checkError(error, "Non linear method has not been implemented yet - Solver::calculateNewGuess");
  }
}

/* Solver::runCode

Spawn specific code with some input data.
This input could be a function of guesses values in x,
like cross sections as a function of termalhydraulic variables.
This funtion is executed by only one process, so errors are returned
in order to be checked by synchronized functions.

input: code ID, system object and mapper object
output: error

*/
int Solver::runCode(int iCode, System* sys, Mapper* map)
{  
  // Extract x values of iCode's interest in sys->xValuesextractToMap
  error = sys->extractToMap(iCode, x);
  if(error!=NEWTON_SUCCESS){
    cout<<"Error extracting x values of interest to code ID: "<<sys->code[iCode].id<<" - Solver::runCode"<<endl;
    return error;
  }
  
  // Map sys->xValuesextractToMap values to sys->xValuesToSend before send
  error = map->map(sys, iCode, NEWTON_PRE_SEND);
  if(error!=NEWTON_SUCCESS){
    cout<<"Error mapping x values of interest to code ID: "<<sys->code[iCode].id<<" - Solver::runCode"<<endl;
    return error;
  }
  
  // Prepare input
  error = NewtonClient->prepareInput(iCode, sys);  
  if(error!=NEWTON_SUCCESS){
    cout<<"Error preparing input to code ID: "<<sys->code[iCode].id<<" - Solver::runCode"<<endl;
    return error;
  }
  
  // Spawn client code
  error = spawnCode(iCode, sys);
  if(error!=NEWTON_SUCCESS){
    cout<<"Error spawning code ID: "<<sys->code[iCode].id<<" - Solver::runCode"<<endl;
    return error;
  }  
  
  error = NEWTON_SUCCESS;
  return error;
}

/* Solver::spawnCode

Spawn n processes of a particular code.

input: -
output: error

*/
int Solver::spawnCode(int iCode, System* sys)
{
  if(sys->code[iCode].nProcs==1){
    // system
    error = system((sys->code[iCode].commandToRun).c_str());
    
  }
  else if(sys->code[iCode].nProcs>1){
    // MPI_Spawn
  
  }
  else{
    cout<<"Wrong number of processes to spawn in code ID: "<<sys->code[iCode].id<<" - Solver::spawnCode"<<endl;
    error = NEWTON_ERROR;
    return error;
  }  
  if(error!=NEWTON_SUCCESS){
    cout<<"Error trying to spawn process from code ID: "<<sys->code[iCode].id<<" - Solver::spawnCode"<<endl;
    return error;
  }
  
  // Need to move output?
  if(sys->code[iCode].outputPath!="."){
    error = system(("mv "+sys->code[iCode].outputPath+sys->code[iCode].actualOutput+" .").c_str());
    if(error!=NEWTON_SUCCESS){
      cout<<"Error moving output from code ID: "<<sys->code[iCode].id<<" - Solver::spawnCode"<<endl;
      return error;
    }
  }
  
  // Need to move restart?
  if(sys->code[iCode].restartPath!="."){
    error = system(("mv "+sys->code[iCode].restartPath+sys->code[iCode].actualRestart+" .").c_str());
    if(error!=NEWTON_SUCCESS){
      cout<<"Error moving restart from code ID: "<<sys->code[iCode].id<<" - Solver::spawnCode"<<endl;
      return error;
    }
  }
  
  error = NEWTON_SUCCESS;
  return error;
}

/* Solver::readOutputFromCode

Load variables from specific ouput file.
Map them before save in x.

input: code ID, System pointer, Mapper pointer
output: error

*/
int Solver::readOutputFromCode(int iCode, System* sys, Mapper* map)
{
  // Read data from output in Sys->code[iCode].yValuesReceived
  error = NewtonClient->readOutput(iCode, sys);
  if(error!=NEWTON_SUCCESS){
    cout<<"Error reading output from code ID: "<<sys->code[iCode].id<<" - Solver::readOutputFromCode"<<endl;
    return error;
  }
      
  // Map data before save it in Sys->code[iCode].yValuesMapped
  error = map->map(sys, iCode, NEWTON_POST_RECV);
  if(error!=NEWTON_SUCCESS){
    cout<<"Error mapping values received from code ID: "<<sys->code[iCode].id<<" - Solver::readOutputFromCode"<<endl;
    return error;
  }
  
  // Save in appropiate possitions in y
  error = sys->setMappedOnY(iCode, y);

  
  error = NEWTON_SUCCESS;
  return error;
}

/* Solver::sendDataToCode

Send data to a client via MPI functions.
This data could be a function of guesses values in x,
like cross sections as a function of termalhydraulic variables.

input: code ID
output: error

*/
int Solver::sendDataToCode(int iCode)
{
  // Extract x values of iCode's interest
  
  
  // Map x values before send
  
  
  // Send data 
  
  
  error = NEWTON_SUCCESS;  
  return NEWTON_SUCCESS;
}

/* Solver::receiveDataFromCode

Receive data from a client via MPI functions.
Map them before save in x.

input: code ID
output: error

*/
int Solver::receiveDataFromCode(int iCode)
{
  // Receive data from client
  
  
  // Map data before save
  
  
  // Save in appropiate possitions in y
  
  
  
  error = NEWTON_SUCCESS;  
  return error;
}
