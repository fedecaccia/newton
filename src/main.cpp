#undef __FUNCT__
#define __FUNCT__ "main"

#include "Newton.h"

int main(int argc,char **argv)
{  
  
  Newton* Coupling = new Newton();

  try{
    (*Coupling).initialize();
  }
  catch(int e){
    return 0;
  }  

  try{
    (*Coupling).run();
  }
  catch(int e){
    //Coupling->NewtonComm->disconnect();
    return 0;
  }

  try{
    (*Coupling).finish();
  }
  catch(int e){
    return 0;
  }

  return 0;
  
}
