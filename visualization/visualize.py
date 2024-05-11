import argparse
import os
import matplotlib.pyplot as plt
import numpy as np
import h5py
import sys

# Normalize the masses
# (Scales the masses to be between 0 and 1)
def normalize_masses(masses):
  mass_min = masses.min()
  mass_max = masses.max()
  if (mass_min == mass_max):
    return np.ones(masses.shape)
  else:
    masses_normalized = (masses - mass_min) / (mass_max - mass_min)
    return masses_normalized
  
def read_input_file(input_file):
  # Read in HDF5 file
  with h5py.File(input_file, "r") as file:

    # Get the datasets stored in the file
    position_dataset = file["positions"]
    mass_dataset = file["masses"]

    # convert to numpy arrays
    positions = np.array(position_dataset)
    masses = np.array(mass_dataset)
    return positions, masses




# Main function
if __name__ == "__main__":
  parser = argparse.ArgumentParser(description="Control input, output, and animation")
  parser.add_argument("-i", "--input", type=str, default="data/results.hdf5",
                        help="Specify the input file (default: data/results.hdf5)")
  parser.add_argument("-o", "--output", type=str, default="data/image.png",
                      help="Specify the output file location (default: data/image.png)")
  parser.add_argument("-a", "--animate", action="store_true", default=False,
                      help="Enable animation (default: False)")
  arguments = parser.parse_args()

  # TODO: make the plotting parameters optional arguments

  # Plotting parameters
  max_point_size = 200 # Scales by mass
  min_point_size = 10
  max_trail_width = 3   # Scales by mass
  min_trail_width = 0.25
  trail_length = 150
  trail_color = (0, 0, 1) # blue

  # TODO: Implement animation (for funsies)
  # my_function(arguments.animate, )



  input_file = arguments.input
  output_file = arguments.output

  positions, masses = read_input_file(input_file) 
  masses_normalized = normalize_masses(masses)

  # Get the shape of the data
  shape = positions.shape
  # num_values = shape[0]  # should be 3: x, y, z
  num_timesteps = shape[1]
  num_elements = shape[2]

  print("Done reading in", input_file)


  




  # Plot the data
  ax = plt.figure().add_subplot(projection='3d')
  ax.set_xlabel('X')
  ax.set_ylabel('Y')
  ax.set_zlabel('Z')

  # Set the aspect ratio of the plot (idk what this does tbh)
  ax.set_box_aspect([1,1,1])

  # Set the limits to be even 
  # with the data being placed in the middle of the plot
  min_x = positions[0].min()
  max_x = positions[0].max()
  min_y = positions[1].min()
  max_y = positions[1].max()
  min_z = positions[2].min()
  max_z = positions[2].max()
  max_range = max(max_x - min_x, max_y - min_y, max_z - min_z)
  middle_x = (max_x + min_x) / 2
  middle_y = (max_y + min_y) / 2
  middle_z = (max_z + min_z) / 2
  ax.set_xlim(middle_x - max_range / 2, middle_x + max_range / 2)
  ax.set_ylim(middle_y - max_range / 2, middle_y + max_range / 2)
  ax.set_zlim(middle_z - max_range / 2, middle_z + max_range / 2)









  # Plot each element
  for i in range(num_elements):
    trail_width = min_trail_width + (max_trail_width - min_trail_width) * masses_normalized[i]
    point_size = min_point_size + (max_point_size - min_point_size) * masses_normalized[i]

    # Get the trail positions
    trail_positions_x = positions[0, -trail_length:, i]
    trail_positions_y = positions[1, -trail_length:, i]
    trail_positions_z = positions[2, -trail_length:, i]

    actual_trail_length = len(trail_positions_x)

    # Iterate over each segment of the trail
    for j in range(actual_trail_length - 1):
      # Define the alpha of the segment
      segment_alpha = (j + trail_length - actual_trail_length) / trail_length
      # Plot the segment
      ax.plot(trail_positions_x[j:j+2], trail_positions_y[j:j+2], trail_positions_z[j:j+2], color=trail_color, linewidth=trail_width, alpha=segment_alpha)
    
    # Plot the current position
    ax.scatter(positions[0, -1, i], positions[1, -1, i], positions[2, -1, i], s=point_size)


  # Changes the viewing angle
  ax.view_init(elev=40, azim=30)

  # Save the plot
  plt.savefig(output_file, dpi=400)
  # plt.savefig('data/foo2.png', bbox_inches='tight')


  print("Done plotting to", output_file)




