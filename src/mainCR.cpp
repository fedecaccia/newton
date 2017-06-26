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
  argv[2]: communication argument (string file name, client ID)
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
std::string comm;
std::string commArg;
int problem;
int error=CR_SUCCESS;
int order=CONTINUE;
int tag;
MPI_Comm Coupling_Comm;

using namespace::std;

int main(int argc,char **argv)
{ 
  // Checking number of arguments
  if(argc!=4){
    cout<<"ERROR. Running control rod code with bad number: "<<argc<<" of arguments."<<endl;
    return 0;
  }

  // Communication mode
  comm = argv[1];
  if(comm != "io" && comm != "mpi_port" && comm != "mpi_comm"){
    cout<<"ERROR. Bad connection type"<<endl;
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
    cout<<"ERROR running control rod."<<endl;
    return 0;
  }

  cout<<" Program finished succesfully"<<endl;  
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
    // Receiving control instruction
    error = mpi_receive_order();
    
    cout<<"First order: "<<order<<endl;
    if(order!=CONTINUE){    
      cout<<"Fatal error. Aborting."<<endl;
      mpi_finish();
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
        else if(comm=="mpi_port"){
          mpi_receive(input, 5);
          actualK = input[4];
        }
        else if(comm=="mpi_comm"){
          
        }
        // Set b values

        // Compute solution
        desiredK = 1.0;
        // SET ABSOLUT POSITION
        /*if( (actualK-desiredK) > 0.01 || (actualK-desiredK) < -0.01){
          crPos[0] = (actualK-desiredK)*200;
          cout<<actualK<<" "<<actualK-desiredK<<" "<<crPos[0]<<endl;
        }
        else if( (actualK-desiredK) > 0.001 || (actualK-desiredK) < -0.001){
          crPos[0] = (actualK-desiredK)*20000;
          cout<<actualK<<" "<<actualK-desiredK<<" "<<crPos[0]<<endl;
        }
        else if( (actualK-desiredK) > 0.0001 || (actualK-desiredK) < -0.0001){
          crPos[0] = (actualK-desiredK)*20000;
          cout<<actualK<<" "<<actualK-desiredK<<" "<<crPos[0]<<endl;
        }
        else if( (actualK-desiredK) > 0.00001 || (actualK-desiredK) < -0.00001){
          crPos[0] = (actualK-desiredK)*1000000;
          cout<<actualK<<" "<<actualK-desiredK<<" "<<crPos[0]<<endl;
        }
        else{
          crPos[0] = 0;
          cout<<actualK<<" "<<actualK-desiredK<<" "<<crPos[0]<<endl;
        }*/
        crPos[0] = 0;

        // Set solution
        output[0] = crPos[0];
        
        // Send results
        if(comm=="io"){                
          printResults(output, 1, fileOutput);
        }
        else if(comm=="mpi_port"){
          mpi_send(output, 1);
          order = mpi_receive_order();         
        }
        else if(comm=="mpi_comm"){         
          
        }
        
        break;
        
      default:
        cout<<"ERROR. Bad problem number received in arg 3."<<endl;
        throw CR_ERROR;
    }
  }while(order==RESTART);
  
  if(order==ABORT){
    mpi_finish();
    cout<<"Finishing program by ABORT order"<<endl;
    throw CR_ERROR;
  }
  
  // Finish connections
  if(comm=="mpi_port"){
    mpi_finish();
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
  
  MPI_Init(NULL, NULL);
  
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
  cout<<"Receiving "<<n<<" values..."<<endl;
  error = MPI_Recv (input, n, MPI_DOUBLE_PRECISION, 0, MPI_ANY_TAG, Coupling_Comm, MPI_STATUS_IGNORE);
  if (error != 0){
    cout<<"Error receiving data"<<endl;
    throw CR_ERROR;
  }
  cout<<"Values received."<<endl;
}

int mpi_receive_order()
{
  // Order reception
  cout<<"Receiving order..."<<endl;
  error = MPI_Recv (&order, 1, MPI_INTEGER, 0, MPI_ANY_TAG, Coupling_Comm, MPI_STATUS_IGNORE);
  if (error != 0){
    cout<<"Error receiving order"<<endl;
    throw CR_ERROR;
  }
  cout<<"Order received: "<<order<<"."<<endl;
  
  return order;
}

void mpi_send(double* output, int n)
{
  // Sending values
  tag = 0;
  cout<<"Sending values..."<<endl;
  error = MPI_Send (output, n, MPI_DOUBLE_PRECISION, 0, tag, Coupling_Comm);
  if (error != 0){
    cout<<"Error sending values"<<endl;
    throw CR_ERROR;
  }
  cout<<"Values sent."<<endl;
}

void mpi_finish()
{
  // Disconnecting
  cout<<"Disconnecting..."<<endl;
  error = MPI_Comm_disconnect(&(Coupling_Comm));
  if (error != 0){
    cout<<"Error disconnecting"<<endl;
    throw CR_ERROR;
  }
  cout<<"Finalizing MPI..."<<endl;
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
