#pragma once

#include <unordered_map>
#include "SerializedData.h"

namespace SequenceManager {

    // Define a type alias for the pair of unsigned int and int
    using data_occurances = std::pair<SerializedData, uint8_t>;

    // First few bits of std::string = readable length of string in bits, anything after is raw bit data
    std::vector<std::unordered_map<std::string, SerializedData>> sequence_maps;

    void print() {
        for (uint8_t i = 0; i < sequence_maps.size(); i++) {
            std::cout << "Data type " << static_cast<int>(i) << ":\n";
            for (auto it = sequence_maps[i].begin(); it != sequence_maps[i].end(); ++it) {
                it->second.print();
                std::cout << std::endl;
            }
        }
    }

    // Adds a new hashmap for a new type of data, returning the new index of the hashmap that's being added
    uint8_t makeAddDataType() {
        std::unordered_map<std::string, SerializedData> typeToAdd;
        sequence_maps.push_back(typeToAdd);
        return sequence_maps.size() - 1;
    }

    // Function to add a sequence to the map
    void addDataEntry(uint8_t dataType, const SerializedData& data) {
        auto& map = sequence_maps[dataType];
        const std::string& serialKey = data.serialize();
        auto it = map.find(serialKey);
        if (it == map.end())
            map.emplace(serialKey, data);
    }
};