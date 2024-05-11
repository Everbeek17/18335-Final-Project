#include <iostream>
#include <string>
#include <time.h> // for timing

#include "include/parameters.hpp"
#include "include/declarations.hpp"
#include "include/system.hpp"



void System::solve_direct() {
  if (this->num_time_steps > 1) {
    printf("[Direct] Solving for %d additional timesteps\n", this->num_time_steps-1);
    clock_t start_time, end_time;
    double time_taken;
    start_time = clock();

    // this->print_element(0, 0);
    // this->print_element(1, 0);

    // Iterate over each subsequent timestep
    for (int timestep = 1; timestep < this->num_time_steps; timestep++) {

      // this->print_element(1, timestep);

      // Initialize the velocities and positions of each element
      // as the velocities and positions from the previous timestep.
      this->propogate_state(timestep);

      // this->print_element(0, timestep);

      // Solve for and update the velocity of each element
      // by directly calculating the gravitational force between 
      // each pair of elements.
      this->update_velocity_direct(timestep);


      // Solve for and update the position of each element
      // (using the updated velocities calculated above)
      this->interpolate_position(timestep);

      // this->print_element(1, timestep);
    }

    end_time = clock();
    time_taken = double(end_time - start_time) / double(CLOCKS_PER_SEC);
    printf("Done. Time taken: %f seconds.\n", time_taken);

  } else {
    printf("No additional timesteps to solve for.\n");
  }
  return;
}

void System::update_velocity_direct(int curr_timestep) {
  // Iterate over each pair of elements
  for (int element_1 = 0; element_1 < this->num_elements; element_1++) {
    for (int element_2 = element_1 + 1; element_2 < this->num_elements; element_2++) {

      // Calculate the gravitational force between the two elements
      float force_x, force_y, force_z;
      this->calculate_gravitational_force_direct(element_1, element_2, curr_timestep, &force_x, &force_y, &force_z);
      // printf("Force between elements %d and %d: (%f, %f, %f)\n", element_1, element_2, force_x, force_y, force_z);

      // Update the velocity of each element
      // Element 1
      this->state.vx[curr_timestep][element_1] -= (force_x / this->state.mass[element_1]) * this->actual_delta_t;
      this->state.vy[curr_timestep][element_1] -= (force_y / this->state.mass[element_1]) * this->actual_delta_t;
      this->state.vz[curr_timestep][element_1] -= (force_z / this->state.mass[element_1]) * this->actual_delta_t;

      // Element 2
      this->state.vx[curr_timestep][element_2] += (force_x / this->state.mass[element_2]) * this->actual_delta_t;
      this->state.vy[curr_timestep][element_2] += (force_y / this->state.mass[element_2]) * this->actual_delta_t;
      this->state.vz[curr_timestep][element_2] += (force_z / this->state.mass[element_2]) * this->actual_delta_t;
    }
  }
  return;
}


void System::calculate_gravitational_force_direct(int element_1, int element_2, int curr_timestep, float* force_x, float* force_y, float* force_z) {
  // Passes the positions and masses of the two elements to the
  // generic Calculate_Gravitational_Force function
  System::Calculate_Gravitational_Force(
    this->state.x[curr_timestep][element_1], 
    this->state.x[curr_timestep][element_2], 
    this->state.y[curr_timestep][element_1], 
    this->state.y[curr_timestep][element_2], 
    this->state.z[curr_timestep][element_1], 
    this->state.z[curr_timestep][element_2], 
    this->state.mass[element_1], 
    this->state.mass[element_2], 
    force_x, 
    force_y, 
    force_z
  );
  return;
}