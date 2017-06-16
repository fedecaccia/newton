#ifndef MATHLIB_H
#define MATHLIB_H

#include <stdlib.h> // abs
#include <iostream> // endl, cout
#include <math.h>

class MathLib
{
	public:
    // Funciones
    
    // Constructor
    MathLib();
    // x1 + x2
    double* sumVectors(double*, double*, int);
    // x1 - x2
    double* differenceInVectors(double*, double*, int);    
    // Vector negativo
    double* negateVector(double*, int);
    // |x|
    double moduleAbs(double*, int);
    // Copia de vectores
    void copyInVector(double*, int, double*, int, int);
    // Copia de matrices
    void copyMat(double**, double**, int);
    // Puesta a cero del vector
    void zeros(double*, int);
    // Make identity matrix
    void identity(double**, int);
    // Suma de delta en una posición del vector
    void sumDeltaInPosition(double*, int, double, int);
    // A*x
    double* matXvec(double**, double*, int) ;
    // x.y
    double vecDotvec(double*, double*, int);
    // x X y
    double** vecXvec(double*, double*, int);
    // x2 = c*x1
    double* scaleVec(double*, double, int);
    // A2 = c*A1
    double** scaleMat(double**, double, int);
    // C = A + B
    double** sumMat(double**, double**, int);        
    // Resolución del sistema lineal mediante descomposición LU
    double* luSolve(double**, double*, int);
    // Resolución del sistema lineal mediante inversión de matriz
    double* invertSystem(double**, double*, int);
    // Matrix inversion
    double** matrixInversion(double**, int);
    // Calculate the cofactor of element (row,col)
    int GetMinor(double**, double**, int, int, int);
    // Calculate the determinant recursively.
    double CalcDeterminant(double**, int);
    
    // Variables
    
		
	private:
};

#endif
