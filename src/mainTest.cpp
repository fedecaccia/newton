/*****************************************************************************\

NEWTON                |
                      |
Multiphysics          | TEST
coupling              | CLASSES
maste code            |
                      |

-------------------------------------------------------------------------------

This programm is a client code made for test Newton functionalities.
args:
 - arg[0]: bin name
 - arg[1]: problem number
 - arg[2]: client in the problem number
 - arg[3]: communication mode: ("io", "mpi_port", "mpi_comm")
 - arg[4]: communication arg: ("io": string of input file name)
                              ("mpi_port": int of code ID)
                              ("mpi_comm": string of communicator) ?

Date: 19 June 2017

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

#include "Test.h"

int problemNumber;
int codeClient;
int codeID;
std::string comm;
std::string commArg;
int error=TEST_SUCCESS;
int order=CONTINUE;
int tag;
MPI_Comm Coupling_Comm;

using namespace::std;

int main(int argc,char **argv)
{ 
  // Checking number of arguments
  if(argc!=5){
    cout<<"ERROR. Running test code with bad number of arguments."<<endl;
    return 0;
  }

  // Problem number
  stringstream(argv[1])>>problemNumber;
  // Client in this problem
  stringstream(argv[2])>>codeClient;
  // Communication mode
  comm = argv[3];
  if(comm != "io" && comm != "mpi_port" && comm != "mpi_comm"){
    cout<<"ERROR. Bad connection type"<<endl;
    return TEST_ERROR;
  }
  // Communication argument
  commArg = argv[4];

  switch(problemNumber){

    case TEST_2_LINEAR:
      linear2* system1;
      system1 = new linear2();
      try{
        system1->solve();
      }
      catch(int e){
        cout<<"ERROR running test."<<endl;
        return 0;
      }
      break;

    case TEST_2_NONLINEAR:
      nonlinear2* system2;
      system2 = new nonlinear2();
      try{
        system2->solve();
      }
      catch(int e){
        cout<<"ERROR running test."<<endl;
        return 0;
      }
      break;

    case TEST_3_LINEAR:
      linear3* system3;
      system3 = new linear3();
      try{
        system3->solve();
      }
      catch(int e){
        cout<<"ERROR running test."<<endl;
        return 0;
      }
      break;

    case TEST_3_NONLINEAR:
      nonlinear3* system4;
      system4 = new nonlinear3();
      try{
        system4->solve();
      }
      catch(int e){
        cout<<"ERROR running test."<<endl;
        return 0;
      }
      break;

    case TEST_2_LINEAR_MAPPER:
      linear2mapper* system5;
      system5 = new linear2mapper();
      try{
        system5->solve();
      }
      catch(int e){
        cout<<"ERROR running test."<<endl;
        return 0;
      }
      break;

    default:
      cout<<"ERROR. Bad problem number received in arg 1."<<endl;
      return 0;
  }

  //cout<<" Program finished succesfully"<<endl;  
  return TEST_SUCCESS;
}


/*-----------------------------------------------------------------------------

System to solve:

  1*w +  2*x +  3*y +  4*z = 17   (1a)
 12*w + 13*x + 14*y +  5*z = 18   (2a)
 11*w + 16*x + 15*y +  6*z = 19   (3a)
 10*w +  9*x +  8*y +  7*z = 20   (4a)

Code number 0 calculates (w,x,y) values as function of (z_guess) value readed from 
input, solving the coupled equations (1b,2b,3b):

 w = (17 -  2*x -  3*y - 4*z_guess)/ 1  (1b)
 x = (18 - 12*w - 14*y - 5*z_guess)/13  (2b)
 y = (19 - 11*w - 16*x - 6*z_guess)/15  (3b)

Code number 1 calculates (z) value as function of (w_guess,x_guess,y_guess) values 
readed from input, solving (4b):

 z = (20 - 10*w_guess -  9*x_guess - 8*y_guess)/ 7  (4b)

Analytical solution: 

 w = -0.70909
 x = -1.89091
 y = 2.36364
 z = 3.60000
-----------------------------------------------------------------------------*/

