#pragma once

#include <unordered_map>
#include <iostream>

class SequenceManager {
private:

    // First few bits of std::string = readable length of string in bits, anything after is raw bit data
    static std::vector<std::unordered_map<std::string, std::string>> sequence_maps;

public:

    static void print() {
        // Iterate over each map in the vector
        for (const auto& map : sequence_maps) {
            // Print the opening bracket for the map
            std::cout << "{\n";

            // Iterate over each key-value pair in the map
            for (const auto& pair : map) {
                // Print the key and value as a string, with quotes around them
                std::cout << "    \"" << pair.first << "\": \"" << pair.second << "\",\n";
            }

            // Print the closing bracket for the map
            std::cout << "}\n";
        }
    }

    // Adds a new hashmap for a new type of data, returning the new index of the hashmap that's being added
    static const uint8_t makeAddDataType() {
        std::unordered_map<std::string, std::string> typeToAdd;
        sequence_maps.push_back(typeToAdd);
        return sequence_maps.size() - 1;
    }

    // Function to add a sequence to the map
    static void addDataEntry(uint8_t dataType, const std::string& data) {
        auto& map = sequence_maps[dataType];
        auto it = map.find(data);
        if (it == map.end())
            map.emplace(data, data);
        //print();
    }

    // Function to add a sequence to the map
    static void addDataEntries(uint8_t dataType, const std::vector<std::string>& datas) {
        auto& map = sequence_maps[dataType];
        for (const auto& data : datas) {
            auto it = map.find(data);
            if (it == map.end())
                map.emplace(data, data);
        }
    }
};

std::vector<std::unordered_map<std::string, std::string>> SequenceManager::sequence_maps; // Implementation needs to be defined outside of class declarr