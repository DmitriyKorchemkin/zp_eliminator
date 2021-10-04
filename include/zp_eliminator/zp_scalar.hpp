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
#ifndef ZP_SCALAR_HPP
#define ZP_SCALAR_HPP

#include <cstdint>
#include <iosfwd>

namespace zp {
template <typename word> struct dword_type;

template <> struct dword_type<uint16_t> { using type = uint32_t; };

template <> struct dword_type<uint32_t> { using type = uint64_t; };

template <> struct dword_type<uint64_t> { using type = __int128; };

template <typename T> using dword_type_t = typename dword_type<T>::type;

template <typename T> struct integer_traits {
  using Word = T;
  using DWord = dword_type_t<T>;
  using QWord = dword_type_t<DWord>;
  static constexpr int NumBits = sizeof(Word) * 8;
  static constexpr int DWordBits = sizeof(DWord) * 8;
  static constexpr int QWordBits = sizeof(QWord) * 8;
  // Scalar that is possible to add to itself without overflow
  static constexpr Word MaxScalar = (Word(1) << (NumBits - 1)) - 1;
};

template <uint64_t scalar, typename current = uint16_t,
          bool need_next = (scalar > integer_traits<current>::MaxScalar)>
struct minimal_type;

template <uint64_t scalar, typename current>
struct minimal_type<scalar, current, true>
    : minimal_type<scalar, dword_type_t<current>> {};

template <uint64_t scalar, typename current>
struct minimal_type<scalar, current, false> {
  using Word = current;
};

template <uint64_t scalar>
using minimal_type_t = typename minimal_type<scalar>::Word;

template <typename T> constexpr int num_bits(T v) {
  int len = 0;
  while (v) {
    ++len;
    v >>= 1;
  }
  return len;
}

template <typename Word, Word Divisor, dword_type_t<Word> MaxMultiply>
struct div_mod_trait {
  using DWord = dword_type_t<Word>;
  using QWord = dword_type_t<DWord>;
  static constexpr int W_Word = 8 * sizeof(Word);
  static constexpr int W_DWord = 8 * sizeof(DWord);
  static constexpr int W_QWord = 8 * sizeof(QWord);
  static constexpr QWord D = Divisor;
  static constexpr int L = num_bits(D);
  static constexpr QWord FC = QWord(1) << (W_DWord + L - 1);
  static constexpr QWord I = FC / D;
  static constexpr QWord J = I + 1;
  static constexpr QWord d = D * J - FC;
  static constexpr QWord Qc = (J + d - 1) / d;
  static constexpr QWord Nc = Qc * D - 1;
  static constexpr bool CheckRequired = MaxMultiply > Nc;

  static constexpr QWord C = (~QWord(0)) / Divisor + 1;
};

template <typename Word, Word Divisor, dword_type_t<Word> MaxMultiply>
struct direct_mod_trait {
  using DWord = dword_type_t<Word>;
  using QWord = dword_type_t<DWord>;

  static constexpr int W_Word = 8 * sizeof(Word);
  static constexpr int W_DWord = 8 * sizeof(DWord);
  static constexpr int W_QWord = 8 * sizeof(QWord);
  static constexpr QWord D = Divisor;
  static constexpr int L = num_bits(D);
  static constexpr int LM = num_bits(MaxMultiply) + 1;
  static constexpr QWord M = QWord(1) << (L + LM);

  static constexpr QWord rem = D - (M % D);
  static constexpr QWord cmn = (M - 1) / ((QWord(1) << LM) - 1) + 1;
  static constexpr bool AddMode = rem >= cmn;

  static constexpr QWord C = AddMode ? M / D : (M - 1) / D + 1;
};

template <typename Word, Word Divisor, dword_type_t<Word> MaxMultiply,
          bool Odd = Divisor % 2,
          bool CheckRequired =
              div_mod_trait<Word, Divisor, MaxMultiply>::CheckRequired>
struct DivMod;

template <typename Word, Word Divisor, dword_type_t<Word> MaxMultiply>
struct DirectMod {
  using Traits = div_mod_trait<Word, Divisor, MaxMultiply>;
  using DWord = typename Traits::DWord;
  using QWord = typename Traits::QWord;
  using OWord = dword_type_t<QWord>;
  static Word Mod(const DWord &dividend) {
    return (OWord(Traits::C * dividend) * Divisor) >> Traits::W_QWord;
  }
};

template <typename Word, Word Divisor, dword_type_t<Word> MaxMultiply,
          bool add = direct_mod_trait<Word, Divisor, MaxMultiply>::AddMode>
struct DirectMod2;

template <typename Word, Word Divisor, dword_type_t<Word> MaxMultiply>
struct DirectMod2<Word, Divisor, MaxMultiply, true> {
  using Traits = direct_mod_trait<Word, Divisor, MaxMultiply>;
  using DWord = typename Traits::DWord;
  using QWord = typename Traits::QWord;
  static constexpr int Shift = Traits::W_QWord - (Traits::L + Traits::LM);

