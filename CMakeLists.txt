cmake_minimum_required(VERSION "3.15")

project(nsa)

if (NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
  message (STATUS "Setting build type to 'Release' as none was specified.")
  set (CMAKE_BUILD_TYPE "Release" CACHE STRING "Choose the type of build." FORCE)
  # Set the possible values of build type for cmake-gui
  set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS "Debug" "Release"
    "MinSizeRel" "RelWithDebInfo")
else ()
  STRING (TOLOWER "${CMAKE_BUILD_TYPE}" CMAKE_BUILD_TYPE_LOWER)
  set (DEBUG OFF)
  if (CMAKE_BUILD_TYPE_LOWER STREQUAL "debug")
    set (DEBUG ON)
  endif ()
endif ()

set(CMAKE_CXX_STANDARD 11)

if (DEBUG)
  message (STATUS "Setting build type to 'Debug'.")
  set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g -DDEBUG -Wall")
endif ()

set(CMAKE_THREAD_PREFER_PTHREAD TRUE)
set(THREADS_PREFER_PTHREAD_FLAG TRUE)
find_package(Threads REQUIRED)

find_package(OpenMP REQUIRED)
add_executable(nsa nsa.cpp ConfigFile.cpp Dataset.cpp Detector.cpp Geometry.cpp)
target_compile_options(nsa PUBLIC -Wall ${OpenMP_CXX_FLAGS})
target_link_libraries(nsa ${OpenMP_CXX_FLAGS})
