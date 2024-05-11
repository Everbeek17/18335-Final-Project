#include <iostream>
#include <string>
#include <H5Cpp.h>
#include <time.h> // for timing
#include <algorithm> // for std::copy

#include "include/parameters.hpp"
#include "include/declarations.hpp"
#include "include/system.hpp"

using namespace H5; // temp

int main(int argc, char *argv[]) {

  std::string input_filename = "data/initial_conditions.hdf5";
  if (argc > 1) {
    input_filename = argv[1];
  }
  printf("Reading initial conditions from %s\n", input_filename.c_str());
  clock_t start_time, end_time;
  double time_taken;
  start_time = clock();

  // Open the ic_file for reading
  H5File ic_file(input_filename, H5F_ACC_RDONLY); 

  // Open the ic_dataset
  DataSet ic_dataset = ic_file.openDataSet("dataset");

  // Get the ic_dataspace
  DataSpace ic_dataspace = ic_dataset.getSpace();

  // Get the dimensions of the ic_dataspace
  int rank = ic_dataspace.getSimpleExtentNdims();
  hsize_t dims[rank];
  ic_dataspace.getSimpleExtentDims(dims);
  // const hsize_t ic_num_values = dims[0];
  const hsize_t num_elements = dims[0];
  // printf("Number of elements: %d\n", num_elements);

  // Read the ic_data to a buffer
  float ic_data[num_elements][NUM_VALUES];
  ic_dataset.read(ic_data, PredType::NATIVE_FLOAT);

  ic_dataset.close();
  ic_file.close();

  end_time = clock();
  time_taken = double(end_time - start_time) / double(CLOCKS_PER_SEC);
  printf("Done. Time taken: %f seconds\n", time_taken);







  // Initialize the system
  const int num_time_steps = 200; // 365 @ 1.0 = 1 year
  const float time_step_size = 1.0; // 1 day per timestep
  // printf("System constructor outside\n");
  struct System system(ic_data, num_elements, num_time_steps, time_step_size);
  // printf("System constructor finished\n");
  // printf("0 layer: %d\n", system.base_block.layer);


  // printf("Initial Conditions:\n");
  // for (int i = 0; i < num_elements; i++) {
  //   system.print_element(i, 0);
  // }


  // Solve the system with both the FMM and direct solvers
  // system.solve_fmm();
  system.solve_direct();
  


  // Output the results (from the direct solver) to a file
  output_results_HDF5(system);

  printf("All Done Solving.\n");


  return 0;
}