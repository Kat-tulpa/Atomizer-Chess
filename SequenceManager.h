#pragma once

#include <random>
#include <string_view>
#include <unordered_map>
#include <tuple>
#include <mutex>
#include <vector>

#include "ThreadCount.h"

namespace SequenceManager {
    static std::unordered_map<std::string, std::tuple<unsigned int, int>> sequence_occurances_score;

    template <typename CharType>
    std::basic_string<CharType> makeRandom(size_t length, const CharType* characters, size_t numCharacters) {
        static std::minstd_rand rng(std::random_device{}());
        static std::uniform_int_distribution<size_t> dist(0, numCharacters - 1);

        std::basic_string<CharType> sequence(length, 0);
        for (size_t i = 0; i < length; i++) {
            sequence[i] = characters[dist(rng)];
        }
        return sequence;
    }

    static void add(const std::string& sequence) {
        auto it = sequence_occurances_score.find(sequence);
        if (it != sequence_occurances_score.end()) {
            std::get<0>(it->second)++;
        }
        else {
            sequence_occurances_score.emplace(sequence, std::make_tuple(1, 0));
        }
    }

    static void addRandom(const unsigned int count, const size_t length, const char* characters, size_t numCharacters) {
        for (unsigned int j = 0; j < count; j++) {
            add(makeRandom(length, characters, numCharacters));
        }
    }
};