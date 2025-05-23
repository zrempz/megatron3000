#pragma once

enum DATATYPE { FLOAT, INT, STRING };

void process_type(char &c, bool &has_sign, bool &has_digits, bool &has_dot,
                  bool &is_valid);
DATATYPE determine_type(bool has_digits, bool has_dot, bool is_valid);
bool is_sign(const char c);
bool is_dot(const char c);
bool is_digit(const char c);
