/*****************************************************************************\

NEWTON					      |
                      |
Implicit coupling 		|	CLASS
in nonlinear			    |	NEWTON
calculations			    |
                      |

-------------------------------------------------------------------------------

Newton manages the three main stages in residual calculation: initializing 
everything to start calculations, administrates evolution parameters, controls 
that solution to residual equations converge in each evolution step, and ends
everything safety.

Author: Federico A. Caccia
Date: 3 June 2017

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

	// Objects initialization
	NewtonParser = new Parser();
	NewtonSystem = new System();
	NewtonEvolution = new Evolution();
	NewtonSolver = new Solver();
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
  
  rootPrints("\n<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<    Newton    >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>");
    rootPrints("                                    Implicit coupling");
    rootPrints("                                in nonlinear calculations");
    rootPrints("<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<    >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>\n");  
  
	// Objects initialization

	NewtonParser->parseInput(NewtonSystem, NewtonEvolution, NewtonSolver);
	
	NewtonSystem->construct();

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
    click = clock();
    rootPrints("Solving step: "+int2str(NewtonEvolution->step+1));
		NewtonSolver->setFirstGuess(NewtonSystem, NewtonEvolution->step);
		NewtonSolver->iterateUntilConverge(NewtonSystem, NewtonComm, NewtonEvolution->step);
		NewtonEvolution->update(NewtonSystem);
    rootPrints(" Total time step: "
               +dou2str((clock()-click)/ CLOCKS_PER_SEC)+" seconds");
	}
  rootPrints("Total function evaluations: "+int2str(NewtonSolver->nEval));
  rootPrints("Total time: "+dou2str((clock()-firstClick)/ CLOCKS_PER_SEC)+" seconds");
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
  rootPrints("\n<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<    >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>\n");
}