linear2::linear2()
{
  // Initialization
  switch(codeClient){
    case 0:
      input = new double[1];
      output = new double[3];
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
      b = new double[3];
      break;
    case 1:
      input = new double[3];
      output = new double[1];
      b = new double[1];
      break;
    default:
      cout<<"ERROR. Bad client number received in arg 2."<<endl;
      throw TEST_ERROR;
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
      throw TEST_ERROR;
    }    
  }
}

void linear2::solve()
{
  do{
    switch(codeClient){
      case 0:
        // Load guess
        if(comm=="io"){                
          input = loaddata(fileInput, 1);
          z = input[0];
        }
        else if(comm=="mpi_port"){
          mpi_receive(input, 1);
          z = input[0];          
        }
        else if(comm=="mpi_comm"){
          
        }
        // Set b values
        b[0] = 17 - 4*z;
        b[1] = 18 - 5*z;
        b[2] = 19 - 6*z;
        // Solve system
        w = mat[0][0] * b[0] + mat[0][1] * b[1] + mat[0][2] * b[2];
        x = mat[1][0] * b[0] + mat[1][1] * b[1] + mat[1][2] * b[2];
        y = mat[2][0] * b[0] + mat[2][1] * b[1] + mat[2][2] * b[2];
        // Set solution
        output[0] = w;
        output[1] = x;
        output[2] = y;
        // Send results
        if(comm=="io"){                
          printResults(output, 3, fileOutput);
        }
        else if(comm=="mpi_port"){
          mpi_send(output, 3);
          order = mpi_receive_order();         
        }
        else if(comm=="mpi_comm"){         
          
        }
        
        break;
        
      case 1:
        // Load guess
        if(comm=="io"){
          input = loaddata(fileInput, 3);
          w = input[0];
          x = input[1];
          y = input[2];
        }
        else if(comm=="mpi_port"){
          mpi_receive(input, 3);
          w = input[0];
          x = input[1];
          y = input[2];   
        }
        else if(comm=="mpi_comm"){
          
        }
        // Set b values
        b[0] = 20.0/7;
        // Solve system
        z = b[0] - 10.0/7*w - 9.0/7*x - 8.0/7*y;
        // Set solution
        output[0] = z;
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
        cout<<"ERROR. Bad client number received in arg 2."<<endl;
        throw TEST_ERROR;
    }
  }while(order==RESTART);
  
  if(order==ABORT){
    mpi_finish();
    cout<<"Finishing program by ABORT order"<<endl;
    throw TEST_ERROR;
  }
  
  // Finish connections
  if(comm=="mpi_port"){
    mpi_finish();
  }
}


/*-----------------------------------------------------------------------------

System to solve:

  1*w +  2*x +  3*y +  4*z = 17   (1a)
 12*w + 13*x + 14*y +  5*z = 18   (2a)
 11*w + 16*x + 15*y +  6*z = 19   (3a)
 10*w +  9*x +  8*y +  7*z = 20   (4a)

Code number 0 calculates (w,x) values as function of (y_guess, z_guess) value 
readed from input, solving the coupled equations (1b,2b):

 w = (17 -  2*x -  3*y_guess - 4*z_guess)/ 1  (1b)
 x = (18 - 12*w - 14*y_guess - 5*z_guess)/13  (2b)
 
Code number 1 calculates (y) value as function of (w_guess, x_guess, z_guess) 
values readed from input, solving (3b):

 y = (19 - 11*w_guess - 16*x_guess - 6*z_guess)/15  (3b)

Code number 1 calculates (z) value as function of (w_guess, x_guess, y_guess) 
values readed from input, solving (4b):

 z = (20 - 10*w_guess -  9*x_guess - 8*y_guess)/ 7  (4b)

Analytical solution: 

 w = -0.70909
 x = -1.89091
 y = 2.36364
 z = 3.60000
-----------------------------------------------------------------------------*/

linear3::linear3()
{
  // Initialization
  switch(codeClient){
    case 0:
      input = new double[1];
      output = new double[2];
      mat = new double*[2];
      for(int i=0; i<2; i++){
        mat[i] = new double[2];
      }
      mat[0][0] = -1.181818;
      mat[0][1] = 0.181818;
      mat[1][0] = 1.090909;
      mat[1][1] = -0.090909;
      b = new double[2];
      break;
      
    case 1:
      input = new double[3];
      output = new double[1];
      b = new double[1];
      break;
      
    case 2:
      input = new double[3];
      output = new double[1];
      b = new double[1];
      break;
      
    default:
      cout<<"ERROR. Bad client number received in arg 2."<<endl;
      throw TEST_ERROR;
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
      throw TEST_ERROR;
    }    
  }
}

