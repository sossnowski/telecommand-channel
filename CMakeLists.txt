cmake_minimum_required(VERSION 3.15)
project(bch C)

set(CMAKE_C_STANDARD 11)

add_executable(bch main.c bchCoder.c prepareData.c generateCLTU.c modulation.c)
