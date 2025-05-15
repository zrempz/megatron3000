#include "csv_importer.hpp"

int main(int argc, const char *argv[]) {
  if (argc == 2) {
    import_from_csv(argv[1]);
  }
}
