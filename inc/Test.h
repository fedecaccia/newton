/*****************************************************************************\

NEWTON                |
                      |
Multiphysics          | TEST
coupling              | HEADER
maste code            |
                      |

-------------------------------------------------------------------------------

This programm is a client code made for test Newton functionalities.
args:

Author: Federico A. Caccia
Date: 17 June 2017

\*****************************************************************************/

#ifndef TEST_H
#define TEST_H

#define TEST_SUCCESS 					    0
#define TEST_ERROR			 			    1
    
#define TEST_2_LINEAR 				    0
#define TEST_3_LINEAR 				    1
#define TEST_2_NONLINEAR 			    2
#define TEST_3_NONLINEAR          3
#define TEST_2_LINEAR_MAPPER      4

#define CONTINUE                  0
#define RESTART                   1
#define FINISH                    2
#define ABORT                     3

#include <sstream> 
#include <fstream> 
#include <iostream>
#include <limits>
#include <string>
#include <mpi.h>
#include <math.h>

std::string int2str (int);
double* loaddata(std::string, int);
void printResults(double*, int, std::string);
void mpi_connection();
void mpi_receive(double*, int);
int mpi_receive_order();
void mpi_send(double*, int);
void mpi_finish();

class linear2
{
	public:
		linear2();
		void solve();

	private:
    double* input;
    double* output;
    double w,x,y,z;
    double** mat;
    double* b;
    std::string file;
    std::string fileInput;
    std::string fileOutput;
		
};

class linear3
{
	public:
		linear3();
		void solve();

	private:
    double* input;
    double* output;
    double w,x,y,z;
    double** mat;
    double* b;
    std::string file;
    std::string fileInput;
    std::string fileOutput;
};

class nonlinear2
{
	public:
		nonlinear2();
		void solve();

	private:
    double* input;
    double* output;    
    std::string file;
    std::string fileInput;
    std::string fileOutput;

    double a, b, l, m, x, y;

};

class nonlinear3
{
  public:
    nonlinear3();
    void solve();

  private:
    double* input;
    double* output;    
    std::string file;
    std::string fileInput;
    std::string fileOutput;

    double a, b, c, l, m, n, x, y, z;

};

class linear2mapper
{
  public:
    linear2mapper();
    void solve();

  private:
    double* input;
    double* output;
    double w,x,y,z, alpha;
    double** mat;
    double* b;
    std::string file;
    std::string fileInput;
    std::string fileOutput;
    
};



#endif