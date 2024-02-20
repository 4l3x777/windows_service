cmake_minimum_required(VERSION 3.22)

set(PATCH_VERSION "1" CACHE INTERNAL "Patch version")
set(PROJECT_VESRION 0.0.${PATCH_VERSION})
project(SERVICE VERSION ${PROJECT_VESRION})

include(FetchContent)

set(BOOST_INCLUDE_LIBRARIES program_options algorithm uuid crc)
set(BOOST_ENABLE_CMAKE ON)
#Windows help debug
#set(Boost_DEBUG ON)
FetchContent_Declare(
    Boost
    GIT_REPOSITORY https://github.com/boostorg/boost.git
    GIT_TAG boost-1.81.0
)

FetchContent_MakeAvailable(Boost)

include_directories(reverse_shell service_dll service)

add_library(
  service_dll
  SHARED
  service_dll/service_dll.cpp
)

add_library(
  reverse_shell
  STATIC
  reverse_shell/reverse_shell.cpp
)

add_library(
  service_manager
  STATIC
  service/service_manager.cpp
)

add_executable(
  service
  service/service.cpp
)

target_link_libraries(
  service
  Boost::program_options
  service_manager)

target_link_libraries(
  service_dll
  reverse_shell)

set_target_properties(
  service
  service_manager
  service_dll
  reverse_shell
  PROPERTIES
  CXX_STANDARD 17
  CXX_STANDARD_REQUIRED ON
)

if (MSVC)
  target_compile_options(service PRIVATE /W4)

  target_compile_options(service_dll PRIVATE /W4)
else ()
  target_link_options(service PRIVATE -static-libgcc -static-libstdc++)

  target_link_options(service_dll PRIVATE -static-libgcc -static-libstdc++)

  target_compile_options(
    service 
    PRIVATE
    -Wall 
    -Wextra 
    -pedantic 
  )

  target_compile_options(
    service_dll 
    PRIVATE
    -Wall 
    -Wextra 
    -pedantic 
  )
endif()

install(
    TARGETS 
    service
    RUNTIME DESTINATION service COMPONENT release
)

install(
    TARGETS 
    service_dll
    RUNTIME DESTINATION service_dll COMPONENT release
)

set(CPACK_ARCHIVE_COMPONENT_INSTALL ON)
set(CPACK_COMPONENTS_ALL release)

# CPack area
if (WIN32)
  set(CPACK_GENERATOR "ZIP")
else()
  set(CPACK_GENERATOR "DEB")
endif()
set(CPACK_PACKAGE_VERSION "0.0.1")
set(CPACK_PACKAGE_VERSION_MAJOR "0")
set(CPACK_PACKAGE_VERSION_MINOR "0")
set(CPACK_PACKAGE_VERSION_PATCH "1")
set(CPACK_PACKAGE_CONTACT https://github.com/4l3x777/service)
include(CPack)