void linear3::solve()
{
  do{
    switch(codeClient){
      case 0:
        // Load guess
        if(comm=="io"){                
          input = loaddata(fileInput, 2);
          y = input[0];
          z = input[1];
        }
        else if(comm=="mpi_port"){
          mpi_receive(input, 1);
          z = input[0];          
        }
        else if(comm=="mpi_comm"){
          
        }
        // Set b values
        b[0] = 17 -  3*y - 4*z;
        b[1] = 18 - 14*y - 5*z;
        // Solve system
        w = mat[0][0] * b[0] + mat[0][1] * b[1];
        x = mat[1][0] * b[0] + mat[1][1] * b[1];
        // Set solution
        output[0] = w;
        output[1] = x;
        // Send results
        if(comm=="io"){                
          printResults(output, 2, fileOutput);
        }
        else if(comm=="mpi_port"){
          mpi_send(output, 3);
          order = mpi_receive_order();         
        }
        else if(comm=="mpi_comm"){         
          
        }
        
        break;
        
      case 1:
        // Load guess
        if(comm=="io"){
          input = loaddata(fileInput, 3);
          w = input[0];
          x = input[1];
          z = input[2];
        }
        else if(comm=="mpi_port"){
          mpi_receive(input, 3);
          w = input[0];
          x = input[1];
          z = input[2];   
        }
        else if(comm=="mpi_comm"){
          
        }
        // Set b values
        b[0] = 19.0/15;
        // Solve system
        y = b[0] - 11.0/15*w - 16.0/15*x - 6.0/15*z;
        // Set solution
        output[0] = y;
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
        
      case 2:
        // Load guess
        if(comm=="io"){
          input = loaddata(fileInput, 3);
          w = input[0];
          x = input[1];
          y = input[2];
        }
        else if(comm=="mpi_port"){
          mpi_receive(input, 3);
          w = input[0];
          x = input[1];
          y = input[2];   
        }
        else if(comm=="mpi_comm"){
          
        }
        // Set b values
        b[0] = 20.0/7;
        // Solve system
        z = b[0] - 10.0/7*w - 9.0/7*x - 8.0/7*y;
        // Set solution
        output[0] = z;
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
        cout<<"ERROR. Bad client number received in arg 2."<<endl;
        throw TEST_ERROR;
    }
  }while(order==RESTART);
  
  if(order==ABORT){
    mpi_finish();
    cout<<"Finishing program by ABORT order"<<endl;
    throw TEST_ERROR;
  }
  
  // Finish connections
  if(comm=="mpi_port"){
    mpi_finish();
  }
}


/*-----------------------------------------------------------------------------

System to solve:

 l*x^2 + m*y = a  (1a)
 l*x + m*y^2 = b  (2a)
   
Code number 0 calculates x value as function of y_guess value readed from 
input, solving the coupled equations (1b):

 x = sqrt((a-m*y_guess)/l)  (1b)
   
Code number 1 calculates y value as function of x_guess  value readed from 
input, solving (2b):

 y = sqrt((b-l*x_guess)/m)  (2b)

Analytical solutions, with l=1, m=1, a=3, b=5:
 x = 1
 y = 2

-----------------------------------------------------------------------------*/

nonlinear2::nonlinear2()
{
  // Initialization
  l = 1;
  m = 1;
  a = 3;
  b = 5;
  input = new double[1];
  output = new double[1];

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
      throw TEST_ERROR;
    }    
  }
}

void nonlinear2::solve()
{
  do{
    switch(codeClient){
      case 0:
        // Load guess
        if(comm=="io"){                
          input = loaddata(fileInput, 1);
          y = input[0];
        }
        else if(comm=="mpi_port"){
          mpi_receive(input, 1);
          y = input[0];          
        }
        else if(comm=="mpi_comm"){
          
        }
        
        // Calculate solution
        x = sqrt((a-m*y)/l);

        // Set solution
        output[0] = x;

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
        
      case 1:
        // Load guess
        if(comm=="io"){                
          input = loaddata(fileInput, 1);
          x = input[0];
        }
        else if(comm=="mpi_port"){
          mpi_receive(input, 1);
          x = input[0];          
        }
        else if(comm=="mpi_comm"){
          
        }

        // Calculate solution
        y = sqrt((b-l*x)/m);

        // Set solution
        output[0] = y;

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
        cout<<"ERROR. Bad client number received in arg 2."<<endl;
        throw TEST_ERROR;
    }
  }while(order==RESTART);
  
  if(order==ABORT){
    mpi_finish();
    cout<<"Finishing program by ABORT order"<<endl;
    throw TEST_ERROR;
  }
  
  // Finish connections
  if(comm=="mpi_port"){
    mpi_finish();
  }
}


