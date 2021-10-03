# Zp elimination

A library of fast operations in Zp fields.

Main ideas:

- Storage
  - Zp elements are stored pre-normalized to values from the set `{0,...,p-1}`,
in the smallest type (*word*) that allows adding two elements without overflow
- Addition
  - Sum is in `{0,...,2p-2}` and we need to subtract p atmost once
- Subtraction
  - `a - b = a + p - b (mod p)` and `a + p - b` is at most `2p - 1`, and we
need to subtract p atmost once
- Multiplication
  - Modulo operation on *double word* product can be reduced to *quad word*
multiplication, shift, *double word* multiplication and *double word*
subtraction
([Integer Division by Constants: Optimal Bounds](https://arxiv.org/pdf/2012.12369.pdf),
[Efficient Algorithms for Integer Division by Constants Using Multiplication](https://citeseerx.ist.psu.edu/viewdoc/download?doi=10.1.1.136.8776&rep=rep1&type=pdf)
[Faster Remainder by Direct Computation: Applications to Compilers and Software Libraries](https://arxiv.org/pdf/1902.01961.pdf))
- Inverse
  - Fermat's little theorem and `log(p)` exponentiation
- Division is a multiplication by inverse

## Scalar stats

Unexpectedly, these techniques allow to get 20..50% improvements even in scalar code:

```
2021-10-03T14:42:59+03:00
Running ./zp_benchmarks
Run on (8 X 4150.71 MHz CPU s)
CPU Caches:
  L1 Data 32 KiB (x4)
  L1 Instruction 32 KiB (x4)
  L2 Unified 256 KiB (x4)
  L3 Unified 8192 KiB (x1)
Load Average: 0.28, 0.27, 0.55
--------------------------------------------------------------------------------
Benchmark                                      Time             CPU   Iterations
--------------------------------------------------------------------------------
Z32749_Plus<PlusMinusAlgo::Explicit>     1009669 ns      1009208 ns          681
Z32749_Plus<PlusMinusAlgo::CondSub>       577953 ns       577731 ns         1208
Z32749_Minus<PlusMinusAlgo::Explicit>    1124633 ns      1124191 ns          621
Z32749_Minus<PlusMinusAlgo::CondSub>      581121 ns       580860 ns         1206
Z32749_Mul<MulAlgo::Explicit>             991529 ns       991019 ns          710
Z32749_Mul<MulAlgo::MulShift>             762371 ns       762054 ns          920
Z32749_Mul<MulAlgo::MulShiftDirect>       760021 ns       759697 ns          923
```

## Vector stats

AVX2 vectorization results:

```
2021-10-03T22:05:47+03:00
Running ./zp_benchmarks
Run on (8 X 4109.52 MHz CPU s)
CPU Caches:
  L1 Data 32 KiB (x4)
  L1 Instruction 32 KiB (x4)
  L2 Unified 256 KiB (x4)
  L3 Unified 8192 KiB (x1)
Load Average: 2.17, 1.81, 1.49
--------------------------------------------------------------------------------
Benchmark                                      Time             CPU   Iterations
--------------------------------------------------------------------------------
Z32749_Plus<PlusMinusAlgo::CondSub>       572376 ns       572184 ns        24437
Z32749_PlusVec                            117182 ns       117091 ns       119402
Z32749_Minus<PlusMinusAlgo::CondSub>      572212 ns       572029 ns        24421
Z32749_MinusVec                           114040 ns       113952 ns       122636
```
