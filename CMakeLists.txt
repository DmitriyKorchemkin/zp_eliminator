# ##############################################################################
# Copyright (c) 2021 Dmitriy Korchemkin
#
# Permission is hereby granted, free of charge, to any person obtaining a copy
# of this software and associated documentation files (the "Software"), to deal
# in the Software without restriction, including without limitation the rights
# to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
# copies of the Software, and to permit persons to whom the Software is
# furnished to do so, subject to the following conditions:
#
# The above copyright notice and this permission notice shall be included in all
# copies or substantial portions of the Software.
#
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
# IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
# FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
# AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
# LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
# OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
# SOFTWARE.
# ##############################################################################
project("zp_eliminator" LANGUAGES CXX)

find_package(benchmark REQUIRED)

add_library(zp_eliminator INTERFACE)
target_include_directories(zp_eliminator INTERFACE ./include)
target_compile_features(zp_eliminator INTERFACE cxx_std_20)

if(CMAKE_BUILD_TYPE MATCHES Release)
  set(BUILD_FLAGS -march=native -Ofast)
elseif(CMAKE_BUILD_TYPE MATCHES RelWithDebInfo)
  set(BUILD_FLAGS -march=native -O3 -g3)
elseif(CMAKE_BUILD_TYPE MATCHES Debug)
  set(BUILD_FLAGS -O0 -g3)
endif()

add_subdirectory(thirdparty/doctest)

add_executable(zp_scalar tests/zp_scalar.cpp)
target_link_libraries(zp_scalar zp_eliminator doctest)
target_compile_options(zp_scalar PRIVATE ${BUILD_FLAGS})

add_executable(zp_vector tests/zp_vector.cpp)
target_link_libraries(zp_vector zp_eliminator doctest)
target_compile_options(zp_vector PRIVATE ${BUILD_FLAGS})


add_executable(zp_benchmarks benchmark/zp_benchmarks.cpp)
target_link_libraries(zp_benchmarks zp_eliminator benchmark)
target_compile_options(zp_benchmarks PRIVATE ${BUILD_FLAGS})