  static Word Mod(const DWord &dividend) {
    QWord cm = Traits::C * (dividend + 1);
    QWord cmm = (cm << Shift) >> Shift;
    QWord res = (cmm * Divisor) >> (Traits::L + Traits::LM);
    return res;
  }
};

template <typename Word, Word Divisor, dword_type_t<Word> MaxMultiply>
struct DirectMod2<Word, Divisor, MaxMultiply, false> {
  using Traits = direct_mod_trait<Word, Divisor, MaxMultiply>;
  using DWord = typename Traits::DWord;
  using QWord = typename Traits::QWord;
  static constexpr int Shift = Traits::W_QWord - (Traits::L + Traits::LM);

  static Word Mod(const DWord &dividend) {
    QWord cm = Traits::C * dividend;
    QWord cmm = (cm << Shift) >> Shift;
    QWord res = (cmm * Divisor) >> (Traits::L + Traits::LM);
    return res;
  }
};

template <typename Word, Word Divisor, dword_type_t<Word> MaxMultiply, bool Odd>
struct DivMod<Word, Divisor, MaxMultiply, Odd, false> {
  using Traits = div_mod_trait<Word, Divisor, MaxMultiply>;
  using DWord = typename Traits::DWord;
  using QWord = typename Traits::QWord;
  // Divisor parity does not matter
  static Word Divide(const DWord &dividend) {
    return (dividend * Traits::J) >> (Traits::W_DWord + Traits::L - 1);
  }
  static Word Mod(const DWord &dividend) {
    return dividend - Divide(dividend) * DWord(Traits::D);
  }
};

template <typename Word, Word Divisor, dword_type_t<Word> MaxMultiply>
struct DivMod<Word, Divisor, MaxMultiply, false, true> {
  using Traits = div_mod_trait<Word, Divisor, MaxMultiply>;
  using DWord = typename Traits::DWord;
  using QWord = typename Traits::QWord;
  // Divisor is even, need to set LSB to 0
  static Word Divide(const DWord &dividend) {
    const DWord corrected = dividend & (~DWord(0) ^ DWord(1));
    return (corrected * Traits::J) >> (Traits::W_DWord + Traits::L - 1);
  }
  static Word Mod(const DWord &dividend) {
    return dividend - Divide(dividend) * DWord(Traits::D);
  }
};

template <typename Word, Word Divisor, dword_type_t<Word> MaxMultiply>
struct DivMod<Word, Divisor, MaxMultiply, true, true> {
  using Traits = div_mod_trait<Word, Divisor, MaxMultiply>;
  using DWord = typename Traits::DWord;
  using QWord = typename Traits::QWord;
  // Divisor is odd, need to check for correction
  static Word Divide(const DWord &dividend) {
    const DWord corrected = dividend >= Traits::Nc ? dividend - 1 : dividend;
    return (corrected * Traits::J) >> (Traits::W_DWord + Traits::L - 1);
  }
  static Word Mod(const DWord &dividend) {
    return dividend - Divide(dividend) * DWord(Traits::D);
  }
};

enum class MulAlgo { Explicit, MulShift, MulShiftDirect, MulShiftDirect2 };
template <typename Word, Word P, MulAlgo algo> struct MulOp;

template <typename Word, Word P> struct MulOp<Word, P, MulAlgo::Explicit> {
  using DWord = dword_type_t<Word>;
  Word operator()(const Word &a, const Word &b) const {
    return (DWord(a) * b) % P;
  }
};

template <typename Word, Word P> struct MulOp<Word, P, MulAlgo::MulShift> {
  using DWord = dword_type_t<Word>;
  static constexpr DWord MaxMul = DWord(P) * (P - 1);
  using DivModT = DivMod<Word, P, MaxMul>;

  Word operator()(const Word &a, const Word &b) const {
    return DivModT::Mod(DWord(a) * b);
  }
};

template <typename Word, Word P>
struct MulOp<Word, P, MulAlgo::MulShiftDirect> {
  using DWord = dword_type_t<Word>;
  static constexpr DWord MaxMul = DWord(P) * (P - 1);
  using ModT = DirectMod<Word, P, MaxMul>;

