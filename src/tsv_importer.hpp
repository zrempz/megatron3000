#pragma once

void import_from_tsv(const char *filename);
bool schema_from_tsv(const char *filename, int fd);
bool data_from_tsv(const char *filename, int fd);
