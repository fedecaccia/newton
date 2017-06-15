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
#include <limits>
#include <string>

class Client
{
	public:
		Client();
    int prepareInput(int, std::string, int, double*, std::string, std::string);
    int readOutput(int, std::string, int, double*, std::string);
		
	private:
  
    int prepareTestClientInput(std::string, int, double*, std::string, std::string);
    int readTestClientOutput(std::string, int, double*, std::string);
    
    int prepareUserClientInput(std::string, int, double*, std::string, std::string);
    int readUserClientOutput(std::string, int, double*, std::string);
    
    int prepareRelapPow2thInput(std::string, int, double*, std::string, std::string);
    int readRelapPow2thOutput(std::string, int, double*, std::string);
    
    int prepareFermiXs2powInput(std::string, int, double*, std::string, std::string);
    int readFermiXs2powOutput(std::string, int, double*, std::string);
    
		int error;
};

#endif
