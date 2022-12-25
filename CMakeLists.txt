############################################
# PATHFINDER COMPILATION CMAKE SCRIPT
############################################

# PROJECT: PATHFINDER
# Create project, load parameters (flags & default values)
project(pathfinder)

# CMAKE MINIMUM REQUIRED VERSION
cmake_minimum_required(VERSION 3.21)

# CXX Standard to be used by the compiler
set(CMAKE_CXX_STANDARD 17)

############################################
# GENERAL INFORMATION
############################################

# OS name & version
#cmake_host_system_information(RESULT OS_NAME QUERY OS_NAME)
#cmake_host_system_information(RESULT OS_RELEASE QUERY OS_RELEASE)
#cmake_host_system_information(RESULT OS_VERSION QUERY OS_VERSION)
#cmake_host_system_information(RESULT OS_PLATFORM QUERY OS_PLATFORM)
#message(STATUS "SYSTEM: ${OS_NAME} ${OS_RELEASE} ${OS_VERSION} ${OS_PLATFORM}")

# CPU info
#cmake_host_system_information(RESULT PROC_NAME QUERY PROCESSOR_DESCRIPTION)
#message(STATUS "PROCESSOR: ${PROC_NAME}")

# Memory
#cmake_host_system_information(RESULT VIRT_MEM QUERY TOTAL_PHYSICAL_MEMORY)
#message(STATUS "MEMORY: ${VIRT_MEM} MiB")

############################################
# GET NUMBER OF CPU CORES
############################################

# Load ProcessorCount module
include(ProcessorCount)

# Try to get number of cores
ProcessorCount(N_CORES)

# Safety-check: if unable to get number of cores, assume quad-core system
if(N_CORES EQUAL 0)
    set(N_CORES 4)
endif()

# Info
message(STATUS "N_CORES: ${N_CORES}")

# Set global compilation flag: number of jobs
set(CTEST_BUILD_FLAGS -j${N_CORES})

############################################
# SET OUTPUT DIRECTORIES
############################################

# Check for install prefix flag, not set (loaded default /usr/local) -> set it to current dir
if (CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)
    set (CMAKE_INSTALL_PREFIX  "${CMAKE_CURRENT_BINARY_DIR}"
            CACHE PATH "${CMAKE_CURRENT_BINARY_DIR}" FORCE)
endif()

# 3rd parties directory
set(CMAKE_3RDPARTY_DIR "${CMAKE_SOURCE_DIR}/build/pathfinder-3rdparty"
        CACHE STRING "Default directory to extracted 3rd parties")
message(STATUS "CMAKE_3RDPARTY_DIR: ${CMAKE_3RDPARTY_DIR}")
string(APPEND UCFLAGS " -D CMAKE_3RDPARTY_DIR=\"\\\"${CMAKE_3RDPARTY_DIR}\\\"\"")

# Output directories
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_INSTALL_PREFIX}/bin/")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_INSTALL_PREFIX}/lib/")
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_INSTALL_PREFIX}/lib/")

# Define var to store all user flags
set(UCFLAGS " ")

# Application version information
# This is automatic; do not touch anything
string(APPEND UCFLAGS " -D CODE_VERSION=\"\\\"$(shell cat ${CMAKE_SOURCE_DIR}/VERSION)\\\"\"")
string(APPEND UCFLAGS " -D OS_VERSION=\"\\\"$(shell uname -iors)\\\"\"")
string(APPEND UCFLAGS " -D GIT_BRANCH=\"\\\"$(shell git symbolic-ref --short HEAD)\\\"\"")
string(APPEND UCFLAGS " -D GIT_HASH=\"\\\"$(shell git log -1 --pretty=format:\"%H\")\\\"\"")
string(APPEND UCFLAGS " -D USER_NAME=\"\\\"$(USER)\\\"\"")

# Force re-compilation of STATIC_FLAGS.h
# This phony target will always re-compile STATIC_FLAGS.h, thus always updating the compilation flag's values
add_custom_target(RECOMPILE_STATIC_FLAGS touch ${CMAKE_SOURCE_DIR}/src/core/STATIC_FLAGS.h)

