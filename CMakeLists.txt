cmake_minimum_required(VERSION 2.8)

project(helix)

set(HELIX_VERSION "0.3.0")

find_package(Doxygen)
if(DOXYGEN_FOUND)
  configure_file(${CMAKE_CURRENT_SOURCE_DIR}/Doxyfile.in ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile @ONLY)
  add_custom_target(doc
  ${DOXYGEN_EXECUTABLE} ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile
  WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
  COMMENT "Generate API reference with Doxygen" VERBATIM
  )
endif(DOXYGEN_FOUND)

find_package(Boost REQUIRED)

find_package(Curses)

find_package(PkgConfig)

pkg_check_modules(LIBUV REQUIRED libuv>=1.0)
include_directories(${LIBUV_INCLUDE_DIRS})

include_directories("include")

set(CMAKE_C_FLAGS "-Iinclude -Wall -O3 -g -std=gnu11")
set(CMAKE_CXX_FLAGS "-Iinclude -Wall -O3 -g -std=c++14")

set(libSrcs ${libSrcs}
    src/event.cc
    src/helix.cc
    src/order_book.cc
    src/nasdaq/binaryfile.cc
    src/nasdaq/itch50_session.cc
    src/nasdaq/itch50_handler.cc
    src/nasdaq/nordic_itch_handler.cc
    src/nasdaq/moldudp.cc
    src/nasdaq/moldudp64.cc
    src/nasdaq/nordic_itch_session.cc
    src/nasdaq/soupfile.cc
    src/parity/pmd_handler.cc
    src/parity/pmd_session.cc
)

add_library(helix ${libSrcs} include/helix/nasdaq/moldudp_messages.h)

set(cxxHeaders
    include/helix/nasdaq/moldudp_messages.h
    include/helix/nasdaq/nordic_itch_handler.hh
    include/helix/nasdaq/nordic_itch_messages.h
    include/helix/nasdaq/itch50_session.hh
    include/helix/nasdaq/nordic_itch_session.hh
    include/helix/nasdaq/itch50_handler.hh
    include/helix/nasdaq/itch50_messages.h
    include/helix/net.hh
    include/helix/helix.hh
    include/helix/order_book.hh
)
set(cHeaders
    include/helix-c/helix.h
)

configure_file("helix.pc.in" "helix.pc" @ONLY)

install(FILES ${cxxHeaders} DESTINATION include/helix)
install(FILES ${cHeaders} DESTINATION include/helix-c)
install(TARGETS helix DESTINATION lib)
install(FILES "${CMAKE_CURRENT_BINARY_DIR}/helix.pc"
              DESTINATION "${CMAKE_INSTALL_PREFIX}/lib/pkgconfig")
add_executable(helix-svm tools/helix-svm/helix-svm.cc)
target_link_libraries(helix-svm helix ${LIBUV_LIBRARIES})

add_executable(helix-trace tools/helix-trace/helix-trace.cc)
target_link_libraries(helix-trace helix ${LIBUV_LIBRARIES})

add_executable(helix-top tools/helix-top/helix-top.c)
target_link_libraries(helix-top helix ncurses ${LIBUV_LIBRARIES})

add_executable(order_book_perf_test tests/order_book_perf_test.cc)
target_link_libraries(order_book_perf_test helix)
