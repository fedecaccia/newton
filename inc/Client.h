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
    int prepareInput(int, std::string, int, double*, std::string);
    int readOutput(int, std::string, int, double*, std::string);
		
	private:
  
    int prepareTestClientInput(std::string, int, double*, std::string);
    int readTestClientOutput(std::string, int, double*, std::string);
    
    int prepareUserClientInput(std::string, int, double*, std::string);
    int readUserClientOutput(std::string, int, double*, std::string);
    
		int error;
};

#endif
