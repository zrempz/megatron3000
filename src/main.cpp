#include "tsv_importer.hpp"

int main(int argc, const char *argv[]) {
  if (argc == 2) {
    import_from_tsv(argv[1]);
  }
}
