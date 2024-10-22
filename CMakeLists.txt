cmake_minimum_required(VERSION 3.26 FATAL_ERROR)

project(
  orderbook
  VERSION 0.1
  LANGUAGES CXX
)

include(cmake/PreventInSourceBuilds.cmake)
include(cmake/SystemLink.cmake)

# Only set the cxx_standard if it is not set by someone else
if (NOT DEFINED CMAKE_CXX_STANDARD)
  set(CMAKE_CXX_STANDARD 20)
endif()

# strongly encouraged to enable this globally to avoid conflicts between
# -Wpedantic being enabled and -std=c++20 and -std=gnu++20 for example
# when compiling with PCH enabled
set(CMAKE_CXX_EXTENSIONS OFF)

add_library(default_warnings INTERFACE)

include(cmake/CompilerWarnings.cmake)
_set_project_warnings(
    default_warnings
    ""
    ""
    "")

include(Dependencies.cmake)

add_subdirectory(include)
add_subdirectory(tests)
add_subdirectory(bench)