#include "types_handler.hpp"

DATATYPE determine_type(bool has_digits, bool has_dot, bool is_valid) {
  if (!is_valid || !has_digits)
    return STRING;
  if (has_dot)
    return FLOAT;
  return INT;
}

void process_type(char &c, bool &has_sign, bool &has_digits, bool &has_dot,
                  bool &valid) {
  if (!valid)
    return;
  if (is_sign(c)) {
    if (has_sign || has_digits || has_dot)
      valid = false;
    has_sign = true;
  } else if (c == '.') {
    if (has_dot || !has_digits) {
      valid = false;
    }
    has_dot = true;
  } else if (is_digit(c)) {
    has_digits = true;
  } else {
    valid = false;
  }
}

bool is_sign(const char c) { return c == '+' || c == '-'; }
bool is_dot(const char c) { return c == '.'; }
bool is_digit(const char c) { return c >= '0' && c <= '9'; }
