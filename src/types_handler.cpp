#include "types_handler.hpp"

void process_type(char c, unsigned char &flags) {
  if (!(flags & IS_VALID))
    return; // Early exit if already invalid

  if (is_sign(c)) {
    // Sign allowed only at start before digits/dot
    if ((flags & (HAS_SIGN | HAS_DIGITS | HAS_DOT)) != 0) {
      flags &= ~IS_VALID; // Invalid position for sign character
    } else {
      flags |= HAS_SIGN;
    }
  } else if (is_dot(c)) {
    // Dot requires preceding digit and no existing dot
    if ((flags & HAS_DOT) || !(flags & HAS_DIGITS)) {
      flags &= ~IS_VALID; // Double dot or leading dot
    } else {
      flags |= HAS_DOT;
    }
  } else if (is_digit(c)) {
    flags |= HAS_DIGITS; // Found at least one numeric character
  } else {
    flags &= ~IS_VALID; // Non-numeric, non-special character
  }
}

DataType determine_type(unsigned char flags) {
  // Must be valid and have at least one digit to be numeric
  if (!(flags & IS_VALID) || !(flags & HAS_DIGITS)) {
    return STRING; // Fallback to string type
  }

  // Float requires valid decimal point, otherwise integer
  return (flags & HAS_DOT) ? FLOAT : INT;
}