# Note for cmake developers: add this phony dependency to all main programs and subroutines that use static flags.
# Ex: add_dependencies(pathfinder_hachitables_c3po RECOMPILE_STATIC_FLAGS)

############################################
# BUILD EXTERNAL PROJECTS
############################################
# Note for cmake developers: this projects have their own CMakeLists, we have to build and install them
find_package(Eigen3 REQUIRED)
IF( NOT EIGEN3_INCLUDE_DIR )
    MESSAGE( FATAL_ERROR "Please point the environment variable EIGEN3_INCLUDE_DIR to the include directory of your Eigen3 installation.")
ENDIF()
message(STATUS "Eigen is in: ${EIGEN3_INCLUDE_DIR}")

find_package(Boost COMPONENTS program_options)
IF( NOT Boost_INCLUDE_DIRS )
    MESSAGE( FATAL_ERROR "Please point the environment variable Boost_INCLUDE_DIRS to the include directory of your BOOST installation.")
ENDIF()
message(STATUS "Boost is in: ${Boost_INCLUDE_DIRS}")

# Load ExternalProject module
include(ExternalProject)

## Build OMPL API
## Please see instructions to do so: https://ompl.kavrakilab.org/buildOptions.html
## If you are building OMPL from source, there are several options that you can use to configure how OMPL is compiled.
## The options can be set via the command line like so: cmake -D<option>=ON ../... If you previously ran cmake in your
## build directory, it is safest to remove the CMakeCache.txt before re-running cmake with different options. Below is
## a list of all options.
ExternalProject_Add(ompl
        SOURCE_DIR ${CMAKE_3RDPARTY_DIR}/ompl-1.5.2/
        PREFIX ${CMAKE_3RDPARTY_DIR}/ompl-1.5.2/
        CMAKE_COMMAND cmake ..
        -DCMAKE_BUILD_TYPE=Release
        -DCMAKE_INSTALL_PREFIX=${CMAKE_LIBRARY_OUTPUT_DIRECTORY}ompl-1.5.2/
        -DOMPL_BUILD_DEMOS=OFF
        -DOMPL_BUILD_PYBINDINGS=OFF
        -DOMPL_BUILD_PYTEST=OFF
        -DOMPL_BUILD_TESTS=OFF
        -DOMPL_REGISTRATION=OFF
        -DOMPL_VERSIONED_INSTALL=OFF
        BUILD_COMMAND make -j${N_CORES}
        INSTALL_DIR ${CMAKE_LIBRARY_OUTPUT_DIRECTORY}ompl-1.5.2/
        INSTALL_COMMAND make install
        )

# ExternalProject_Add(RSJP
#         SOURCE_DIR ${CMAKE_3RDPARTY_DIR}/RSJp-cpp-master/
#         PREFIX ${CMAKE_3RDPARTY_DIR}/RSJp-cpp-master/
#         INSTALL_DIR ${CMAKE_LIBRARY_OUTPUT_DIRECTORY}RSJp-cpp-master/
#         )

############################################
# EXTERNAL INFORMATION
############################################
# Get info from external ompl library
ExternalProject_Get_Property(ompl install_dir)
set(omplinclude "${install_dir}/include")
set(ompllib_dir "${install_dir}/lib")
set(ompllib_file "${CMAKE_SHARED_LIBRARY_PREFIX}ompl${CMAKE_SHARED_LIBRARY_SUFFIX}")
set(ompllib_path "${ompllib_dir}/${ompllib_file}")

# Get info from external JSON parser: RSJp-cpp-master
# ExternalProject_Get_Property(RSJP install_dir)
# set(RSJPinclude "${install_dir}/include")
# set(RSJPlib_dir "${install_dir}/lib")
# set(RSJPlib_file "${CMAKE_SHARED_LIBRARY_PREFIX}RSJp-cpp-master${CMAKE_SHARED_LIBRARY_SUFFIX}")
# set(RSJPlib_path "${RSJPlib_dir}/${RSJPlib_file}")

############################################
# FILES LOCATION
############################################
# Location of header files and libraries (inside source dir, except the includes)
include_directories(
        ${omplinclude}
        ${CMAKE_LIBRARY_OUTPUT_DIRECTORY}RSJp-cpp-master/include
        src/core
        ${EIGEN3_INCLUDE_DIR}
        ${Boost_INCLUDE_DIRS})
