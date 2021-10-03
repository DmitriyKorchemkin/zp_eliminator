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

#include <zp_eliminator/zp_scalar.hpp>

#include <random>

using namespace zp;

using ZP13 = ZpScalar<13>;
TEST_CASE("Z13_Add") {
  for (int i = 0; i < 13; ++i)
    for (int j = 0; j < 13; ++j) {
      const ZP13 I(i), J(j);
      const ZP13 IplusJ = I + J;
      CHECK(IplusJ.value() == (i + j) % 13);
    }
}

TEST_CASE("Z13_Sub") {
  for (int i = 0; i < 13; ++i)
    for (int j = 0; j < 13; ++j) {
      const ZP13 I(i), J(j);
      const ZP13 IminusJ = I - J;
      CHECK(IminusJ.value() == (i + 13 - j) % 13);
    }
}

TEST_CASE("Z13_Mul") {
  for (int i = 0; i < 13; ++i)
    for (int j = 0; j < 13; ++j) {
      const ZP13 I(i), J(j);
      const ZP13 ImulJ = I * J;
      CHECK(ImulJ.value() == (i * j) % 13);
    }
}

TEST_CASE("Z13_Div") {
  for (int i = 1; i < 13; ++i)
    for (int j = 1; j < 13; ++j) {
      const ZP13 I(i), J(j);
      const ZP13 IdivJ = I / J;
      const ZP13 IdivJJ = IdivJ * J;
      CHECK(IdivJJ.value() == i);
    }
}

TEST_CASE("Z13_Inv") {
  for (int i = 1; i < 13; ++i) {
    const ZP13 I(i);
    const ZP13 Iinv = I.inverse();
    const ZP13 id = I * Iinv;
    CHECK(id.value() == 1);
  }
}

using ZP32749 = ZpScalar<32749>;
TEST_CASE("Z32749_Add") {
  std::mt19937 rng;
  std::uniform_int_distribution<int> runif(0, 32748);
  for (int it = 0; it < 32749; ++it) {
    const int i = runif(rng);
    const int j = runif(rng);
    const ZP32749 I(i), J(j);
    const ZP32749 IplusJ = I + J;
    CHECK(IplusJ.value() == (i + j) % 32749);
  }
}

TEST_CASE("Z32749_Sub") {
  std::mt19937 rng;
  std::uniform_int_distribution<int> runif(0, 32748);
  for (int it = 0; it < 32749; ++it) {
    const int i = runif(rng);
    const int j = runif(rng);
    const ZP32749 I(i), J(j);
    const ZP32749 IplusJ = I - J;
    CHECK(IplusJ.value() == (i + 32749 - j) % 32749);
  }
}

TEST_CASE("Z32749_Mul") {
  std::mt19937 rng;
  std::uniform_int_distribution<int> runif(0, 32748);
  for (int it = 0; it < 32749; ++it) {
    const int i = runif(rng);
    const int j = runif(rng);
    const ZP32749 I(i), J(j);
    const ZP32749 IplusJ = I * J;
    CHECK(IplusJ.value() == (i * j) % 32749);
  }
}

TEST_CASE("Z32749_Div") {
  std::mt19937 rng;
  std::uniform_int_distribution<int> runif(1, 32748);
  for (int it = 0; it < 32749; ++it) {
    const int i = runif(rng);
    const int j = runif(rng);
    const ZP32749 I(i), J(j);
    const ZP32749 IdivJ = I / J;
    const ZP32749 IdivJJ = IdivJ * J;
    CHECK(IdivJJ.value() == i);
  }
}

TEST_CASE("Z32749_Inv") {
  for (int i = 1; i < 32749; ++i) {
    const ZP32749 I(i);
    const ZP32749 Iinv = I.inverse();
    const ZP32749 id = I * Iinv;
    CHECK(id.value() == 1);
  }
}

// Tests for different operator implementations
TEST_CASE("Z13_Add_Explicit") {
  for (int i = 0; i < 13; ++i)
    for (int j = 0; j < 13; ++j) {
      const ZP13 I(i), J(j);
      const ZP13 IplusJ = I.operator+<PlusMinusAlgo::Explicit>(J);
      CHECK(IplusJ.value() == (i + j) % 13);
    }
}
TEST_CASE("Z13_Add_CondSub") {
  for (int i = 0; i < 13; ++i)
    for (int j = 0; j < 13; ++j) {
      const ZP13 I(i), J(j);
      const ZP13 IplusJ = I.operator+<PlusMinusAlgo::CondSub>(J);
      CHECK(IplusJ.value() == (i + j) % 13);
    }
}

TEST_CASE("Z13_Sub_Explicit") {
  for (int i = 0; i < 13; ++i)
    for (int j = 0; j < 13; ++j) {
      const ZP13 I(i), J(j);
      const ZP13 IminusJ = I.operator-<PlusMinusAlgo::Explicit>(J);
      CHECK(IminusJ.value() == (i + 13 - j) % 13);
    }
}
TEST_CASE("Z13_Sub_CondSub") {
  for (int i = 0; i < 13; ++i)
    for (int j = 0; j < 13; ++j) {
      const ZP13 I(i), J(j);
      const ZP13 IminusJ = I.operator-<PlusMinusAlgo::CondSub>(J);
      CHECK(IminusJ.value() == (i + 13 - j) % 13);
    }
}

TEST_CASE("Z13_Mul_Explicit") {
  for (int i = 0; i < 13; ++i)
    for (int j = 0; j < 13; ++j) {
      const ZP13 I(i), J(j);
      const ZP13 ImulJ = I.operator*<MulAlgo::Explicit>(J);
      CHECK(ImulJ.value() == (i * j) % 13);
    }
}

TEST_CASE("Z13_Mul_MulShift") {
  for (int i = 0; i < 13; ++i)
    for (int j = 0; j < 13; ++j) {
      const ZP13 I(i), J(j);
      const ZP13 ImulJ = I.operator*<MulAlgo::MulShift>(J);
      CHECK(ImulJ.value() == (i * j) % 13);
    }
}

TEST_CASE("Z13_Mul_MulDirect") {
  for (int i = 0; i < 13; ++i)
    for (int j = 0; j < 13; ++j) {
      const ZP13 I(i), J(j);
      const ZP13 ImulJ = I.operator*<MulAlgo::MulShiftDirect>(J);
      CHECK(ImulJ.value() == (i * j) % 13);
    }
}

TEST_CASE("Z13_Mul_MulDirect2") {
  for (int i = 0; i < 13; ++i)
    for (int j = 0; j < 13; ++j) {
      const ZP13 I(i), J(j);
      const ZP13 ImulJ = I.operator*<MulAlgo::MulShiftDirect2>(J);
      CHECK(ImulJ.value() == (i * j) % 13);
    }
}
