#include <iostream>
#include "SequenceManager.h"

static constexpr size_t RAND_PIECESEQUENCE_LENGTH = 3;

void addRandomSequences(const unsigned int count, const size_t length) {
	for (unsigned int i = 0; i < count; i++)
		SequenceManager::add(SequenceManager::makeRandom(length));
}

void printSequences() {
	for (auto& entry : SequenceManager::sequence_occurances_score)
		std::cout << std::get<0>(entry) << " : " << std::get<1>(entry) << std::endl;
}

int main() {
	addRandomSequences(50000, 3);
	printSequences();
	return 0;
}