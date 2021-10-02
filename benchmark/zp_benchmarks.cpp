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
#include <benchmark/benchmark.h>
#include <random>
#include <zp_eliminator/zp_scalar.hpp>

namespace bm = benchmark;
using namespace zp;

static const int P = 32749;
using ZP = ZpScalar<P>;
using Word = typename ZP::Word;
static const int N = 1024 * 1024;

template <PlusMinusAlgo algo> static void Z32749_Plus(bm::State &state) {
  std::mt19937 rng;
  std::uniform_int_distribution<Word> runif(0, P - 1);
  ZP a[N], b[N];
  for (int i = 0; i < N; ++i) {
    a[i] = runif(rng);
    b[i] = runif(rng);
  }

  int i = 0;
  for (auto _ : state) {
    for (int i = 0; i < N; ++i)
      bm::DoNotOptimize(a[i].operator+<algo>(b[i]));
  }
};

template <PlusMinusAlgo algo> static void Z32749_Minus(bm::State &state) {
  std::mt19937 rng;
  std::uniform_int_distribution<Word> runif(0, P - 1);
  ZP a[N], b[N];
  for (int i = 0; i < N; ++i) {
    a[i] = runif(rng);
    b[i] = runif(rng);
  }

  int i = 0;
  for (auto _ : state) {
    for (int i = 0; i < N; ++i)
      bm::DoNotOptimize(a[i].operator-<algo>(b[i]));
  }
};

template <MulAlgo algo> static void Z32749_Mul(bm::State &state) {
  std::mt19937 rng;
  std::uniform_int_distribution<Word> runif(0, P - 1);
  ZP a[N], b[N];
  for (int i = 0; i < N; ++i) {
    a[i] = runif(rng);
    b[i] = runif(rng);
  }

  int i = 0;
  for (auto _ : state) {
    for (int i = 0; i < N; ++i)
      bm::DoNotOptimize(a[i].operator*<algo>(b[i]));
  }
};
BENCHMARK_TEMPLATE(Z32749_Plus, PlusMinusAlgo::Explicit);
BENCHMARK_TEMPLATE(Z32749_Plus, PlusMinusAlgo::CondSub);
BENCHMARK_TEMPLATE(Z32749_Minus, PlusMinusAlgo::Explicit);
BENCHMARK_TEMPLATE(Z32749_Minus, PlusMinusAlgo::CondSub);
BENCHMARK_TEMPLATE(Z32749_Mul, MulAlgo::Explicit);
BENCHMARK_TEMPLATE(Z32749_Mul, MulAlgo::MulShift);

BENCHMARK_MAIN();
