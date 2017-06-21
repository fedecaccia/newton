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

```bash
sh createLinksAndFolders.sh
# After setting MPI_DIR and PETSC_DIR environmental variables:
make
```
### Running tests
Several tests has been included as examples to demonstrate the capabilities of the code. Type in console:
```bash
cd examples/linear2_IO/
./run.sh
```
This example solves a system of four coupled equations with four unknowns (`w`, `x`, `y`, `z`). Client `code 0` solves `x`, `y`, `z` as funcion of `w` and client `code 1` solves `w` as funtion of `x`, `y`, `z`. Both clients are spawned by `Newton` as many times as necessary until the converged solution is reached. 
The parameters of the problem, as well as parameters needed by the numerical methods implemented in `Newton` and all that `Newton` has to know to spawn the clients is set in the configuration file `examples/test_IO-test_IO/newton.config`. Change value `explicit_serial` in METHOD card to `secant` and run again the test. Then look what happends.

Now move on to another test. Type:
```bash
cd ../linear2_MPI/
./run.sh
```
This test solves the same system of coupled equations but now connecting with client codes by MPI. Now take a look at the configuration file. It is so much simpler! This is because now Newton doesn't has to know nothing about file names and paths: communication is between codes while the are running. Again, you can play as much as you want with the configuration file. For example, change the value of the initial conditions and see how change the amount of iterations needed to convege to the solution using different methods.

## Running `Newton` on multiple processors

`Newton` is designed to run in one or multiple processes.
Wether it is better one or other choice depends on the way that `Newton` connects with clients. Problems involving more than one spawn of codes are adressed more eficiently running `Newton` on multiple threads:

```bash
mpirun -np N -machinefile mf bin/Newton # N threads, mf file with node names
```
To see an example of this look at ```examples/nonlinear3_IO/run3procs.sh```.
Communication with client codes via MPI option requieres an script to run the problem. (Look at any example inside ```_MPI``` folder).

## Main features

* Explicit and implicit methods implemented to solve nonlinear residuals.
Newton uses PETSc functions to solve linear systems in methods that build jacobian matrix of the system.

* Efficient error handling.

* Ready-to-use functions to read outputs & write inputs of clients that connect by I/O.

* Ready-to-use mappers of variables received and sended to clients.

* Sets of specific variables that need to be storage and updated along the evolution of systems.

## Mappers

Newton includes the possibility of using mapings between variables to adecuate them to the unknowns that are used to solve the residuals. For example, in neutronic-temal-hydraulic calculations it is common to treat as unknown temperatures and densities, and cross sections are calculated based on the values of the previous variables. Different mappers could be used for different client codes to adecuate as well guesses as calculated variables. Its implementation requieres minimal programming in ready-to-use functions of class `Mapper`.

## Documentation

Compile tex files in /doc to pdf (some packages from ```texlive``` are needed: ```texlive-fonts-recommended``` and ```texlive-latex-recommended```). Type:
```bash
cd doc/
pdflatex newton-u-m.tex
```
to get user's manual and:
```bash
cd doc/
pdflatex newton-d-m.tex
```
to get developer's manual.

## The future

* Communication with [Fermi](https://github.com/GG1991/fermi) & RELAP by I/O both connected with different threads of `Newton` (now serial).

* Implement efficient mappers to get cross-sections as function of termal-hydraulic and burnup variables.

* Implement communication using PPLEP.

* Complete documentation.

* More tests.

* Performance evaluation.

## References

[1] Jorge S. Leiva, Pablo J. Blanco and Gustavo C. Buscaglia. 
"Iterative strong coupling of dimensionally heterogeneous models".
International journal for numerical methods in engineering, 2010, vol 81, pags 1558–1580.

[2] [Caccia, F. and Dari. E. “Acoplamiento multiescala en cálculos fluidodinámicos" (“Multiscale
coupling in fluid dynamics calculations”). XXll Congress on Numerical Methods and its
Applications, 7-11 November 2016, Córdoba, Argentina.](https://goo.gl/mZ3A7o)

## Contact
Any questions or suggestions feel free to contact me at [federicoagustincaccia@gmail.com]

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
