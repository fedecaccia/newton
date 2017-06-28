/*****************************************************************************\

NEWTON                |
                      |
Multiphysics          | CLASS
coupling              | COMMUNICATOR
maste code            |
                      |

-------------------------------------------------------------------------------

Stablish mpi communitaction with clients.

Date: 27 June 2017

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

  // Initial amount of codes connected by special communicators.
  nCommCodes = 0;
  
  // Initial state of error
	error = NEWTON_SUCCESS;
}

/* Communicator::allocate
Set number of clients connected by communicators.

input: -
output: -

*/
void Communicator::allocate()
{
  clientRootGlobalRank = new int[nCommCodes];
  roots_group = new MPI_Group[nCommCodes];
  codeComm = new int[nCommCodes];
  int codeConnected = -1;
  for(int iCode=0; iCode<sys->nCodes; iCode++){
    if(sys->code[iCode].connection==NEWTON_MPI_COMM){
      if(sys->code[iCode].rootGlobalRank<1){
        error = NEWTON_ERROR;
        checkError(error, "Bad root global rank in code: "+int2str(iCode)+" - Communicator::allocate");
      }
      codeConnected++;
      if(codeConnected==nCommCodes){
        error = NEWTON_ERROR;
        checkError(error, "Error setting code roots - Communicator::allocate ");
      }
      if(sys->code[iCode].rootGlobalRank>global_size-1){
        error = NEWTON_ERROR;
        checkError(error, "Bad client global rank: "+int2str(sys->code[iCode].rootGlobalRank) +" in client: "+int2str(iCode)+". Remember you should run Newton in MIMD paradigm - Communicator::allocate");
      }
      clientRootGlobalRank[codeConnected] = sys->code[iCode].rootGlobalRank;
      codeComm[codeConnected] = iCode;
    }
  }
  debug.log("Allocating "+int2str(nCommCodes)+" communications by colors\n\n");

}

