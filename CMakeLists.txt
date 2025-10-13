cmake_minimum_required(VERSION 3.10)
project(key-value-store)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_compile_options(-fdiagnostics-color=always)

add_library(
  db_lib
  src/database.cpp  
)

target_include_directories(db_lib PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

add_subdirectory(cmd)
