/*****************************************************************************\

NEWTON                |
                      |
Multiphysics          | CONTOL ROD
coupling              | MANAGER
maste code            |
                      |

-------------------------------------------------------------------------------

This programm is a client code made for manager control rod positions.
Args: 
  argv[0]: program_name
  argv[1]: string with communication mode
  argv[2]: communication argument (string file name, client ID, color code)
  argv[3]: number of problem

Date: 23 June 2017

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

#undef __FUNCT__
#define __FUNCT__ "main"

#include "control_rod.h"

int codeID;
int colorCode;
int global_rank;
int global_size;
int local_rank;
int local_size;
std::string comm;
std::string commArg;
int problem;
int error=CR_SUCCESS;
int order=CONTINUE;
int tag;
MPI_Group world_group;
MPI_Group roots_group;
MPI_Comm Coupling_Comm;
MPI_Comm Local_Comm;

using namespace::std;

int main(int argc,char **argv)
{ 
  // Checking number of arguments
  if(argc!=4){
    cout<<"ERROR. Running control rod manager code with bad number: "<<argc<<" of arguments."<<endl;
    return 0;
  }

  // MPI initialization
  MPI_Init(NULL, NULL);

  // Communication mode
  comm = argv[1];
  if(comm != "io" && comm != "mpi_port" && comm != "mpi_comm"){
    cout<<"ERROR. Bad connection type in control rod manager"<<endl;
    return CR_ERROR;
  }
  // Communication argument
  commArg = argv[2];

  // Problema number
  stringstream(argv[3]) >> problem;

  controlRod* cr;
  cr = new controlRod();
  try{
    cr->solve();
  }
  catch(int e){
    cout<<"ERROR running control rod manager."<<endl;
    return CR_ERROR;
  }

  mpi_finalize();

  cout<<" Control rod manager finished successfully"<<endl;  
  return CR_SUCCESS;
}


/*-----------------------------------------------------------------------------

Operation mode:



-----------------------------------------------------------------------------*/

controlRod::controlRod()
{
  // Initialization
  switch(problem){
    case POW4K2CR1:
      input = new double[5];
      output = new double[1];
      crPos = new double[1];
      newCrPos = new double[1];
      dCR = new double[1];
      desiredK = 1.0;
      break;

    default:
      cout<<"ERROR. Unknown problem type - controlRod::controlRod"<<endl;
      error = CR_ERROR;
  }
  if(comm=="io"){
    file = commArg;
    fileInput = file+".dat";
    fileOutput = file+".out";  
  }
  else if(comm=="mpi_port"){
    stringstream(commArg) >> codeID;
    // Connection
    mpi_connection();
    // Receive first data
    mpi_receive(input, 1);
    crPos[0] = input[0];
    // Receiving control instruction
    error = mpi_receive_order();
    
    //cout<<"First order: "<<order<<endl;
    if(order!=CONTINUE){    
      cout<<"Fatal error in control rod manager. Aborting."<<endl;
      mpi_disconnect();
      throw CR_ERROR;
    }    
  }
  else if(comm=="mpi_comm"){
    stringstream(commArg) >> colorCode;
    // Communication
    mpi_split_and_comm();
    // Receive first data
    mpi_receive(input, 1);
    crPos[0] = input[0];
    // Receiving control instruction
    error = mpi_receive_order();
    
    //cout<<"First order: "<<order<<endl;
    if(order!=CONTINUE){
      cout<<"Fatal errori in control rod manager. Aborting."<<endl;
      mpi_free();
      throw CR_ERROR;
    } 
  }
}

