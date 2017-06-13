/*****************************************************************************\

NEWTON					      |
                      |
Implicit coupling 		|	FUNCTIONS
in nonlinear			    |	GLOBAL
calculations			    |
                      |

-------------------------------------------------------------------------------

Global sets global defines, variables and functions commonly used.

Author: Federico A. Caccia
Date: 3 June 2017

\*****************************************************************************/

#include "global.h"

#include <sstream>

using namespace::std;

// Global variables
// MPI world size
int world_size=1;
// MPI process rank
int irank=0;
// MPI_Initialized flag
int flag=0;
// mpierror
int mpierror=0;

/* functionTrying(*f)();
Try function and catch exception.
input: function pointer
output: -
*//*
void functionTrying(void (*f)())
{
	try{
		f();
	}
	catch(int e){
		rootPrints("FATAL ERROR. ABORTING.");
	}
}*/

/* checkError

This function has to be called synchronized by all processes.
It checks the value of the variable error previously computed by another 
function. The error value is shared between processes. If just one has a 
possitive error value, everyone abort.

input: error
output: -

*/
void checkError(int error, string errorPhrase)
{
  MPI_Initialized(&flag);
	if(flag){
		mpierror = MPI_Allreduce(MPI_IN_PLACE, // const void *sendbuf, 
								 &error, // void *recvbuf, 
								 1, // int count,
                 MPI_INTEGER, // MPI_Datatype datatype, 
                 MPI_SUM, // MPI_Op op, 
                 MPI_COMM_WORLD); // MPI_Comm comm);
	}

	if(error!=NEWTON_SUCCESS || mpierror != MPI_SUCCESS){
		rootPrints(errorPhrase);
    rootPrints("FATAL ERROR. ABORTING.");
		throw error;
	}
}

/* rootPrints

This function receives an string, check if the process is root, and only in
that case prints to the std output.

input: string to print
output: -

*/
void rootPrints(string phrase)
{
	if(irank==NEWTON_ROOT){
		cout<<phrase<<endl;
	}
}
  
/* int2str

Converts integers to strings.

input: int to print
output: string

*/
string int2str (int a)
{
    ostringstream temp;
    temp<<a;
    return temp.str();
}

/* dou2str

Converts doubles to strings.

input: double to print
output: string

*/
string dou2str (double a)
{
    ostringstream temp;
    temp<<a;
    return temp.str();
}
