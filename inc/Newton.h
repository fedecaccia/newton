/*****************************************************************************\

NEWTON					|
						|
Implicit coupling 		|	CLASS
in nonlinear			|	NEWTON
calculations			|
						|

-------------------------------------------------------------------------------

Newton manages the three main stages in residual calculation: initializing 
everything to start calculations, administrates evolution parameters, controls 
that solution to residual equations converge in each evolution step, and ends
everything safety.

Author: Federico A. Caccia
Date: 3 June 2017

\*****************************************************************************/

#ifndef NEWTON_H
#define NEWTON_H

#include "global.h"
#include "Parser.h"
#include "System.h"
#include "Evolution.h"
#include "Communicator.h"
#include "Solver.h"
#include "Mapper.h"

#include "mpi.h"

class Newton
{
	public:
		Newton();
		void initialize();
		void run();
		void finish();

		Parser* NewtonParser;
		System* NewtonSystem;
		Evolution* NewtonEvolution;
		Communicator* NewtonComm;
		Solver* NewtonSolver;
		Mapper* NewtonMap;		
		
	private:
		void mpiInit();

		int error;

};

#endif