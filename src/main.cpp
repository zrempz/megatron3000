#include "csv_importer.hpp"
#include "disk.hpp"
#include <iostream>
#include <limits>

void menu(const Disk &disk) {
  while (true) {
    std::cout << "Choose an option:\n";
    std::cout << "1) Import from CSV\n";
    std::cout << "0) Exit\n";

    int option;
    std::cin >> option;

    switch (option) {
    case 1: {
      std::cout << "SELECT ";
      char filename[256];

      // Clear any leftover newline from previous input
      std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
      // Read the entire line for the filename
      std::cin.getline(filename, 256);

      // Call the CSV import function with the provided filename
      if (import_from_csv(filename, disk)) {
        std::cout << "Successfully imported data from '" << filename
                  << "'.\n\n";
      } else {
        std::cerr << "Failed to import data from '" << filename << "'.\n\n";
      }
      break;
    }

    case 0:
      std::cout << "Exiting...\n";
      return;

    default:
      std::cout << "Invalid option. Please try again.\n";
    }
  }
}

int main() {
  // Initialize the Disk object (assumed to be needed for CSV import logic)
  Disk disk(0, 2, 8, 32, 32);

  // Run the menu loop
  menu(disk);

  return 0;
}
