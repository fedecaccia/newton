/*****************************************************************************\

NEWTON                |
                      |
Multiphysics          | CLASS
coupling              | NEWTON
maste code            |
                      |

-------------------------------------------------------------------------------

Newton manages the three main stages in residual calculation: initializing 
everything to start calculations, administrates evolution parameters, controls 
that solution to residual equations converge in each evolution step, and ends
everything safety.

Date: 3 June 2017

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

#include "Newton.h"

using namespace::std;

/* Newton constructor
input: -

*/
Newton::Newton()
{
	// Variables initialization
	error = NEWTON_SUCCESS;
  
  // Other objects creation
	NewtonParser = new Parser();
	NewtonSystem = new System();  
	NewtonEvolution = new Evolution();
	NewtonSolver = new Solver();
  NewtonMap = new Mapper(NewtonSolver->NewtonClient);
	NewtonComm = new Communicator(NewtonSystem, NewtonEvolution);	
}

/* Newton::initialize

Starts MPI and set world rank and size global variables.
Creates necessary objects that are going to be used in the programm.

input: -
output: -

*/
void Newton::initialize()
{
	// MPI init and main variables
  
	// Initialize the MPI environment
	error = MPI_Init(NULL, NULL);
	checkError(error, "MPI_INIT error");
	// Get the number of processes
	error = MPI_Comm_size(MPI_COMM_WORLD, &world_size);
	checkError(error, "MPI_Comm_size error");
	// Get the rank of the process
	error = MPI_Comm_rank(MPI_COMM_WORLD, &irank);
	checkError(error, "MPI_Comm_rank error");
  
  // PETSc init
  PetscInitialize(PETSC_NULL,PETSC_NULL,PETSC_NULL,PETSC_NULL);
  
  // Set debugger
  configureDebugger();
  
  rootPrints("\n<<<<<<<<<<<<<<<<<<<<<<   >>>>>>>>>>>>>>>>>>>>>>");
    rootPrints("                    Newton");
    rootPrints("       Multiphysics coupling master code");
    rootPrints("<<<<<<<<<<<<<<<<<<<<<<   >>>>>>>>>>>>>>>>>>>>>>\n");

  // Objects initialization
  
	NewtonParser->parseInput(NewtonSystem, NewtonEvolution, NewtonSolver, NewtonSolver->NewtonClient, NewtonMap, NewtonComm, &debug);
	
	NewtonSystem->construct(NewtonMap);

	NewtonComm->initialize();	
}

/* Newton::run

Control the solution of the residual equations in each evolution step.

input: -
output: -

*/
void Newton::run()
{	
  firstClick = clock();
	while(NewtonEvolution->status != NEWTON_COMPLETE){
    click1 = clock();
    rootPrints("Nonlinear method: "+NewtonSolver->printMethod());
    rootPrints("Solving step: "+int2str(NewtonEvolution->step+1));
		NewtonSolver->setFirstGuess(NewtonSystem, NewtonEvolution->step);
		NewtonSolver->iterateUntilConverge(NewtonSystem, NewtonComm, NewtonEvolution->step);
		NewtonEvolution->update(NewtonSystem, NewtonSolver->NewtonClient);
    if(NewtonEvolution->status != NEWTON_COMPLETE){
      NewtonComm->sendOrder(NEWTON_CONTINUE);
    }
    click2 = clock();
    rootPrints(" Total time step: "+dou2str((click2-click1)/ CLOCKS_PER_SEC)+" seconds");
    debug.log("step: "+int2str(NewtonEvolution->step));
    debug.log("time: "+dou2str((click2-click1)/ CLOCKS_PER_SEC));
    debug.log("f_eval: "+int2str(NewtonSolver->nEvalInStep)+"\n");
	}
  rootPrints("Total function evaluations: "+int2str(NewtonSolver->nEval));
  lastClick = clock();
  rootPrints("Total time: "+dou2str((lastClick-firstClick)/ CLOCKS_PER_SEC)+" seconds");
    debug.log("steps: "+int2str(NewtonEvolution->step));
    debug.log("time: "+dou2str((lastClick-firstClick)/ CLOCKS_PER_SEC));
    debug.log("f_evals: "+int2str(NewtonSolver->nEval)+"\n");
  rootPrints("Calculation finished successfully.");
}

/* Newton::finish

Ends everything in the programm to complete a succesfull calculation,
like MPI connections. 

input: -
output: -

*/
void Newton::finish()
{ 
	NewtonComm->disconnect();
  PetscFinalize();
	MPI_Finalize();
  
  // Set debuggers  off
  this->debug.setOff();
  NewtonParser->debug.setOff();
	NewtonSystem->debug.setOff();
  NewtonEvolution->debug.setOff();
  NewtonSolver->debug.setOff();
  NewtonMap->debug.setOff();
  NewtonComm->debug.setOff();
  NewtonSolver->NewtonClient->debug.setOff();
}

void Newton::configureDebugger()
{
  string* stringLog = new string[MAX_LOG];

  // Set debugger amount of log files & their names
  stringLog[0] = "newton.log";
  this->debug.setOutput(stringLog);
  stringLog[0] = "parser.log";
  NewtonParser->debug.setOutput(stringLog);
  stringLog[0] = "system.log";
  NewtonSystem->debug.setOutput(stringLog);
  stringLog[0] = "evolution.log";
  NewtonEvolution->debug.setOutput(stringLog);
  stringLog[GLOBAL_LOG] = "solver_global.log";
  stringLog[LOCAL_LOG] = "solver_iter.log";
  stringLog[RES_LOG] = "res.log";
  stringLog[X_LOG] = "x.log";
  stringLog[J_LOG] = "j.log";
  stringLog[UNK_LOG] = "unk.log";
  NewtonSolver->debug.setOutput(stringLog, 6);
  stringLog[0] = "mapper.log";
  NewtonMap->debug.setOutput(stringLog);
  stringLog[0] = "comm.log";
  NewtonComm->debug.setOutput(stringLog);
  stringLog[0] = "client.log";
  NewtonSolver->NewtonClient->debug.setOutput(stringLog);
}