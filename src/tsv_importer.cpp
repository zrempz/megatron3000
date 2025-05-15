#include "tsv_importer.hpp"
#include "file_utils.hpp"
#include "megatron.hpp"
#include "types_handler.hpp"
#include <cstring>
#include <fcntl.h>
#include <iostream>
#include <unistd.h>

void import_from_tsv(const char *filename) {
  int fd = open(filename, O_RDONLY);
  if (fd == -1) {
    std::cerr << "Error opening file\n";
    return;
  }
  if (!schema_from_tsv(filename, fd)) {
    std::cerr << "Error importing to schema\n";
    return;
  }
  if (!data_from_tsv(filename, fd)) {
    std::cerr << "Error importing data\n";
    return;
  }
}

bool schema_from_tsv(const char *filename, int fd) {
  int scfd = open_schema();
  if (scfd == -1)
    return false;

  for (int i = 0; filename[i] != '\0' && filename[i] != '.'; i++) {
    if (write(scfd, &filename[i], 1) == -1) {
      close(scfd);
      return false;
    }
  }

  int fdtype = open(filename, O_RDONLY);
  if (fdtype == -1) {
    close(scfd);
    return false;
  }

  if (!skip_line(fdtype)) {
    close(fdtype);
    close(scfd);
    return false;
  }

  char c;
  bool success = false;

  while (read(fd, &c, 1) > 0) {
    if (c == '\n') {
      write(scfd, &c, 1);
      break;
    }
    if (write(scfd, &sep, 1) == -1) {
      goto cleanup;
    }

    while (c != '\t' && c != '\r') {
      if (write(scfd, &c, 1) == -1) {
        goto cleanup;
      }
      if (read(fd, &c, 1) <= 0) {
        goto cleanup;
      }
    }

    if (write(scfd, &sep, 1) == -1) {
      goto cleanup;
    }

    bool has_sign = false;
    bool has_digits = false;
    bool has_dot = false;
    bool is_valid = true;

    while (read(fdtype, &c, 1) > 0 && c != '\t' && c != '\r') {
      process_type(c, has_sign, has_digits, has_dot, is_valid);
    }

    int type = determine_type(has_digits, has_dot, is_valid);
    if (type == FLOAT)
      write(scfd, FLOAT_STR, FLOAT_STR_LEN);
    else if (type == INT)
      write(scfd, INT_STR, INT_STR_LEN);
    else
      write(scfd, STRING_STR, STRING_STR_LEN);

    if (c == '\n')
      break;
  }

  success = true;

cleanup:
  close(fdtype);
  close(scfd);
  return success;
}

bool data_from_tsv(const char *filename, int fd) {
  const char *last_slash = strrchr(filename, '/');
  const char *last_backslash = strrchr(filename, '\\');
  const char *fname_start = filename;

  if (last_slash || last_backslash) {
    fname_start =
        (last_slash > last_backslash ? last_slash : last_backslash) + 1;
  }

  const char *dot = strrchr(fname_start, '.');
  size_t base_len = dot ? (dot - fname_start) : strlen(fname_start);

  char *new_filename = new char[base_len + 5];
  memcpy(new_filename, fname_start, base_len);
  strcpy(new_filename + base_len, ".txt");
  int datfd = open(new_filename, O_CREAT | O_TRUNC | O_WRONLY, 0644);
  if (datfd == -1)
    return false;

  char c;
  while (read(fd, &c, 1) > 0) {
    if (c == '\r')
      continue;
    if (c == '\t') {
      write(datfd, &sep, 1);
      continue;
    }
    if (write(datfd, &c, 1) == -1) {
      close(datfd);
      return false;
    }
  }
  close(datfd);
  return true;
}
