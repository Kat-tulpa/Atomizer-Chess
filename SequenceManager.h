#pragma once

#include <unordered_map>
#include <iostream>

class SequenceManager {
private:

    using data_occurrence = std::pair<std::string, uint8_t>;

    // First few bits of std::string = readable length of std::string in bits, anything after is raw bit data
    static std::vector<std::unordered_map<std::string, data_occurrence>> sequence_maps;

public:

    static void print() {
        // Define a lambda function to print the total number of entries
        auto printTotalEntries = [](int numEntries, int sumValues) {
            std::cout << "Total number of entries: " << numEntries << std::endl;
            std::cout << "Sum of all values: " << sumValues << std::endl;
        };

        // Initialize counters for the total number of entries and the sum of all key values
        int totalEntries = 0;
        int sumValues = 0;

        // Iterate over each map in the vector
        for (const auto& map : sequence_maps) {
            // Add the number of entries in the current map to the total counter
            totalEntries += map.size();

            // Iterate over each key-value pair in the map and add its value to the sum counter
            for (const auto& pair : map) {
                sumValues += pair.second.second;
            }
        }

        // Call the lambda function to print the total number of entries and the sum of all key values
        printTotalEntries(totalEntries, sumValues);

        // Iterate over each map in the vector again and print out its entries
        for (const auto& map : sequence_maps) {
            // Print the opening bracket for the map
            std::cout << "{\n";

            // Iterate over each key-value pair in the map
            for (const auto& pair : map) {
                // Print the key and value as a std::string, with quotes around them
                std::cout << "\"" << pair.second.first << "\" : " << static_cast<unsigned int>(pair.second.second) << "," << std::endl;
            }

            // Print the closing bracket for the map
            std::cout << "}\n";
        }
    }

    // Adds a new hashmap for a new type of data, returning the new index of the hashmap that's being added
    static const uint8_t makeAddDataType() {
        std::unordered_map<std::string, std::pair<std::string, uint8_t>> typeToAdd;
        sequence_maps.push_back(typeToAdd);
        return sequence_maps.size() - 1;
    }

    static void addDataEntry(const uint8_t registrationID, std::string entry) {
        auto& map = sequence_maps[registrationID];
        auto it = map.find(entry);
        if (it == map.end()) {
            data_occurrence pair = std::make_pair(entry, 1);
            map.emplace(entry, pair);
        }
        else {
            it->second.second++;
        }
    }

    static void addDataEntries(const uint8_t registrationID, std::vector<std::string>& entries) {
        auto& map = sequence_maps[registrationID];
        for (auto& entry : entries) {
            auto it = map.find(entry);
            if (it == map.end()) {
                data_occurrence pair = std::make_pair(entry, 1);
                map.emplace(entry, pair);
            }
            else {
                it->second.second++;
            }
        }
    }

    static std::vector<data_occurrence>& getAll(const uint8_t registration_ID) {
        std::vector<data_occurrence> entries;
        auto& map = sequence_maps[registration_ID];
        for (const auto& pair : map) {
            entries.push_back(pair.second);
        }
        return entries;
    }
};

std::vector<std::unordered_map<std::string, std::pair<std::string, uint8_t>>> SequenceManager::sequence_maps; // Implementation needs to be defined outside of class declaration