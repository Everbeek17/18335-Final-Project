#include <iostream>
#include <string>
#include <H5Cpp.h>
#include <time.h> // for timing
#include <algorithm> // for std::copy
#include <vector>
#include <math.h> // for sqrt

#include "include/parameters.hpp"
#include "include/declarations.hpp"
#include "include/system.hpp"


System::System(float ic_data[][NUM_VALUES], const int num_elements, const int num_time_steps, const float delta_t)
  : num_elements(num_elements), num_time_steps(num_time_steps), delta_t(delta_t), actual_delta_t(0), state(num_time_steps, num_elements), base_block() {
  // printf("System constructor inside\n");
  this->actual_delta_t = delta_t * TIME_SCALING_FACTOR;

  // copy the initial condition array to the first timestep of the state struct
  for (int element_idx = 0; element_idx < num_elements; element_idx++) {
    state.x[0][element_idx] = ic_data[element_idx][0];
    state.y[0][element_idx] = ic_data[element_idx][1];
    state.z[0][element_idx] = ic_data[element_idx][2];
    state.vx[0][element_idx] = ic_data[element_idx][3];
    state.vy[0][element_idx] = ic_data[element_idx][4];
    state.vz[0][element_idx] = ic_data[element_idx][5];
    state.mass[element_idx] = ic_data[element_idx][6];
  }
}

// Destructor
System::~System() {
  // if (base_block) {
  //   delete base_block;
  // }
}

void System::interpolate_position(int curr_timestep) {
  // iterate over each element
  for (int element = 0; element < this->num_elements; element++) {
    
    // Advance the position by the velocity times delta_t
    this->state.x[curr_timestep][element] = 
        this->state.x[curr_timestep-1][element] + 
        this->state.vx[curr_timestep][element] * this->actual_delta_t;
    this->state.y[curr_timestep][element] =
        this->state.y[curr_timestep-1][element] + 
        this->state.vy[curr_timestep][element] * this->actual_delta_t;
    this->state.z[curr_timestep][element] =
        this->state.z[curr_timestep-1][element] + 
        this->state.vz[curr_timestep][element] * this->actual_delta_t;
  }
}

void System::propogate_state(int curr_timestep) {
  // iterate over each element
  for (int element = 0; element < this->num_elements; element++) {
    // Directly copy the position from the previous timestep
    this->state.x[curr_timestep][element] = 
        this->state.x[curr_timestep-1][element];
    this->state.y[curr_timestep][element] =
        this->state.y[curr_timestep-1][element];
    this->state.z[curr_timestep][element] =
        this->state.z[curr_timestep-1][element];
    
    // Directly copy the velocity from the previous timestep
    this->state.vx[curr_timestep][element] = 
        this->state.vx[curr_timestep-1][element];
    this->state.vy[curr_timestep][element] = 
        this->state.vy[curr_timestep-1][element];
    this->state.vz[curr_timestep][element] = 
        this->state.vz[curr_timestep-1][element];
  }
}

void System::Calculate_Gravitational_Force(float pos_x1, float pos_x2, float pos_y1, float pos_y2, float pos_z1, float pos_z2, float mass1, float mass2, float* force_x, float* force_y, float* force_z) {
  // Calculate the gravitational force between two elements
  // using the formula:
  // F = G * m1 * m2 / r^2
  // where G is the gravitational constant, 
  // m1 and m2 are the masses of the elements,
  // and r is the distance between the elements.

  // Use the scaling factors to adjust force calculation
  // m^3 kg^-1 s^-2
  // const float adjusted_constant = GRAVITATIONAL_CONSTANT * MASS_SCALING_FACTOR * MASS_SCALING_FACTOR / (DISTANCE_SCALING_FACTOR * DISTANCE_SCALING_FACTOR);

  const float adjusted_constant = GRAVITATIONAL_CONSTANT * GRAVITATIONAL_FORCE_FACTOR;



  // printf("Adjusted constant: %f\n", adjusted_constant);

  // Calculate distances between the elements
  const float dx = pos_x1 - pos_x2;
  const float dy = pos_y1 - pos_y2;
  const float dz = pos_z1 - pos_z2;
  const float r_squared = dx*dx + dy*dy + dz*dz;
  const float r = sqrt(r_squared);
  // printf("Distance between elements: %f <%f, %f, %f>\n", r, dx, dy, dz);

  // Calculate the force magnitude
  const float magnitude = adjusted_constant * mass1 * mass2  / (r_squared);

  // Calculate the force components
  // (with error checking for division by zero)
  if (r == 0) {
    *force_x = 0;
    *force_y = 0;
    *force_z = 0;
  } else {
    *force_x = magnitude * dx / r;
    *force_y = magnitude * dy / r;
    *force_z = magnitude * dz / r;
  }
  return;
}

void System::print_element(int element, int timestep) {
  printf("Element %d at timestep %d:\n", element, timestep);
  printf("Position: (%f, %f, %f)\n", this->state.x[timestep][element], this->state.y[timestep][element], this->state.z[timestep][element]);
  printf("Velocity: (%f, %f, %f)\n", this->state.vx[timestep][element], this->state.vy[timestep][element], this->state.vz[timestep][element]);
  printf("Mass: %f\n", this->state.mass[element]);
  return;
}