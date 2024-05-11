/* This file holds the functions used to write to HDF5. */

#include <iostream>
#include <string>
#include <H5Cpp.h>
#include <time.h> // for timing
#include <algorithm> // for std::copy

#include "include/parameters.hpp"
#include "include/declarations.hpp"
#include "include/system.hpp"

using namespace H5; // for convenience

void output_results_HDF5(System system) {
  clock_t start_time, end_time;
  double time_taken;

  // TODO: write mass to output for plotting??
  // Write results to new HDF5 file
  std::string output_filename = "data/results.hdf5";
  printf("Writing results to %s\n", output_filename.c_str());
  start_time = clock();

  // Create a new file
  H5File out_file(output_filename, H5F_ACC_TRUNC); 


  /********************** Position Data **********************/
  int const position_num_values = 3; // x, y, z

  // Dataspace
  hsize_t const position_DIMS[3] = {position_num_values,
      static_cast<hsize_t>(system.num_time_steps),
      static_cast<hsize_t>(system.num_elements)}; 
  DataSpace position_dataspace(3, position_DIMS);

  // Dataset
  DataSet position_dataset = out_file.createDataSet("positions",
      PredType::NATIVE_FLOAT,
      position_dataspace);
  
  // Buffer
  float positions[position_num_values][system.num_time_steps][system.num_elements];

  // Populate Buffer
  for (int timestep = 0; timestep < system.num_time_steps; timestep++) {
    for (int element = 0; element < system.num_elements; element++) {
      positions[0][timestep][element] = system.state.x[timestep][element];
      positions[1][timestep][element] = system.state.y[timestep][element];
      positions[2][timestep][element] = system.state.z[timestep][element];
    }
  }

  // File Write
  position_dataset.write(positions, PredType::NATIVE_FLOAT);  
  position_dataset.close();


  /********************** Mass Data **********************/
  // Dataspace
  hsize_t const mass_DIMS[1] = {system.num_elements}; 
  DataSpace mass_dataspace(1, mass_DIMS);

  // Dataset
  DataSet mass_dataset = out_file.createDataSet("masses",
      PredType::NATIVE_FLOAT,
      mass_dataspace);
  
  // Buffer
  float masses[system.num_elements];

  // Populate Buffer
  for (int element = 0; element < system.num_elements; element++) {
    masses[element] = system.state.mass[element];
  }

  // File Write
  mass_dataset.write(masses, PredType::NATIVE_FLOAT);  
  mass_dataset.close();




  out_file.close();

  end_time = clock();
  time_taken = double(end_time - start_time) / double(CLOCKS_PER_SEC);
  printf("Done. Time taken: %f seconds.\n", time_taken);
}

