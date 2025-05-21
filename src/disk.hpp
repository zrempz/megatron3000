#pragma once

#include <cstdint>

class Disk {
  const uint8_t disk_id;
  const uint16_t platters;
  const uint32_t tracks;
  const uint32_t sectors;
  const uint32_t sector_size;

public:
  Disk() = delete;
  Disk(uint8_t disk_id, uint16_t platters, uint32_t tracks, uint32_t sectors,
       uint32_t sector_size);
};
