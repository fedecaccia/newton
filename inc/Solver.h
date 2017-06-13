/*****************************************************************************\

NEWTON					      |
                      |
Implicit coupling 		|	CLASS
in nonlinear			    |	SOLVER
calculations			    |
                      |

-------------------------------------------------------------------------------

Solver computes the residual values and solves the nonlinear system with 
different kind of methods.

Author: Federico A. Caccia
Date: 4 June 2017

\*****************************************************************************/

#ifndef SOLVER_H
#define SOLVER_H

#include "global.h"
#include "Client.h"
#include "System.h"
#include "Communicator.h"
#include "MathLib.h"

#include <iomanip>
#include "petsc.h"

class Solver
{
	public:
		Solver();
    void initialize(System*);
    void setFirstGuess(System*, int);
		void iterateUntilConverge(System*, Communicator*, int);
    void calculateNewGuess(System*, Communicator*, int, int);
    void calculateResiduals(System*, Communicator*);
    
    double* x;
    double nltol;
    int maxIter;
    int method;
    double dxJacCalc;
    int sJacCalc;
    int iJacCalc;
    
    int nEvalInStep;
    int nEval;
		
	private:
    int runCode(int, System*);
    int spawnCode(int, System*);
    int readOutputFromCode(int, System*);
    int sendDataToCode(int);
    int receiveDataFromCode(int);
    void jacobianConstruction(System*, Communicator*);
    void broydenUpdate(System*, int);
    void updateJacobian(System*, Communicator*, int, int);
    void solveLinearSystem(System*);
    void x2gamma2delta(System*);
  
		int error;
    double residual;    
    int iter;
    int iCode;
    MathLib* math;
    int freeRank;
    
    double* deltaX;
    double* xItPrev;
    double* xBackUp;
    double* resVector;
    double* resVectorItPrev;
    double* resVectorBackUp;
    double** J;
    double** JItPrev;
    
    Vec u, b;
    Mat A;
    PC pc;
    KSP ksp;
    PetscScalar valueToAssembly;
    
    int codeConnected;
    
    
    Client* NewtonClient;
};

#endif
