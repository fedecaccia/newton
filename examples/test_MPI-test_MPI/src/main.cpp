/*****************************************************************************\

NEWTON					      |
                      |
Implicit coupling 		|	TEST
in nonlinear			    |	PROGRAMM
calculations			    |
                      |

-------------------------------------------------------------------------------

This programm is a client code made for test Newton functionalities.
argv[2] gives the code ID. Running the same test programm with different
code IDs simulates differents client codes.
System to solve:

  1*w +  2*x +  3*y +  4*z = 17   (1a)
 12*w + 13*x + 14*y +  5*z = 18   (2a)
 11*w + 16*x + 15*y +  6*z = 19   (3a)
 10*w +  9*x +  8*y +  7*z = 20   (4a)

Code ID 0 calculates (w,x,y) values as function of (z_guess) value readed from 
input, solving the coupled equations (1b,2b,3b):

 w = (17 -  2*x -  3*y - 4*z_guess)/ 1  (1b)
 x = (18 - 12*w - 14*y - 5*z_guess)/13  (2b)
 y = (19 - 11*w - 16*x - 6*z_guess)/15  (3b)

Code ID 1 calculates (z) value as function of (w_guess,x_guess,y_guess) values 
readed from input, solving (4b):

 z = (20 - 10*w_guess -  9*x_guess - 8*y_guess)/ 7  (4b)

Analytical solution: 

 w = -0.70909
 x = -1.89091
 y = 2.36364
 z = 3.60000

Author: Federico A. Caccia
Date: 14 June 2017

\*****************************************************************************/

#undef __FUNCT__
#define __FUNCT__ "main"

#define CONTINUE  0
#define RESTART   1
#define FINISH    2
#define ABORT     3

#include <sstream> 
#include <iostream>
#include <string>
#include <mpi.h>
#include <stdlib.h>

using namespace::std;

double* loaddata(string);
void printResults(double*, int, string);

// Global variables
int codeID, error, tag, order=CONTINUE;
MPI_Comm Coupling_Comm;

void mpi_connection();
void mpi_receive(double*, int);
void mpi_send(double*, int);
void mpi_finish();
int mpi_receive_order();
string int2str (int);

int main(int argc,char **argv)
{    
  if(argc!=2){
    cout<<"ERROR. Running code with bad arguments."<<endl;
    return 0;
  }
  
  // code ID
  stringstream(argv[1]) >> codeID;
  
  // Variables
  double w, x, y, z;
  
  // Input vector
  double* input;
  switch(codeID){
    case 0:
      input = new double[1];
      break;
    case 1:
      input = new double[3];
      break;
    default:
      cout<<"Bad code ID: "<<codeID<<endl;
      cout<<"Fatal error. Aborting."<<endl;
      mpi_finish();
      return 0;
  }
  // Output vector
  double* output;
  switch(codeID){
    case 0:
      output = new double[3];
      break;
    case 1:
      output = new double[1];
      break;
    default:
      cout<<"Bad code ID: "<<codeID<<endl;
      cout<<"Fatal error. Aborting."<<endl;
      mpi_finish();
      return 0;
  }
  
  // Connection
  mpi_connection();
  // Receiving control instruction
  error = mpi_receive_order();
  
  cout<<"First order: "<<order<<endl;
  if(order!=CONTINUE){    
    cout<<"Fatal error. Aborting."<<endl;
    mpi_finish();
    return 0;
  }
  
  do{
    // Switch code ID
    switch(codeID){
      
      case 0:
        // Load guess
        mpi_receive(input, 1);
        
        // Set values
        z = input[0];
        
        // Inverted martix of the system
        double** mat;
        mat = new double*[3];
        for(int i=0; i<3; i++){
          mat[i] = new double[3];
        }
        mat[0][0] = -0.439394;
        mat[0][1] = 0.272727;
        mat[0][2] = -0.166667;
        mat[1][0] = -0.393939;
        mat[1][1] = -0.272727;
        mat[1][2] = 0.333333;
        mat[2][0] = 0.742424;
        mat[2][1] = 0.090909;
        mat[2][2] = -0.166667;
        
        // System rhs
        double* b;
        b = new double[3];
        b[0] = 17 - 4*z;
        b[1] = 18 - 5*z;
        b[2] = 19 - 6*z;
        
        // Solve system
        w = mat[0][0] * b[0] + mat[0][1] * b[1] + mat[0][2] * b[2];
        x = mat[1][0] * b[0] + mat[1][1] * b[1] + mat[1][2] * b[2];
        y = mat[2][0] * b[0] + mat[2][1] * b[1] + mat[2][2] * b[2];
        output[0] = w;
        output[1] = x;
        output[2] = y;
        
        // Send results
        mpi_send(output, 3);
        break;
      
      case 1:
        // Load guess
        mpi_receive(input, 3);

        w = input[0];
        x = input[1];
        y = input[2];
        
        // Solve system
        z = (20 - 10*w - 9*x - 8*y) / 7.0;
        output[0]=z;
        
        // Send results
        mpi_send(output, 1);
        break;
      
      default:
        cout<<"ERROR. Bad code ID running test"<<endl;
        return 0;
    }
    
    order = mpi_receive_order();
    
  }while(order==RESTART);
  
  if(order==ABORT){
    mpi_finish();
    cout<<"Finishing program by ABORT order"<<endl;
    return 0;
  }
  
  // Finish connections
  mpi_finish();
  
  cout<<" Program finished succesfully"<<endl;

  return 0;
  
}

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
    exit(1);
  }
  cout<<" ...connected."<<endl;

}

void mpi_receive(double* input, int n)
{
  // Values reception
  cout<<"Receiving "<<n<<" values..."<<endl;
  error = MPI_Recv (input, n, MPI_DOUBLE_PRECISION, 0, MPI_ANY_TAG, Coupling_Comm, MPI_STATUS_IGNORE);
  cout<<"Values received."<<endl;
}

int mpi_receive_order()
{
  // Order reception
  cout<<"Receiving order..."<<endl;
  error = MPI_Recv (&order, 1, MPI_INTEGER, 0, MPI_ANY_TAG, Coupling_Comm, MPI_STATUS_IGNORE);
  cout<<"Order received: "<<order<<"."<<endl;
  
  return order;
}

void mpi_send(double* output, int n)
{
  // Sending values
  tag = 0;
  cout<<"Sending values..."<<endl;
  error = MPI_Send (output, n, MPI_DOUBLE_PRECISION, 0, tag, Coupling_Comm);
  cout<<"Values sent."<<endl;
}

void mpi_finish()
{
  // Disconnecting
  cout<<"Disconnecting..."<<endl;
  error = MPI_Comm_disconnect(&(Coupling_Comm));
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
  ostringstream temp;
  temp<<a;
  return temp.str();
}
