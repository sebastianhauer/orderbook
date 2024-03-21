add_library(ob INTERFACE)

target_include_directories(
  ob
  INTERFACE $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
)

target_link_system_libraries(
  ob
  INTERFACE
  fmt::fmt
  Boost::intrusive
  gtl)
