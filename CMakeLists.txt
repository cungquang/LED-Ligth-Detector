# CMake Build Configuration for root of project
cmake_minimum_required(VERSION 3.18)
project(assignment2 VERSION 1.0 DESCRIPTION "Starter project" LANGUAGES C)

# Compiler options (inherited by sub-folders)
set(CMAKE_C_STANDARD 11)
add_compile_options(-Wall -g -Werror -Wpedantic -Wextra)
add_compile_options(-fdiagnostics-color)

# Enable address sanitizer
# (Comment this out for production)
#add_compile_options(-fsanitize=address)
#add_link_options(-fsanitize=address)
add_link_options(-pthread)

# What folders to build
add_subdirectory(hal)  
add_subdirectory(app)

