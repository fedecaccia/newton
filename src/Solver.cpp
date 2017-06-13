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
  // X previous iteration
  xItPrev = new double[sys->nUnk];
  // X saved to construct Jacobian
  xBackUp = new double[sys->nUnk];
  // Residual vector
  resVector = new double[sys->nUnk];
  // Residual vector back up
  resVectorBackUp = new double[sys->nUnk];
  // Jacobian matrix
  J = new double*[sys->nUnk];
  // Jacobian matrix previous iteration
  Jk = new double*[sys->nUnk];  
  
  // Initialization
  math->zeros(x, sys->nUnk);
  math->zeros(xItPrev, sys->nUnk);
  math->zeros(resVector, sys->nUnk);
  for(int iUnk=0; iUnk<sys->nUnk; iUnk++){
    J[iUnk] = new double[sys->nUnk];
    Jk[iUnk] = new double[sys->nUnk];
    math->zeros(J[iUnk], sys->nUnk);
    math->zeros(Jk[iUnk], sys->nUnk);  
  }
  
  // PETSc elements
  MatCreate(PETSC_COMM_WORLD,&A);
  MatSetSizes(A,PETSC_DECIDE,PETSC_DECIDE,sys->nUnk*sys->nUnk,sys->nUnk*sys->nUnk);
  MatSetFromOptions(A);
  MatSetUp(A);
  MatAssemblyBegin(A,MAT_FINAL_ASSEMBLY);
  MatAssemblyEnd(A,MAT_FINAL_ASSEMBLY);
  
  VecCreate(PETSC_COMM_WORLD,&b);
  VecSetSizes(b,PETSC_DECIDE,sys->nUnk*sys->nUnk);
  VecSetFromOptions(b);
  
  VecCreate(PETSC_COMM_WORLD,&u);
  VecSetSizes(u,PETSC_DECIDE,sys->nUnk*sys->nUnk);
  VecSetFromOptions(u);
  
  KSPCreate(PETSC_COMM_WORLD,&ksp);
  
  
}


/* Solver:: setFirstGuess

This function sets the intial guess. In first evolution step it corresponds
to the initial condition loaded from configuration file, or, in case that
it couldn't be supported, every guess value is zero. In following steps
it is calculated interpolating previous values.

input: System pointer, step value
output: -

*/
void Solver::setFirstGuess(System* sys, int step)
{
  if(step!=0){
    // Interpolate previous values? Do nothing to keep x values    
  }
  else{
    // Initial condition has been loaded by parser    
  }
  
  // Put x values in xItPrev
  math->copyInVector(xItPrev, 0, x, 0, sys->nUnk);
  
  // Put x values in gamma and in delta
  x2gamma2delta(sys);
    
  // TEST
  //~ cout<<"x: "<<endl;
  //~ for(int iX=0; iX<sys->nUnk; iX++){
    //~ cout<<"x("<<iX<<") = "<<x[iX]<<endl;    
  //~ }
  //~ cout<<"Gammas: "<<endl;
  //~ for(int iGamma=0; iGamma<sys->nGamma; iGamma++){
    //~ cout<<"g("<<iGamma<<") = "<<sys->gamma[iGamma]<<endl;
  //~ }  
  //~ cout<<"Deltas: "<<endl;
  //~ for(int iCode=0; iCode<sys->nCodes; iCode++){
    //~ cout<<"Code: "<<iCode<<endl;
    //~ for(int iDelta=0; iDelta<sys->code[iCode].nDelta; iDelta++){
      //~ cout<<"d("<<iDelta<<") = "<<sys->code[iCode].delta[iDelta]<<endl;
    //~ }
  //~ }
  
}

/*





*/
void Solver::x2gamma2delta(System* sys)
{
  // Put x values in gamma in order to send to codes
  sys->x2gamma(x);
  
  // Map gamma values into deltas for each code
  for(int iCode=0; iCode<sys->nCodes; iCode++){
    sys->gamma2delta(iCode);
  }  
}


