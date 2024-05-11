
find_package(HDF5 REQUIRED COMPONENTS CXX HL) # Find the HDF5 libraries

# Create the executable
add_executable(Solver_exe
  main.cpp
  system.cpp
  direct_solver.cpp
  fmm_solver.cpp 
  output_results.cpp)

target_include_directories(Solver_exe PUBLIC
  ${HDF5_INCLUDE_DIRS}
  ${CMAKE_SOURCE_DIR}/src/include)
           

target_link_libraries(Solver_exe 
  ${HDF5_LIBRARIES})

