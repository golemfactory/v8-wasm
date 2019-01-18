cmake_minimum_required(VERSION 3.10.2)
project(wasm-sandbox)

option(DEBUG "Debug mode" OFF)
set(V8_INCLUDE_DIR "" CACHE PATH "Path to the V8 include dir.")
set(V8_MONOLITH_LIB_PATH "" CACHE FILEPATH "Path to the compiled V8 monolith *.a library.")

set(CMAKE_CXX_STANDARD 17)
add_definitions("-Wall")

if(DEBUG)
    add_definitions("-g")
else()
    add_definitions("-O3")
endif()

include_directories("${V8_INCLUDE_DIR}")
include_directories("${CMAKE_SOURCE_DIR}/lib/tclap/include")

add_executable(wasm-sandbox src/main.cpp src/v8_exec.cpp src/v8_exec.h src/v8_callbacks.cpp
        src/v8_callbacks.h src/files.cpp src/files.h src/emsc_integrator.cpp src/emsc_integrator.h)

add_library(v8_monolith STATIC IMPORTED)
set_target_properties(v8_monolith PROPERTIES IMPORTED_LOCATION "${V8_MONOLITH_LIB_PATH}")
target_link_libraries(wasm-sandbox v8_monolith)
target_link_libraries(wasm-sandbox pthread)
target_link_libraries(wasm-sandbox stdc++fs)
