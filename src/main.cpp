/*****************************************************************************\

NEWTON                |
                      |
Multiphysics          | MAIN
coupling              | 
maste code            |
                      |

-------------------------------------------------------------------------------

Initialize, run and finish the coupling object that does all the work.

Author: Federico A. Caccia
Date: 13 June 2017

\*****************************************************************************/
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
    (*Coupling).finish();
    return 0;
  }

  try{
    (*Coupling).run();
  }
  catch(int e){
    (*Coupling).finish();
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
