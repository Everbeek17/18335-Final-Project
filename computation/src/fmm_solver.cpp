#include <iostream>
#include <string>
#include <time.h> // for timing

#include "include/parameters.hpp"
#include "include/declarations.hpp"
#include "include/system.hpp"



void System::solve_fmm() {
  if (this->num_time_steps > 1) {
    printf("[FMM] Solving for %d additional timesteps\n", this->num_time_steps-1);
    clock_t start_time, end_time;
    double time_taken;
    start_time = clock();

    // this->print_element(0, 0);
    // this->print_element(2, 0);

    // Iterate over each subsequent timestep
    for (int timestep = 1; timestep < this->num_time_steps; timestep++) {

      // Initialize the velocities and positions of each element
      // as the velocities and positions from the previous timestep.
      this->propogate_state(timestep);

      // printf("2 layer: %d\n", this->base_block.layer);

      // Initialize the blocks
      this->decompose_domain_fmm(timestep);


      // Now all the elements are referenced in their blocks,
      // now we have to actually do the actual FMM calculations.
      this->solve_time_step_fmm(timestep);


      // Solve for and update the position of each element
      // (using the updated velocities calculated above)
      this->interpolate_position(timestep);
    }

    end_time = clock();
    time_taken = double(end_time - start_time) / double(CLOCKS_PER_SEC);
    printf("Done. Time taken: %f seconds.\n", time_taken);

  } else {
    printf("No additional timesteps to solve for.\n");
  }
  return;
}

const float boundary_buffer = 0.1;
void System::decompose_domain_fmm(int curr_timestep) {
  // printf("Decomposing domain for timestep %d\n", curr_timestep);
  // INEFFECIENTLY IMPLEMENTED O(n)
  // find the max and min of the x, y, and z values
  // (state is already propogated to the current timestep)
  float x_min = this->state.x[curr_timestep][0];
  float x_max = x_min;
  float y_min = this->state.y[curr_timestep][0];
  float y_max = y_min;
  float z_min = this->state.z[curr_timestep][0];
  float z_max = z_min;
  for (int element = 0; element < this->num_elements; element++) {
    if (this->state.x[curr_timestep][element] < x_min) {
      x_min = this->state.x[curr_timestep][element];
    } else if (this->state.x[curr_timestep][element] > x_max) {
      x_max = this->state.x[curr_timestep][element];
    }
    if (this->state.y[curr_timestep][element] < y_min) {
      y_min = this->state.y[curr_timestep][element];
    } else if (this->state.y[curr_timestep][element] > y_max) {
      y_max = this->state.y[curr_timestep][element];
    }
    if (this->state.z[curr_timestep][element] < z_min) {
      z_min = this->state.z[curr_timestep][element];
    } else if (this->state.z[curr_timestep][element] > z_max) {
      z_max = this->state.z[curr_timestep][element];
    }
  }

  // add the buffer to the max and min values
  // (incase the max and min are the same)
  x_min -= boundary_buffer;
  x_max += boundary_buffer;
  y_min -= boundary_buffer;
  y_max += boundary_buffer;
  z_min -= boundary_buffer;
  z_max += boundary_buffer;
  // printf("x_min: %f, x_max: %f, y_min: %f, y_max: %f, z_min: %f, z_max: %f\n", x_min, x_max, y_min, y_max, z_min, z_max);

  // this->base_block.x_min = x_min;
  // Now we have the max and min of the x, y, and z values
  // and we can recursively define the boundaries of the blocks
  this->base_block.init_block_recursive(x_min, x_max, y_min, y_max, z_min, z_max);

  // printf("layer: %d\n", this->base_block.layer);


  // Finally, we can assign each element to its blocks.
  // We iterate over each element, and pass it to the base block.
  // The base block will pass it to the appropriate child block.
  for (int element = 0; element < this->num_elements; element++) {
    float x = this->state.x[curr_timestep][element];
    float y = this->state.y[curr_timestep][element];
    float z = this->state.z[curr_timestep][element];
    float mass = this->state.mass[element];
    this->base_block.add_element(element, x, y, z, mass);
  }

  // printf("Finished decomposing domain for timestep %d\n", curr_timestep);
}