void controlRod::solve()
{
  do{
    switch(problem){
      case POW4K2CR1:
        // Load guess
        if(comm=="io"){
          input = loaddata(fileInput, 5);
          actualK = input[4];
        }
        else if(comm=="mpi_port" || comm=="mpi_comm"){
          mpi_receive(input, 5);
          actualK = input[4];
        }

        // Compute solution
        
        // Move CR only until +/-100 pcm
        deltaK = actualK-desiredK;
        
        
        if( fabs(deltaK)>0.1 ){
          dCR[0] = 10000*deltaK;
        }
        else if( fabs(deltaK)>0.01 ){
          dCR[0] = 10000*deltaK;
        }
        else if( fabs(deltaK)>0.001 ){
          dCR[0] = 10000*deltaK;
        }
        else{
          dCR[0] = 0;
        }
        

        // Set absolute solution
        newCrPos[0] = crPos[0] + dCR[0];
        
        cout<<"keff: "<<actualK<<" delta: "<<dCR[0]<<" - new pos: "<<newCrPos[0]<<endl;

        // Set solution
        output[0] = newCrPos[0];
        
        // Send results
        if(comm=="io"){                
          printResults(output, 1, fileOutput);
        }
        else if(comm=="mpi_port" || comm=="mpi_comm"){
          mpi_send(output, 1);
          order = mpi_receive_order();         
        }
        
        break;
        
      default:
        cout<<"ERROR. Bad problem number received in arg 3 in control rod manager."<<endl;
        throw CR_ERROR;
    }
  }while(order==RESTART);
  
  if(order==ABORT){
    if(comm=="mpi_port"){
      mpi_disconnect();
    }
    else if(comm=="mpi_comm"){
      mpi_free();
    }
    mpi_finalize();
    cout<<"Finishing program by ABORT order"<<endl;
    throw CR_ERROR;
  }
  
  // Finish connections
  if(comm=="mpi_port"){
    mpi_disconnect();
  }
  else if(comm=="mpi_comm"){
    mpi_free();
  }
}

/* loaddata
Load variables from input file.

input: string file, n values to load
output: data loaded in vector

*/
double* loaddata(string file, int n)
{
  // Values to load
  double* values = new double[n];
  // Input file
  ifstream inputFile;
  
  inputFile.open(file.c_str());
	if (inputFile.is_open()){
    for(int i=0; i<n; i++){
      inputFile >> values[i];
    }
	}
	else{
		cout<<"ERROR reading input file: "<<file<<" - loaddata"<<endl;
	}
  inputFile.close();
  
  return values;
}

/* printResults
Print variables in output file.

input: data in vector, size of vector, output string file
output: -

*/
void printResults(double* vec, int n, string file)
{
  // Output file
  ofstream outputFile;
  outputFile.precision(numeric_limits<double>::digits10 + 1);
  
  outputFile.open(file.c_str());
	if (outputFile.is_open()){
		for(int i=0; i<n; i++){      
      outputFile <<vec[i]<<endl;
    }
	}
	else{
		cout<<"ERROR writing output file: "<< file<<" - printResults"<<endl;
	}
  outputFile.close();  
}

/* mpi_connection
Starts connection by mpi

input: -
output: -

*/

void mpi_connection()
{
  char Port_Name[MPI_MAX_PORT_NAME];
  
  
  // codeID is the code ID
  cout<<" MPI connection with code: "<<codeID<<endl;
  // Service Name is constructed in base of code ID codeID
  string Srvc_NameCPP = "Newton-" + int2str(codeID);
  char* Srvc_Name=(char*)Srvc_NameCPP.c_str();
  cout<<" Looking for service: "<< Srvc_Name<<endl;
  // Connection tries with master code via Coupling_Comm MPI communicator
  int nTries = 0;
  while (nTries < 5){
    error = MPI_Lookup_name (Srvc_Name, MPI_INFO_NULL, Port_Name);
    if (error != 0) {
      cout<<" Can’t find service, trying again"<<endl;
    }
    else{
      break;
    }
    nTries++;
  }
  if (error == 0){
    cout<<" Service found at port: "<<Port_Name<<" after "<< nTries+1<<" tries."<<endl;
    nTries = 0;
  }
  while (nTries < 5){
    cout<<" Connecting..."<<endl;
    error = MPI_Comm_connect(Port_Name, MPI_INFO_NULL, 0, MPI_COMM_SELF, &Coupling_Comm);
    if (error != 0){
      cout<<" Can’t connect to service, re-trying..."<<endl;
    }
    else{
      break;
    }
    nTries = nTries + 1;
  }
  
  if (error != 0){
    cout<<" ERROR: "<<error<<" connecting to server."<<endl;
    throw CR_ERROR;
  }
  cout<<" ...connected."<<endl;
}

void mpi_receive(double* input, int n)
{
  // Values reception
  //cout<<"Receiving "<<n<<" values..."<<endl;
  error = MPI_Recv (input, n, MPI_DOUBLE_PRECISION, 0, MPI_ANY_TAG, Coupling_Comm, MPI_STATUS_IGNORE);
  if (error != 0){
    cout<<"Error receiving data"<<endl;
    throw CR_ERROR;
  }
  //cout<<"Values received."<<endl;
}

