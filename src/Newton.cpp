/*****************************************************************************\

NEWTON					|
						|
Implicit coupling 		|	CLASS
in nonlinear			|	NEWTON
calculations			|
						|

-------------------------------------------------------------------------------

Newton manages the three main stages in residual calculation: initializing everything to start calculations, administrates evolution parameters, controls that solution to residual equations converge in each evolution step, and finishes everything safety.

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
	NewtonMap = new Mapper();
	NewtonComm = new Communicator();
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
  
	// Objects initialization

	NewtonParser->parseInput(NewtonSystem, NewtonEvolution);
	NewtonParser->checkConsistency();
	
	NewtonSystem->construct();
	
  NewtonSolver->initialize(NewtonSystem);
  
	NewtonMap->config();

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
		NewtonSolver->setInitialCondition(NewtonEvolution->step);
		NewtonSolver->iterateUntilConverge(NewtonSystem, NewtonMap, NewtonComm);
		NewtonEvolution->update();
    rootPrints(" Total time step: "
               +dou2str((clock()-click)/ CLOCKS_PER_SEC)+" seconds");
	}
  rootPrints("Calculation finished successfully.");
  rootPrints("Total time: "
             +dou2str((clock()-firstClick)/ CLOCKS_PER_SEC)+" seconds");
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
	MPI_Finalize();
}