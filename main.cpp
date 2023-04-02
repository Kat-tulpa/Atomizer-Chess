#include <iostream>
#include "ChessSequence.h"

static constexpr size_t RAND_PIECESEQUENCE_LENGTH = 2;

void printSequences() {
    for (const auto& entry : SequenceManager::sequence_occurances_score) {
        std::cout << entry.first << " : " << std::get<0>(entry.second) << std::endl;
    }
}

int main() {
    ChessSequenceManager::addRandom(1000, RAND_PIECESEQUENCE_LENGTH);
    printSequences();
    return 0;
}