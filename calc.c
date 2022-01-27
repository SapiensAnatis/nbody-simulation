/*
 * PHYM004 - Assessment 2
 * Jay Malhotra - 2021-11-15
 * 
 * calc.c contains the 'mathematical' parts of the program, as opposed to the
 * user-interface parts featured in main.c. The methods are given explanations
 * in the header file calc.h.
 */

#include "calc.h"
#include <stdlib.h>
#include <math.h>

double vector_sq_mag(Vector3D* v) {
    double result = 0;
    result += pow(v->x, 2);
    result += pow(v->y, 2);
    result += pow(v->z, 2);
    return result;
}

Body* body_factory() {
    char* name_ptr = (char*)malloc(BODY_NAME_SIZE * sizeof(char));
    // Allocate vector pointers all at once
    Vector3D* vector_ptrs = (Vector3D*)calloc(3, sizeof(Vector3D));

    Body* result = (Body*)malloc(sizeof(Body));

    if (!name_ptr || !vector_ptrs || !result) {
        // If memory allocation failed at any point
        return NULL;
    }

    result->name = name_ptr;
    result->pos = vector_ptrs + 0;
    result->vel = vector_ptrs + 1;
    result->acc = vector_ptrs + 2;

    return result;
}

void body_free(Body* b) {
    free(b->name);
    // pos, vel, and acc structs are allocated as block of 3; it should only be
    // necessary to free the pointer to the start of the block.
    free(b->pos);
    free(b);
}

double body_distance_sq(Body* b1, Body* b2) {
    Vector3D dist_vector = {
        b1->pos->x - b2->pos->x,
        b1->pos->y - b2->pos->y,
        b1->pos->z - b2->pos->z
    };

    return vector_sq_mag(&dist_vector);
}

double body_ke(Body* b) {
    return 0.5 * b->mass * vector_sq_mag(b->vel);
}

double body_gpe(Body* b1, Body** body_arr, int n_bodies, bool nat_units) {
    double result = 0;
    double g_const;
    if (nat_units) { g_const = 1; } else { g_const = G_CONSTANT_SI; }

    // Sum GPE due to all other bodies in the system
    for (int i = 0; i < n_bodies; i++) {
        Body* b2 = *(body_arr + i);

        // Don't calculate potential due to itself
        if (b1 == b2) { continue; }

        // E_p = -GMm/r
        double r_squared = body_distance_sq(b1, b2);
        return -(g_const * b1->mass * b2->mass) / sqrt(r_squared);
    }
}

void system_com(Body** body_arr, int n_bodies, 
Vector3D* pos_buffer, Vector3D* vel_buffer) {
    // Formula = 1/M * sum(m_i r_i)
    // with M = total mass, m_i, r_i = individual masses, individual positions
    
    // Same for velocity of CoM, except v_i (velocities) in place of x_i
    double total_mass = 0;

    for (int i = 0; i < n_bodies; i++) {
        Body* b = *(body_arr + i);

        total_mass += b->mass;

        // This code is a bit...repetitive, but I don't want to mess around with
        // implementing vector_add, vector_multiply etc. methods as it's a bit
        // tricky to decide how the results of those should be stored, since
        // vectors are sometimes stack variables (e.g. in body_am) but are other
        // times on the heap (e.g. members of Body struct).
        pos_buffer->x += (b->mass * b->pos->x);
        pos_buffer->y += (b->mass * b->pos->y);
        pos_buffer->z += (b->mass * b->pos->z);

        vel_buffer->x += (b->mass * b->vel->x);
        vel_buffer->y += (b->mass * b->vel->y);
        vel_buffer->z += (b->mass * b->vel->z);
    }

    // Divide by M
    pos_buffer->x /= total_mass;
    pos_buffer->y /= total_mass;
    pos_buffer->z /= total_mass;

    vel_buffer->x /= total_mass;
    vel_buffer->y /= total_mass;
    vel_buffer->z /= total_mass;
    /*
    printf("System CoM:\n\tr = (%le, %le, %le)\n\tv = (%le, %le, %le)\n",
        pos_buffer->x, pos_buffer->y, pos_buffer->z,
        vel_buffer->x, vel_buffer->y, vel_buffer->z); 
    */
}

