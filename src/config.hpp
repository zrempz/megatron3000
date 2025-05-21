#pragma once

#include <cstddef>
#include <string_view>

namespace config {
// Disk configuration
constexpr std::string_view DISKS_DIR = "./disks/";

// Schema configuration
constexpr const char *SCHEMA_FILENAME = "schema.txt";
constexpr char DB_DELIMITER = '#';

// Type system - string_view for zero-copy comparisons
constexpr std::string_view TYPE_FLOAT = "float";
constexpr std::string_view TYPE_INTEGER = "int";
constexpr std::string_view TYPE_STRING = "str";

// Compile-time computed lengths
constexpr size_t TYPE_FLOAT_LEN = TYPE_FLOAT.length();
constexpr size_t TYPE_INTEGER_LEN = TYPE_INTEGER.length();
constexpr size_t TYPE_STRING_LEN = TYPE_STRING.length();

} // namespace config
