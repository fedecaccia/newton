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
	NewtonComm = new Communicator();
	NewtonSolver = new Solver();
	NewtonMap = new Mapper();
}

/* Newton::initialize

Initialize MPI communication, creates necessary objects that are going to be used in the programm.

input: -
output: -

*/
void Newton::initialize()
{
	// MPI init and main variables
	mpiInit();

	// Objects initialization

	NewtonParser->parseInput(NewtonSystem);
	NewtonParser->checkConsistency();
	
	NewtonSystem->construct();
	
	NewtonEvolution->start();
	
	NewtonComm->initialize();
	
	NewtonMap->config();
	
}

/* Newton::run

Control the solution of the residual equations in each evolution step.

input: -
output: -

*/
void Newton::run()
{
	
}

/* Newton::finish

Ends everything in the programm to complete a succesfull calculation,
like MPI connections. 

input: -
output: -

*/
void Newton::finish()
{
	MPI_Finalize();
}

/* Newton::mpiInit

Ends everything in the programm to complete a succesfull calculation,
like MPI connections. 

input: -
output: -

*/
void Newton::mpiInit()
{
	// Initialize the MPI environment
	error = MPI_Init(NULL, NULL);checkError(error, "MPI_INIT error");
	// Get the number of processes
	error = MPI_Comm_size(MPI_COMM_WORLD, &world_size);checkError(error, "MPI_Comm_size error");
	// Get the rank of the process
	error = MPI_Comm_rank(MPI_COMM_WORLD, &irank);checkError(error, "MPI_Comm_rank error");
}