void Block::init_block_recursive(float x_min, float x_max, float y_min, float y_max, float z_min, float z_max) {
  this->x_min = x_min;
  this->x_max = x_max;
  this->y_min = y_min;
  this->y_max = y_max;
  this->z_min = z_min;
  this->z_max = z_max;
  this->x_mid = (x_min + x_max) / 2;
  this->y_mid = (y_min + y_max) / 2;
  this->z_mid = (z_min + z_max) / 2;

  // Reset the elements
  this->element_idx.clear();
  this->num_elements = 0;
  this->mass = 0;

  // printf("[layer %d] %f %f %f %f %f %f\n", layer, this->x_min, this->x_max, this->y_min, this->y_max, this->z_min, this->z_max);
  // Recursively define boundaries for children
  if (this->layer < NUM_LAYERS) {
    // Bottom half
    // printf("[layer %d] child %d: %f %f %f %f %f %f\n", layer, 0, x_min, x_mid, y_min, y_mid, z_min, z_mid);
    this->children[0].init_block_recursive(x_min, x_mid, y_min, y_mid, z_min, z_mid);
    // printf("[layer %d] child %d: %f %f %f %f %f %f\n", layer, 1, x_mid, x_max, y_min, y_mid, z_min, z_mid);
    this->children[1].init_block_recursive(x_mid, x_max, y_min, y_mid, z_min, z_mid);
    // printf("[layer %d] child %d: %f %f %f %f %f %f\n", layer, 2, x_min, x_mid, y_mid, y_max, z_min, z_mid);
    this->children[2].init_block_recursive(x_min, x_mid, y_mid, y_max, z_min, z_mid);
    // printf("[layer %d] child %d: %f %f %f %f %f %f\n", layer, 3, x_mid, x_max, y_mid, y_max, z_min, z_mid);
    this->children[3].init_block_recursive(x_mid, x_max, y_mid, y_max, z_min, z_mid);

    // Top half
    // printf("[layer %d] child %d: %f %f %f %f %f %f\n", layer, 4, x_min, x_mid, y_min, y_mid, z_mid, z_max);
    this->children[4].init_block_recursive(x_min, x_mid, y_min, y_mid, z_mid, z_max);
    // printf("[layer %d] child %d: %f %f %f %f %f %f\n", layer, 5, x_mid, x_max, y_min, y_mid, z_mid, z_max);
    this->children[5].init_block_recursive(x_mid, x_max, y_min, y_mid, z_mid, z_max);
    // printf("[layer %d] child %d: %f %f %f %f %f %f\n", layer, 6, x_min, x_mid, y_mid, y_max, z_mid, z_max);
    this->children[6].init_block_recursive(x_min, x_mid, y_mid, y_max, z_mid, z_max);
    // printf("[layer %d] child %d: %f %f %f %f %f %f\n", layer, 7, x_mid, x_max, y_mid, y_max, z_mid, z_max);
    this->children[7].init_block_recursive(x_mid, x_max, y_mid, y_max, z_mid, z_max);
  }
}

void Block::add_element(int element_idx, float x, float y, float z, float mass) {
  if (this->layer != 0) {
    // printf("Adding element %d to [%d, %d]\n", element_idx, this->layer, this->layer_idx);
    // printf("x: %f, y: %f, z: %f\n", x, y, z);
    // printf("x_min: %f, x_max: %f, y_min: %f, y_max: %f, z_min: %f, z_max: %f\n", this->x_min, this->x_max, this->y_min, this->y_max, this->z_min, this->z_max);
    this->element_idx.push_back(element_idx);
    this->num_elements++;
    this->mass += mass;
  }
  if (this->layer < NUM_LAYERS) {
    
    // x first
    if (x < this->x_mid) {
      // y next
      if (y < this->y_mid) {
        // z last
        if (z < this->z_mid) {
          this->children[0].add_element(element_idx, x, y, z, mass);
        } else {
          this->children[4].add_element(element_idx, x, y, z, mass);
        }
      } else {
        // z last
        if (z < this->z_mid) {
          this->children[2].add_element(element_idx, x, y, z, mass);
        } else {
          this->children[6].add_element(element_idx, x, y, z, mass);
        }
      }
    } else {
      // y next
      if (y < this->y_mid) {
        // z last
        if (z < this->z_mid) {
          this->children[1].add_element(element_idx, x, y, z, mass);
        } else {
          this->children[5].add_element(element_idx, x, y, z, mass);
        }
      } else {
        // z last
        if (z < this->z_mid) {
          this->children[3].add_element(element_idx, x, y, z, mass);
        } else {
          this->children[7].add_element(element_idx, x, y, z, mass);
        }
      }
    }
  }
}

void System::solve_time_step_fmm(int curr_timestep) {
  // Iterate over each element
  for (int element = 0; element < this->num_elements; element++) {
    // Perform one force calculation for each element
    
    // TEMP
    float sign = (curr_timestep % 2 == 0) ? 1 : -1;
    float force_x, force_y, force_z;
    int other_element = (int (element * this->state.vy[curr_timestep][element])) % this->num_elements;
    this->calculate_gravitational_force_direct(element, other_element, curr_timestep, &force_x, &force_y, &force_z);

    // update element velocity
    this->state.vx[curr_timestep][element] += sign * force_x / this->state.mass[element] * this->actual_delta_t;
    this->state.vy[curr_timestep][element] += sign * force_y / this->state.mass[element] * this->actual_delta_t;
    this->state.vz[curr_timestep][element] += sign * force_z / this->state.mass[element] * this->actual_delta_t;

    // apply to other element as well
    this->state.vx[curr_timestep][other_element] -= sign * force_x / this->state.mass[other_element] * this->actual_delta_t;
    this->state.vy[curr_timestep][other_element] -= sign * force_y / this->state.mass[other_element] * this->actual_delta_t;
    this->state.vz[curr_timestep][other_element] -= sign * force_z / this->state.mass[other_element] * this->actual_delta_t;
  }
}

