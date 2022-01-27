# Overview

This is a program that uses a Verlet integrator to evolve a gravitational n-body problem in time, based on some initial conditions given by the user when invoking the program. It was originally written and submitted for a scientific programming class. The file report.pdf contains a more in-depth analysis of the program and the results it produced.

Use `make` to build the code. There are no external dependencies. The resulting
executable 'nbody' is invoked as follows:

nbody [FILENAME] -n/--natural-units

with the arguments being:

- FILENAME: required, path to initial conditions textfile 
    
- n/natural-units: optional command-line switch to set G = 1, if omitted G will be used as 6.67e-11

The other .txt files besides this one (dragonfly.txt, moth.txt, etc.) are initial
condition files, corresponding to the ones used in the report.

The program asks the user to input the end time and the timestep. These are just
passed to strtod(), so anything accepted by the %e format specifier will most
likely work.

It will produce a file nbody_results.txt which contains the results of the
simulation. The columns will vary based on how many bodies are specified, but
the file itself has column definitions, so refer to those.

# Reading the code

I tend to put method explanations in headers rather than source files, so read
those if you want to know what all the functions do.
