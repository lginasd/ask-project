#include "conversion.h"
#include "utils.h"
#include <algorithm>
#include <cassert>
#include <cctype>
#include <cstddef>
#include <stdexcept>

namespace conversion {

using namespace std;

namespace {

string Trim(const string &text) {
  size_t start = 0;
  while (start < text.size() &&
         isspace(static_cast<unsigned char>(text[start]))) {
    start++;
  }

  if (start == text.size()) {
    return "";
  }

  size_t end = text.size() - 1;
  while (end > start && isspace(static_cast<unsigned char>(text[end]))) {
    end--;
  }

  return text.substr(start, end - start + 1);
}

} // namespace

bool ValidateInput(const string &rawInput) {
  if (rawInput.length() == 0) return false;

  bool is_fraction = false;
  bool is_negative = (rawInput[0] == '-') ? true : false;

  for (char c : (is_negative) ? rawInput.substr(1) : rawInput) {
    if (isdigit(c)) continue;
    else if (c == '.' || c == ',') {
      if (is_fraction) return false;
      else is_fraction = true;
    }
    else return false;
  }
  return true;
}

ConversionResult ConvertToIEEE(const string &rawInput) {
  ConversionResult result;

  if (!ValidateInput(rawInput)) {
    result.errorMessage = "Nie można odczytać liczby. Sprawdź format.";
    return result;
  }

  result.originalInput = rawInput;

  string normalizedInput = Trim(rawInput);
  replace(normalizedInput.begin(), normalizedInput.end(), ',', '.');
  if (normalizedInput.empty()) {
    result.errorMessage = "Wpisz liczbę dziesiętną.";
    return result;
  }

  try {
    result.isZero = isZeroesString(normalizedInput);
    result.isNegative = (normalizedInput.length() > 0 && normalizedInput[0] == '-');

    if (result.isZero) {
      result.success = true;
      result.signBit = result.isNegative ? "1" : "0";
      result.exponentBits = string(15, '0');
      result.mantissaBits = string(112, '0');
      result.ieeeBits =
          result.signBit + result.exponentBits + result.mantissaBits;
      result.exponentValue = 0;
      return result;
    }

    size_t dotPosition = normalizedInput.find('.');

    string integerPart = normalizedInput.substr(0, normalizedInput.find('.'));
    if (result.isNegative) integerPart = integerPart.substr(1);
    result.integerPart = integerPart;
    result.integerBits = binary(integerPart);

    string fractionPart;
    if (dotPosition == string::npos) fractionPart = "";
    else fractionPart = normalizedInput.substr(dotPosition + 1);

    int expshift = 0;
    string fractionBits = fraction(fractionPart, 112, &expshift) + "0"; // + "0" na wypadek liczb typu 0.5,
                                                                        // gdzie jest tylko "1" i substr(1) nic nie da

    int exp = exponent(result.integerBits);
    if (integerPart.length() == 0 || isZeroesString(integerPart)) {
      fractionBits = fractionBits.substr(expshift + 1);
      exp -= expshift;
    }
    string exponentBits = binary(to_string(exp + BIAS));
    if (exponentBits.length() < 15)
      exponentBits = string(15 - exponentBits.length(), '0') + exponentBits;
    exponentBits = exponentBits.substr(0, min<size_t>(exponentBits.length(), 15));

    result.signBit = (result.isNegative ? "1" : "0");
    result.exponentValue = exp;
    result.exponentBits = exponentBits;

    if (result.integerBits.length() > 0 && !isZeroesString(result.integerBits))
      result.mantissaBits = result.integerBits.substr(1, // niejawna 1
          min<size_t>(result.integerBits.length(), 112 + 1)
      );
    else result.mantissaBits = ""; // mantysa będzie złożona tylko z ułamku

    if (result.mantissaBits.length() < 112)
      result.mantissaBits += fractionBits.substr(0, min<size_t>(fractionBits.length(), 112 - result.integerBits.length()));
    result.mantissaBits = result.mantissaBits.substr(0, min<size_t>(result.mantissaBits.length(), 112));

    if (result.mantissaBits.length() < 112)
      result.mantissaBits += string(112 - result.mantissaBits.length(), '0'); // zera na końcu

    result.ieeeBits = result.signBit + result.exponentBits + result.mantissaBits;
    result.success = true;

    // TODO: IEEE 754 rounding

    assert(result.signBit.length() == 1);
    assert(result.exponentBits.length() == 15);
    assert(result.mantissaBits.length() == 112);
    assert(result.ieeeBits.length() == 128);

  } catch (const std::exception &) {
    result.errorMessage = "Nie można odczytać liczby. Sprawdź format.";
  }

  return result;
}

} // namespace conversion
