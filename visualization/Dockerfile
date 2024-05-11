
# Starts image with a base that has HDF5 already installed
# Includes Python 3.10.14
FROM hdfgroup/hdf5lib:1.14.3

# Adds source file to the container's base folder
COPY visualize.py .

# Installs the required Python packages
RUN pip install numpy matplotlib h5py

# Runs the Python script
ENTRYPOINT [ "python", "./visualize.py" ]
# CMD [ "pip", "freeze" ]
# CMD [ "touch", "/data/test.txt" ]
