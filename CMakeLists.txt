cmake_minimum_required(VERSION 3.20)

set(target_name games)
project(${target_name})

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_C_STANDARD 17)

file(GLOB_RECURSE src_code LIST_DIRECTORIES false
        src/*.hpp src/*.h src/*.cpp src/*.c )

add_subdirectory(libs)

file(GLOB_RECURSE game_101_code LIST_DIRECTORIES false
        games101/*.hpp games101/*.h games101/*.cpp games101/*.c )

add_executable(games101 ${src_code} ${game_101_code})
target_link_libraries(games101 third_libs)
target_include_directories(games101 PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/src)
target_include_directories(games101 PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/games101)
target_compile_options(games101 PRIVATE "$<$<CXX_COMPILER_ID:MSVC>:/utf-8>")
target_compile_definitions(games101 PRIVATE CURRENT_WORKING_DIR="${CMAKE_CURRENT_SOURCE_DIR}")


file(GLOB_RECURSE game_202_code LIST_DIRECTORIES false
        games202/*.hpp games202/*.h games202/*.cpp games202/*.c )

add_executable(games202 ${src_code} ${game_202_code})
target_link_libraries(games202 third_libs)
target_include_directories(games202 PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/src)
target_include_directories(games202 PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/games202)
target_compile_options(games202 PRIVATE "$<$<CXX_COMPILER_ID:MSVC>:/utf-8>")
target_compile_definitions(games202 PRIVATE CURRENT_WORKING_DIR="${CMAKE_CURRENT_SOURCE_DIR}")
