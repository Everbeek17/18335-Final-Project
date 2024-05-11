#ifndef DECLARATIONS_H
#define DECLARATIONS_H

#define NUM_VALUES 7 // x, y, z, vx, vy, vz, mass
#define GRAVITATIONAL_CONSTANT 6.67430e-11

#define NUM_LAYERS 3
#define NUM_BLOCKS_PER_LAYER 8

struct System;  // defined in system.hpp
struct Block;   // defined in system.hpp

void output_results_HDF5(System system); // defined in output_results.cpp

#endif
