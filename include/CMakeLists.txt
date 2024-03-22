add_library(sh INTERFACE)

target_include_directories(
  sh
  INTERFACE $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
)

target_link_system_libraries(
  sh
  INTERFACE
  fmt::fmt
  Boost::align
  gtl)
