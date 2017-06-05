# README 

 `Newton` is a master code that solves implicit coupling in nonlinear calculations.
It was designed with a general purpose. 
For example, it can be used in multiscale coupled problems, 
in cfd-temalhydraulic problems, in neutronic-termalhydraulic problems, 
and others.
 
`Newton` solves residual coupling equations explicitly or implicitly.
It has been demostrated in several works that the implicit approach improves the convergence of the iterations.

##Compiling `Newton`

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

* Benchmarking

* Documentation

Federico Caccia - [federicoagustincaccia@gmail.com]
