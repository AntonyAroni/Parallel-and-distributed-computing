cmake_minimum_required(VERSION 3.28)
project(paralela)

set(CMAKE_CXX_STANDARD 20)

add_executable(paralela main.cpp)

add_executable(1_bucles_anidados memoria_cache/1_bucles_anidados.cpp)
add_executable(2_matriz_clasica memoria_cache/2_matriz_clasica.cpp)
add_executable(3_matriz_bloques_x_clasica memoria_cache/3_matriz_bloques_x_clasica.cpp)