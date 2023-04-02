#include <iostream>
#include "SequenceManager.h"

static constexpr size_t RAND_PIECESEQUENCE_LENGTH = 9;

void addRandomSequences(const unsigned int count, const size_t length) {
	for (unsigned int i = 0; i < count; i++)
		SequenceManager::add(SequenceManager::makeRandom(length));
}

void printSequences() {
	for (const auto& entry : SequenceManager::sequence_occurances_score) {
		std::cout << entry.first << " : " << std::get<0>(entry.second) << std::endl;
	}
}

int main() {
	addRandomSequences(1000000, RAND_PIECESEQUENCE_LENGTH);
	printSequences();
	return 0;
}