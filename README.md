# Zp elimination

A library of fast operations in Zp fields.

Main ideas:

- Storage
  - Zp elements are stored pre-normalized to values from the set `{0,...,p-1}`, in the smallest type (*word*) that allows adding two elements without overflow
- Addition
  - Sum is in `{0,...,2p-2}` and we need to subtract p atmost once
- Subtraction
  - `a - b = a + p - b (mod p)` and `a + p - b` is at most `2p - 1`, and we need to subtract p atmost once
- Multiplication
  - Modulo operation on *double word* product can be reduced to *quad word* multiplication, shift, *double word* multiplication and *double word* subtraction ([Integer Division by Constants: Optimal Bounds](https://arxiv.org/pdf/2012.12369.pdf), [Efficient Algorithms for Integer Division by Constants Using Multiplication](https://citeseerx.ist.psu.edu/viewdoc/download?doi=10.1.1.136.8776&rep=rep1&type=pdf))
- Inverse
  - Fermat's little theorem and `log(p)` exponentiation
- Division is a multiplication by inverse

## Scalar stats

Unexpectedly, these techniques allow to get 20..50% improvements even in scalar code:

```
2021-10-02T22:22:22+03:00
Running ./zp_benchmarks
Run on (8 X 4009.62 MHz CPU s)
CPU Caches:
  L1 Data 32 KiB (x4)
  L1 Instruction 32 KiB (x4)
  L2 Unified 256 KiB (x4)
  L3 Unified 8192 KiB (x1)
Load Average: 1.24, 1.29, 1.54
--------------------------------------------------------------------------------
Benchmark                                      Time             CPU   Iterations
--------------------------------------------------------------------------------
Z32749_Plus<PlusMinusAlgo::Explicit>     1088135 ns      1083368 ns          646
Z32749_Plus<PlusMinusAlgo::CondSub>       635329 ns       631727 ns         1134
Z32749_Minus<PlusMinusAlgo::Explicit>    1269378 ns      1262871 ns          567
Z32749_Minus<PlusMinusAlgo::CondSub>      638374 ns       635052 ns         1086
Z32749_Mul<MulAlgo::Explicit>            1064451 ns      1059575 ns          654
Z32749_Mul<MulAlgo::MulShift>             823240 ns       817642 ns          850
```

## Vector stats

