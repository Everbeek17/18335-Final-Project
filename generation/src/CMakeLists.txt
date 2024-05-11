
find_package(HDF5 REQUIRED COMPONENTS CXX HL) # Find the HDF5 libraries

add_executable(Generator_exe main.cpp)

target_include_directories(Generator_exe PUBLIC
  ${HDF5_INCLUDE_DIRS})
target_link_libraries(Generator_exe
  ${HDF5_LIBRARIES})
