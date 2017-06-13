/*****************************************************************************\

NEWTON					      |
                      |
Implicit coupling 		|	CLASS
in nonlinear			    |	COMMUNICATOR
calculations			    |
                      |

-------------------------------------------------------------------------------

Stablish mpi communitaction with clients.

Author: Federico A. Caccia
Date: 4 June 2017

\*****************************************************************************/

#include "Communicator.h"

using namespace::std;

/* Evolution constructor
*/
Communicator::Communicator()
{
	// No connections at beginig
	isConnectedByMPI = NEWTON_DISCONNECTED;
	error = NEWTON_SUCCESS;
}

/* Communicator::initialize
Starts the communication.

input: System pointer
output: -

*/
void Communicator::initialize(System* sys)
{
  rootPrints("Checking communication with clients...");
  
  // Only roots stablish communication
  if(irank==NEWTON_ROOT){
    Port_Name = new char*[sys->nCodes];
    
    for(int iCode=0; iCode<sys->nCodes; iCode++){
      
      if(sys->code[iCode].connection==NEWTON_MPI_COMMUNICATION){
        error = MPI_Open_port(MPI_INFO_NULL, Port_Name[iCode]);
        cout<<"******aa"<<endl;
        string Srvc_NameCPP= "('Coupling_C', I0)"+int2str(sys->code[iCode].id);
        char* Srvc_Name=(char*)Srvc_NameCPP.c_str();
        cout<<"******bb"<<endl;
        error += MPI_Publish_name(Srvc_Name, MPI_INFO_NULL, Port_Name[iCode]);
        cout<<"******cc"<<endl;
      }    
    }
  }
  
  checkError(error, "Error publishing services with clients by MPI");
  
  if(irank==NEWTON_ROOT){
    Coupling_Comm = new MPI_Comm[sys->nCodes];
    
    for(int iCode=0; iCode<sys->nCodes; iCode++){
      if(sys->code[iCode].connection==NEWTON_MPI_COMMUNICATION){
        error = MPI_Comm_accept(Port_Name[iCode], MPI_INFO_NULL, 0, MPI_COMM_SELF, &Coupling_Comm[iCode]);
        string Srvc_NameCPP= ("('Coupling_C', I0)"+int2str(sys->code[iCode].id));
        char* Srvc_Name=(char*)Srvc_NameCPP.c_str();
        error += MPI_Unpublish_name(Srvc_Name, MPI_INFO_NULL, Port_Name[iCode]);
      }
    }
  }
  checkError(error, "Error accepting communication with clients by MPI");  
  
}

/* Communicator::disconnnect
Finishes the communication.

input: -
output: -

*/
void Communicator::disconnect()
{
	if(isConnectedByMPI==NEWTON_CONNECTED){
		rootPrints("Finishing client codes communications.");
		/*for(int iCode=0; iCode<Ld->nCodes;iCode++){
		    if (Ld->connectionType[iCode]==0){  // Tipo de conexión MPI
		      // tag de la recepción, el del código que lo recibe debe ser igual
		      tag = 100;
		      // Envío de órden: abortar (Están esperando recepción de orden al final de cada paso temporal)
		      ierror = MPI_Send (&order, 1, MPI_DOUBLE_PRECISION, 0, tag, (Comp_Comm[iCode]));
		      ierror = MPI_Comm_disconnect(&(Comp_Comm[iCode]));
		      ierror = MPI_Close_port(Port_Name[iCode]);
		    }
 		}*/
	}

	checkError(error,"Error starting communication.");
}
