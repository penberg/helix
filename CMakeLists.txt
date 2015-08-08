cmake_minimum_required(VERSION 2.8)

project(helix)

set(VERSION_MAJOR   0   CACHE STRING "Project major version number.")
set(VERSION_MINOR   1   CACHE STRING "Project minor version number.")
set(VERSION_PATCH   0   CACHE STRING "Project patch version number.")
mark_as_advanced(VERSION_MAJOR VERSION_MINOR VERSION_PATCH)

find_package(Doxygen)
if(DOXYGEN_FOUND)
  configure_file(${CMAKE_CURRENT_SOURCE_DIR}/Doxyfile.in ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile @ONLY)
  add_custom_target(doc
  ${DOXYGEN_EXECUTABLE} ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile
  WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
  COMMENT "Generate API reference with Doxygen" VERBATIM
  )
endif(DOXYGEN_FOUND)

find_package(PkgConfig)

pkg_check_modules(LIBUV REQUIRED libuv)
include_directories(${LIBUV_INCLUDE_DIRS})

include_directories("include")

set(CMAKE_C_FLAGS "-Iinclude -Wall -O3 -g -std=gnu11")
set(CMAKE_CXX_FLAGS "-Iinclude -Wall -O3 -g -std=c++14")

set(libSrcs ${libSrcs}
    src/helix.cc
    src/order_book.cc
    src/nasdaq/nordic_itch.cc
    src/nasdaq/moldudp.cc
    src/nasdaq/itch_session.cc
    src/nasdaq/soupfile.cc
)

add_library(helix ${libSrcs} include/helix/nasdaq/moldudp_messages.h)

add_executable(helix-svm tools/helix-svm/helix-svm.cc)
target_link_libraries(helix-svm LINK_PUBLIC helix)
target_link_libraries(helix-svm PUBLIC uv)

add_executable(helix-trace tools/helix-trace/helix-trace.c)
target_link_libraries(helix-trace LINK_PUBLIC helix)
target_link_libraries(helix-trace PUBLIC uv)

add_executable(helix-top tools/helix-top/helix-top.c)
target_link_libraries(helix-top LINK_PUBLIC helix)
target_link_libraries(helix-top PUBLIC uv)
target_link_libraries(helix-top PUBLIC ncurses)