/* Communicator::initialize
Starts the communication.

input: System pointer
output: -

*/
void Communicator::initialize()
{
  rootPrints("Checking communication with clients...");
  
/*-----------------------------------------------------------------------------
            Publishing ports
-----------------------------------------------------------------------------*/

  // Only roots stablish communication
  if(local_rank==NEWTON_ROOT){
    Port_Name = new string[sys->nCodes];
    
    for(int iCode=0; iCode<sys->nCodes; iCode++){      
      
      if(sys->code[iCode].connection==NEWTON_MPI_PORT){
        char portname[MPI_MAX_PORT_NAME];        
        error += MPI_Open_port(MPI_INFO_NULL, portname);
        Port_Name[iCode].assign(portname);
        
        string Srvc_NameCPP= "Newton-"+int2str(sys->code[iCode].id);
        rootPrints("Publishing service:"+Srvc_NameCPP);
        const char* Srvc_Name=(char*)Srvc_NameCPP.c_str();
        error += MPI_Publish_name(Srvc_Name, MPI_INFO_NULL, portname);

        debug.log("Publising port: "+Port_Name[iCode]+" to code: "+int2str(iCode)+"\n");
      }    
    }
  }  
  checkError(error, "Error publishing services with clients by MPI");

/*-----------------------------------------------------------------------------
            Creating communicator
-----------------------------------------------------------------------------*/
  
  if(local_rank==NEWTON_ROOT){
    Coupling_Comm = new MPI_Comm[sys->nCodes];
  }

/*-----------------------------------------------------------------------------
            Accepting port communication
-----------------------------------------------------------------------------*/

  if(local_rank==NEWTON_ROOT){
    for(int iCode=0; iCode<sys->nCodes; iCode++){
      if(sys->code[iCode].connection==NEWTON_MPI_PORT){
        char portname[MPI_MAX_PORT_NAME];
        strcpy(portname, Port_Name[iCode].c_str());
        error += MPI_Comm_accept(portname, MPI_INFO_NULL, 0, MPI_COMM_SELF, &Coupling_Comm[iCode]);
        rootPrints("Connection with code iD: "+int2str(sys->code[iCode].id)+" accepted.");
        string Srvc_NameCPP= "Newton-"+int2str(sys->code[iCode].id);
        char* Srvc_Name=(char*)Srvc_NameCPP.c_str();
        error += MPI_Unpublish_name(Srvc_Name, MPI_INFO_NULL, portname);
        isConnectedByMPI=NEWTON_CONNECTED;

        debug.log("Communication accepted with code: "+int2str(iCode)+"\n");
      }
    }
  }
  checkError(error, "Error accepting communication with clients by MPI");  

/*-----------------------------------------------------------------------------
            Create world group
-----------------------------------------------------------------------------*/

  // Get the group of processes in MPI_COMM_WORLD 
  error = MPI_Comm_group(MPI_COMM_WORLD, &world_group);
  checkError(error, "Error creating world group - Communicator::initialize"); 
  debug.log("World group created\n");

/*-----------------------------------------------------------------------------
            Create one group for each master-root communication
-----------------------------------------------------------------------------*/

  if(local_rank==NEWTON_ROOT){

    int* ranks = new int[2];
    ranks[0] = local_rank;
    for(int i=0; i<nCommCodes; i++){
      ranks[1] = clientRootGlobalRank[i];

      /* This MPI_Group_incl only runs if client root ranks provided are real,
      I mean, mpirun should has be executed in MIMD paradigm */

      error = MPI_Group_incl(world_group, 2, ranks, &roots_group[i]);
      debug.log("Master-client root group created with client root global rank:"+int2str(ranks[1])+"\n");
    }
  }
  checkError(error, "Error creating group - Communicator::initialize"); 

/*-----------------------------------------------------------------------------
            Create one communicator for each master-root communication
-----------------------------------------------------------------------------*/

  if(local_rank==NEWTON_ROOT){
    for(int i=0; i<nCommCodes; i++){
      error = MPI_Comm_create_group(MPI_COMM_WORLD, roots_group[i], 0, &Coupling_Comm[codeComm[i]]);
      isConnectedByMPI=NEWTON_CONNECTED;
      debug.log("Master-client root communicator created with client :"+int2str(codeComm[i])+"\n");
    }
  }
  checkError(error, "Error creating group communicator - Communicator::initialize"); 


/*-----------------------------------------------------------------------------
            First Newton->client communication
-----------------------------------------------------------------------------*/

  // First communication
  error = firstCommunication();

  checkError(error, "Error sharing first data with clients by MPI - Communicator::initialize");  
}

/* Communicator::disconnnect
Finishes the communication.

input: -
output: -

*/
void Communicator::disconnect()
{
  error = NEWTON_SUCCESS;
	if(isConnectedByMPI==NEWTON_CONNECTED){
	
    // Clients are waiting for order at the end of the evolution step
    int order;    
    if(evol->status==NEWTON_COMPLETE){
      order = NEWTON_FINISH;
    }
    else{
      order = NEWTON_ABORT;
    }     
    error += sendOrder(order);   
  
    if(local_rank==NEWTON_ROOT){

      // Disconnect ports
      for(int iCode=0; iCode<sys->nCodes; iCode++){
        if(sys->code[iCode].connection==NEWTON_MPI_PORT){
          char portname[MPI_MAX_PORT_NAME];
          strcpy(portname, Port_Name[iCode].c_str());

          error += MPI_Comm_disconnect(&Coupling_Comm[iCode]);
          error += MPI_Close_port(portname);
          debug.log("Communication with code id: "+int2str(sys->code[iCode].id)+" closed\n");
        }
      }
      
      // Free groups and communicators
      MPI_Group_free(&world_group);
      debug.log("World group released\n");
      for(int i=0; i<nCommCodes; i++){
        MPI_Group_free(&roots_group[i]);
        MPI_Comm_free(&Coupling_Comm[codeComm[i]]);
        debug.log("Group with code id: "+int2str(codeComm[i])+" released\n");
        debug.log("Communicator with code id: "+int2str(codeComm[i])+" released\n");
      }

    }
    isConnectedByMPI=NEWTON_DISCONNECTED;
    if(error!=NEWTON_SUCCESS){
      rootPrints("Error finishing communication.");
    }
  }

}