double body_am(Body* b, Body** body_arr, int n_bodies, Vector3D* ref_point) {
    Vector3D rel_pos = {
        b->pos->x - ref_point->x,
        b->pos->y - ref_point->y,
        b->pos->z - ref_point->z
    };
    // Don't bother calculating relative velocity to CoM, should be 0 anyway

    // L = mrv
    double dist = sqrt(vector_sq_mag(&rel_pos));
    double vel = sqrt(vector_sq_mag(b->vel));
    
    return b->mass * dist * vel;
}

void body_update_accel(Body* b1, Body** body_arr, int n_bodies, bool nat_units) {
    double g_const;
    if (nat_units) { g_const = 1; } else { g_const = G_CONSTANT_SI; }
    
    double x = 0;
    double y = 0;
    double z = 0;

    for (int i = 0; i < n_bodies; i++) {
        Body* b2 = *(body_arr + i);

        // Do not calculate the force on a body due to itself
        if (b1 == b2) { continue; }

        double sq_dist = body_distance_sq(b1, b2);
        double dist = sqrt(sq_dist);
        
        // Calculate absolute magnitude of the force
        double magnitude = (g_const * b2->mass) / sq_dist;

        // Multiply that by the unit vector that points from b1 to b2
        x += magnitude * (b2->pos->x - b1->pos->x) / dist;
        y += magnitude * (b2->pos->y - b1->pos->y) / dist;
        z += magnitude * (b2->pos->z - b1->pos->z) / dist;
    }

    b1->acc->x = x;
    b1->acc->y = y;
    b1->acc->z = z;
}

void verlet(Body** body_arr, int n_bodies, 
            double end, double step, bool nat_units,
            FILE* stream) {
    printf("[INFO] Running Verlet simulation.\n");
    printf("[INFO] Max time = %e s / Timestep = %e s / Steps = %ld \n",
    end, step, (long)floor(end/step));
    
    double t = 0;

    double g_const = 6.67e-11;
    if (nat_units) {  g_const = 1; }

    // Write column definitions
    fprintf(stream, "# Column definitions:\n# Time (s)\t");
    for (int i = 0; i < n_bodies; i++) { 
        // Columns other than time are defined for each body
        Body* b1 = *(body_arr + i);
        fprintf(stream, "'%s' pos x,y,z (m)\t", b1->name);
        fprintf(stream, "'%s' KE+GPE (J)\t", b1->name);
        fprintf(stream, "'%s' ang mom (J.s)\t", b1->name);
    }

    while (t + step <= end) {
        fprintf(stream, "\n%le\t", t + step);

        // Recalculate CoM and set all body velocities offset by it
        // Only want to perform CoM calculation once
        Vector3D com_pos = {0, 0, 0};
        Vector3D com_vel = {0, 0, 0};
        system_com(body_arr, n_bodies, &com_pos, &com_vel);

        for (int i = 0; i < n_bodies; i++) {
            Body* b1 = *(body_arr + i);
            /*
            // Zero relative to CoM
            // Commented out: it seems to make the 3-body orbits less stable,
            // and has little impact on angular momentum conservation anyway.
            b1->vel->x -= com_vel.x;
            b1->vel->y -= com_vel.y;
            b1->vel->z -= com_vel.z;
            */
            
            // Half-step velocities
            body_update_accel(b1, body_arr, n_bodies, nat_units);
            b1->vel->x += (b1->acc->x * (step / 2));
            b1->vel->y += (b1->acc->y * (step / 2));
            b1->vel->z += (b1->acc->z * (step / 2));

            // Position + dt*v_(1/2)
            b1->pos->x += (b1->vel->x * step);
            b1->pos->y += (b1->vel->y * step);
            b1->pos->z += (b1->vel->z * step);

            // Final half-step for velocities
            body_update_accel(b1, body_arr, n_bodies, nat_units);
            b1->vel->x += (b1->acc->x * (step / 2));
            b1->vel->y += (b1->acc->y * (step / 2));
            b1->vel->z += (b1->acc->z * (step / 2));
            
            double E_k = body_ke(b1);
            double E_p = body_gpe(b1, body_arr, n_bodies, nat_units);
            double E_t = E_k + E_p;
            double L = body_am(b1, body_arr, n_bodies, &com_pos);

            // Log position    
            fprintf(stream, "%le\t%le\t%le\t%le\t%le\t", 
            b1->pos->x, b1->pos->y, b1->pos->z, E_t, L);
        }

        t += step;
    }

    printf("[INFO] Verlet simulation concluded successfully.\n");
}