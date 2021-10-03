/******************************************************************************
Copyright (c) 2021 Dmitriy Korchemkin

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
******************************************************************************/
#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include <doctest/doctest.h>
#include <zp_eliminator/vector_kernels.hpp>

#include <random>

using namespace zp;

TEST_CASE("Add_16x16") {
  const int N = 1024;
  const uint16_t P = 32749;
  uint16_t a[N];
  uint16_t b[N];
  uint16_t c[N];

  std::mt19937 rng;
  std::uniform_int_distribution<uint16_t> runif(0, P - 1);
  for (int i = 0; i < N; ++i) {
    a[i] = runif(rng);
    b[i] = runif(rng);
  }

  VecAddOp<uint16_t, 16, P> vadd;
  vadd.run(a, b, c, N);

  for (int i = 0; i < N; ++i) {
    CHECK((a[i] + b[i]) % P == c[i]);
  }
}

TEST_CASE("Sub_16x16") {
  const int N = 1024;
  const uint16_t P = 32749;
  uint16_t a[N];
  uint16_t b[N];
  uint16_t c[N];

  std::mt19937 rng;
  std::uniform_int_distribution<uint16_t> runif(0, P - 1);
  for (int i = 0; i < N; ++i) {
    a[i] = runif(rng);
    b[i] = runif(rng);
  }

  VecSubOp<uint16_t, 16, P> vsub;
  vsub.run(a, b, c, N);

  for (int i = 0; i < N; ++i) {
    CHECK((a[i] + P - b[i]) % P == c[i]);
  }
}