/*-----------------------------------------------------------------------------

System to solve:

 l*x + sin(m*y) + cos(n*z) = a          (1a)
 m*y + (l*x)^2 = b                      (2a)
 n*z + l*x*m*y = c                      (3a)
   
Code number 0 calculates x value as function of (y_guess, z_guess) value 
readed from input, solving the coupled equations (1b):

 x = (a - sin(m*y) - cos(n*z)) / l      (1b)
   
Code number 1 calculates y value as function of (x_guess)  value readed from 
input, solving (2b):

 y = (b - (l*x)^2) /m                   (2b)

Code number 2 calculates z value as function of (x_guess, _guess)  value 
readed from input, solving (3b):

 z = cbrt((c - l*x*m*y)/n)              (3b)

One of the solutions, with l=1, m=1, n=1, a = 0.91930, b=3, c=29:
 x = 1
 y = 2
 z = 3

-----------------------------------------------------------------------------*/

nonlinear3::nonlinear3()
{
  // Initialization
  l = 1;
  m = 1;
  n = 1;
  a = 0.91930;
  b = 3;
  c = 29;
  input = new double[2];
  output = new double[1];

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
      throw TEST_ERROR;
    }    
  }
}

void nonlinear3::solve()
{
  do{
    switch(codeClient){
      
      case 0:

        // Load guess
        if(comm=="io"){                
          input = loaddata(fileInput, 2);
          y = input[0];
          z = input[1];
        }
        else if(comm=="mpi_port"){
          mpi_receive(input, 2);
          y = input[0];
          z = input[1];         
        }
        else if(comm=="mpi_comm"){
          
        }
        
        // Calculate solution
        x = (a - sin(m*y) - cos(n*z)) / l;

        // Set solution
        output[0] = x;

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
        
      case 1:

        // Load guess
        if(comm=="io"){                
          input = loaddata(fileInput, 1);
          x = input[0];
        }
        else if(comm=="mpi_port"){
          mpi_receive(input, 1);
          x = input[0];          
        }
        else if(comm=="mpi_comm"){
          
        }

        // Calculate solution
        y = (b - pow(l*x,2)) / m;

        // Set solution
        output[0] = y;

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

      case 2:

        // Load guess
        if(comm=="io"){                
          input = loaddata(fileInput, 2);
          x = input[0];
          y = input[1];
        }
        else if(comm=="mpi_port"){
          mpi_receive(input, 2);
          x = input[0];
          y = input[1];
        }
        else if(comm=="mpi_comm"){
          
        }

        // Calculate solution
        z = cbrt(c-l*x*m*y) / n;

        // Set solution
        output[0] = z;

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

        cout<<"ERROR. Bad client number received in arg 2."<<endl;
        throw TEST_ERROR;
    }
  }while(order==RESTART);
  
  if(order==ABORT){
    mpi_finish();
    cout<<"Finishing program by ABORT order"<<endl;
    throw TEST_ERROR;
  }
  
  // Finish connections
  if(comm=="mpi_port"){
    mpi_finish();
  }
}


