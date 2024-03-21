include(cmake/CPM.cmake)

# Done as a function so that updates to variables like
# CMAKE_CXX_FLAGS don't propagate out to other
# targets
function(_setup_dependencies)

  # For each dependency, see if it's
  # already been provided to us by a parent project

  if(NOT TARGET gtl)
    CPMAddPackage(
      NAME gtl
      GITHUB_REPOSITORY greg7mdp/gtl
      GIT_TAG v1.1.8
    )
  endif()

  if(NOT TARGET fmtlib::fmtlib)
    CPMAddPackage("gh:fmtlib/fmt#9.1.0")
  endif()

  if(NOT TARGET Catch2::Catch2WithMain)
    CPMAddPackage("gh:catchorg/Catch2@3.3.2")
  endif()

  if(NOT TARGET Boost::headers)
    # boost is a huge project and directly downloading the 'alternate release'
    # from github is much faster than recursively cloning the repo.
    CPMAddPackage(
        NAME Boost
        VERSION 1.84.0
        URL https://github.com/boostorg/boost/releases/download/boost-1.84.0/boost-1.84.0.tar.xz
        URL_HASH SHA256=2e64e5d79a738d0fa6fb546c6e5c2bd28f88d268a2a080546f74e5ff98f29d0e
        OPTIONS "BOOST_ENABLE_CMAKE ON"
    )
  endif()
endfunction()
_setup_dependencies()