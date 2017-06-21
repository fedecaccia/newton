/*****************************************************************************\

NEWTON                |
                      |
Multiphysics          | MAIN
coupling              | 
maste code            |
                      |

-------------------------------------------------------------------------------

Initialize, run and finish the coupling object that does all the work.

Date: 13 June 2017

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
along with Foobar.  If not, see <http://www.gnu.org/licenses/>.

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
