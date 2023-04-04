#pragma once
/*
Defines a set of static methods for managing sequences of characters.
It provides functions for generating random sequences, 
adding sequences to an unordered map along with their occurrence scores, 
and multithreading the process of generating and adding sequences.
*/

#include <string>
#include <unordered_map>

namespace SequenceManager {

    // Define a type alias for the pair of unsigned int and int
    using occurance_score = std::pair<unsigned int, int>;

    // Create an unordered map with string keys and occurance_score values
    std::unordered_map<std::string, occurance_score> sequence_map;

    // Function to add a sequence to the map or increment its occurrence score if it already exists
    void add(const std::string& sequence, int score) {
        auto it = sequence_map.find(sequence);
        if (it != sequence_map.end())
            it->second.first++;
        else
            sequence_map.emplace(sequence, std::make_pair(1, score));
    }

    void print() {
        for (const auto& pair : sequence_map) {
            std::cout << pair.first << " " << pair.second.first << " " << pair.second.second << std::endl;
        }
    }
};