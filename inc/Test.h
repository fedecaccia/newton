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

Author: Federico A. Caccia
Date: 17 June 2017

\*****************************************************************************/

#ifndef TEST_H
#define TEST_H

#define TEST_SUCCESS 					0
#define TEST_ERROR			 			1

#define TEST_2_LINEAR 				0
#define TEST_2_NONLINEAR 			1
#define TEST_3_LINEAR 				2

#include <sstream> 
#include <fstream> 
#include <iostream>
#include <limits>
#include <string>

int problemNumber;
extern int codeClient;
extern std::string comm;
extern std::string commArg;

class linear2
{
	public:
		linear2();
		void solve();

	private:
		
};

class nonlinear2
{
	public:
		nonlinear2();
		void solve();

	private:
		//extern int codeClient;
		//extern std::string comm;
		//extern std::string commArg;
		
};

class linear3
{
	public:
		linear3();
		void solve();

	private:
		
};





#endif