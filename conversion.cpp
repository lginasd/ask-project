#include "conversion.h"
#include "constants.h"
#include "utils.h"
#include <algorithm>
#include <cassert>
#include <cctype>
#include <cstddef>
#include <stdexcept>

namespace conversion {

using namespace std;

namespace {

} // namespace

// sprawdza format podanej liczby
static bool ValidateInput(const string &rawInput) {
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
  replace(normalizedInput.begin(), normalizedInput.end(), ',', '.'); // przecinek też działa
  if (normalizedInput.empty()) {
    result.errorMessage = "Wpisz liczbę dziesiętną.";
    return result;
  }

  try {
    result.isNegative = (normalizedInput.length() > 0 && normalizedInput[0] == '-');
    size_t dotPosition = normalizedInput.find('.');

    string integerPart = normalizedInput.substr(0, dotPosition);
    if (result.isNegative) integerPart = integerPart.substr(1);
    result.integerPart = integerPart;
    result.integerBits = binary(integerPart);

    string fractionPart;
    if (dotPosition == string::npos) fractionPart = "";
    else fractionPart = normalizedInput.substr(dotPosition + 1);

    // dla zero
    result.isZero = (integerPart.length() == 0 || isZeroesString(integerPart))
                 && (fractionPart.length() == 0 || isZeroesString(fractionPart));
    if (result.isZero) {
      result.success = true;
      result.signBit = result.isNegative ? "1" : "0";
      result.exponentBits = string(EXPONENT_LEN, '0');
      result.mantissaBits = string(MANTISSA_LEN, '0');
      result.ieeeBits =
          result.signBit + result.exponentBits + result.mantissaBits;
      result.exponentValue = 0;

      return result;
    }

    int fractionLeadingZeroesCount = 0; // dla liczb typu 0,0...01
    string fractionBits = fraction(fractionPart, MANTISSA_LEN, &fractionLeadingZeroesCount); // dla liczby "0.5" ("1") substr(1)
                                                                                             // zwróci pusty string

    // Dla liczb >= 1 wykładnik zależy od pozycji pierwszej jedynki w części całkowitej
    // Dla liczb 0 < x < 1 wykładnik zależy od liczby zer przed pierwszą jedynką w części ułamkowej
    int exp;
    if (integerPart.length() == 0 || isZeroesString(integerPart)) {
      // przesunięcie mantysy dla liczb typu 0,0...01 i wyznaczenie wykładnika
      fractionBits = fractionBits.substr(fractionLeadingZeroesCount + 1);
      exp = -(fractionLeadingZeroesCount + 1);
    } else {
      exp = exponentFromInteger(result.integerBits);
    }

    string exponentBits = binary(to_string(exp + EXPONENT_BIAS));
    if (exponentBits.length() < EXPONENT_LEN)
      exponentBits = string(EXPONENT_LEN - exponentBits.length(), '0') + exponentBits;
    exponentBits = exponentBits.substr(0, min<size_t>(exponentBits.length(), EXPONENT_LEN));


    result.signBit = (result.isNegative ? "1" : "0");
    result.exponentValue = exp;
    result.exponentBits = exponentBits;

    // wpisanie liczby całej do mantysy bez niejawnej 1 na początku
    if (result.integerBits.length() > 0 && !isZeroesString(result.integerBits))
      result.mantissaBits = result.integerBits.substr(1, // niejawna 1 jest ominięta
          min<size_t>(result.integerBits.length(), MANTISSA_LEN + 1)
      );
    else result.mantissaBits = ""; // mantysa będzie złożona tylko z ułamku

    if (result.mantissaBits.length() < MANTISSA_LEN) // dopełnienie mantysy częścią ułamkową
      result.mantissaBits += fractionBits.substr(0, min<size_t>(fractionBits.length(), MANTISSA_LEN - result.mantissaBits.length()));
    result.mantissaBits = result.mantissaBits.substr(0, min<size_t>(result.mantissaBits.length(), MANTISSA_LEN));

    if (result.mantissaBits.length() < MANTISSA_LEN)
      result.mantissaBits += string(MANTISSA_LEN - result.mantissaBits.length(), '0'); // zera na końcu, żeby było 112 bit

    // TODO: IEEE 754 rounding

    result.ieeeBits = result.signBit + result.exponentBits + result.mantissaBits;
    result.success = true;

    assert(result.signBit.length() == 1);
    assert(result.exponentBits.length() == EXPONENT_LEN);
    assert(result.mantissaBits.length() == MANTISSA_LEN);
    assert(result.ieeeBits.length() == IEEE_LEN);

  } catch (const std::exception &) {
    result.errorMessage = "Nie można odczytać liczby. Sprawdź format.";
  }

  return result;
}

} // namespace conversion
