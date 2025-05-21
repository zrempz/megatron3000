#pragma once

#include <cstdint>

class Disk {
  const uint8_t disk_id;
  const uint16_t platters;
  const uint32_t tracks;
  const uint32_t sectors;
  const uint32_t sector_size;
  mutable char path_buffer[128];

public:
  Disk() = delete;
  Disk(uint8_t disk_id, uint16_t platters, uint32_t tracks, uint32_t sectors,
       uint32_t sector_size);
  const char *get_schema_path() const;
  const char *get_available_data_path() const;
};