/*-----------------------------------------------------------------------------

System to solve:

 a11*w + a12*x + a13*y + a14*z = b1     (1a)
 a21*w + a22*x + a23*y + a24*z = b2     (2a)
 a31*(w+x)     + a33*y + a34*z = b3     (3a)
 a41*(w+x)     + a43*y + a44*z = b4     (4a)

Code number 0 calculates (w,x) values as function of (y_guess, z_guess) value readed from input, solving the coupled equations (1b, 2b):

 w +  = (b1 - (a12*x + a13*y_guess + a14*z_guess) )/a11    (1b)
 x +  = (b2 - (a21*w + a13*y_guess + a14*z_guess) )/a21    (2b)

Code number 1 calculates (y,z) values as function of (alpha_guess = w_guess + 
x_guess) values readed from input, solving (3b, 4b):

 y = (b3 - a31*alpha_guess -a34*z) / a33    (3b)
 z = (b4 - a41*alpha_guess -a43*y) / a44    (3b)

With matrix of the original system A:    
  1   2   3   4
  4   3   2   1
  2   2   3   1
  3   3   4   1
and right hand side b:
  30
  20
  19
  25
Analytical solution is:
  w = 1, x = 2, y = 3, z = 4 
-----------------------------------------------------------------------------*/

linear2mapper::linear2mapper()
{
  // Initialization
  switch(codeClient){

    case 0:
      input = new double[2];
      output = new double[2];
      mat = new double*[2];
      for(int i=0; i<2; i++){
        mat[i] = new double[2];
      }
      mat[0][0] = -0.6;
      mat[0][1] = 0.4;
      mat[1][0] = 0.8;
      mat[1][1] = -0.2;
      b = new double[2];
      break;

    case 1:
      input = new double[2];
      output = new double[2];
      mat = new double*[2];
      for(int i=0; i<2; i++){
        mat[i] = new double[2];
      }
      mat[0][0] = -1;
      mat[0][1] = 1;
      mat[1][0] = 4;
      mat[1][1] = -3;
      b = new double[2];
      break;

    default:
      cout<<"ERROR. Bad client number received in arg 2."<<endl;
      throw TEST_ERROR;
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
      throw TEST_ERROR;
    }    
  }
}

void linear2mapper::solve()
{
  do{
    switch(codeClient){
      case 0:
        // Load guess
        if(comm=="io"){                
          input = loaddata(fileInput, 2);
          y = input[0];
          z = input[1];
        }
        else if(comm=="mpi_port"){
          mpi_receive(input, 2);
          y = input[0];
          z = input[1];
        }
        else if(comm=="mpi_comm"){
          
        }

        // Set b values
        b[0] = 30 - 3*y - 4*z;
        b[1] = 20 - 2*y - 1*z;
        
        // Solve system
        w = mat[0][0] * b[0] + mat[0][1] * b[1];
        x = mat[1][0] * b[0] + mat[1][1] * b[1];

        // Set solution
        output[0] = w;
        output[1] = x;

        // Send results
        if(comm=="io"){                
          printResults(output, 2, fileOutput);
        }
        else if(comm=="mpi_port"){
          mpi_send(output, 2);
          order = mpi_receive_order();         
        }
        else if(comm=="mpi_comm"){         
          
        }
        
        break;
        
      case 1:
        // Load guess
        if(comm=="io"){
          input = loaddata(fileInput, 1);
          alpha = input[0];
        }
        else if(comm=="mpi_port"){
          mpi_receive(input, 1);
          alpha = input[0];
        }
        else if(comm=="mpi_comm"){
          
        }
        // Set b values
        b[0] = 19 - 2*alpha;
        b[1] = 25 - 3*alpha;

        // Solve system
        y = mat[0][0] * b[0] + mat[0][1] * b[1];
        z = mat[1][0] * b[0] + mat[1][1] * b[1];

        // Set solution
        output[0] = y;
        output[1] = z;

        // Send results
        if(comm=="io"){
          printResults(output, 2, fileOutput);
        }
        else if(comm=="mpi_port"){
          mpi_send(output, 2);
          order = mpi_receive_order();         
        }
        else if(comm=="mpi_comm"){
          
        }
        break;
        
      default:
        cout<<"ERROR. Bad client number received in arg 2."<<endl;
        throw TEST_ERROR;
    }
  }while(order==RESTART);
  
  if(order==ABORT){
    mpi_finish();
    cout<<"Finishing program by ABORT order"<<endl;
    throw TEST_ERROR;
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
		cout<<"ERROR reading input file: "<<file<<" from code client: "<<codeClient<<endl;
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
		cout<<"ERROR writing output file: "<< file<<" from code client: "<<codeClient<<endl;
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
    throw TEST_ERROR;
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
    throw TEST_ERROR;
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
    throw TEST_ERROR;
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
    throw TEST_ERROR;
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
    throw TEST_ERROR;
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
