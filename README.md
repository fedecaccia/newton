# README 

##`Newton` a multiphysics coupling master code
 `Newton` is a master code that solves implicit coupling in nonlinear calculations.
It was designed with a general purpose. 
For example, it can be used in multiscale coupled problems, 
in CFD-temalhydraulic problems, in neutronic-termalhydraulic problems, 
and others.
 
`Newton` solves residual coupling equations explicitly or implicitly.
It has been demostrated in several works([1], [2]) that implicit approaches improve convergence of the iterations.

##Compiling `Newton`
After seting MPI environment variables:

```bash
make
```

##Running `Newton` on multiple processors

```bash
mpirun -np 2 bin/Newton
```

## The future

* Performance evaluation

* Scripts to test the code

* Documentation

## References
[1] Jorge S. Leiva, Pablo J. Blanco and Gustavo C. Buscaglia. 
"Iterative strong coupling of dimensionally heterogeneous models".
International journal for numerical methods in engineering, 2010, vol 81, pags 1558–1580.

[2] Caccia, F. and Dari. E. “Acoplamiento multiescala en cálculos fluidodinámicos” (“Multiscale
coupling in fluid dynamics calculations”). XXll Congress on Numerical Methods and its
Applications, 7-11 November 2016, Córdoba, Argentina.

## Author

Federico Agustín Caccia - [federicoagustincaccia@gmail.com]