  Word operator()(const Word &a, const Word &b) const {
    return ModT::Mod(DWord(a) * b);
  }
};

template <typename Word, Word P>
struct MulOp<Word, P, MulAlgo::MulShiftDirect2> {
  using DWord = dword_type_t<Word>;
  static constexpr DWord MaxMul = DWord(P) * (P - 1);
  using ModT = DirectMod2<Word, P, MaxMul>;

  Word operator()(const Word &a, const Word &b) const {
    return ModT::Mod(DWord(a) * b);
  }
};

enum class PlusMinusAlgo { Explicit, CondSub };
template <typename Word, Word P, PlusMinusAlgo algo> struct AddOp;
template <typename Word, Word P, PlusMinusAlgo algo> struct SubOp;

template <typename Word, Word P>
struct AddOp<Word, P, PlusMinusAlgo::Explicit> {
  Word operator()(const Word &a, const Word &b) const { return (a + b) % P; }
};

template <typename Word, Word P> struct AddOp<Word, P, PlusMinusAlgo::CondSub> {
  Word operator()(const Word &a, const Word &b) const {
    const Word sum = a + b;
    if (sum < P)
      return sum;
    return sum - P;
  }
};

template <typename Word, Word P>
struct SubOp<Word, P, PlusMinusAlgo::Explicit> {
  Word operator()(const Word &a, const Word &b) const {
    return (a + P - b) % P;
  }
};

template <typename Word, Word P> struct SubOp<Word, P, PlusMinusAlgo::CondSub> {
  Word operator()(const Word &a, const Word &b) const {
    const Word sum = a - b;
    if (sum < P)
      return sum;
    return sum + P;
  }
};

template <uint64_t prime, typename T = minimal_type_t<prime>> struct ZpScalar {
  using Word = T;
  using DWord = dword_type_t<T>;
  static constexpr Word P = prime;
  ZpScalar() = default;
  ZpScalar(const Word &v) : v(v) {}

  ZpScalar operator-() const {
    if (!v)
      return *this;
    return P - v;
  }

  template <PlusMinusAlgo algorithm = PlusMinusAlgo::CondSub>
  ZpScalar operator+(const ZpScalar &other) const {
    return AddOp<Word, P, algorithm>()(v, other.v);
  }

  template <PlusMinusAlgo algorithm = PlusMinusAlgo::CondSub>
  ZpScalar operator-(const ZpScalar &other) const {
    return SubOp<Word, P, algorithm>()(v, other.v);
  }

  template <MulAlgo algorithm = MulAlgo::MulShift>
  ZpScalar operator*(const ZpScalar &other) const {
    return MulOp<Word, P, algorithm>()(v, other.v);
  }

  ZpScalar operator/(const ZpScalar &other) const {
    return *this * other.inverse();
  }

  ZpScalar &operator*=(const ZpScalar &other) {
    v = (*this * other).v;
    return *this;
  }

  ZpScalar &operator+=(const ZpScalar &other) {
    v = (*this + other).v;
    return *this;
  }

  ZpScalar &operator-=(const ZpScalar &other) {
    v = (*this - other).v;
    return *this;
  }

  bool operator==(const ZpScalar &other) const { return other.v == v; }

  bool operator!=(const ZpScalar &other) const { return other.v != v; }

  operator bool() const { return v; }

  ZpScalar inverse() const {
    Word pow = P - 2;
    ZpScalar exp(v);
    ZpScalar res(1);
    while (pow) {
      if (pow & 1)
        res *= exp;
      exp *= exp;
      pow >>= 1;
    }
    return res;
  }

  static ZpScalar Fit(const Word &v) { return v % P; }

  ZpScalar &operator=(const ZpScalar &s) = default;

  template <uint64_t p, typename S>
  friend std::ostream &operator<<(std::ostream &o, const ZpScalar<p, S> &z);

  Word value() const { return v; }

private:
  Word v;
};

template <uint64_t prime, typename T>
std::ostream &operator<<(std::ostream &o, const ZpScalar<prime, T> &z) {
  return o << z.v;
}
} // namespace zp
#endif
