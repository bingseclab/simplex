cmake_minimum_required(VERSION 3.2)
project (simplex)

set (Simplex_VERSION_MAJOR 1)
set (Simplex_VERSION_MINOR 0)

# Set build type
if (NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Release CACHE
      STRING "Build type (default Release):" FORCE)
endif()

set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)

option (SIMPLEX_BUILD_EVALS "Build Simplex's evaluations" OFF)
option (SIMPLEX_BUILD_TESTS "Build Simplex's test suite" OFF)

set (INSTALL_HAYAI OFF CACHE BOOL "Install Hayai in addition of library compilation")
set (BUILD_HAYAI_TESTS OFF CACHE BOOL "Build Hayai tests")
set (BUILD_HAYAI_SAMPLES OFF CACHE BOOL "Build Hayai samples")

# Print build resume.
# This function has been adapted from the OpenCV CMake utilities.
function(print_status txt var)
  if (var)
    message(STATUS ${txt} "YES")
  else (var)
    message(STATUS ${txt} "NO")
  endif (var)
endfunction()
message(STATUS "")
message(STATUS "Simplex build resume:")
print_status("  Building evaluations:  " ${SIMPLEX_BUILD_EVALS})
print_status("  Building tests:        " ${SIMPLEX_BUILD_TESTS})
message(STATUS "")

# Add subtargets
# This is done last so that all definitions and link/include paths 
# are available for subprojects
add_subdirectory(simplex)

if(SIMPLEX_BUILD_EVALS)
add_subdirectory(hayai)
endif(SIMPLEX_BUILD_EVALS)

if(SIMPLEX_BUILD_TESTS)
add_subdirectory(cunit/CUnit)
endif(SIMPLEX_BUILD_TESTS)
