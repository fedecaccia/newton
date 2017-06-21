/*****************************************************************************\

NEWTON                |
                      |
Multiphysics          | CLASS
coupling              | MATLIB
maste code            |
                      |

-------------------------------------------------------------------------------

MatLib is a library of vector and matrix function operations.

Author: Federico A. Caccia
Date: 4 June 2017

\*****************************************************************************/

#ifndef MAHTLIB_CPP
#define MAHTLIB_CPP

#include "MathLib.h"

using namespace std;
using std::cout;
using std::endl;

MathLib::MathLib()
{  

}

// x1 + x2
double* MathLib::sumVectors(double* x1, double* x2, int N)
{
  double* y = new double[N];
  for (int i=0; i<N; i++){
    y[i] = x1[i] + x2[i];
  }
  return y;  
}

// x1 - x2
double* MathLib::differenceInVectors(double* x1, double* x2, int N)
{
  double* y = new double[N];
  for (int i=0; i<N; i++){
    y[i] = x1[i] - x2[i];
  }
  return y;  
}

// |x|_2
double MathLib::moduleAbs(double* x1, int N)
{
  double mod = 0;
  for (int i=0; i<N; i++){
    mod += x1[i]*x1[i];
  }
  mod = sqrt(mod);
  return mod;
}

// En la Pos1 de x1 copio desde la Pos2 de x2, x2Amount elementos
void MathLib::copyInVector(double* x1, int x1Pos1, double* x2, int x2Pos1, int x2Amount)
{
  for (int i=x2Pos1; i<x2Pos1+x2Amount; i++){
    x1[x1Pos1 + i] = x2[x2Pos1 + i];
  }
}

// Copia de matrices
void MathLib::copyMat(double** A, double** B, int N)
{
  for (int i=0; i<N; i++){
    for(int j=0; j<N; j++){
      A[i][j] = B[i][j];
    }
  }
}


// Puesta a cero del vector
void MathLib::zeros(double* x1, int N)
{
  for(int i=0; i<N; i++){
    x1[i] = 0;
  }
}

// Make identity matrix
void MathLib::identity(double** A, int n)
{
  for(int i=0; i<n; i++){
    for(int j=0; j<n; j++){
      A[i][j] = 0;
      if(i==j) {
        A[i][j] = 1;
      }
    }
  }
}

// Vector negativo
double* MathLib::negateVector(double* x1, int N)
{
  double* x2 = new double[N];
  for (int i=0; i<N; i++){
    x2[i] = -x1[i];
  }
  return x2; 
}

// Suma de delta en una posición del vector
void MathLib::sumDeltaInPosition(double* x1, int position, double delta, int N)
{
  x1[position] = x1[position] + delta;
}

double* MathLib::matXvec(double** A, double* x1, int N)
{
  double* x2 = new double[N];
  for(int i=0; i<N; i++){
    x2[i] = 0;
    for(int j=0; j<N; j++){
      x2[i]+= A[i][j] * x1[j];
    }
  }
  return x2;
}

double MathLib::vecDotvec(double* x1, double* x2, int N)
{
  double x3 = 0;
  for(int i=0; i<N; i++){
    x3 += x1[i]*x2[i];
  }
  return x3;
}

double** MathLib::vecXvec(double* x1, double* x2, int N)
{
  double** A = new double*[N];
  for(int i=0; i<N; i++){
    A[i] = new double[N];
  }
  for(int i=0; i<N; i++){
    for(int j=0; j<N; j++){
      A[i][j] = x1[i]*x2[j];
    }
  }  
  return A;
}

// x2 = c*x1
double* MathLib::scaleVec(double* x1, double a, int N)
{
  double* x2 = new double[N];
  for (int i=0; i<N; i++){
    x2[i] = a*x1[i];
  }
  return x2;
}

// A2 = c*A1
double** MathLib::scaleMat(double** A1, double c, int N)
{
  double** A2 = new double*[N];
  for (int i=0; i<N; i++){
    A2[i] = new double[N];
    for (int j=0; j<N; j++){
      A2[i][j] = c*A1[i][j];
    }
  }
  return A2;
}

// C = A + B
double** MathLib::sumMat(double** A, double** B, int N)
{
  double** C = new double*[N];
  for (int i=0; i<N; i++){
    C[i] = new double[N];
    for (int j=0; j<N; j++){
      C[i][j] = A[i][j] + B[i][j];
    }
  }
  return C; 
}

// matrix inversion
double** MathLib::matrixInversion(double **A, int N)
{
    double** B = new double*[N];
    for (int i=0; i<N; i++){
      B[i] = new double[N];
      for (int j=0; j<N; j++){
        B[i][j] = 0;
      }
    }
    // get the determinant of a
    double det = 1.0/CalcDeterminant(A,N);
 
    // memory allocation
    double *temp = new double[(N-1)*(N-1)];
    double **minor = new double*[N-1];
    for(int i=0;i<N-1;i++)
        minor[i] = temp+(i*(N-1));
 
    for(int j=0;j<N;j++)
    {
        for(int i=0;i<N;i++)
        {
            // get the co-factor (matrix) of A(j,i)
            GetMinor(A,minor,j,i,N);
            B[i][j] = det*CalcDeterminant(minor,N-1);
            if( (i+j)%2 == 1)
                B[i][j] = -B[i][j];
        }
    }
 
    // release memory
    //delete [] minor[0];
    delete [] temp;
    delete [] minor;
    
    return B;
}
 
