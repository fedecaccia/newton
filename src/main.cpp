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
    rootPrints("FATAL ERROR. ABORTING.");
  }  

  try{
    (*Coupling).run();
  }
  catch(int e){
    rootPrints("FATAL ERROR. ABORTING.");
  }

  try{
    (*Coupling).finish();
  }
  catch(int e){
    rootPrints("FATAL ERROR. ABORTING.");
  }

  
  return 0;
  
}
