/*
 * PHYM004 - Assessment 2
 * Jay Malhotra - 2021-11-15
 * 
 * main.c defines the main() entrypoint of the program, which contains the
 * routine that reads in the initial conditions file and sets up the problem.
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "calc.h"

// The maximum number of bodies that can be handled by the program. Up-front
// this will only result in space for the pointers being allocated; the actual
// bodies will not be allocated unless they are found in the file.
#define BODY_ARRAY_SIZE 64

// Individual line buffer -- the max length of any given line in initial
// conditions text file.
#define LINE_BUFFER_SIZE 512

// Filename to output results to
#define OUTPUT_FILENAME "nbody_results.txt"

Body** setup_bodies(char* filename, int* count_buffer);
void get_time_input(double* end, double* step);

int main(int argc, char* argv[]) {
    if (argc < 2) {
        // No filename given
        printf("[ERROR] No filename argument was given.");
        exit(1);
    }

    // Load bodies
    int body_count;
    Body** bodies = setup_bodies(argv[1], &body_count);

    // Get user input
    double end = 0;
    double step = 0;
    get_time_input(&end, &step);

    // Natural units command-line flag
    bool nat_units = false;
    if (argc > 2) {
        if (strcmp(argv[2], "--natural-units") == 0 || strcmp(argv[2], "-n") == 0) {
            printf("[INFO] Natural units enabled.\n");
            nat_units = true;
        }
    }

    // Run Verlet
    FILE* out_stream = fopen(OUTPUT_FILENAME, "w");
    verlet(bodies, body_count, end, step, nat_units, out_stream);

    printf("[INFO] Results written to " OUTPUT_FILENAME ".");

    // Finish up
    for (int i = 0; i < body_count; i++) {
        body_free(*(bodies + i));
    }
    free(bodies);

    return 0;
}

// Display a body's name and statistics.
void print_body(Body* body_ptr) {
    printf("  Name: %s\n  Mass: %.3e kg\n"
           "  Position: (%.3le, %.3le, %.3le) m\n"
           "  Velocity: (%.3le, %.3le, %.3le) m/s\n",
           body_ptr->name,   body_ptr->mass,
           body_ptr->pos->x, body_ptr->pos->y, body_ptr->pos->z,
           body_ptr->vel->x, body_ptr->vel->y, body_ptr->vel->z);
}

// Get user input for end time and timestep. These are reasonable parameters
// to want to adjust between simulations, so I won't go with my usual approach
// of consts or #defines that require recompiling.
// Params: buffers of end and step, since it can't return multiple values.
void get_time_input(double* end, double* step) {
    char* input_buffer = (char*)malloc(LINE_BUFFER_SIZE * sizeof(char));
    char* err_ptr;

    if (input_buffer == NULL) {
        printf("Could not allocate memory for reading of user input.");
        exit(1);
    }

    // fgets() -> strtod() because scanf() causes issues if the format string isn't
    // fulfilled.
    printf("Please input the desired running time of the simulation in seconds: ");
    fgets(input_buffer, LINE_BUFFER_SIZE, stdin);
    // fgets includes the trailing newline, so get rid of it
    input_buffer[strlen(input_buffer)-1] = '\0';
    *end = strtod(input_buffer, &err_ptr);

    if (*err_ptr != '\0') {
        printf("[WARNING] Failed to convert all characters in the input to floating point.\n");
    }

    printf("Please input the desired time-step of the simulation in seconds: ");
    fgets(input_buffer, LINE_BUFFER_SIZE, stdin);
    input_buffer[strlen(input_buffer)-1] = '\0';
    *step = strtod(input_buffer, &err_ptr);

    if (*err_ptr != '\0') {
        printf("[WARNING] Failed to convert all characters in the input to floating point.\n");
    }

    free(input_buffer);
}

// Read in the file and set up the bodies. Returns a pointer to an array of 
// body pointers, and writes the number of bodies to count_buffer.
Body** setup_bodies(char* filename, int* count_buffer) {
    FILE* file_handle = fopen(filename, "r");
    if (file_handle == NULL) {
        printf("[ERROR] File \"%s\" could not be opened.", filename);
        exit(1);
    }

    // Initialize array of pointers to bodies
    Body** bodies = (Body**)malloc(BODY_ARRAY_SIZE * sizeof(Body*));
    if (bodies == NULL) {
        printf("[ERROR] Could not allocate %ld bytes for initialization of "
        "array holding %d body pointers.", BODY_ARRAY_SIZE * sizeof(Body*), BODY_ARRAY_SIZE);
        exit(1);
    }

    // Iterate through each line of file and create bodies
    char line[LINE_BUFFER_SIZE];
    int body_count = 0;
    while(fgets(line, sizeof(line), file_handle)) {

        // Ignore comments, i.e. lines starting with #
        if (*(line) == '#') {
            continue;
        }

        body_count++;

        Body* body = body_factory();
        if (body == NULL) {
            printf("[ERROR] Could not allocate memory for body #%d.", body_count);
            exit(1);
        }

        char name_buffer[LINE_BUFFER_SIZE];
        sscanf(line, "%s\t%le\t%le\t%le\t%le\t%le\t%le\t%le",
               name_buffer,
               &(body->mass),
               &(body->pos->x), &(body->pos->y), &(body->pos->z),
               &(body->vel->x), &(body->vel->y), &(body->vel->z)
        );
        strcpy(body->name, name_buffer);

        // body_count starts at 1; going to array indexs requires - 1
        *(bodies + body_count - 1) = body;
    }

    if (body_count < 1) {
        printf("[ERROR] At least one body must be defined for the program to run.");
        exit(1);
    }

    printf("[INFO] The following %d bodies have been set up:\n", body_count);
    for (int i = 0; i < body_count; i++) {
        printf("Body #%d:\n", i+1);
        print_body(*(bodies + i));
    }

    *count_buffer = body_count;
    return bodies;
}