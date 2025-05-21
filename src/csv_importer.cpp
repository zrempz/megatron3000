#include "csv_importer.hpp"
#include "config.hpp"
#include "file_handler.hpp"
#include "type_handler.hpp"
#include <fcntl.h>
#include <iostream>
#include <unistd.h>

static inline bool import_to_schema(const char *filename, int fd) {
  int schema_fd =
      open(config::SCHEMA_FILENAME, O_WRONLY | O_APPEND | O_CREAT, 0644);
  if (schema_fd == -1)
    return false;

  // Write table/relation name (exclude extension)
  for (int i = 0; filename[i] && filename[i] != '.'; ++i) {
    if (write(schema_fd, &filename[i], 1) == -1) {
      close(schema_fd);
      return false;
    }
  }

  write(schema_fd, &config::DB_DELIMITER, 1);
  int types_fd = open(filename, O_RDONLY);
  if (types_fd == -1) {
    close(schema_fd);
    return false;
  }

  // Skip csv header line
  if (!skip_line(types_fd)) {
    close(schema_fd);
    close(types_fd);
    return false;
  }

  char c;
  unsigned char flags = IS_VALID;

  // Read column names from fd (first line)
  while (read(fd, &c, 1) > 0) {
    if (c == '\r')
      continue;

    if (c == '\n') {
      // Handle last column's type
      write(schema_fd, &config::DB_DELIMITER, 1);
      switch (determine_type(flags)) {
      case INT:
        write(schema_fd, config::TYPE_INTEGER.data(), config::TYPE_INTEGER_LEN);
        break;
      case FLOAT:
        write(schema_fd, config::TYPE_FLOAT.data(), config::TYPE_FLOAT_LEN);
        break;
      default:
        write(schema_fd, config::TYPE_STRING.data(), config::TYPE_STRING_LEN);
        break;
      }
      write(schema_fd, "\n", 1);
      break;
    }

    if (c == ',') {
      // Write delimiter and type for previous column
      write(schema_fd, &config::DB_DELIMITER, 1);
      switch (determine_type(flags)) {
      case INT:
        write(schema_fd, config::TYPE_INTEGER.data(), config::TYPE_INTEGER_LEN);
        break;
      case FLOAT:
        write(schema_fd, config::TYPE_FLOAT.data(), config::TYPE_FLOAT_LEN);
        break;
      default:
        write(schema_fd, config::TYPE_STRING.data(), config::TYPE_STRING_LEN);
        break;
      }
      write(schema_fd, &config::DB_DELIMITER,
            1); // Delimiter before next column name

      // Reset for next column
      flags = IS_VALID;

      // Process type info for next column
      while (read(types_fd, &c, 1) > 0 && c != ',') {
        if (c == '"') {
          while (read(types_fd, &c, 1) > 0 && c != '"') {
          }
        } else {
          process_type(c, flags);
        }
      }
    } else {
      // Write column name character
      write(schema_fd, &c, 1);
    }
  }

  close(schema_fd);
  close(types_fd);
  return true;
}

bool import_from_csv(const char *filename) {
  int fd = open(filename, O_RDONLY);
  if (fd == -1) {
    std::cerr << "Error opening" << filename << "file\n";
    return false;
  }

  if (!import_to_schema(filename, fd)) {
    std::cerr << "Error importing to schema\n";
    return false;
  }

  return true;
}