/* Communicator::firstCommunication

Share important data in first communication with code iCode.

input: code number
output: error

*/
int Communicator::firstCommunication()
{    
  if(local_rank==NEWTON_ROOT){
    for(int iCode=0; iCode<sys->nCodes; iCode++){
  
      switch(sys->code[iCode].type){
        
        case TEST:          
          break;
          
        case RELAP_POW2TH:
          break;
          
        case FERMI_XS2POW:
          break;
          
        case USER_CODE:
          break;    
      }
    }
  }
  
  error = sendOrder(NEWTON_CONTINUE);
  
  return error;
}
/* Communicator::sendOrder

Send order to client code iCode.

input: code number, order
output: error

*/
int Communicator::sendOrder(int order)
{
  if(local_rank==NEWTON_ROOT){    
    for(int iCode=0; iCode<sys->nCodes; iCode++){      
      if(sys->code[iCode].connection==NEWTON_MPI_PORT ){
        debug.log("Sending order "+ int2str(order)+" to code: "+sys->code[iCode].name+" by MPI_Port...\n");
        tag = 0;
        error = MPI_Send (&order, 1, MPI_INTEGER, 0, tag, Coupling_Comm[iCode]);    
        debug.log("Sended\n");
      }   
      else if(sys->code[iCode].connection==NEWTON_MPI_COMM ){
        debug.log("Sending order "+ int2str(order)+" to code: "+sys->code[iCode].name+" by MPI_Comm...\n");
        tag = 0;
        error = MPI_Send (&order, 1, MPI_INTEGER, 1, tag, Coupling_Comm[iCode]);    
        debug.log("Sended\n");
      }      
    }
    
  }
  return error;
}

/* Communicator::send

Send nDelta values from delta to client code iCode.

input: code number, number of elements to send, array of elements to send
output: error

*/
int Communicator::send(int iCode, int nDelta, double* delta)
{
  if(local_rank==NEWTON_ROOT){
    if(sys->code[iCode].connection==NEWTON_MPI_PORT){
      debug.log("Sending data to code: "+sys->code[iCode].name+" by MPI_Port...\n");
      for(int i=0; i<nDelta; i++){
        debug.log(" delta: "+dou2str(delta[i]));
      }
      debug.log("\n");
      tag = 0;
      error = MPI_Send (delta, nDelta, MPI_DOUBLE_PRECISION, 0, tag, Coupling_Comm[iCode]);
      debug.log("Sended\n");
    }
    if(sys->code[iCode].connection==NEWTON_MPI_COMM){
      debug.log("Sending data to code: "+sys->code[iCode].name+" by MPI_Port...\n");
      for(int i=0; i<nDelta; i++){
        debug.log(" delta:"+dou2str(delta[i]));
      }
      debug.log("\n");
      tag = 0;
      error = MPI_Send (delta, nDelta, MPI_DOUBLE_PRECISION, 1, tag, Coupling_Comm[iCode]);
      debug.log("Sended\n");
    }
  }

  return error;
}

/* Communicator::receive

Receive nAlpha values in alpha from client code iCode.

input: code number, number of elements to receive, array of elements to receive
output: error

*/
int Communicator::receive(int iCode, int nAlpha, double* alpha)
{
  if(local_rank==NEWTON_ROOT){    
    if(sys->code[iCode].connection==NEWTON_MPI_PORT){
      debug.log("Receiving data from code: "+sys->code[iCode].name+" by MPI_Port...\n");
      error = MPI_Recv (alpha, nAlpha, MPI_DOUBLE_PRECISION, 0, MPI_ANY_TAG, Coupling_Comm[iCode], MPI_STATUS_IGNORE); 
    }
    else if(sys->code[iCode].connection==NEWTON_MPI_COMM){
      debug.log("Receiving data from code: "+sys->code[iCode].name+" by MPI_Port...\n"); 
      error = MPI_Recv (alpha, nAlpha, MPI_DOUBLE_PRECISION, 1, MPI_ANY_TAG, Coupling_Comm[iCode], MPI_STATUS_IGNORE); 
    }

    debug.log("Values received:\n");
    for(int i=0; i<nAlpha; i++){
      debug.log(dou2str(alpha[i]));
    }
    debug.log("\n");
  }    
  return error;
}
