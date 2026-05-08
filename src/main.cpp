#include <iostream>
#include <string>
#include <stdexcept>
#include "DummyDataGenerator.h"

#ifdef _WIN32
#include <windows.h>
#endif

static void printUsage(const char* prog) {
    std::cout
        << "Usage: " << prog << " [options]\n"
        << "\nOptions:\n"
        << "  -n <count>    Orders to generate (default: 10)\n"
        << "  -t <path>     Template JSON path\n"
        << "                (default: data/templates/production_order_template.json)\n"
        << "  -o <path>     Output file path (default: output/dummy_orders.json)\n"
        << "  -s <seed>     Random seed for reproducibility (default: random)\n"
        << "  -h            Show this help\n"
        << "\nExamples:\n"
        << "  " << prog << " -n 100\n"
        << "  " << prog << " -n 50 -s 42 -o my_data.json\n";
}

int main(int argc, char* argv[]) {
#ifdef _WIN32
    SetConsoleOutputCP(CP_UTF8);
    SetConsoleCP(CP_UTF8);
#endif

    int         count        = 10;
    std::string templatePath = "data/templates/production_order_template.json";
    std::string outputPath   = "output/dummy_orders.json";
    unsigned int seed        = 0;

    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        try {
            if      (arg == "-n" && i + 1 < argc) count        = std::stoi(argv[++i]);
            else if (arg == "-t" && i + 1 < argc) templatePath = argv[++i];
            else if (arg == "-o" && i + 1 < argc) outputPath   = argv[++i];
            else if (arg == "-s" && i + 1 < argc) seed = static_cast<unsigned int>(std::stoul(argv[++i]));
            else if (arg == "-h") { printUsage(argv[0]); return 0; }
            else { std::cerr << "Unknown option: " << arg << "\n"; printUsage(argv[0]); return 1; }
        } catch (const std::exception&) {
            std::cerr << "Invalid value for " << arg << "\n";
            return 1;
        }
    }

    if (count <= 0) {
        std::cerr << "Count must be a positive integer\n";
        return 1;
    }

    std::cout << "=== DummyDataGenerator v1.0.0 ===\n"
              << "Domain : Semiconductor Production Order Management\n"
              << "Count  : " << count << "\n"
              << "Seed   : " << (seed == 0 ? "random" : std::to_string(seed)) << "\n\n";

    try {
        DummyDataGenerator gen(seed);
        gen.loadTemplate(templatePath);

        auto orders = gen.generate(count);

        if (!gen.saveToFile(outputPath, orders)) {
            std::cerr << "Failed to save output\n";
            return 1;
        }

        std::cout << "[OK] Generated " << orders.size() << " production orders\n"
                  << "[OK] Saved to  : " << outputPath << "\n";
    } catch (const std::exception& e) {
        std::cerr << "Fatal error: " << e.what() << "\n";
        return 1;
    }

    return 0;
}
