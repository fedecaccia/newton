/*****************************************************************************\

NEWTON					      |
                      |
Implicit coupling 		|	CLASS
in nonlinear			    |	CLIENT
calculations			    |
                      |

-------------------------------------------------------------------------------

In this class the user can programm specific routines to read output and write
inputs for a particular client code.

Author: Federico A. Caccia
Date: 7 June 2017

\*****************************************************************************/

#ifndef CLIENT_H
#define CLIENT_H

#include "global.h"
#include "System.h"

#include <sstream> 
#include <fstream> 
#include <iostream>
#include <string>

class Client
{
	public:
		Client();
    int prepareInput(int, System*);
    int readOutput(int, System*);
		
	private:
  
    int prepareTestClientInput(int, int, double*, string);
    int readTestClientOutput(int, int, double*, string);
    
    int prepareUserClientInput(int, int, double*, string);
    int readUserClientOutput(int, int, double*, string);
    
		int error;
};

#endif
