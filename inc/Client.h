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

#include <sstream> 
#include <fstream> 
#include <iostream>
#include <limits>
#include <string>

class Client
{
	public:
		Client();
    void allocate1();
    void allocate2();
    void updateVars(double);
    int prepareInput(int, std::string, int, double*, std::string, std::string);
    int readOutput(int, std::string, int, double*, std::string);
    
    int nFermi;
    struct fermiStruct{
      std::string name;
      int nPhysicalEntities;
      int nGroups;
      int nXS;
      struct physicalEntity{
        std::string name;
        double burnup;
        double** xs;
        double* EFissionRate;
      };
      physicalEntity* pe;
    };
    fermiStruct* fermi;
    
    int nRelap;
    struct relapStruct{
      std::string name;
      int nAxialZones;
      int nRadialPoints;
      double t0;
      double tf;
      double t;
      double maxDt;
      std::string* hs;
      std::string* pipe;
      double* fpow;
      double voidf;
      double voidg;
      double tempf;
      double tempg;
      double* tempRef;
      double* rhomix;
      double tempFuelC;
      double tempFuelV;
      double* tempDoppFuel;
    };
    relapStruct* relap;
		
	private:
  
    int prepareTestClientInput(std::string, int, double*, std::string, std::string);
    int readTestClientOutput(std::string, int, double*, std::string);
    
    int prepareUserClientInput(std::string, int, double*, std::string, std::string);
    int readUserClientOutput(std::string, int, double*, std::string);
    
    int prepareRelapPow2thInput(std::string, int, double*, std::string, std::string);
    int readRelapPow2thOutput(std::string, int, double*, std::string);
    
    int prepareFermiXs2powInput(std::string, int, double*, std::string, std::string);
    int readFermiXs2powOutput(std::string, int, double*, std::string);
    
    std::string takeNextWord();
    std::string takeNextLine();
    
		int error;
    std::string word;
    std::string line;
    std::ofstream inputFile;
    std::ifstream inputModelFile;
    std::ifstream outputFile;

    int nValuesExpected;
};

#endif
