/*****************************************************************************\

NEWTON					      |
                      |
Implicit coupling 		|	TEST
in nonlinear			    |	PROGRAMM
calculations			    |
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

Author: Federico A. Caccia
Date: 17 June 2017

\*****************************************************************************/

#undef __FUNCT__
#define __FUNCT__ "main"

#include "Test.h"

int codeClient;
std::string comm;
std::string commArg;

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

    default:
      cout<<"ERROR. Bad problem number received in arg 1."<<endl;
      return 0;
  }

  return 0;  
}

linear2::linear2()
{

}

void linear2::solve()
{
  switch(codeClient){
    case 0:
      break;
    case 1:
      break;
    default:
      cout<<"ERROR. Bad client number received in arg 2."<<endl;
      throw TEST_ERROR;
  }
}

nonlinear2::nonlinear2()
{

}

void nonlinear2::solve()
{
  switch(codeClient){
    case 0:
      break;
    case 1:
      break;
    default:
      cout<<"ERROR. Bad client number received in arg 2."<<endl;
      throw TEST_ERROR;
  }
}

linear3::linear3()
{

}

void linear3::solve()
{
  switch(codeClient){
    case 0:
      break;
    case 1:
      break;
    default:
      cout<<"ERROR. Bad client number received in arg 2."<<endl;
      throw TEST_ERROR;
  }
}