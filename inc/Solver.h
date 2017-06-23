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

#ifndef SOLVER_H
#define SOLVER_H

#include "global.h"
#include "Client.h"
#include "System.h"
#include "Communicator.h"
#include "MathLib.h"
#include "Debugger.h"

#include <iomanip>
#include "petsc.h"

class Solver
{
	public:
		Solver();
    void initialize(System*);
    void setFirstGuess(System*, int);
		void iterateUntilConverge(System*, Communicator*, int);
    void calculateNewGuess(System*, Communicator*, int);
    void calculateResiduals(System*, Communicator*);
    std::string printMethod();
    
    Client* NewtonClient;    
    double* x;
    double nltol;
    int maxIter;
    int method;
    double dxJacCalc;
    int sJacCalc;
    int iJacCalc;
    int nXStPrev;
    int nJStPrev;
    
    int nEvalInStep;
    int nEval;
    
		Debugger debug;
    
	private:
    int runCode(int, System*);
    int spawnCode(int, System*);
    int readOutputFromCode(int, System*);
    int sendDataToCode(int, System*, Communicator*);
    int receiveDataFromCode(int, System*, Communicator*);
    void jacobianConstruction(System*, Communicator*, int);
    void broydenUpdate(System*, int, int);
    void updateJacobian(System*, Communicator*, int);
    void solveLinearSystem(System*);
    void x2gamma2delta(System*);
    void saveX(System*, int);
    void extrapolateX(System*);
  
		int error;
    int order;
    double residual;    
    int iter;
    int iCode;
    MathLib* math;    
    int freeRank;
    int codeConnected;
    MPI_Comm mpi_comm_spawn;
    
    double* deltaX;
    double* xItPrev;
    double** xStPrev;
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
};

#endif
