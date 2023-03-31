#pragma once

#include <string>
#include <vector>
#include <cstdlib>
#include <ctime>

namespace Piece {
    static const char notation[] =
        "0123456789"
        "abcdefghijklmnopqrstuvwxyz"
        "ABCDEFGHIJKLMNOPQRSTUVWXYZ";
    size_t differentTypes = sizeof(notation) - 1;
}

namespace PieceSequences {

    static std::vector<std::pair<std::string, unsigned int>> sequenceOccurances;
    static std::vector<int> scores;

    static bool sequenceComparator(const std::pair<std::string, unsigned int>& a, const std::pair<std::string, unsigned int>& b) {
        return a.first < b.first;
    }

    static void init() {
        srand(time(nullptr));
    }

    std::string makeRandom(size_t length) {
        std::string sequence = "";
        sequence.reserve(length);
        for (unsigned int i = 0; i < length; i++)
            sequence += Piece::notation[rand() % Piece::differentTypes];
        return sequence;
    }

    static void add(std::string sequence) {
        auto it = std::lower_bound(sequenceOccurances.begin(), sequenceOccurances.end(), std::make_pair(sequence, 0), sequenceComparator);
        if (it != sequenceOccurances.end() && it->first == sequence) {
            it->second++;
        }
        else {
            sequenceOccurances.insert(it, std::make_pair(sequence, 1));
            scores.push_back(0);
        }
    }
}

