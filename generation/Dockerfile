
# Starts image with a base linux alpine installation
FROM alpine:3.19 AS build_stage

# Installs the required build packages
# cmake for cmake
# build-base for make
# hdf5-dev for hdf5
RUN apk update && \
  apk add --no-cache \
    cmake=3.27.8-r0 \
    build-base=0.5-r3 \
    hdf5-dev=1.14.3-r0

# Used to check which version is installed
# ENTRYPOINT [ "apk", "info", "hdf5-dev" ]

# # Tried switching to Ubuntu to fix HDF5 issues:
# FROM ubuntu:22.04 AS build_stage 

# # Install the required build packages
# RUN apt-get update && \
#   apt-get install -y \
#     cmake=3.22.1-1ubuntu1.22.04.2 \
#     build-essential=12.9ubuntu3 \ 
#     libhdf5-dev=1.10.7+repack-4ubuntu2 
# Used to check which versions are installed
# ENTRYPOINT [ "apt-cache", "policy", "cmake", "build-essential", "libhdf5-dev" ]

# Makes folder and moves to it
WORKDIR /Generator

# Copy the source code to the container
COPY src/ ./src/
COPY CMakeLists.txt ./

# Make and switch to a build directory
WORKDIR /Generator/build

# Run cmake to build the source code
RUN cmake .. && \
  cmake --build .

# Starts a new image with a base linux alpine installation
FROM alpine:3.19

# Installs the required runtime packages
RUN apk update && \
  apk add --no-cache \
    libstdc++=13.2.1_git20231014-r0 \
    hdf5-dev=1.14.3-r0
# FROM ubuntu:22.04
# RUN apt-get update


# Copies the built executable from the previous image to the new image.
# Copies into the /app/ folder
# (--from specifies which stage to copy from)
COPY --from=build_stage \
  ./Generator/build/src/Generator_exe \
  ./app/

# Runs the executable (with any passed in arguments)
ENTRYPOINT [ "./app/Generator_exe" ]
# ENTRYPOINT [ "ls", "-a", "/Generator/build" ]
# ENTRYPOINT [ "ls", "-a", "/app" ]
