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
Date: 14 June 2017

\*****************************************************************************/

#include "Communicator.h"

using namespace::std;

/* Evolution constructor
*/
Communicator::Communicator(System* sysPointer, Evolution* evolPointer)
{
	// No connections at beginig
	isConnectedByMPI = NEWTON_DISCONNECTED;
  
  // Pointers
  sys = sysPointer;
  evol = evolPointer;
  
  // Initial state of error
	error = NEWTON_SUCCESS;
}

/* Communicator::initialize
Starts the communication.

input: System pointer
output: -

*/
void Communicator::initialize()
{
  rootPrints("Checking communication with clients...");
  
  // Only roots stablish communication
  if(irank==NEWTON_ROOT){
    Port_Name = new string[sys->nCodes];
    
    for(int iCode=0; iCode<sys->nCodes; iCode++){      
      
      if(sys->code[iCode].connection==NEWTON_MPI_COMMUNICATION){
        char portname[MPI_MAX_PORT_NAME];        
        error += MPI_Open_port(MPI_INFO_NULL, portname);
        Port_Name[iCode].assign(portname);
        
        string Srvc_NameCPP= "Newton-"+int2str(sys->code[iCode].id);
        rootPrints("Publishing service:"+Srvc_NameCPP);
        const char* Srvc_Name=(char*)Srvc_NameCPP.c_str();
        error += MPI_Publish_name(Srvc_Name, MPI_INFO_NULL, portname);
      }    
    }
  }
  
  checkError(error, "Error publishing services with clients by MPI");
  
  if(irank==NEWTON_ROOT){
    Coupling_Comm = new MPI_Comm[sys->nCodes];
    
    for(int iCode=0; iCode<sys->nCodes; iCode++){
      if(sys->code[iCode].connection==NEWTON_MPI_COMMUNICATION){
        char portname[MPI_MAX_PORT_NAME];
        strcpy(portname, Port_Name[iCode].c_str());
        error += MPI_Comm_accept(portname, MPI_INFO_NULL, 0, MPI_COMM_SELF, &Coupling_Comm[iCode]);
        rootPrints("Connection with code iD: "+int2str(sys->code[iCode].id)+" accepted.");
        string Srvc_NameCPP= "Newton-"+int2str(sys->code[iCode].id);
        char* Srvc_Name=(char*)Srvc_NameCPP.c_str();
        error += MPI_Unpublish_name(Srvc_Name, MPI_INFO_NULL, portname);
        isConnectedByMPI=NEWTON_CONNECTED;
      }
    }
  }
  checkError(error, "Error accepting communication with clients by MPI");  
  
  // FIrst communication
  
  if(irank==NEWTON_ROOT){
    
    for(int iCode=0; iCode<sys->nCodes; iCode++){
      if(sys->code[iCode].connection==NEWTON_MPI_COMMUNICATION){
        error += firstCommunication(iCode);
      }
    }
  }
  checkError(error, "Error sharing first data with clients by MPI");  
}

/* Communicator::disconnnect
Finishes the communication.

input: -
output: -

*/
void Communicator::disconnect()
{
	if(isConnectedByMPI==NEWTON_CONNECTED){
	
    if(irank==NEWTON_ROOT){
      for(int iCode=0; iCode<sys->nCodes; iCode++){
        if(sys->code[iCode].connection==NEWTON_MPI_COMMUNICATION){
          char portname[MPI_MAX_PORT_NAME];
          strcpy(portname, Port_Name[iCode].c_str());
          
          // Clients are waiting for order at the end of the evolution step
          int order;
          
          if(evol->status==NEWTON_COMPLETE){
            order = NEWTON_FINISH;
          }
          else{
            order = NEWTON_ABORT;
          }
          error += sendOrder(iCode, order);
          error += MPI_Comm_disconnect(&Coupling_Comm[iCode]);
          error += MPI_Close_port(portname);
          rootPrints("Communication with code id: "+int2str(sys->code[iCode].id)+" closed");
        }
      }
    }  
  }  

	checkError(error,"Error starting communication.");
}

/* Communicator::firstCommunication

Share important data in first communication with code iCode.

input: code number
output: error

*/
int Communicator::firstCommunication(int iCode)
{   
  switch(sys->code[iCode].type){
    
    case TEST:
      if(sys->code[iCode].connection==NEWTON_MPI_COMMUNICATION){
        error = sendOrder(iCode, NEWTON_CONTINUE);
      }
      break;
      
    case RELAP_POW2TH:
      break;
      
    case FERMI_XS2POW:
      break;
      
    case USER_CODE:
      break;    
  }  
  
  return error;
}
/* Communicator::sendOrder

Send order to client code iCode.

input: code number, order
output: error

*/
int Communicator::sendOrder(int iCode, int order)
{
  //~ cout<<"Sending orders to code: "<<iCode<<"..."<<endl;
  if(sys->code[iCode].connection==NEWTON_MPI_COMMUNICATION){
    tag = 0;
    error = MPI_Send (&order, 1, MPI_INTEGER, 0, tag, Coupling_Comm[iCode]);    
  }
  //~ cout<<"Sended."<<endl;
  return error;
}

/* Communicator::send

Send nDelta values from delta to client code iCode.

input: code number, number of elements to send, array of elements to send
output: error

*/
int Communicator::send(int iCode, int nDelta, double* delta)
{
  // TEST
  //~ cout<<"Sending data to code: "<<iCode<<"..."<<endl;
  //~ for(int i=0; i<nDelta; i++){
    //~ cout<<" d:"<<delta[i]<<endl;
  //~ }
  if(sys->code[iCode].connection==NEWTON_MPI_COMMUNICATION){
    tag = 0;
    error = MPI_Send (delta, nDelta, MPI_DOUBLE_PRECISION, 0, tag, Coupling_Comm[iCode]);
  }
  //~ cout<<"Sended."<<endl;
  return error;
}

/* Communicator::receive

Receive nAlpha values in alpha from client code iCode.

input: code number, number of elements to receive, array of elements to receive
output: error

*/
int Communicator::receive(int iCode, int nAlpha, double* alpha)
{
  //~ cout<<"Receiving data from code: "<<iCode<<"..."<<endl;
  if(sys->code[iCode].connection==NEWTON_MPI_COMMUNICATION){  
    error = MPI_Recv (alpha, nAlpha, MPI_DOUBLE_PRECISION, 0, MPI_ANY_TAG, Coupling_Comm[iCode], MPI_STATUS_IGNORE); 
  }
  // TEST
  //~ cout<<"Received."<<endl;
  //~ for(int i=0; i<nAlpha; i++){
    //~ cout<<" a:"<<alpha[i]<<endl;
  //~ }  
  return error;
}
