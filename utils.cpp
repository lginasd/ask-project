#include "utils.h"
#include <algorithm>
#include <cassert>
#include <cstddef>
#include <string>

using namespace std;

static string binaryInt(int n);

bool isBinaryString(const string &n) {
  for (char c : n)
    if (!(c == '0' || c == '1')) return false;
  return true;
}

bool isNumberString(const string &n) {
  for (char c : n) if (!isdigit(c)) return false;
  return true;
}

bool isZeroesString(string n) {
  for (char c : n) if (c != '0')
      return false;
  return true;
}

string binary(string n) {
  assert(isNumberString(n));
  if (n == "") return "";

  string bits = "";

  string n_half = "";
  bool carry;

  while (n.length() > 0 && !isZeroesString(n)) {
    carry = false;
    n_half = "";
    for (char c : n) {
      int num = (int)(c - '0');
      if (carry)
        num += 10;

      int num_half = num / 2;
      carry = (num % 2) == 1;

      n_half.push_back(num_half + '0');
    }

    bits.push_back(carry ? '1' : '0');
    n = n_half;
  }

  reverse(bits.begin(), bits.end());
  return bits;
}

string fraction(string frac, size_t precision, int *pExpshift) {
  string result = "";
  size_t leadingZeroes = 0;
  bool countingLeadingZeroes = true;

  assert(isNumberString(frac));
  if (frac == "") return "";

  for (size_t _ = 0; _ < precision + leadingZeroes; _++) {
    string doubled = "";
    bool carry = false;
    for (size_t i = frac.length(); i != 0; i--) {
      int digit = frac[i - 1] - '0';

      digit *= 2;
      if (carry) digit++;

      if (digit >= 10)
        carry = true;
      else
        carry = false;

      doubled += (digit % 10) + '0';
    }

    result.append(carry ? "1" : "0");
    if (carry) countingLeadingZeroes = false;
    if (!carry && countingLeadingZeroes) leadingZeroes++;

    reverse(doubled.begin(), doubled.end());
    frac = doubled;

    if (isZeroesString(frac)) break;
  }
  if (pExpshift != nullptr)
    *pExpshift = leadingZeroes;

  return result;
}

int exponent(string base) {
  int shifts = base.length() - 1;

  return shifts;
}

static string binaryInt(int n) {
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
