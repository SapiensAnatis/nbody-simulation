/*
 * PHYM004 - Assessment 2
 * Jay Malhotra - 2021-11-15
 * 
 * calc.h contains the definitions of the functions in calc.c (plus comments
 * explaining their purpose and usage) as well as the definition of the Body
 * struct.
 */

#ifndef CALC_H
#define CALC_H

// Need for FILE* and bool in function signatures
#include <stdio.h>
#include <stdbool.h>

// How many characters to allocate for the body's name string
#define BODY_NAME_SIZE 32

// Newton's constant of gravitation (in SI units, m^3 kg^-1 s^-2). This value
// is only used if the -n / --natural-units flag is not supplied.
#define G_CONSTANT_SI 6.67e-11

typedef struct {
    double x;
    double y;
    double z;
} Vector3D;

// Calculate the squared magnitude of a Vector3D v.
double vector_sq_mag(Vector3D* v);

typedef struct {
    char*  name;
    double mass;

    Vector3D* pos;
    Vector3D* vel;
    Vector3D* acc;
} Body;

// Create an empty Body struct. It must be created like this because memory
// needs to be allocated for the string name field. 
Body* body_factory();

// Free a body -- actually requires two calls (to free the memory associated
// with the struct and also the name).
void body_free(Body* b);

// Calculate the square magnitude of the distance between two bodies.
// Simple Pythagoras method.
double body_distance_sq(Body* b1, Body* b2);

// Calculate the kinetic energy of a body b.
double body_ke(Body* b);

// Calculate the gravitational potential energy of a body b with respect to
// every other body in the system. E_p = -GMm/r. If nat_units is false, SI G
// is used, otherwise 1 is used.
double body_gpe(Body* b1, Body** body_arr, int n_bodies, bool nat_units);

// Calculate the centre of mass of the system's position and velocity vectors.
// As we want to return two values, instead of a return value it takes in two
// vector buffers.
void system_com(Body** body_arr, int n_bodies, 
Vector3D* pos_buffer, Vector3D* vel_buffer);

// Calculate the angular momentum of a body b with respect to the position of
// ref_point. Velocities are taken as absolute.
double body_am(Body* b, Body** body_arr, int n_bodies, Vector3D* ref_point);

// Use Newton's law of gravitation to calculate the acceleration felt by a body
// due to the action of all other bodies in body_arr. If nat_units is false, SI
// G is used, otherwise 1 is used.
void body_update_accel(Body* b1, Body** body_arr, int n_bodies, bool nat_units);

// Run the Verlet method and output to a file.
void verlet(Body** body_arr, int n_bodies, 
            double end, double step, bool nat_unitsm,
            FILE* stream);

#endif