int mpi_receive_order()
{
  // Order reception
  //cout<<"Receiving order..."<<endl;
  error = MPI_Recv (&order, 1, MPI_INTEGER, 0, MPI_ANY_TAG, Coupling_Comm, MPI_STATUS_IGNORE);
  if (error != 0){
    cout<<"Error receiving order"<<endl;
    throw CR_ERROR;
  }
  //cout<<"Order received: "<<order<<"."<<endl;
  
  return order;
}

void mpi_send(double* output, int n)
{
  // Sending values
  tag = 0;
  //cout<<"Sending values..."<<endl;
  error = MPI_Send (output, n, MPI_DOUBLE_PRECISION, 0, tag, Coupling_Comm);
  if (error != 0){
    cout<<"Error sending values"<<endl;
    throw CR_ERROR;
  }
  //cout<<"Values sent."<<endl;
}

void mpi_disconnect()
{
  // Disconnecting
  cout<<"Disconnecting..."<<endl;
  error = MPI_Comm_disconnect(&(Coupling_Comm));
  if (error != 0){
    cout<<"Error disconnecting"<<endl;
    throw CR_ERROR;
  }
}

void mpi_split_and_comm()
{
/*-----------------------------------------------------------------------------
            Communicator split, global and local variables
-----------------------------------------------------------------------------*/
  // Get the number of processes in the original communicator
  error = MPI_Comm_size(MPI_COMM_WORLD, &global_size);
  // Get the rank of the process in the original communicator
  error = MPI_Comm_rank(MPI_COMM_WORLD, &global_rank);
//cout<<global_size<<" "<<global_rank<<" from client "<<codeClient<<endl;
  // Split communicator
  error += MPI_Comm_split(MPI_COMM_WORLD, colorCode, global_rank, &Local_Comm);
  // Get the number of processes in the local communicator
  error += MPI_Comm_size(Local_Comm, &local_size);
  // Get the rank of process in the local communicator
  error += MPI_Comm_rank(Local_Comm, &local_rank);
//cout<<" color: "<<colorCode<<" from client "<<codeClient<<endl;
  if(global_rank==0){
    error = CR_ERROR;
    cout<<"Only master root has to have global rank 0"<<endl;
  }
  if (error != 0){
    cout<<"Error spliting"<<endl;
    throw CR_ERROR;
  }
/*  cout<<"WORLD RANK/SIZE: "<<global_rank <<"/ "<<global_size<<"\t LOCAL RANK/SIZE: "<< local_rank<<"/ "<<local_size<<"\n"<<endl;
    if (error != 0){
    cout<<"Error splitting"<<endl;
    throw CR_ERROR;*/

/*-----------------------------------------------------------------------------
            Create world group
-----------------------------------------------------------------------------*/

  // Get the group of processes in MPI_COMM_WORLD 
  MPI_Comm_group(MPI_COMM_WORLD, &world_group);

/*-----------------------------------------------------------------------------
            Create one group for master-root communication
-----------------------------------------------------------------------------*/

  // Create group between master and slave root
  if(local_rank==0){
    int* ranks = new int[2];
    ranks[0] = 0;
    ranks[1] = global_rank;
    MPI_Group_incl(world_group, 2, ranks, &roots_group);
  }

/*-----------------------------------------------------------------------------
            Create one communicator for master-root communication
-----------------------------------------------------------------------------*/

  // Now create a communicator in roots_group
  MPI_Comm_create_group(MPI_COMM_WORLD, roots_group, 0, &Coupling_Comm);
  //cout<<"Communication set in client: "<<codeClient<<endl;

}

void mpi_free()
{
  MPI_Comm_free(&Local_Comm);
  if(local_rank==0){
    MPI_Group_free(&world_group);
    MPI_Group_free(&roots_group);
    MPI_Comm_free(&Coupling_Comm);
  }
}

void mpi_finalize()
{
  // In case that the process was spawned by a parent, he is waiting for fermi to finish with an MPI_Barrier
  // With this order sended, the parent can read the output
  MPI_Comm parent;
     // Obtain an intercommunicator to the parent MPI job
  MPI_Comm_get_parent(&parent);
  // Check if this process is a spawned one and if so enter the barrier
  if (parent != MPI_COMM_NULL)
    MPI_Barrier(parent);
  //cout<<"  Finalizing MPI in cr..."<<endl;
  MPI_Finalize();  
}

/* int2str

Converts integers to strings.

input: int to print
output: string

*/
string int2str (int a)
{
  char str[10];
  sprintf(str, "%d", a);
  string myStr(str);
  return myStr;  
}
