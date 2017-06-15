/*****************************************************************************\

NEWTON                 |
                       |
Implicit coupling      |    CLASS
in nonlinear           |    PARSER
calculations           |
                       |

-------------------------------------------------------------------------------

Parser reads the input newton file and load all necessary data to solve the 
problem.

Author: Federico A. Caccia
Date: 8 June 2017

\*****************************************************************************/

#ifndef PARSER_H
#define PARSER_H

#include "global.h"
#include "Client.h"
#include "Evolution.h"
#include "Solver.h"
#include "System.h"

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <algorithm>

class Parser
{
	public:
		Parser();
		void parseInput(System*, Evolution*, Solver*, Client*);
		void checkConsistency(System*);

	private:
		bool isAComment(std:: string);
		std::string takeNextWord();
    bool wordIsCard(std::string, std::string);
	  bool wordIsForbidden(std::string);

	  void checkImportantCards(System*, Solver*);
	  void checkClientProperties(System*, int);
    void checkConsistencyInPhases(System*);

		std::string loadClientAndTakeWord(System*);
		std::string loadCalcsAndTakeWord(System*);
		std::string loadGuessesAndTakeWord(System*);

		int error;
    string delim;
	  bool unkSaved;
    int globalRes;
		std::string word;
		std::ifstream configFile;
		int clientReaded;
    int fermiReaded;
    int relapReaded;
		int betaLoaded;
		int gammaLoaded;

};

#endif