/* Solver iterateUntilConverge
This function controls the nonlinear iterations solving the residual equations.

input: -
output: -

*/
void Solver::iterateUntilConverge(System* sys, Communicator* comm)
{
  calculateResiduals(sys, comm);
  rootPrints(" First guess: \t\t\t Residual: "+dou2str(residual));
  while(residual > nltol && iter < maxIter){
    
    calculateNewGuess(sys, comm);
    calculateResiduals(sys, comm);
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
void Solver::calculateResiduals(System* sys, Communicator* comm)
{
  // TEST
  //~ cout<<"ALPHAS, BETAS, GAMMAS & DELTAS pre calculating residuals"<<endl;
  //~ for (int iCode=0; iCode<sys->nCodes; iCode++){
    //~ cout<<"Code: "<<iCode<<endl;
    //~ cout<<"Alphas:"<<endl;
    //~ for(int iAlpha = 0; iAlpha<sys->code[iCode].nAlpha; iAlpha++){
      //~ cout<<sys->code[iCode].alpha[iAlpha]<<endl;
    //~ }
    //~ cout<<"Betas:"<<endl;
    //~ for(int iBeta = 0; iBeta<sys->code[iCode].nBeta; iBeta++){
      //~ cout<<sys->beta[sys->code[iCode].betaFirstValuePos+iBeta]<<endl;
    //~ }
    //~ cout<<"Gammas:"<<endl;
    //~ for(int iGamma= 0; iGamma<sys->code[iCode].nGamma; iGamma++){
      //~ cout<<sys->gamma[sys->code[iCode].gammaFirstValuePos+iGamma]<<endl;
    //~ }
    //~ cout<<"Deltas:"<<endl;
    //~ for(int iDelta = 0; iDelta<sys->code[iCode].nDelta; iDelta++){
      //~ cout<<sys->code[iCode].delta[iDelta]<<endl;
    //~ }
  //~ }
  
  // Zeroing vector that receives calculation values
  for(int iCode=0; iCode<sys->nCodes; iCode++){    
    math->zeros(sys->code[iCode].alpha, sys->code[iCode].nAlpha);
  }
      
  /* Each iteration is composed by phases.
   * After each phase, Beta is updated with previous calculations.
   */    
  for(int iPhase=0; iPhase<sys->nPhasesPerIter; iPhase++){
    
    // Update gamma and beta in each phase>0 in EXPLICIT_SERIAL
    if(method==EXPLICIT_SERIAL && iPhase!=0){
      sys->beta2gamma();
      for(int iPhaseCode = 0; iPhaseCode<sys->nCodesInPhase[iPhase]; iPhaseCode++){
        codeConnected = sys->findCodeInPhase(iPhase, iPhaseCode);
        // Some gammas has zero values, (alpha->beta->gamma propagated)
        // Calling mappers with zero values could return innecesary errors
        sys->gamma2delta(codeConnected);
      }
    }
  
    // Send variables to all clients that connect by MPI in this phase
    for(int iPhaseCode = 0; iPhaseCode<sys->nCodesInPhase[iPhase]; iPhaseCode++){      
      codeConnected = sys->findCodeInPhase(iPhase, iPhaseCode);
      
      if(sys->code[codeConnected].connection==NEWTON_MPI_COMMUNICATION){
        if(irank==0){ // only root works
          error = sendDataToCode(codeConnected);
        }
        // All processes check
        checkError(irank, "Error sending data to code - Solver::calculateResiduals");
      }
    }
    // Run codes by script in this phase
    freeRank = 0;
    for(int iPhaseCode = 0; iPhaseCode<sys->nCodesInPhase[iPhase]; iPhaseCode++){
      codeConnected = sys->findCodeInPhase(iPhase, iPhaseCode);
      
      if(sys->code[codeConnected].connection==NEWTON_SPAWN){
        if(irank==freeRank){
          error = runCode(codeConnected, sys);
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
      codeConnected = sys->findCodeInPhase(iPhase, iPhaseCode);
      
      if(sys->code[codeConnected].connection==NEWTON_SPAWN){
        if(irank==freeRank){
          error = readOutputFromCode(codeConnected, sys);
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
      codeConnected = sys->findCodeInPhase(iPhase, iPhaseCode);

      if(sys->code[codeConnected].connection==NEWTON_MPI_COMMUNICATION){
        if(irank==0){ // only root works
          error = receiveDataFromCode(codeConnected);
        }
        // All processes check
        checkError(irank, "Error receiving data from code - Solver::calculateResiduals");
      }
    }
    

    for(int iPhaseCode = 0; iPhaseCode<sys->nCodesInPhase[iPhase]; iPhaseCode++){
      codeConnected = sys->findCodeInPhase(iPhase, iPhaseCode);
      // Update alphas in every process
      error = MPI_Allreduce(MPI_IN_PLACE, // const void *sendbuf
                            sys->code[codeConnected].alpha, // void *recvbuf
                            sys->code[codeConnected].nAlpha, // int count: number of elements in send buffer (integer) 
                            MPI_DOUBLE_PRECISION, // MPI_Datatype datatype
                            MPI_SUM, // MPI_Op op
                            MPI_COMM_WORLD); // MPI_Comm comm  
      checkError(error, "Error sharing alpha between processes - Solver::calculateResiduals");
      
      sys->alpha2beta(codeConnected);
      
    }
    
  //~ // TEST
  //~ cout<<"ALPHAS, BETAS, GAMMAS & DELTAS"<<endl;
  //~ for (int iCode=0; iCode<sys->nCodes; iCode++){
    //~ cout<<"Code: "<<iCode<<endl;
    //~ cout<<"Alphas:"<<endl;
    //~ for(int iAlpha = 0; iAlpha<sys->code[iCode].nAlpha; iAlpha++){
      //~ cout<<sys->code[iCode].alpha[iAlpha]<<endl;
    //~ }
    //~ cout<<"Betas:"<<endl;
    //~ for(int iBeta = 0; iBeta<sys->code[iCode].nBeta; iBeta++){
      //~ cout<<sys->beta[sys->code[iCode].betaFirstValuePos+iBeta]<<endl;
    //~ }
    //~ cout<<"Gammas:"<<endl;
    //~ for(int iGamma= 0; iGamma<sys->code[iCode].nGamma; iGamma++){
      //~ cout<<sys->gamma[sys->code[iCode].gammaFirstValuePos+iGamma]<<endl;
    //~ }
    //~ cout<<"Deltas:"<<endl;
    //~ for(int iDelta = 0; iDelta<sys->code[iCode].nDelta; iDelta++){
      //~ cout<<sys->code[iCode].delta[iDelta]<<endl;
    //~ }
  //~ }
  //exit(1);
  
    
  }
  
  //~ // TEST
  //~ cout<<sys->nPhasesPerIter<<endl;
  //~ cout<<"ALPHAS, BETAS, GAMMAS & DELTAS post calculating residuals"<<endl;
  //~ for (int iCode=0; iCode<sys->nCodes; iCode++){
    //~ cout<<"Code: "<<iCode<<endl;
    //~ cout<<"Alphas:"<<endl;
    //~ for(int iAlpha = 0; iAlpha<sys->code[iCode].nAlpha; iAlpha++){
      //~ cout<<sys->code[iCode].alpha[iAlpha]<<endl;
    //~ }
    //~ cout<<"Betas:"<<endl;
    //~ for(int iBeta = 0; iBeta<sys->code[iCode].nBeta; iBeta++){
      //~ cout<<sys->beta[sys->code[iCode].betaFirstValuePos+iBeta]<<endl;
    //~ }
    //~ cout<<"Gammas:"<<endl;
    //~ for(int iGamma= 0; iGamma<sys->code[iCode].nGamma; iGamma++){
      //~ cout<<sys->gamma[sys->code[iCode].gammaFirstValuePos+iGamma]<<endl;
    //~ }
    //~ cout<<"Deltas:"<<endl;
    //~ for(int iDelta = 0; iDelta<sys->code[iCode].nDelta; iDelta++){
      //~ cout<<sys->code[iCode].delta[iDelta]<<endl;
    //~ }
  //~ }
  
  // Compute residuals as (beta - x)
  sys->computeResiduals(resVector, x);
  
  // TEST
  cout<<"Beta"<<setw(20)<<"x"<<setw(20)<<"res"<<endl;
  for(int iUnk=0; iUnk<sys->nUnk; iUnk++){
    cout<<sys->beta[iUnk]<<setw(20)<<x[iUnk]<<setw(20)<<resVector[iUnk]<<endl;    
  }
  
  // Calculate norm 2 of the residual vector
  residual = math->moduleAbs(resVector, sys->nUnk);  
}

/* Solver calculateNewGuess

Calculate new guess by different methods.

input: -
output: -

*/
void Solver::calculateNewGuess(System* sys, Communicator* comm)
{
  switch(method){
    case EXPLICIT_SERIAL:
      // New x as the calculated values - they are going to be updated in phases
      sys->beta2x(x);
      break;
      
    case EXPLICIT_PARALLEL:
      // New x as the calculated values
      sys->beta2x(x);      
      break;   
      
    case NEWTON:
      jacobianConstruction(sys, comm);
      solveLinearSystem(sys);
      break;   
      
    case SECANT:
      break;   
      
    case BROYDEN:
      break;   
      
    default:
      error = NEWTON_ERROR;
      checkError(error, "Non linear method has not been implemented yet - Solver::calculateNewGuess");
  }
  
  // Put x values in gamma and delta
  x2gamma2delta(sys);
}

/* Solver::jacobianConstruction





*/
void Solver::jacobianConstruction(System* sys, Communicator* comm)
{
  // Back up
  math->copyInVector(xBackUp, 0, x, 0, sys->nUnk);
  math->copyInVector(resVectorBackUp, 0, resVector, 0, sys->nUnk);
  
  // f(x) was calculated in calculateResiduals() previously
  
  // Loop for partial derivative calculation
  rootPrints("  Calculating Jacobian by finite difference...");
  for(int i=0; i<sys->nRes; i++){
    //rootPrints(" Jacobian Row: "+int2str(i+1)); // ORDENAR POR RESIDUOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOS
    math->sumDeltaInPosition(x, i, dxJacCalc, sys->nUnk);
    
    x2gamma2delta(sys);
    calculateResiduals(sys, comm);
    for(int j=0; j<sys->nUnk; j++){
      J[i][j] = (resVector[j]-resVectorBackUp[j]) / dxJacCalc;
    }

    // Restore x
    math->copyInVector(x, 0, xBackUp, 0, sys->nUnk);
  }
  
  if(irank==0){cout<<"  End Jacobian calculation"<<endl;}
  
  //~ // TEST
  //~ if(irank==0){
    //~ for (int i=0; i<sys->nUnk; i++){
      //~ for (int j=0; j<sys->nUnk; j++){
        //~ cout<<J[i][j]<<" ";
      //~ }
      //~ cout<<endl;
    //~ }
  //~ }
  //~ exit(1);
  
  // Restore resVector
  math->copyInVector(resVector, 0, resVectorBackUp, 0, sys->nUnk);  
  
}

/* Solver::solveLinearSystem





*/
void Solver::solveLinearSystem(System* sys)
{
  // Save resVector in b
  for (int i=0; i<sys->nUnk; i++) {
    VecSetValues(b,1,&i,&resVector[i],INSERT_VALUES);
    //cout<<"b"<<resVector[i]<<endl;
  }
  
  VecAssemblyBegin(b);
  VecAssemblyEnd(b);
  
  // Save -J in A
  for(int i=0; i<sys->nUnk; i++){
    for(int j=0; j<sys->nUnk; j++){
      MatSetValue(A,i,j,-J[i][j],INSERT_VALUES);
    }
  } 
  
  MatAssemblyBegin(A,MAT_FINAL_ASSEMBLY);
  MatAssemblyEnd(A,MAT_FINAL_ASSEMBLY);  
  // Solve
  KSPSetOperators(ksp,A,A);
  KSPGetPC(ksp,&pc);
  PCSetType(pc,PCNONE);
  KSPSolve(ksp,b,u);
  
  // Get x solution from u
  //math->zeros(x, sys->nUnk);  
  VecGetArray(u,&x);
  x = math->sumVectors(x, xItPrev, sys->nUnk);
  //~ cout<<"x:"<<endl;
  //~ for(int iUnk=0; iUnk<sys->nUnk; iUnk++){
    //~ cout<<x[iUnk]<<endl;
  //~ }
  //~ exit(1);
  
  
}



/* Solver::runCode

Spawn specific code with some input data.
This input could be a function of guesses values in x,
like cross sections as a function of termalhydraulic variables.
This funtion is executed by only one process, so errors are returned
in order to be checked by synchronized functions.

input: code and system object
output: error

*/
int Solver::runCode(int iCode, System* sys)
{   
  // Prepare input
  error = NewtonClient->prepareInput(sys->code[iCode].type, 
                                     sys->code[iCode].name, 
                                     sys->code[iCode].nDelta, 
                                     sys->code[iCode].delta, 
                                     sys->code[iCode].actualInput);
  if(error!=NEWTON_SUCCESS){
    cout<<"Error preparing input to code: "<<sys->code[iCode].name<<" - Solver::runCode"<<endl;
    return error;
  }
  
  // Spawn client code
  error = spawnCode(iCode, sys);
  if(error!=NEWTON_SUCCESS){
    cout<<"Error spawning code: "<<sys->code[iCode].name<<" - Solver::runCode"<<endl;
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
    cout<<"Wrong number of processes to spawn in code: "<<sys->code[iCode].name<<" - Solver::spawnCode"<<endl;
    error = NEWTON_ERROR;
    return error;
  }  
  if(error!=NEWTON_SUCCESS){
    cout<<"Error trying to spawn process from code: "<<sys->code[iCode].name<<" - Solver::spawnCode"<<endl;
    return error;
  }
  
  // Need to move output?
  if(sys->code[iCode].outputPath!="." && sys->code[iCode].outputPath!=""){
    error = system(("mv "+sys->code[iCode].outputPath+sys->code[iCode].actualOutput+" .").c_str());
    if(error!=NEWTON_SUCCESS){
      cout<<"Error moving output from code: "<<sys->code[iCode].name<<" - Solver::spawnCode"<<endl;
      return error;
    }
  }
  
  // Need to move restart?
  if(sys->code[iCode].restartPath!="." && sys->code[iCode].restartPath!=""){
    error = system(("mv "+sys->code[iCode].restartPath+sys->code[iCode].actualRestart+" .").c_str());
    if(error!=NEWTON_SUCCESS){
      cout<<"Error moving restart from code: "<<sys->code[iCode].name<<" - Solver::spawnCode"<<endl;
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
int Solver::readOutputFromCode(int iCode, System* sys)
{
  // Read output
  error = NewtonClient->readOutput(sys->code[iCode].type, 
                                   sys->code[iCode].name, 
                                   sys->code[iCode].nAlpha, 
                                   sys->code[iCode].alpha, 
                                   sys->code[iCode].actualOutput);
  if(error!=NEWTON_SUCCESS){
    cout<<"Error reading output from code: "<<sys->code[iCode].name<<" - Solver::readOutputFromCode"<<endl;
    return error;
  }
  
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
  // Save in appropiate possitions in y
  
  
  
  error = NEWTON_SUCCESS;  
  return error;
}
