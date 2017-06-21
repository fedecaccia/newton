/*****************************************************************************\

NEWTON                |
                      |
Multiphysics          | CLASS
coupling              | COMMUNICATOR
maste code            |
                      |

-------------------------------------------------------------------------------

Stablish mpi communitaction with clients.

Date: 14 June 2017

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
  
  // First communication
  error = firstCommunication();
  
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
	
    // Clients are waiting for order at the end of the evolution step
    int order;    
    if(evol->status==NEWTON_COMPLETE){
      order = NEWTON_FINISH;
    }
    else{
      order = NEWTON_ABORT;
    }     
    error += sendOrder(order);   
  
    if(irank==NEWTON_ROOT){
      for(int iCode=0; iCode<sys->nCodes; iCode++){
        if(sys->code[iCode].connection==NEWTON_MPI_COMMUNICATION){
          char portname[MPI_MAX_PORT_NAME];
          strcpy(portname, Port_Name[iCode].c_str());

          error += MPI_Comm_disconnect(&Coupling_Comm[iCode]);
          error += MPI_Close_port(portname);
          rootPrints("Communication with code id: "+int2str(sys->code[iCode].id)+" closed");
        }
      }
    }
    isConnectedByMPI=NEWTON_DISCONNECTED;
  }  

	checkError(error,"Error finishing communication.");
}

/* Communicator::firstCommunication

Share important data in first communication with code iCode.

input: code number
output: error

*/
int Communicator::firstCommunication()
{    
  if(irank==NEWTON_ROOT){
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
  if(irank==NEWTON_ROOT){    
    for(int iCode=0; iCode<sys->nCodes; iCode++){      
      if(sys->code[iCode].connection==NEWTON_MPI_COMMUNICATION){
        //~ cout<<"Sending order to code: "<<sys->code[iCode].name<<"..."<<endl;
        tag = 0;
        error = MPI_Send (&order, 1, MPI_INTEGER, 0, tag, Coupling_Comm[iCode]);    
        //~ cout<<"Sended."<<endl;
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
  if(irank==NEWTON_ROOT){
    if(sys->code[iCode].connection==NEWTON_MPI_COMMUNICATION){
        // TEST
      //~ cout<<"Sending data to code: "<<sys->code[iCode].name<<"..."<<endl;
      //~ for(int i=0; i<nDelta; i++){
        //~ cout<<" d:"<<delta[i]<<endl;
      //~ }
      tag = 0;
      error = MPI_Send (delta, nDelta, MPI_DOUBLE_PRECISION, 0, tag, Coupling_Comm[iCode]);
      //~ cout<<"Sended."<<endl;
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
  if(irank==NEWTON_ROOT){    
    if(sys->code[iCode].connection==NEWTON_MPI_COMMUNICATION){
      //~ cout<<"Receiving data from code: "<<sys->code[iCode].name<<"..."<<endl;  
      error = MPI_Recv (alpha, nAlpha, MPI_DOUBLE_PRECISION, 0, MPI_ANY_TAG, Coupling_Comm[iCode], MPI_STATUS_IGNORE); 
    }
    //~ // TEST
    //~ cout<<"Received."<<endl;
    //~ for(int i=0; i<nAlpha; i++){
      //~ cout<<" a:"<<alpha[i]<<endl;
    //~ }
  }    
  return error;
}
