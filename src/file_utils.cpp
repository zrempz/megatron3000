#include "file_utils.hpp"
#include <unistd.h>

bool skip_line(int fd) {
  char c;
  while (read(fd, &c, 1) > 0) {
    if (c == '\n')
      return true;
  }
  return false;
}
