#pragma once

// Bit flags for tracking parser state
constexpr unsigned char IS_VALID = 0x01;
constexpr unsigned char HAS_SIGN = 0x02;
constexpr unsigned char HAS_DIGITS = 0x04;
constexpr unsigned char HAS_DOT = 0x08;

// Resulting data type classification
enum DataType { FLOAT, INT, STRING };

// Processes single character and updates parsing state flags
void process_type(char c, unsigned char &flags);

// Determines final type from accumulated state flags
DataType determine_type(unsigned char flags);

// Character classification helpers (inline for performance)
inline bool is_sign(char c) { return c == '+' || c == '-'; }
inline bool is_dot(char c) { return c == '.'; }
inline bool is_digit(char c) { return c >= '0' && c <= '9'; }
