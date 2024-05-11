#include <iostream>
#include <string>
#include <H5Cpp.h>
#include <time.h>
#include <math.h> // for sqrt

using namespace H5;

#define DEFAULT_MASS 1.0
#define NUM_VALUES 7  // x, y, z, vx, vy, vz, mass
#define DEFAULT_FILENAME "initial_conditions.hdf5"

int main(int argc, char *argv[]) {

  const int num_elements = 16;
  const int scenario = 2;

  std::string filename = DEFAULT_FILENAME;
  if (argc > 1) {
    filename = argv[1];
  }
  filename = "data/" + filename; // prepend "data/" to the filename
  printf("Writing initial conditions to %s\n", filename.c_str());

  // printf("creating %d elements\n", num_elements);


  clock_t start_time, end_time;
  start_time = clock();
  
  // Create a new file
  H5File file(filename, H5F_ACC_TRUNC); 

  // Create a dataspace for a dataset
  // [ element, value ]
  const hsize_t DIMS[2] = {num_elements, NUM_VALUES}; 
  DataSpace dataspace(2, DIMS);

  // Create a dataset
  DataSet dataset = file.createDataSet("dataset", PredType::NATIVE_FLOAT, dataspace);
  
  // Fill the dataset with some data
  float data[num_elements][NUM_VALUES];

  if (scenario == 1) {
    // Element 0 (The Sun)
    data[0][0] = 0.0;
    data[0][1] = 0.0;
    data[0][2] = 0.0;
    data[0][3] = 0.0;
    data[0][4] = 0.0;
    data[0][5] = 0.0;
    data[0][6] = 1e7; // mass

    // Element 1 (Earth)
    data[1][0] = 1.0; // x position
    data[1][1] = 0.0;
    data[1][2] = 0.0;
    data[1][3] = 0.0;
    data[1][4] = 1.0; // y velocity
    data[1][5] = 0.0;
    data[1][6] = 1.0; // mass

    // // Element 2 (Earth's Moon)
    // data[2][0] = 1.0 + 0.0022; // x position +0.0024
    // data[2][1] = 0.0;
    // data[2][2] = 0.0;
    // data[2][3] = 0.0;
    // data[2][4] = 1.0 + 1/31; // y velocity (1/30 of the velocity of the Earth)
    // data[2][5] = 0.0;
    // data[2][6] = 1.0/81; // mass (1/81 of the mass of the Earth)

    // New Element 2 (Another Planet)
    if (num_elements > 2) {
      const float dist_factor = 1.45;
      data[2][0] = 1.0 * dist_factor;
      data[2][1] = 0.0;
      data[2][2] = 0.0;
      data[2][3] = 0.0;
      data[2][4] = 1.0/sqrt(dist_factor); // 1 / sqrt(2) of the velocity 
      data[2][5] = 0;
      data[2][6] = 1.0; // mass

      // The rest of the planets will be randomly placed
      for (int i = 3; i < num_elements; i++) {
        data[i][0] = (rand() % 1000) / 1000.0; // x position
        data[i][1] = (rand() % 1000) / 1000.0; // y position
        data[i][2] = (rand() % 1000) / 1000.0; // z position
        data[i][3] = (rand() % 1000) / 1000.0; // x velocity
        data[i][4] = (rand() % 1000) / 1000.0; // y velocity
        data[i][5] = (rand() % 1000) / 1000.0; // z velocity
        data[i][6] = DEFAULT_MASS; // mass
      }
    }
  } else if (scenario == 2) {
    for (int i = 0; i < num_elements; i++) {
      // random values between -1 and 1
      data[i][0] = 5 * ((rand() % 1000) / 500.0 - 1); // x position
      data[i][1] = 5 * ((rand() % 1000) / 500.0 - 1); // y position
      data[i][2] = 5 * ((rand() % 980) / 500.0 - 1); // z position
      data[i][3] = (rand() % 1000) / 500.0 - 1; // x velocity
      data[i][4] = (rand() % 1000) / 500.0 - 1; // y velocity
      data[i][5] = (rand() % 1000) / 500.0 - 1; // z velocity
      data[i][6] = (rand() % 10000) * 1000 * DEFAULT_MASS; // mass
    }
  }


  // Write the dataset to the file
  dataset.write(data, PredType::NATIVE_FLOAT);  

  dataset.close();
  file.close();

  end_time = clock();
  double time_taken = double(end_time - start_time) / double(CLOCKS_PER_SEC);
  printf("Done. Time taken: %f seconds.\n", time_taken);
  return 0;
}