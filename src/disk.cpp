#include "disk.hpp"
#include "config.hpp"
#include <cstdio>

// Platform-specific mkdir wrapper
#if defined(_WIN32) || defined(WIN32)
#include <direct.h>
#define MKDIR(path) _mkdir(path)
#else
#include <sys/stat.h>
#define MKDIR(path) mkdir(path, 0755)
#endif

// Fixed-size buffer for building file paths
#define PATH_BUFFER_SIZE 64

Disk::Disk(uint8_t disk_id, uint16_t platters, uint32_t tracks,
           uint32_t sectors, uint32_t sector_size)
    : disk_id(disk_id), platters(platters), tracks(tracks), sectors(sectors),
      sector_size(sector_size) {

  char path[PATH_BUFFER_SIZE]; // Buffer for constructing paths
  size_t pos = 0;

  // Build and create base directory
  pos = snprintf(path, sizeof(path), "%.*s", (int)config::DISKS_DIR.size(),
                 config::DISKS_DIR.data());
  MKDIR(path); // Create base directory

  pos = snprintf(path, sizeof(path), "%.*sdisk_%u/",
                 (int)config::DISKS_DIR.size(), config::DISKS_DIR.data(),
                 disk_id);
  MKDIR(path); // Create disk directory

  // Save length of current path prefix for reuse
  const size_t disk_base_len = pos;

  for (uint16_t p = 0; p < platters; ++p) {
    pos = snprintf(path + disk_base_len, sizeof(path) - disk_base_len,
                   "platter_%u/", p);
    MKDIR(path); // Create platter directory

    const size_t platter_base_len = disk_base_len + pos;

    for (uint16_t t = 0; t < tracks; ++t) {
      pos = snprintf(path + platter_base_len, sizeof(path) - platter_base_len,
                     "track_%u/", t);
      MKDIR(path); // Create track directory

      const size_t track_base_len = platter_base_len + pos;

      for (uint16_t s = 0; s < sectors; ++s) {
        snprintf(path + track_base_len, sizeof(path) - track_base_len,
                 "sector_%u", s);
        MKDIR(path); // Create sector directory
      }
    }
  }
}
