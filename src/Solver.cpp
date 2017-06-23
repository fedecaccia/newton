/*****************************************************************************\

NEWTON                |
                      |
Multiphysics          | CLASS
coupling              | SOLVER
maste code            |
                      |

-------------------------------------------------------------------------------

Solver computes the residual values and solves the nonlinear system with 
different kind of methods.

Date: 14 June 2017

-------------------------------------------------------------------------------

Copyright (C) 2017 Federico A. Caccia

This file is part of Newton.

Newton is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

Newton is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with Newton.  If not, see <http://www.gnu.org/licenses/>.

\*****************************************************************************/

#include "Solver.h"

using namespace::std;

/* Evolution constructor
*/
Solver::Solver()
{
  // Default values
  
  // Nonlinear method
  method = NO_METHOD;
  // Nonlinear tolerance
  nltol = 1e-07;
  // Nonlinear iterations
  iter = 0;
  // Maximum amount of nonlinear iterations allowed in each evolution step
  maxIter = 10;
  // Amount of steps to save solution
  nXStPrev = 2;
  // Amount of steps to save jacobians
  nJStPrev = 2;
  // Delta x in Jacobian calculation by finitte difference method
  dxJacCalc = 0.1;
  // Steps between Jacobian calculation by finite difference method
  sJacCalc = 0;    
  // Iterations between Jacobian calculation by finite difference method
  iJacCalc = 0;
  // Math object
  math = new MathLib();
  // client code object
  NewtonClient = new Client();
  // Total funtion evaluations in step
  nEvalInStep = 0;
  // Total function evaluation in problem
  nEval = 0;

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
  // X previous steps
  xStPrev = new double*[nXStPrev];
  for(int i=0; i<nXStPrev; i++){
    xStPrev[i] = new double[sys->nUnk];
  }
  // x - sItPrev
  deltaX = new double[sys->nUnk];
  // X saved to construct Jacobian
  xBackUp = new double[sys->nUnk];
  // Residual vector
  resVector = new double[sys->nUnk];
  // Residual vector previous iteration
  resVectorItPrev = new double[sys->nUnk];
  // Residual vector back up
  resVectorBackUp = new double[sys->nUnk];
  // Jacobian matrix
  J = new double*[sys->nUnk];
  // Jacobian matrix previous iteration
  JItPrev = new double*[sys->nUnk];  
  // Jacobian matrix previous steps
  JStPrev = new double**[nJStPrev];
  for(int i=0; i<nJStPrev; i++){
    JStPrev[i] = new double*[sys->nUnk];  
  }
  
  // Initialization
  math->zeros(x, sys->nUnk);
  math->zeros(xItPrev, sys->nUnk);
  for(int i=0; i<nXStPrev; i++){
    math->zeros(xStPrev[i], sys->nUnk);
  }
  math->zeros(deltaX, sys->nUnk);
  math->zeros(resVector, sys->nUnk);
  math->zeros(resVectorItPrev, sys->nUnk);
  for(int iUnk=0; iUnk<sys->nUnk; iUnk++){
    J[iUnk] = new double[sys->nUnk];
    JItPrev[iUnk] = new double[sys->nUnk];    
  }
  for(int i=0; i<nJStPrev; i++){
    for(int iUnk=0; iUnk<sys->nUnk; iUnk++){
      JStPrev[i][iUnk] = new double[sys->nUnk];
    }
  }
  math->identity(J, sys->nUnk);
  math->identity(JItPrev, sys->nUnk);
  for(int i=0; i<nJStPrev; i++){
    math->identity(JStPrev[i], sys->nUnk);
  }
  
  //~ for(int i=0; i<sys->nUnk; i++){
    //~ for(int j=0; j<sys->nUnk; j++){
      //~ cout<<J[i][j]<<" ";
    //~ }
    //~ cout<<endl;
  //~ }
  //~ exit(1);
  
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
it wasn't supported, every guess value is zero. In following steps
it is calculated extrapolating previous values.
It also set guesses for the Jacobian matrix.

input: System pointer, step value
output: -

*/
void Solver::setFirstGuess(System* sys, int step)
{
  if(step>=nXStPrev){
    // Extrapolate previous values? Do nothing to keep last x solution
    extrapolateX(sys, step);
  }
  if(step>=nJStPrev){
    // Extrapolate previous values? Do nothing to keep J
    extrapolateJ(sys,step);
  }
  else{
    // X_CI has been loaded by parser    
    // J CI? not yet
  }

  // Put x values in xItPrev
  math->copyInVector(xItPrev, 0, x, 0, sys->nUnk);
  // Put J values in JItPrev
  math->copyMat(JItPrev, J, sys->nUnk);
   
  // Put x values in gamma and in delta
  x2gamma2delta(sys);
    
  // TEST
  /*
  cout<<"x: "<<endl;
  for(int iX=0; iX<sys->nUnk; iX++){
    cout<<"x("<<iX<<") = "<<x[iX]<<endl;    
  }
  cout<<"Gammas: "<<endl;
  for(int iGamma=0; iGamma<sys->nGamma; iGamma++){
    cout<<"g("<<iGamma<<") = "<<sys->gamma[iGamma]<<endl;
  }  
  cout<<"Deltas: "<<endl;
  for(int iCode=0; iCode<sys->nCodes; iCode++){
    cout<<"Code: "<<iCode<<endl;
    for(int iDelta=0; iDelta<sys->code[iCode].nDelta; iDelta++){
      cout<<"d("<<iDelta<<") = "<<sys->code[iCode].delta[iDelta]<<endl;
    }
  }
  exit(1);*/
  
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
void Solver::iterateUntilConverge(System* sys, Communicator* comm, int step)
{
  // New step
  iter = 0;
  nEvalInStep = 0;
  calculateResiduals(sys, comm);
  rootPrints(" First guess: \t\t\t Residual: "+dou2str(residual));
 
  while(residual > nltol && iter < maxIter){
    
    // Send NEWTON_RESTART order to clients connected
    order = NEWTON_RESTART;    
    comm->sendOrder(order);
        
    calculateNewGuess(sys, comm, step);
    calculateResiduals(sys, comm);
    iter++;
    
    rootPrints(" Nonlinear iteration: "+int2str(iter)+"   Residual: "+dou2str(residual));
    debug.log("step: "+int2str(step), ITER_LOG);
    debug.log("iter: "+int2str(iter), ITER_LOG);
    debug.log("f_evals: "+int2str(nEvalInStep), ITER_LOG);
    debug.log("res: "+dou2str(residual)+"\n", ITER_LOG);
  }

  rootPrints(" Total iterations in step: "+int2str(iter)+" - Total funtion evaluations: "+int2str(nEvalInStep));
  debug.log("step: "+int2str(step), GLOBAL_LOG);
  debug.log("iters: "+int2str(iter), GLOBAL_LOG);
  debug.log("f_evals: "+int2str(nEvalInStep)+"\n", GLOBAL_LOG);
  
  // Bad ending
  if(residual>nltol){
    error = NEWTON_ERROR;
    checkError(error, "Maximum nonlinear iterations reached - Solver-iterateUntilConverge");      
  }

  // Save solution and jacobian
  saveX(sys, step);
  saveJ(sys, step);  
}

  
/* Solver calculateResiduals

Calls particular solvers and compute the difference between guesses and calculations.

input: -
output: -

*/
void Solver::calculateResiduals(System* sys, Communicator* comm)
{  
  // New function evaluation
  nEval++;
  nEvalInStep++;
  
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
      
      if(sys->code[codeConnected].connection!=NEWTON_SPAWN){
        error = sendDataToCode(codeConnected, sys, comm);
        // All processes check
        checkError(error, "Error sending data to code - Solver::calculateResiduals");
      }
    }
    // Run codes by script in this phase
    freeRank = 0;
    for(int iPhaseCode = 0; iPhaseCode<sys->nCodesInPhase[iPhase]; iPhaseCode++){
      codeConnected = sys->findCodeInPhase(iPhase, iPhaseCode);
      if(sys->code[codeConnected].connection==NEWTON_SPAWN){
        if(irank==freeRank){
          error += runCode(codeConnected, sys);
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
    // Check errors
    checkError(error, "Error running code - Solver::calculateResiduals");
    
    
    // Other communication types?
    
    
    
    // Read output from codes that run by script in this phase
    freeRank = 0;
    for(int iPhaseCode = 0; iPhaseCode<sys->nCodesInPhase[iPhase]; iPhaseCode++){
      codeConnected = sys->findCodeInPhase(iPhase, iPhaseCode);
      
      if(sys->code[codeConnected].connection==NEWTON_SPAWN){
        if(irank==freeRank){
          error += readOutputFromCode(codeConnected, sys);
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
    // Check errors
    checkError(error, "Error reading output from code - Solver::calculateResiduals");
    
    // Receive variables from all clients  in this phase that connect by MPI
    for(int iPhaseCode = 0; iPhaseCode<sys->nCodesInPhase[iPhase]; iPhaseCode++){
      codeConnected = sys->findCodeInPhase(iPhase, iPhaseCode);

      if(sys->code[codeConnected].connection!=NEWTON_SPAWN){
        error = receiveDataFromCode(codeConnected, sys, comm);
        // All processes check
        checkError(error, "Error receiving data from code - Solver::calculateResiduals");
      }
    }
    
    
    // Other communication types?
    
    
    
    

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
  }  
  
  // Save previous value of residual
  math->copyInVector(resVectorItPrev, 0, resVector, 0, sys->nUnk); 
  // Compute residuals as (beta - x)
  sys->computeResiduals(resVector, x);
  
  // Calculate norm 2 of the residual vector
  residual = math->moduleAbs(resVector, sys->nUnk);
  
  /*
   // TEST
   cout<<sys->nPhasesPerIter<<endl;
   cout<<"ALPHAS, BETAS, GAMMAS & DELTAS post calculating residuals"<<endl;
   for (int iCode=0; iCode<sys->nCodes; iCode++){
     cout<<"Code: "<<iCode<<endl;
     cout<<"Alphas:"<<endl;
     for(int iAlpha = 0; iAlpha<sys->code[iCode].nAlpha; iAlpha++){
      cout<<sys->code[iCode].alpha[iAlpha]<<endl;
     }
     cout<<"Betas:"<<endl;
     for(int iBeta = 0; iBeta<sys->code[iCode].nBeta; iBeta++){
       cout<<sys->beta[sys->code[iCode].betaFirstValuePos+iBeta]<<endl;
     }
    cout<<"Gammas:"<<endl;
     for(int iGamma= 0; iGamma<sys->code[iCode].nGamma; iGamma++){
       cout<<sys->gamma[sys->code[iCode].gammaFirstValuePos+iGamma]<<endl;
     }
     cout<<"Deltas:"<<endl;
    for(int iDelta = 0; iDelta<sys->code[iCode].nDelta; iDelta++){
       cout<<sys->code[iCode].delta[iDelta]<<endl;
     }
  }
  */
   /*
  //TEST
  cout<<"Beta"<<setw(20)<<"x"<<setw(20)<<"res"<<endl;
  for(int iUnk=0; iUnk<sys->nUnk; iUnk++){
    cout<<sys->beta[iUnk]<<setw(20)<<x[iUnk]<<setw(20)<<resVector[iUnk]<<endl; 
  }*/

  debug.log("iter: "+int2str(iter)+"\n", UNK_LOG, 0);
  for (int iCode=0; iCode<sys->nCodes; iCode++){
    debug.log("code: "+sys->code[iCode].name+"\n", UNK_LOG, 0);
    debug.log("alphas:\n", UNK_LOG, 0);
    for(int iAlpha = 0; iAlpha<sys->code[iCode].nAlpha; iAlpha++){
      debug.log(dou2str(sys->code[iCode].alpha[iAlpha]), UNK_LOG, 10);
    }
    debug.log("\n", UNK_LOG);
    debug.log("betas:\n", UNK_LOG, 0);
    for(int iBeta = 0; iBeta<sys->code[iCode].nBeta; iBeta++){
      debug.log(dou2str(sys->beta[sys->code[iCode].betaFirstValuePos+iBeta]), UNK_LOG, 10);
    }
    debug.log("\n", UNK_LOG);
    debug.log("gammas:\n", UNK_LOG, 0);
    for(int iGamma= 0; iGamma<sys->code[iCode].nGamma; iGamma++){
      debug.log(dou2str(sys->gamma[sys->code[iCode].gammaFirstValuePos+iGamma]), UNK_LOG, 10);
    }
    debug.log("\n", UNK_LOG);
    debug.log("deltas\n", UNK_LOG, 0);
    for(int iDelta = 0; iDelta<sys->code[iCode].nDelta; iDelta++){
      debug.log(dou2str(sys->code[iCode].delta[iDelta]), UNK_LOG, 10);
    }
    debug.log("\n\n", UNK_LOG);
  }
  debug.log("\n\n", UNK_LOG);

  debug.log("iter: "+int2str(iter), RES_LOG,0);
  debug.log("|res|:"+dou2str(residual)+"\n", RES_LOG,20);
  debug.log("beta", RES_LOG);
  debug.log("x", RES_LOG);
  debug.log("res\n", RES_LOG);
  for(int iUnk=0; iUnk<sys->nUnk; iUnk++){
    debug.log(dou2str(sys->beta[iUnk]), RES_LOG);
    debug.log(dou2str(x[iUnk]), RES_LOG);
    debug.log(dou2str(resVector[iUnk])+"\n", RES_LOG);
  }
  debug.log("\n\n", RES_LOG);
  
}

/* Solver calculateNewGuess

Calculate new guess by different methods.

input: System pointer, Communicator pointer, actual evolution step
output: -

*/
void Solver::calculateNewGuess(System* sys, Communicator* comm, int step)
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
      jacobianConstruction(sys, comm, step);
      solveLinearSystem(sys);
      break;   
      
    case SECANT:
      updateJacobian(sys, comm, step);
      solveLinearSystem(sys);
      break;   
      
    case BROYDEN:
      updateJacobian(sys, comm, step);
      solveLinearSystem(sys);
      break;   
      
    default:
      error = NEWTON_ERROR;
      checkError(error, "Nonlinear method has not been implemented yet - Solver::calculateNewGuess");
  }
  
  // Put x values in gamma and delta
  x2gamma2delta(sys);
}

/* Solver::updateJacobian





*/
void Solver::updateJacobian(System* sys, Communicator* comm, int step)
{
  switch(method){
    case SECANT:
      if(step==0 && iter==0){
        jacobianConstruction(sys, comm, step);
      }
      else if(sJacCalc>0 && iter==0){
        if(step % sJacCalc==0){
          jacobianConstruction(sys, comm, step);
        }
      }
      else if(iJacCalc>0){
        if(iter>0 && (iter % iJacCalc)==0){
          jacobianConstruction(sys, comm, step);
        }
      }
      break;
    
    case BROYDEN:
      if(step==0 && iter==0 && sJacCalc>0){
        jacobianConstruction(sys, comm, step);
      }
      else if(sJacCalc>0 && iter==0){
        if(step % sJacCalc==0){
          jacobianConstruction(sys, comm, step);
        }
      }
      else if(iJacCalc>0){
        if(iter>0 && (iter % iJacCalc)==0){
          jacobianConstruction(sys, comm, step);
        }
      }
      else{
        broydenUpdate(sys, step, iter);
      }
      break;
      
    default:
      break;
  }
}

/* Solver::broydenUpdate





*/
void Solver::broydenUpdate(System* sys, int step, int iter)
{  
  if(iter>0){
    // J = Jk + (deltaRes - Jk*deltaX) / (deltaXT*deltaX) * deltaXT;
    math->copyMat(JItPrev, J, sys->nUnk);
    double* deltaRes = math->differenceInVectors(resVector, resVectorItPrev, sys->nUnk);
    double* Jdx = math->matXvec(JItPrev, deltaX, sys->nUnk);
    double* num = math->differenceInVectors(deltaRes, Jdx, sys->nUnk);
    double div = math->vecDotvec(deltaX, deltaX, sys->nUnk);
    double* t2a = math->scaleVec(num, 1.0/div, sys->nUnk);
    double** t2b = math->vecXvec(t2a, deltaX, sys->nUnk);
    // Saving just J
    J = math->sumMat(JItPrev, t2b, sys->nUnk);
    
    //~ debug.log("step: "+int2str(step), J_LOG, 0);
    //~ debug.log("iter: "+int2str(iter), J_LOG, 10);
    //~ debug.log("J:\n", J_LOG, 5);
    //~ for(int iUnk=0; iUnk<sys->nUnk; iUnk++){
      //~ for(int jUnk=0; jUnk<sys->nUnk; jUnk++){
        //~ debug.log(dou2str(J[iUnk][jUnk]), J_LOG, 12);
      //~ }
      //~ debug.log("\n", J_LOG);
    //~ }
    //~ debug.log("\n", J_LOG);


    // TEST
    //~ cout<<"x:"<<endl;
    //~ for(int i=0; i<sys->nUnk; i++){
      //~ cout<<x[i]<<" ";
    //~ }
    //~ cout<<endl;
    //~ cout<<"xItPrev:"<<endl;
    //~ for(int i=0; i<sys->nUnk; i++){
      //~ cout<<xItPrev[i]<<" ";
    //~ }
    //~ cout<<endl;
    //~ cout<<"res:"<<endl;
    //~ for(int i=0; i<sys->nUnk; i++){
      //~ cout<<resVector[i]<<" ";
    //~ }
    //~ cout<<endl;
    //~ cout<<"resVectorItPrev:"<<endl;
    //~ for(int i=0; i<sys->nUnk; i++){
      //~ cout<<resVectorItPrev[i]<<" ";
    //~ }
    //~ cout<<endl;  
    //~ cout<<"JItPrev:"<<endl;
    //~ for(int i=0; i<sys->nUnk; i++){
      //~ for(int j=0; j<sys->nUnk; j++){
        //~ cout<<JItPrev[i][j]<<" ";
      //~ }
      //~ cout<<endl;
    //~ }
    //~ cout<<endl;
    //~ 
    //~ 
    //~ cout<<"deltaRes:"<<endl;
    //~ for(int i=0; i<sys->nUnk; i++){
      //~ cout<<deltaRes[i]<<" ";
    //~ }
    //~ cout<<endl;
    //~ 
    //~ cout<<"Jdx:"<<endl;
    //~ for(int i=0; i<sys->nUnk; i++){
      //~ cout<<Jdx[i]<<" ";
    //~ }
    //~ cout<<endl;
    //~ 
    //~ cout<<"num:"<<endl;
    //~ for(int i=0; i<sys->nUnk; i++){
      //~ cout<<num[i]<<" ";
    //~ }
    //~ cout<<endl;
    //~ 
    //~ cout<<"div:"<<div<<endl;
    //~ 
    //~ cout<<"t2a:"<<endl;
    //~ for(int i=0; i<sys->nUnk; i++){
      //~ cout<<t2a[i]<<" ";
    //~ }
    //~ cout<<endl;
    //~ 
    //~ cout<<"t2b:"<<endl;
    //~ for(int i=0; i<sys->nUnk; i++){
      //~ for(int j=0; j<sys->nUnk; j++){
        //~ cout<<t2b[i][j]<<" ";
      //~ }
      //~ cout<<endl;
    //~ }
    //~ cout<<endl;   
    
    
    //~ cout<<"J:"<<endl;
    //~ for(int i=0; i<sys->nUnk; i++){
      //~ for(int j=0; j<sys->nUnk; j++){
        //~ cout<<J[i][j]<<" ";
      //~ }
      //~ cout<<endl;
    //~ }
    //exit(1); 
  }
}


/* Solver::jacobianConstruction





*/
void Solver::jacobianConstruction(System* sys, Communicator* comm, int step)
{
  // Back up
  math->copyInVector(xBackUp, 0, x, 0, sys->nUnk);
  math->copyInVector(resVectorBackUp, 0, resVector, 0, sys->nUnk);
  
  // f(x) was calculated in calculateResiduals() previously
  
  // Loop in partial derivative calculation (cols)
  rootPrints("  Calculating Jacobian by finite difference...");
  for(int j=0; j<sys->nUnk; j++){
    rootPrints("   Jacobian Row: "+int2str(j+1));
    math->sumDeltaInPosition(x, j, dxJacCalc, sys->nUnk);
    
    x2gamma2delta(sys);
    calculateResiduals(sys, comm);
    // Loop in residuals (rows)
    // Saving -J in order to solve deltaX = -J * res
    // Now just saving J
    for(int i=0; i<sys->nRes; i++){
      J[i][j] = (resVector[i]-resVectorBackUp[i]) / dxJacCalc;
    }

    // Restore x
    math->copyInVector(x, 0, xBackUp, 0, sys->nUnk);
    
    // Send NEWTON_RESTART order to clients connected by MPI
    order = NEWTON_RESTART;
    comm->sendOrder(order);
  }
  
  rootPrints("  End Jacobian calculation");
  
  //~ debug.log("step: "+int2str(step), J_LOG, 0);
  //~ debug.log("iter: "+int2str(iter), J_LOG, 10);
  //~ debug.log("J:\n", J_LOG, 5);
  //~ for(int iUnk=0; iUnk<sys->nUnk; iUnk++){
    //~ for(int jUnk=0; jUnk<sys->nUnk; jUnk++){
      //~ debug.log(dou2str(J[iUnk][jUnk]), J_LOG, 12);
    //~ }
    //~ debug.log("\n", J_LOG);
  //~ }
  //~ debug.log("\n", J_LOG);

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
  
  // Save J in A
  for(int i=0; i<sys->nUnk; i++){
    for(int j=0; j<sys->nUnk; j++){
      switch (method){
          case(NEWTON):
            valueToAssembly = -J[i][j];
            break;
            
          case(SECANT):
            valueToAssembly = -J[i][j];
            break;
            
          case(BROYDEN):
            valueToAssembly = -J[i][j];
            break;
          
          default:
            valueToAssembly = J[i][j];
            break;      
      }
      MatSetValue(A,i,j,valueToAssembly,INSERT_VALUES);
    }
  } 
    
  MatAssemblyBegin(A,MAT_FINAL_ASSEMBLY);
  MatAssemblyEnd(A,MAT_FINAL_ASSEMBLY);  
  // Solve
  KSPSetOperators(ksp,A,A);
  KSPGetPC(ksp,&pc);
  PCSetType(pc,PCNONE);
  KSPSolve(ksp,b,u);

  if(irank==NEWTON_ROOT){
    // Get x solution from u
    VecGetArray(u,&deltaX);
  }
  
  // Share solution
  error =  MPI_Bcast( deltaX,// void *buffer, 
                          sys->nUnk,// int count, 
                          MPI_DOUBLE_PRECISION,// MPI_Datatype datatype, 
                          NEWTON_ROOT,// int root, 
                          MPI_COMM_WORLD);// MPI_Comm comm )
  checkError(error, "Error sharing solution between processes");                            
  
  // Save previous value of x
  math->copyInVector(xItPrev, 0, x, 0, sys->nUnk);
  x = math->sumVectors(deltaX, xItPrev, sys->nUnk);
  // TEST
  //~ cout<<"delta x  -  x:"<<endl;
  //~ for(int iUnk=0; iUnk<sys->nUnk; iUnk++){
    //~ cout<<deltaX[iUnk]<<"\t"<<x[iUnk]<<endl;
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
                                     sys->code[iCode].actualInput,
                                     sys->code[iCode].inputModel);
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

input: code number, System pointer
output: error

*/
int Solver::spawnCode(int iCode, System* sys)
{
  if(sys->code[iCode].spawnType=="mpi" || sys->code[iCode].nProcs>1){
    // MPI_Spawn
    error = NEWTON_ERROR;
    cout<<"MPI_Spawn is not implemented yet"<<endl;
    return error;
    
    const char** argv = new const char*[sys->code[iCode].nArgs+1];
    for(int iArg=0; iArg<sys->code[iCode].nArgs; iArg++){
      argv[iArg] = (sys->code[iCode].arg[iArg]).c_str();
      if(sys->code[iCode].arg[iArg]=="code_id"){
        argv[iArg] = (int2str(sys->code[iCode].id)).c_str();
      }
      else if(sys->code[iCode].arg[iArg]=="input" || sys->code[iCode].arg[iArg]=="input_file"){
        argv[iArg] = (sys->code[iCode].actualInput).c_str();
      }
      else if(sys->code[iCode].arg[iArg]=="input_name"){
        argv[iArg] = (sys->code[iCode].actualInputName).c_str();
      }
      else if(sys->code[iCode].arg[iArg]=="output"){
        argv[iArg] = (sys->code[iCode].actualOutput).c_str();
      }
      else if(sys->code[iCode].arg[iArg]=="output_name"){
        argv[iArg] = (sys->code[iCode].actualOutputName).c_str();
      }
    }
    argv[sys->code[iCode].nArgs] = NULL;
    cout<<"Spawning..."<<endl;
    error = MPI_Comm_spawn( (sys->code[iCode].binCommand).c_str(), // const char *command
                            (char**)argv, //MPI_ARGV_NULL,// char *argv[] 
                            sys->code[iCode].nProcs,// int maxprocs 
                            MPI_INFO_NULL,// MPI_Info info
                            0,// int root (of the group)
                            MPI_COMM_SELF,// MPI_Comm comm
                            &mpi_comm_spawn,// MPI_Comm *intercomm
                            MPI_ERRCODES_IGNORE);// int array_of_errcodes[]    
   
    cout<<"Finish spawing"<<endl;
    // Wait for client (in client there should be a Barrier too)
    error += MPI_Barrier(mpi_comm_spawn);
  }
  
  else if(sys->code[iCode].nProcs==1){
    // system
    error = system((sys->code[iCode].commandToRun).c_str());
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

input: code number, System pointer, Mapper pointer
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

input: code number, System pointer, Communicator pointer
output: error

*/
int Solver::sendDataToCode(int iCode, System* sys, Communicator* comm)
{ 
  // Send data
  error = comm->send(iCode, sys->code[iCode].nDelta, sys->code[iCode].delta);
  
  return NEWTON_SUCCESS;
}

/* Solver::receiveDataFromCode

Receive data from a client via MPI functions.
Map them before save in x.

input: code number, System pointer, Communicator pointer
output: error

*/
int Solver::receiveDataFromCode(int iCode, System* sys, Communicator* comm)
{ 
  // Receive data
  error = comm->receive(iCode, sys->code[iCode].nAlpha, sys->code[iCode].alpha);
  
  return error;
}

/* Solver::printMethod()
Print string name of the nonlinear method selected to solve the problem.

input: -
output: string

*/
string Solver::printMethod()
{
  switch(method){
    case EXPLICIT_SERIAL:
      return("EXPLICIT_SERIAL");
      
    case EXPLICIT_PARALLEL:
      return("EXPLICIT_PARALLEL");
      
    case NEWTON:
      return("NEWTON");
      
    case SECANT:
      return("SECANT");   
      
    case BROYDEN:
      return("BROYDEN"); 
      
    default:
      error = NEWTON_ERROR;
      checkError(error, "Nonlinear method has not been selected - Solver::printMethod");
  }
  return NULL;
}

/* Solver::saveX
Save the solution in vector of previous X.

input: System pointer, step
output: -

*/
void Solver::saveX(System* sys, int step)
{
  debug.log("step: "+int2str(step)+"\n", X_LOG, 0);
    
  debug.log("x:", X_LOG, 10);
  for(int iUnk=0; iUnk<sys->nUnk; iUnk++){    
    debug.log(dou2str(x[iUnk]), X_LOG, 10);
  }
  debug.log("\n", X_LOG);

  for (int i=0; i<nXStPrev-1; i++){
    math->copyInVector(xStPrev[nXStPrev-1-i], 0, xStPrev[nXStPrev-1-i-1], 0, sys->nUnk);
  }
  math->copyInVector(xStPrev[0], 0, x, 0, sys->nUnk);
}

/* Solver::saveJ
Save the jacobian in vector of previous J.

input: System pointer, step
output: -

*/
void Solver::saveJ(System* sys, int step)
{    
  debug.log("step: "+int2str(step)+"\n", J_LOG, 0);
  
  debug.log("J:", J_LOG, 10);
  for(int iUnk=0; iUnk<sys->nUnk; iUnk++){    
    for(int jUnk=0; jUnk<sys->nUnk; jUnk++){    
      debug.log(dou2str(J[iUnk][jUnk]), J_LOG);
    }
    debug.log("\n", J_LOG);
  }
  debug.log("\n", J_LOG);
  
  for (int i=0; i<nJStPrev-1; i++){
    for(int iUnk=0; iUnk<sys->nUnk; iUnk++){
      math->copyInVector(JStPrev[nJStPrev-1-i][iUnk], 0, JStPrev[nJStPrev-1-i-1][iUnk], 0, sys->nUnk);
    }
  }  
  for(int iUnk=0; iUnk<sys->nUnk; iUnk++){
    math->copyInVector(JStPrev[0][iUnk], 0, J[iUnk], 0, sys->nUnk);
  }
}

/* Solver::extrapolateX
Set the first x guess as extrapolation of previous solutions.

input: System pointer, int step
output: -

*/
void Solver::extrapolateX(System* sys, int step)
{
  if(nXStPrev==2){
    for(int iUnk=0; iUnk<sys->nUnk; iUnk++){
      x[iUnk] = 2*xStPrev[0][iUnk] - xStPrev[1][iUnk];
    }
    debug.log("\n", X_LOG);
    debug.log("pre step: "+int2str(step)+"\n", X_LOG, 0);
    debug.log("x prev[-1]:", X_LOG, 10);
    for(int iUnk=0; iUnk<sys->nUnk; iUnk++){    
      debug.log(dou2str(xStPrev[1][iUnk]), X_LOG, 10);
    }
    debug.log("\n", X_LOG);
    debug.log("x prev[0]:", X_LOG, 10);
    for(int iUnk=0; iUnk<sys->nUnk; iUnk++){
      debug.log(dou2str(xStPrev[0][iUnk]), X_LOG, 10);
    }
  }
  else if(nXStPrev>2){
    rootPrints("WARNING: Only extrapolation with 2 points has been implemented - Solver::extrapolateX");
  }

  debug.log("\n", X_LOG);
  debug.log("x guess:", X_LOG, 10);
  for(int iUnk=0; iUnk<sys->nUnk; iUnk++){    
    debug.log(dou2str(x[iUnk]), X_LOG, 10);
  }
  debug.log("\n", X_LOG);
  debug.log("\n", X_LOG);
}

/* Solver::extrapolateJ
Set the first J guess as extrapolation of previous jacobians.

input: System pointer, int step
output: -

*/
void Solver::extrapolateJ(System* sys, int step)
{
  if(nJStPrev==2){
    
    for(int iUnk=0; iUnk<sys->nUnk; iUnk++){
      for(int jUnk=0; jUnk<sys->nUnk; jUnk++){
        J[iUnk][jUnk] = 2*JStPrev[0][iUnk][jUnk] - JStPrev[1][iUnk][jUnk];
      }
    }
    debug.log("\n", X_LOG);
    debug.log("pre step: "+int2str(step)+"\n", J_LOG, 0);
    debug.log("J:\n", J_LOG, 0);
    for(int iUnk=0; iUnk<sys->nUnk; iUnk++){    
      for(int jUnk=0; jUnk<sys->nUnk; jUnk++){    
        debug.log(dou2str(J[iUnk][jUnk]), J_LOG);
      }
      debug.log("\n", J_LOG);
    }
    debug.log("\n", J_LOG);   
    
  }
  else if(nXStPrev>2){
    rootPrints("WARNING: Only extrapolation with 2 points has been implemented - Solver::extrapolateJ");
  }
}
