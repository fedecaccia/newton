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
#include "System.h"
#include "Mapper.h"
#include "Communicator.h"
#include "MathLib.h"

class Solver
{
	public:
		Solver();
    void initialize(System*);
    void setInitialCondition(int);
		void iterateUntilConverge(System*, Mapper*, Communicator*);
    void calculateNewGuess();
    void calculateResiduals(System*, Mapper*, Communicator*);
    
    double nltol;
    int maxIter;
    int method;
		
	private:
    int runCode(int);
    int readOutputFromCode(int);
    int sendDataToCode(int);
    int receiveDataFromCode(int);
  
		int error;
    double residual;    
    int iter;
    int iCode;
    MathLib* math;
    
    double* x;
    double* xStar;
    double* y;
    double* resVector;
};

#endif
