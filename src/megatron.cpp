#include "megatron.hpp"
#include <fcntl.h>
#include <unistd.h>

#define SCHEMA_FLAGS O_WRONLY | O_APPEND | O_CREAT
#define SCHEMA_MODE 0644

int open_schema() {
  int fd = open(SCHEMA_NAME, SCHEMA_FLAGS, SCHEMA_MODE);
  return fd;
}
