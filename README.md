# README 

## Newton: a multiphysics coupling master code

 `Newton` is a master code that solves implicit coupling in nonlinear calculations.
It was designed with a general purpose. 
For example, it can be used in multiscale coupled problems, 
in CFD-temalhydraulic problems, in neutronic-termalhydraulic problems, 
and others.
 
`Newton` solves residual coupling equations explicitly or implicitly.
It has been demostrated in several works([1], [2]) that implicit approaches improve convergence of the iterations.

## Quickstart

### Compiling `Newton`
Make sure you have set MPI_DIR and PETSC_DIR environmental variables, also set NEWTON_DIR in your path, then:

```bash
# After setting MPI_DIR and PETSC_DIR environmental variables:
source install.sh
```

### Running tests
Several tests has been included as examples to demonstrate the capabilities of the code. Type in console:
```bash
cd ${NEWOTN_DIR}/examples/linear_system_2_clients/io-io/
./run.sh
```
This example solves a system of four coupled equations with four unknowns (`w`, `x`, `y`, `z`). Client `code 0` solves `x`, `y`, `z` as funcion of `w` and client `code 1` solves `w` as funtion of `x`, `y`, `z`. Both clients are spawned by `Newton` as many times as necessary until the converged solution is reached. 
The parameters of the problem, as well as parameters needed by the numerical methods implemented in `Newton` and all that `Newton` has to know to spawn the clients is set in the configuration file `examples/linear_system_2_clients/io-io/newton.config`. Change value `broyden` in METHOD card to `secant` and run again the test. Then look what happends.

Now move on to another test. Type:
```bash
cd ${NEWOTN_DIR}/examples/linear_system_2_clients/mpi_comm-mpi_comm/
./run.sh
```
This test solves the same system of coupled equations but now connecting with client codes by MPI. Now take a look at the configuration file. It is so much simpler! This is because now `Newton` doesn't have to know nothing about file names and paths: communication is between codes while they are running. Again, you can play as much as you want with the configuration file. For example, change the value of the initial conditions and see how change the amount of iterations needed to convege to the solution using different methods.

## Running `Newton` on multiple processors

`Newton` is designed to run in one or multiple processes.
Wether it is better one or other choice depends on the way that `Newton` connects with clients. Problems involving more than one spawn of codes are adressed more eficiently running `Newton` on multiple threads:

```bash
mpirun -np N -machinefile mf ${NEWTON_DIR}/bin/Newton # N threads, mf file with node names
```
To see an example running with 3 threads take a look at ```examples/nonlinear_system_3_clients/io-io-io/run.sh```.

## Main features

* Explicit and implicit methods implemented to solve nonlinear residuals.

* Newton uses PETSc functions to solve linear systems in methods that build jacobian matrix of the system.

* Efficient error handling.

* Multiple connection modes with clients: input writing and output reading (I/O), communication by MPI ports and communication by MPI communicators stablished in the same execution.

* Ready-to-use functions to read outputs & write inputs of clients that connect by I/O.

* Implemented communication with [Fermi](https://github.com/GG1991/fermi) & RELAP by I/O both connected with different threads of `Newton`.

* Ready-to-use mappers of variables received and sended to clients.

* Mappers implemented to get cross-sections as function of termal-hydraulic and burnup variables.

* Sets of specific variables that need to be storage and updated along the evolution of systems.

* Multiple debugger modes that can be activated by keywords from input file.

## Mappers

Newton includes the possibility of using mapings between variables to adecuate them to the unknowns that are used to solve the residuals. For example, in neutronic-temal-hydraulic calculations it is common to treat as unknown temperatures and densities, and cross sections are calculated based on the values of the previous variables. Different mappers could be used for different client codes to adecuate as well guesses as calculated variables. Its implementation requieres minimal programming in ready-to-use functions of class `Mapper`.

## Documentation
Documentation is available in ```${NEWOTN_DIR}/doc```. User's manual is available right now and developer's manual is being written.

<!-- Compile tex files in /doc to pdf (some packages from ```texlive``` are needed: ```texlive-fonts-recommended``` and ```texlive-latex-recommended```). Type:
```bash
cd doc/
pdflatex newton-u-m.tex
```
to get user's manual and:
```bash
cd doc/
pdflatex newton-d-m.tex
```
to get developer's manual. -->

## The future

* Complete documentation.

* More tests.

* Performance evaluation.

## Licensing

Copyright (C) 2017 Federico A. Caccia

Newton is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

Newton is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with Newton.  If not, see <http://www.gnu.org/licenses/>.

## Contact
Any questions or suggestions feel free to contact me at [federicoagustincaccia@gmail.com]

## References

[1] Jorge S. Leiva, Pablo J. Blanco and Gustavo C. Buscaglia. 
"Iterative strong coupling of dimensionally heterogeneous models".
International journal for numerical methods in engineering, 2010, vol 81, pags 1558–1580.

[2] [Caccia, F. and Dari. E. “Acoplamiento multiescala en cálculos fluidodinámicos" (“Multiscale
coupling in fluid dynamics calculations”). XXll Congress on Numerical Methods and its
Applications, 7-11 November 2016, Córdoba, Argentina.](https://goo.gl/mZ3A7o)
