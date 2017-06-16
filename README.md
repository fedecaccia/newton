# README 

##Newton: a multiphysics coupling master code
 `Newton` is a master code that solves implicit coupling in nonlinear calculations.
It was designed with a general purpose. 
For example, it can be used in multiscale coupled problems, 
in CFD-temalhydraulic problems, in neutronic-termalhydraulic problems, 
and others.
 
`Newton` solves residual coupling equations explicitly or implicitly.
It has been demostrated in several works([1], [2]) that implicit approaches improve convergence of the iterations.

## Quickstart

### Compiling `Newton`

```bash
. ev # Set MPI and PETSc environment variables:
sh createLinksAndFolders.sh
make
```
### Running tests
Several tests has been included as examples to demonstrate the capabilities of the code. Type in console:
```bash
cd examples/test_IO-test_IO/
./run.sh
```
This example solves a system of four coupled equations with four unknowns (`w`, `x`, `y`, `z`). Client `code 0` solves `x`, `y`, `z` as funcion of `w` and client `code 1` solves `w` as funtion of `x`, `y`, `z`. Both clients are spawned by `Newton` as many times as necessary until the converged solution is reached. 
The parameters of the problem, as well as parameters needed by the numerical methods implemented in `Newton` and all that `Newton` has to know to spawn the clients is set in the configuration file `examples/test_IO-test_IO/newton.config`. Change value `explicit_serial` in METHOD card to `secant` and run again the test. Then look what happends.

Now move on to another test. Make sure you have set environmental variables and then type:
```bash
cd ../test_MPI-test_MPI/
./run.sh
```
This test solves the same system of coupled equations but now communicating with client codes by MPI. Now take a look at the configuration file. It is so much simpler! This is because now Newton doesn't has to know nothing about file names and paths: communication is between codes while the are running. Again, you can play as much as you want with the configuration file. For example, change the value of the initial conditions and see how change the amount of iterations needed to convege to the solution using different methods.s

## Running `Newton` on multiple processors

`Newton` is designed to run in one or multiple processes.
Wether it is better one or other choice depends on the way that `Newton` connects with clients. Problems involving more than one spawn of codes are adressed more eficiently running `Newton` on multiple threads:

```bash
mpirun -np N -machinefile mf bin/Newton # N threads, mf file with node names
```
To see an example of this look at ```bash examples/test_IO-test_IO/run2procs.sh```.
Communication with client codes via MPI option requieres an script to run the problem. See ```bash examples/test_MPI-test_MPI/run.sh```.

## The future

* Implement communication using PPLEP.

* Performance evaluation

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
