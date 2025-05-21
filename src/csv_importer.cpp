#include "csv_importer.hpp"
#include "config.hpp"
#include "file_handler.hpp"
#include "type_handler.hpp"
#include <cstring>
#include <fcntl.h>
#include <iostream>
#include <unistd.h>

#define MAX_FILENAME 256
#define OUTPUT_EXT ".txt"

static inline bool import_to_schema(const char *filename, int fd,
                                    const Disk &disk) {
  const char *path = disk.get_schema_path();
  char full_path[MAX_FILENAME];
  snprintf(full_path, sizeof(full_path), "%s/%s", path,
           config::SCHEMA_FILENAME);

  int schema_fd = open(full_path, O_WRONLY | O_APPEND | O_CREAT, 0644);
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

bool import_to_data(const char *filename, int fd, const Disk &disk) {
  // Generate output filename
  char out_filename[MAX_FILENAME];
  const char *dot = strrchr(filename, '.');
  size_t base_len = dot ? (size_t)(dot - filename) : strlen(filename);

  if (base_len >= sizeof(out_filename) - strlen(OUTPUT_EXT) - 1) {
    // Not enough space for .txt + null terminator
    return false;
  }

  // Build output filename
  strncpy(out_filename, filename, base_len);
  out_filename[base_len] = '\0';
  strncat(out_filename, OUTPUT_EXT, sizeof(out_filename) - base_len - 1);

  const char *path = disk.get_available_data_path();
  char full_path[MAX_FILENAME];
  snprintf(full_path, sizeof(full_path), "%s/%s", path, out_filename);

  // Open output file
  int out_fd = open(full_path, O_WRONLY | O_CREAT | O_TRUNC, 0644);
  if (out_fd == -1) {
    return false;
  }

  char c;
  bool in_quotes = false;
  bool needs_delimiter = false;

  // Process each character
  while (true) {
    if (read(fd, &c, 1) <= 0) {
      break; // End of file or error
    }

    if (c == '\r') {
      continue; // Skip Windows-style carriage returns
    }

    if (in_quotes) {
      if (c == '"') {
        // Look ahead to check for escaped quote ""
        char next;
        if (read(fd, &next, 1) > 0) {
          if (next == '"') {
            // Write one " for "" escape
            if (write(out_fd, &c, 1) != 1) {
              close(out_fd);
              return false;
            }
          } else {
            // Closing quote, rewind to reprocess 'next'
            in_quotes = false;
            lseek(fd, -1, SEEK_CUR);
          }
        } else {
          // Unexpected EOF after opening quote
          close(out_fd);
          return false;
        }
      } else {
        // Inside quotes: just write the character
        if (write(out_fd, &c, 1) != 1) {
          close(out_fd);
          return false;
        }
      }
    } else {
      if (c == '"') {
        in_quotes = true;
      } else if (c == ',') {
        if (write(out_fd, "#", 1) != 1) {
          close(out_fd);
          return false;
        }
        needs_delimiter = false;
      } else if (c == '\n') {
        if (write(out_fd, "\n", 1) != 1) {
          close(out_fd);
          return false;
        }
        needs_delimiter = false;
      } else {
        if (write(out_fd, &c, 1) != 1) {
          close(out_fd);
          return false;
        }
        needs_delimiter = true;
      }
    }
  }

  // Final newline if needed
  if (needs_delimiter) {
    if (write(out_fd, "\n", 1) != 1) {
      close(out_fd);
      return false;
    }
  }

  close(out_fd);
  return true;
}

bool import_from_csv(const char *filename, const Disk &disk) {
  int fd = open(filename, O_RDONLY);
  if (fd == -1) {
    std::cerr << "Error opening" << filename << "file\n";
    return false;
  }

  if (!import_to_schema(filename, fd, disk)) {
    std::cerr << "Error importing to schema\n";
    return false;
  }

  if (!import_to_data(filename, fd, disk)) {
    std::cerr << "Error importing to data\n";
    return false;
  }

  close(fd);
  return true;
}
