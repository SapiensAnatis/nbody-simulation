# Overview

This is an nbody simulation code that uses a Verlet integrator to produce position and velocity data for the given initial conditions, which are taken as a textfile passed when invoking the program. It is set up to simulate the gravitational interactions between large bodies, but the calculation of acceleration could be swapped to use a different potential very easily. It was originally written and submitted for a scientific programming class. The file report.pdf contains a more in-depth analysis of the program and the results it produced.

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
