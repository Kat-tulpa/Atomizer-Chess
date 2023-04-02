#include <iostream>
#include "ChessSequence.h"

static constexpr size_t RAND_PIECESEQUENCE_LENGTH = 64;

void printSequences() {
    for (const auto& entry : SequenceManager::sequence_map) {
        std::cout << entry.first << " : " << std::get<0>(entry.second) << std::endl;
    }
}

int main() {
    ChessSequenceManager::addRandom(1000000, RAND_PIECESEQUENCE_LENGTH);
    printSequences();
    return 0;
}