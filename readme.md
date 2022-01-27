# Overview

Use `make` to build the code. There are no external dependencies. The resulting
executable 'nbody' is invoked as follows:

nbody [FILENAME] -n/--natural-units

with the arguments being:

    - FILENAME: required, path to initial conditions textfile 
    
    - n/natural-units: optional command-line switch to set G = 1, if omitted G 
      will be used as 6.67e-11

The other .txt files besides this one are initial conditions files,
corresponding to the ones used in the report. These are formatted in the way as
called for in the assessment brief, with the addition of the fact that lines
starting with a '#' are considered comments and ignored when parsing.

The program asks the user to input the end time and the timestep. These are just
passed to strtod(), so anything accepted by the %e format specifier will most
likely work.

It will produce a file nbody_results.txt which contains the results of the
simulation. The columns will vary based on how many bodies are specified, but
the file itself has column definitions, so refer to those.

# Reading the code

I tend to put method explanations in headers rather than source files, so read
those if you want to know what all the functions do.
