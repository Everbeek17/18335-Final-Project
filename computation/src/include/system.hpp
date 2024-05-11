#ifndef SYSTEM_H
#define SYSTEM_H

#include "parameters.hpp"
#include "declarations.hpp"

#include <vector>


// Holds the state of the system at each time step
struct State_Data {

  // Position
  std::vector<std::vector<float>> x;
  std::vector<std::vector<float>> y;
  std::vector<std::vector<float>> z;

  // Velocity
  std::vector<std::vector<float>> vx;
  std::vector<std::vector<float>> vy;
  std::vector<std::vector<float>> vz;

  // Mass
  std::vector<float> mass;

  // Constructor
  State_Data(int num_time_steps, int num_elements) {
    x.resize(num_time_steps);
    y.resize(num_time_steps);
    z.resize(num_time_steps);
    vx.resize(num_time_steps);
    vy.resize(num_time_steps);
    vz.resize(num_time_steps);
    mass.resize(num_elements);

    for (int i = 0; i < num_time_steps; i++) {
      x[i].resize(num_elements);
      y[i].resize(num_elements);
      z[i].resize(num_elements);
      vx[i].resize(num_elements);
      vy[i].resize(num_elements);
      vz[i].resize(num_elements);
    }
  }
};


struct Block {
  // Stuff initialized only once
  // struct Block** children;
  struct Block* parent = nullptr;
  std::vector<Block> children;
  int layer;  // 0 is the base layer
  int layer_idx;  // 0-7 for each layer

  // Stuff re-initialized every time step
  int num_elements;
  std::vector<int> element_idx; // stores the index of the element in the system
  float mass;
  float x_min, x_max, x_mid;  // I don't actually need these, but it's convenient
  float y_min, y_max, y_mid;
  float z_min, z_max, z_mid;

  Block() : Block(0, -1, nullptr) {}

  // Constructor
  Block(const int layer, const int layer_idx, Block* parent) : layer(layer), layer_idx(layer_idx), parent(parent) {
    // Recursively initialize children (until lowest layer)
    if (this->layer < NUM_LAYERS) {
      for (int i = 0; i < NUM_BLOCKS_PER_LAYER; i++) {
        // printf("[layer %d]Creating Child Block %d\n", this->layer, i);
        this->children.emplace_back(this->layer + 1, i, this);
        // printf("[layer %d]Created Child Block %d\n", this->layer, i);
      }
    }
    // printf("layer sanity check: %d\n", this->layer);
  }

  void initialize_parent(Block* parent) {
    if (this->layer != 0) {
      this->parent = parent;
    }

    // Recursively initialize children (until lowest layer)
    if (this->layer < NUM_LAYERS) {
      for (int i = 0; i < NUM_BLOCKS_PER_LAYER; i++) {
        this->children[i].initialize_parent(this);
      }
    }
  }

  std::vector<Block*> get_one_away_neighbors() {
    std::vector<Block*> relevant_neighbors;

    // This is nearly impossible to do recursively...
    // TODO: Make this real. This is a placeholder.
    // if (this->layer == 0) {
    //   for (int i = 0; i < NUM_BLOCKS_PER_LAYER; i++) {
    //     relevant_neighbors.push_back(&this->children[i]);
    //   }
    // } else {
    //   relevant_neighbors.push_back(this->parent);
    //   for (int i = 0; i < NUM_BLOCKS_PER_LAYER; i++) {
    //     if (this->layer_idx != i) {
    //       relevant_neighbors.push_back(&this->parent->children[i]);
    //     }
    //   }
    // }

    return relevant_neighbors;
  }

  // defined in fmm_solver.cpp
  void init_block_recursive(float x_min, float x_max, float y_min, float y_max, float z_min, float z_max);

  // defined in fmm_solver.cpp
  void add_element(int element_idx, float x, float y, float z, float mass);
};

// Holds the metadata of the system
struct System {
  int num_time_steps;
  int num_elements;
  float delta_t;
  float actual_delta_t;
  struct State_Data state;

  // Constructor
  System(float ic_data[][NUM_VALUES], const int num_elements, const int num_time_steps, const float delta_t);

  // Destructor
  ~System();

  void print_element(int element, int timestep);

  // Propogates the velocity of each element forward one timestep
  void propogate_state(int curr_timestep);
  
  // Propogates the position of each element forward one timestep
  void interpolate_position(int curr_timestep);

  // Calculate the gravitational force between any two objects
  void static Calculate_Gravitational_Force(float pos_x1, float pos_x2, float pos_y1, float pos_y2, float pos_z1, float pos_z2, float mass1, float mass2, float* force_x, float* force_y, float* force_z);

  // Direct Solver Methods
  void solve_direct();
  void update_velocity_direct(int curr_timestep);
  void calculate_gravitational_force_direct(int element_1, int element_2, int curr_timestep, float* force_x, float* force_y, float* force_z);

  // FMM Solver Methods & Variables
  struct Block base_block;
  // struct Block* base_block = nullptr;
  void solve_fmm();
  // void initialize_fmm();
  void decompose_domain_fmm(int curr_timestep);
  void solve_time_step_fmm(int curr_timestep);
  
};




#endif  // SYSTEM_H
