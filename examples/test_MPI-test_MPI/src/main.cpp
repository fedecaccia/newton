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
Date: 7 June 2017

\*****************************************************************************/

#undef __FUNCT__
#define __FUNCT__ "main"

#define CONTINUE 0
#define RESTART 1
#define ABORT 2

#include <sstream> 
#include <fstream> 
#include <iostream>
#include <iomanip>
#include <string>
#include <mpi.h>
#include <stdlib.h>

using namespace::std;

double* loaddata(string);
void printResults(double*, int, string);

// Global variables
int codeID, error, order=CONTINUE;
MPI_Comm Coupling_Comm;

void mpi_connection();
void mpi_receive(double*, int);
void mpi_send(double*, int);
void mpi_finish();
string int2str (int);

int main(int argc,char **argv)
{    
  if(argc!=4){
    cout<<"ERROR. Running code with bad arguments."<<endl;
    return 0;
  }
  
  // code ID
  stringstream(argv[1]) >> codeID;
  //cout<<"Code ID: "<<codeID<<endl;
  string file = argv[2];
  string fileInput = file+".dat";
  string fileOutput = file+".out";
  
  string connection = argv[3];
  
  // Variables
  double w, x, y, z;
  
  // Input vector
  double* input;
  
  // Necessary connections
  if(connection=="mpi"){
    mpi_connection();
  }  
  
    do{
    // Switch code ID
    switch(codeID){
      
      case 0:
        // Load guess
        if(connection=="io"){
          input = loaddata(fileInput);
        }
        else if(connection=="mpi"){
          mpi_receive(input, 1);
        }
        else{
          cout<<"Unknown connection type: \""<<connection<<"\""<<endl;
        }
        
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
        double* sol;
        sol = new double[3];
        sol[0] = w;
        sol[1] = x;
        sol[2] = y;
        // Send results
        if(connection=="io"){
          printResults(sol, 3, fileOutput);
        }
        else if(connection=="mpi"){
          mpi_send(sol, 3);
        }
        else{
          cout<<"Unknown connection type: \""<<connection<<"\""<<endl;
        }
        break;
      
      case 1:
        // Load guess
        if(connection=="io"){
          input = loaddata(fileInput);
        }
        else if(connection=="mpi"){
          mpi_receive(input, 3);
        }
        else{
          cout<<"Unknown connection type: \""<<connection<<"\""<<endl;
        }
        w = input[0];
        x = input[1];
        y = input[2];
        
        // Solve system
        z = (20 - 10*w - 9*x - 8*y) / 7.0;
        
        // Send results
        if(connection=="io"){
          printResults(&z, 1, fileOutput);
          }
        else if(connection=="mpi"){
          mpi_send(sol, 1);
        }
        else{
          cout<<"Unknown connection type: \""<<connection<<"\""<<endl;
        }
        break;
      
      default:
        cout<<"ERROR. Bad code ID running test"<<endl;
        return 0;
    }
  }while(order==RESTART);
  
  // Finish connections
  if(connection=="mpi"){
    mpi_finish();
  }

  return 0;
  
}


/* loaddata
Load variables from input file.

input: string file
output: data loaded in vector

*/
double* loaddata(string file )
{
  // Values to load
  double* values;
  // Input file
  ifstream inputFile;
  
  inputFile.open(file.c_str());
	if (inputFile.is_open()){
		switch (codeID){
      case 0:
        values = new double[1];
        inputFile >> values[0];        
        break;
      
      case 1:
        values = new double[3];
        inputFile >> values[0];      
        inputFile >> values[1];      
        inputFile >> values[2];
        break;      
    }
	}
	else{
		cout<<"ERROR reading input file: "<< file<<" from code ID: "<<codeID<<endl;
    exit(1);
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
  
  outputFile.open(file.c_str());
	if (outputFile.is_open()){
		for(int i=0; i<n; i++){
      outputFile << setprecision(9)<<vec[i]<<endl;
    }
	}
	else{
		cout<<"ERROR writing output file: "<< file<<" from code ID: "<<codeID<<endl;
    exit(1);
	}
  outputFile.close();  
}


void mpi_connection()
{
  char* Port_Name;
  
  MPI_Init(NULL, NULL);
  
  // codeID is the code ID
  cout<<" MPI connection with code: "<<codeID<<endl;
  // Service Name is constructed in base of code ID codeID
  string Srvc_NameCPP = "Coupling_C" + int2str(codeID);
  char* Srvc_Name=(char*)Srvc_NameCPP.c_str();
  cout<<" Looking for service: "<< Srvc_Name<<endl;
  // Connection tries with master code via Coupling_Comm MPI communicator
  int nTries = 0;
  while (nTries < 5){
    error = MPI_Lookup_name (Srvc_Name, MPI_INFO_NULL, Port_Name);
    if (error == 0) {
      cout<<" Can’t find service, trying again"<<endl;
    }
    nTries++;
  }
  if (error == 0){
    cout<<" Service found at port: "<<Port_Name<<" After"<< nTries+1<<" tries."<<endl;
    nTries = 0;
  }
  while (nTries < 5){
    cout<<" Connecting..."<<endl;
    error = MPI_Comm_connect(Port_Name, MPI_INFO_NULL, 0, MPI_COMM_SELF, &Coupling_Comm);
    if (error == 0){
      cout<<" Can’t connect to service, re-trying..."<<endl;
    }  
    nTries = nTries + 1;
  }
  
  if (error != 0){
    cout<<" ERROR: "<<error<<" connecting to server."<<endl;
    exit(1);
  }
  cout<<" ...connected."<<endl;
  
  // Receiving control instruction:
  // 0: continue / 1: restart step / 2: abort
  error = MPI_Recv (&order, 1, MPI_DOUBLE_PRECISION, 0, MPI_ANY_TAG, Coupling_Comm, MPI_STATUS_IGNORE);  
}

void mpi_receive(double* input, int n)
{
  // Values reception
  error = MPI_Recv (input, n, MPI_DOUBLE_PRECISION, 0, MPI_ANY_TAG, Coupling_Comm, MPI_STATUS_IGNORE);
}

void mpi_send(double* output, int n)
{
  // Sending values
  error = MPI_Send (output, n, MPI_DOUBLE_PRECISION, 0, MPI_ANY_TAG, Coupling_Comm);
  // Order reception
  error = MPI_Recv (&order, 1, MPI_INTEGER, 0, MPI_ANY_TAG, Coupling_Comm, MPI_STATUS_IGNORE);


}

void mpi_finish()
{
  // Disconnecting
  error = MPI_Comm_disconnect(&(Coupling_Comm));
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
