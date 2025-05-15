#pragma once

#define SCHEMA_NAME "schema.txt"
#define DB_SEPARATOR '#'
#define FLOAT_STR "float"
#define FLOAT_STR_LEN 5
#define INT_STR "int"
#define INT_STR_LEN 3
#define STRING_STR "str"
#define STRING_STR_LEN 3
constexpr char sep = DB_SEPARATOR;

int open_schema();