// calculate the cofactor of element (row,col)
int MathLib::GetMinor(double **src, double **dest, int row, int col, int N)
{
    // indicate which col and row is being copied to dest
    int colCount=0,rowCount=0;
 
    for(int i = 0; i < N; i++ )
    {
        if( i != row )
        {
            colCount = 0;
            for(int j = 0; j < N; j++ )
            {
                // when j is not the element
                if( j != col )
                {
                    dest[rowCount][colCount] = src[i][j];
                    colCount++;
                }
            }
            rowCount++;
        }
    }
 
    return 1;
}
 
// Calculate the determinant recursively.
double MathLib::CalcDeterminant( double **mat, int N)
{
    // N must be >= 0
    // stop the recursion when matrix is a single element
    if( N == 1 )
        return mat[0][0];
 
    // the determinant value
    double det = 0;
 
    // allocate the cofactor matrix
    double **minor;
    minor = new double*[N-1];
    for(int i=0;i<N-1;i++)
        minor[i] = new double[N-1];
 
    for(int i = 0; i < N; i++ )
    {
        // get minor of element (0,i)
        GetMinor( mat, minor, 0, i , N);
        // the recusion is here!
 
        det += (i%2==1?-1.0:1.0) * mat[0][i] * CalcDeterminant(minor,N-1);
        //det += pow( -1.0, i ) * mat[0][i] * CalcDeterminant( minor,N-1 );
    }
 
    // release memory
    for(int i=0;i<N-1;i++)
        delete [] minor[i];
    delete [] minor;
 
    return det;
}

// Resolución del sistema lineal mediante inversión de matriz
double* MathLib::invertSystem(double** A, double* b, int N)
{
  double* sol = new double[N];
  
  // Algoritmo de inversión de matriz
  
  double** invA = matrixInversion(A, N);
  sol = matXvec(invA, b, N);
  
  return sol;
}


// LU solve
double* MathLib::luSolve(double **A, double *b, int N) {

    double Tol =1e-7;
    int* P = new int[N];
    int i, j, k, imax; 
    double maxA, *ptr, absA;
    double* x = new double[N];

    for (i = 0; i <= N; i++)
        P[i] = i; // Unit permutation matrix, P[N] initialized with N

    for (i = 0; i < N; i++) {
        maxA = 0.0;
        imax = i;

        for (k = i; k < N; k++){
            absA = fabs(A[k][i]);
            if (absA > maxA) { 
                maxA = absA;
                imax = k;
            }
        }

        if (maxA < Tol) {
          cout<<" ERROR: Matrix degenerate, row: "<<k<<" col: "<<i<<" size: "<<N<<" maxA: "<<maxA<<" absA: "<<absA<<endl;
          for (int j=0; j<N; j++){
            for(int k=0; k<N; k++){
              cout<<A[j][k]<<" ";
            }
            cout<<endl;
          }
          return 0; // failure, matrix is degenerate
        }
        
        if (imax != i) {
            //pivoting P
            j = P[i];
            P[i] = P[imax];
            P[imax] = j;

            //pivoting rows of A
            ptr = A[i];
            A[i] = A[imax];
            A[imax] = ptr;

            // counting pivots starting from N (for determinant)
            P[N]++;
        }

        for (j = i + 1; j < N; j++) {
            A[j][i] /= A[i][i];

            for (k = i + 1; k < N; k++)
                A[j][k] -= A[j][i] * A[i][k];
        }
    }

    // decomposition done: solve
      
   
    for (int i = 0; i < N; i++) {
        x[i] = b[P[i]];

        for (int k = 0; k < i; k++)
            x[i] -= A[i][k] * x[k];
    }

    for (int i = N - 1; i >= 0; i--) {
        for (int k = i + 1; k < N; k++)
            x[i] -= A[i][k] * x[k];

        x[i] = x[i] / A[i][i];
    }
   
   
  //~ // TEST
  //~ cout<<"A:"<<endl;
  //~ for(int i=0; i<N; i++){
    //~ for(int j=0; j<N; j++){
      //~ cout<<A[i][j]<<" ";
    //~ }
    //~ cout<<endl;
  //~ }
  //~ cout<<"b:"<<endl;
  //~ for(int i=0; i<N; i++){
    //~ cout<<b[i]<<" ";
    //~ cout<<endl;
  //~ }
  //~ cout<<"sol:"<<endl;
  //~ for(int i=0; i<N; i++){
    //~ cout<<x[i]<<" ";
    //~ cout<<endl;
  //~ }    
   
   
   return x;
}

#endif
