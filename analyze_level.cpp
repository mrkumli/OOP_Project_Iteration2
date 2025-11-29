// Compile: g++ analyze_level.cpp -o analyze.exe -std=c++17
// Run: ./analyze.exe data/level1.txt

#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <map>
#include <string>

int main(int argc, char* argv[]) {
    std::string filename = (argc > 1) ? argv[1] : "data/level1.txt";

    std::ifstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Failed to open: " << filename << std::endl;
        return 1;
    }

    std::vector<std::vector<std::string>> gameMap;
    std::map<std::string, int> tileCounts;

    std::string line;
    while (std::getline(file, line)) {
        std::vector<std::string> row;
        std::stringstream ss(line);
        std::string tile;

        while (std::getline(ss, tile, ',')) {
            row.push_back(tile);
            tileCounts[tile]++;
        }

        gameMap.push_back(row);
    }
    file.close();

    std::cout << "=== LEVEL ANALYSIS ===" << std::endl;
    std::cout << "File: " << filename << std::endl;
    std::cout << "Rows: " << gameMap.size() << std::endl;
    if (!gameMap.empty()) {
        std::cout << "Columns: " << gameMap[0].size() << std::endl;
    }
    std::cout << std::endl;

    std::cout << "Tile Type Counts:" << std::endl;
    for (const auto& pair : tileCounts) {
        std::string type = pair.first;
        int count = pair.second;

        std::string description;
        if (type == "0") description = "Empty space";
        else if (type == "2") description = "LAVA (kills cold)";
        else if (type == "3") description = "WATER (kills hot)";
        else if (type == "4") description = "GOO (kills both)";
        else if (type == "100") description = "Wall type 1";
        else if (type == "111") description = "Wall type 2";
        else description = "Wall/Platform";

        std::cout << "  " << type << ": " << count << " (" << description << ")" << std::endl;
    }
    std::cout << std::endl;

    // Analyze spawn safety at common spawn locations
    std::cout << "Spawn Safety Analysis:" << std::endl;

    // Check position (32, 416) - pixel coords
    int row_416 = 416 / 16;  // = 26
    int col_32 = 32 / 16;    // = 2

    if (row_416 < gameMap.size() && col_32 < gameMap[row_416].size()) {
        std::string tile = gameMap[row_416][col_32];
        std::cout << "  Position (32, 416) = Row " << row_416 << ", Col " << col_32
                  << ": Tile '" << tile << "'";
        if (tile == "2" || tile == "3" || tile == "4") {
            std::cout << " ❌ HAZARD - UNSAFE!" << std::endl;
        } else if (tile == "0") {
            std::cout << " ✓ Empty - SAFE" << std::endl;
        } else {
            std::cout << " ⚠️ Solid block - will spawn inside wall!" << std::endl;
        }
    }

    // Check position (580, 416)
    int col_580 = 580 / 16;  // = 36

    if (row_416 < gameMap.size() && col_580 < gameMap[row_416].size()) {
        std::string tile = gameMap[row_416][col_580];
        std::cout << "  Position (580, 416) = Row " << row_416 << ", Col " << col_580
                  << ": Tile '" << tile << "'";
        if (tile == "2" || tile == "3" || tile == "4") {
            std::cout << " ❌ HAZARD - UNSAFE!" << std::endl;
        } else if (tile == "0") {
            std::cout << " ✓ Empty - SAFE" << std::endl;
        } else {
            std::cout << " ⚠️ Solid block - will spawn inside wall!" << std::endl;
        }
    }
    std::cout << std::endl;

    // Show visual representation of bottom rows (where players spawn)
    std::cout << "Bottom 6 Rows (where players typically spawn):" << std::endl;
    std::cout << "Legend: 0=empty, 2=LAVA, 3=WATER, 4=GOO, X=wall" << std::endl;
    std::cout << std::endl;

    size_t startRow = (gameMap.size() > 6) ? gameMap.size() - 6 : 0;
    for (size_t y = startRow; y < gameMap.size(); ++y) {
        std::cout << "Row " << y << " (y=" << (y*16) << "): ";
        for (size_t x = 0; x < gameMap[y].size() && x < 40; ++x) {
            const std::string& tile = gameMap[y][x];
            if (tile == "0") std::cout << ".";
            else if (tile == "2") std::cout << "L";  // Lava
            else if (tile == "3") std::cout << "W";  // Water
            else if (tile == "4") std::cout << "G";  // Goo
            else std::cout << "#";  // Wall
        }
        std::cout << std::endl;
    }
    std::cout << std::endl;

    // Check for common mistakes
    std::cout << "Potential Issues:" << std::endl;
    bool issues = false;

    if (gameMap.size() != 30) {
        std::cout << "  ⚠️ Level should have 30 rows, has " << gameMap.size() << std::endl;
        issues = true;
    }

    if (!gameMap.empty() && gameMap[0].size() != 40) {
        std::cout << "  ⚠️ Level should have 40 columns, has " << gameMap[0].size() << std::endl;
        issues = true;
    }

    if (tileCounts["2"] == 0 && tileCounts["3"] == 0) {
        std::cout << "  ⚠️ No hazards found - game will be too easy!" << std::endl;
        issues = true;
    }

    if (!issues) {
        std::cout << "  ✓ No obvious issues detected" << std::endl;
    }

    return 0;
}
