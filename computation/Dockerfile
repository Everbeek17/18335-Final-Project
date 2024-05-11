
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

# Makes folder and moves to it
WORKDIR /Solver

# Copy the source code to the container
COPY src/ ./src/
COPY CMakeLists.txt ./

# Make and switch to a build directory
WORKDIR /Solver/build

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

# Copies the built executable from the previous image to the new image.
# Copies into the /app/ folder
# (--from specifies which stage to copy from)
COPY --from=build_stage \
  ./Solver/build/src/Solver_exe \
  ./app/

# Runs the executable (with any passed in arguments)
ENTRYPOINT [ "./app/Solver_exe" ]
