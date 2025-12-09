#include "utils.h"
#include "boost/multiprecision/float128.hpp"
#include <algorithm>
#include <string>

using namespace std;
using namespace boost::multiprecision;

string binary(int n) {
  if (n == 0) {
    return "0";
  }

  string bits = "";
  int current = n;

  while (current != 0) {
    int remainder = current % 2;
    bits = to_string(remainder) + bits;
    current = current / 2;
  }

  return bits;
}

string fraction(float128 num, int precision) {
  string result = "";

  if (precision <= 0) {
    return result;
  }

  if (num == 0) {
    return string(static_cast<size_t>(precision), '0');
  }

  for (int i = 0; i < precision; i++) {
    num = num * 2;
    if (num >= 1) {
      result += "1";
      num = num - 1;
    } else {
      result += "0";
    }
  }

  return result;
}

int exponent(string base) {
  int shifts = base.length() - 1;

  return shifts;
}

string ieee74(float128 digit, string bits, int exp, int precision, int bias) {
  int sign = (digit < 0) ? 1 : 0;

  digit = abs(digit);

  int exponent_bits = 15;
  int mantissa_bits = precision - exponent_bits - 1;

  int fraction_count =
      std::max(0, mantissa_bits - static_cast<int>(bits.length()));
  string fraction_str = fraction(digit - int(digit), fraction_count);

  int exp_decimal = bias + exp;
  string exp_str = binary(exp_decimal);

  while (static_cast<int>(exp_str.length()) < exponent_bits) {
    exp_str = "0" + exp_str;
  }

  string mantissa_str = bits + fraction_str;

  if (static_cast<int>(mantissa_str.length()) > mantissa_bits) {
    mantissa_str = mantissa_str.substr(0, mantissa_bits);
  } else if (static_cast<int>(mantissa_str.length()) < mantissa_bits) {
    mantissa_str +=
        string(static_cast<size_t>(mantissa_bits - mantissa_str.length()), '0');
  }

  return to_string(sign) + exp_str + mantissa_str;
}
