#include "disk.hpp"
#include "config.hpp"
#include <cstdio>
#include <cstring>
#include <dirent.h>
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

    for (uint8_t surface = 0; surface < 2;
         ++surface) { // Two surfaces per platter
      pos = snprintf(path + platter_base_len, sizeof(path) - platter_base_len,
                     "surface_%u/", surface);
      MKDIR(path); // Create surface directory

      const size_t surface_base_len = platter_base_len + pos;

      for (uint32_t t = 0; t < tracks; ++t) {
        pos = snprintf(path + surface_base_len, sizeof(path) - surface_base_len,
                       "track_%u/", t);
        MKDIR(path); // Create track directory

        const size_t track_base_len = surface_base_len + pos;

        for (uint32_t s = 0; s < sectors; ++s) {
          snprintf(path + track_base_len, sizeof(path) - track_base_len,
                   "sector_%u", s);
          MKDIR(path); // Create sector directory
        }
      }
    }
  }
}
const char *Disk::get_schema_path() const {
  snprintf(path_buffer, sizeof(path_buffer),
           "%.*sdisk_%u/platter_0/surface_0/track_0/sector_0",
           (int)config::DISKS_DIR.size(), config::DISKS_DIR.data(), disk_id);
  return path_buffer;
}

const char *Disk::get_available_data_path() const {
  for (uint16_t p = 0; p < platters; ++p) {
    for (uint8_t surface = 0; surface < 2; ++surface) {
      for (uint32_t t = 0; t < tracks; ++t) {
        // Build path to sector_0 directory
        snprintf(path_buffer, sizeof(path_buffer),
                 "%.*sdisk_%u/platter_%u/surface_%u/track_%u/sector_0",
                 (int)config::DISKS_DIR.size(), config::DISKS_DIR.data(),
                 disk_id, p, surface, t);

        DIR *dir = opendir(path_buffer);
        if (!dir) {
          // Directory doesn't exist — available!
          return path_buffer;
        }

        // Directory exists — check if it's empty
        bool has_content = false;
        struct dirent *entry;
        while ((entry = readdir(dir)) != nullptr) {
          // Skip . and ..
          if (strcmp(entry->d_name, ".") != 0 &&
              strcmp(entry->d_name, "..") != 0) {
            has_content = true;
            break;
          }
        }
        closedir(dir);

        if (!has_content) {
          // Directory is empty — available!
          return path_buffer;
        }
      }
    }
  }

  // No available sector_0 found
  return nullptr;
}
