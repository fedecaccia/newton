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

#include <sstream> 
#include <fstream> 
#include <iostream>
#include <limits>
#include <string>

using namespace::std;

double* loaddata(string);
void printResults(double*, int, string);

// Global variables
int codeID;

int main(int argc,char **argv)
{    
  if(argc!=3){
    cout<<"ERROR. Running code with bad arguments."<<endl;
    return 0;
  }
  
  // code ID
  stringstream(argv[1]) >> codeID;
  //cout<<"Code ID: "<<codeID<<endl;
  string file = argv[2];
  string fileInput = file+".dat";
  string fileOutput = file+".out";
  
  
  // Variables
  double w, x, y, z;
  
  // Input vector
  double* input;
  
  // Switch code ID
  switch(codeID){
    
    case 0:
      // Load guess
      input = loaddata(fileInput);
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
      printResults(sol, 3, fileOutput);
      break;
    
    case 1:
      // Load guess
      input = loaddata(fileInput);
      w = input[0];
      x = input[1];
      y = input[2];
      
      // Solve system
      z = (20 - 10*w - 9*x - 8*y) / 7.0;
      
      // Send results
      printResults(&z, 1, fileOutput);
      break;
    
    default:
      cout<<"ERROR. Bad code ID running test"<<endl;
      return 0;
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
		cout<<"ERROR writing output file: "<< file<<" from code ID: "<<codeID<<endl;
	}
  outputFile.close();  
}