#===========================================
# LIBRARY: OMPL (IMPORTED LIBRARY)
#===========================================
set(LIBRARY_OMPL "ompllib")

add_library(${LIBRARY_OMPL} SHARED IMPORTED)

# Provide the full path to the library, so CMake knows where to find it
set_target_properties(${LIBRARY_OMPL} PROPERTIES
        IMPORTED_LOCATION ${ompllib_path})

#===========================================
# LIBRARY: RSJp-cpp-master (IMPORTED LIBRARY)
#===========================================
add_library(rsjp SHARED
        ${CMAKE_3RDPARTY_DIR}/RSJp-cpp-master/RSJparser.tcc)

# Provide the full path to the library, so CMake knows where to find it
set_target_properties(rsjp PROPERTIES LINKER_LANGUAGE CXX COMPILE_FLAGS "-fPIC"
        PUBLIC_HEADER "${CMAKE_3RDPARTY_DIR}/RSJp-cpp-master/RSJparser.tcc")

# TODO: INSTALLATION STILL NOT WORKING, should change the prefix of the PREFIX and make install the makefile
install(TARGETS rsjp
        LIBRARY DESTINATION ${CMAKE_LIBRARY_OUTPUT_DIRECTORY}RSJp-cpp-master/lib
        PUBLIC_HEADER DESTINATION ${CMAKE_LIBRARY_OUTPUT_DIRECTORY}RSJp-cpp-master/include)

# add_custom_target(rsjp COMMAND make install
#         WORKING_DIRECTORY ${CMAKE_3RDPARTY_DIR}/RSJp-cpp-master/
#         )
#===========================================
# LIBRARY: POST-PROCESS, free OMPL and BOOST dependent
#===========================================
set(LIBRARY_PREPROCESS "preprocess")

add_library(${LIBRARY_PREPROCESS} SHARED
        src/preprocess/duavc_input_file.cpp)

target_link_libraries(${LIBRARY_PREPROCESS})

set_target_properties(${LIBRARY_CORE} PROPERTIES
        COMPILE_FLAGS "-fPIC"
        LINK_FLAGS "-Wl,-rpath,./") # To use relative paths in shared libs

#===========================================
# LIBRARY: PATHFINDER CORE
#===========================================
set(LIBRARY_CORE "core")

add_library(${LIBRARY_CORE} SHARED
        src/core/preprocessor.cpp
        src/core/simple_square.cpp
        src/core/motion_validator.cpp
        src/core/state_sampler.cpp
        src/core/Ampli_DubinsStateSpace.cpp
        src/core/plans.cpp
        src/core/state_validity_checker.cpp
        src/core/json_parser.cpp
        src/core/tools_str.cpp
        src/core/constants.h)

add_dependencies(${LIBRARY_CORE}
        ompl)

target_link_libraries(${LIBRARY_CORE}
        ${LIBRARY_OMPL})

set_target_properties(${LIBRARY_CORE} PROPERTIES
        COMPILE_FLAGS "-fPIC"
        LINK_FLAGS "-Wl,-rpath,./") # To use relative paths in shared libs

############################################
# EXECUTABLES: GEOMETRIC CAR
############################################
set(EXECUTABLE_NAME "pathfinder_geometric_car")

add_executable(${EXECUTABLE_NAME} src/main/geometric_car.cpp)

add_dependencies(${EXECUTABLE_NAME}
        RECOMPILE_STATIC_FLAGS)

target_link_libraries(${EXECUTABLE_NAME}
        ${LIBRARY_CORE}
        ${Boost_LIBRARIES})


############################################
# EXECUTABLES: DUAVC main
############################################
set(EXECUTABLE_NAME "duavc_planning")

add_executable(${EXECUTABLE_NAME} src/main/duavc_planning.cpp)

add_dependencies(${EXECUTABLE_NAME}
        RECOMPILE_STATIC_FLAGS)

target_link_libraries(${EXECUTABLE_NAME}
        ${LIBRARY_CORE}
        ${Boost_LIBRARIES})