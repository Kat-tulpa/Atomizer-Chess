/*
Defines a set of static methods for managing sequences of characters.
It provides functions for generating random sequences, 
adding sequences to an unordered map along with their occurrence scores, 
and multithreading the process of generating and adding sequences.
*/

#pragma once

#include <random>
#include <string>
#include <unordered_map>
#include <utility>

namespace SequenceManager {

    // Define a type alias for the pair of unsigned int and int
    using occurance_score = std::pair<unsigned int, int>;

    // Create an unordered map with string keys and occurance_score values
    std::unordered_map<std::string, occurance_score> sequence_map;

    // Function to generate a random string of a given length
    template <typename CharType>
    std::basic_string<CharType> makeRandomSequence(size_t length, const CharType* characters, size_t numCharacters) {
        static std::minstd_rand rng(std::random_device{}());
        static std::uniform_int_distribution<size_t> dist(0, numCharacters - 1);

        std::basic_string<CharType> sequence(length, 0);
        for (size_t i = 0; i < length; i++)
            sequence[i] = characters[dist(rng)];
        return sequence;
    }

    // Function to add a sequence to the map or increment its occurrence score if it already exists
    void add(const std::string& sequence) {
        auto it = sequence_map.find(sequence);
        if (it != sequence_map.end())
            it->second.first++;
        else
            sequence_map.emplace(sequence, occurance_score(1, 0));
    }
};