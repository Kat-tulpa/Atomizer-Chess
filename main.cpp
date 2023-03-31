#include <iostream>
#include "PieceSequences.h"

static constexpr size_t RAND_PIECESEQUENCE_LENGTH = 9;

void init() {
	PieceSequences::init();
}

void addRandomSequences(const unsigned int count, const size_t length) {
	for (unsigned int i = 0; i < count; i++)
		PieceSequences::add(PieceSequences::makeRandom(length));
}

void printSequences() {
	const unsigned int entryCount = PieceSequences::sequenceOccurances.size();
	for (unsigned int i = 0; i < entryCount; i++)
		std::cout << PieceSequences::sequenceOccurances[i].first << " : "
		<< PieceSequences::sequenceOccurances[i].second << std::endl;
}

int main() {
	init();
	addRandomSequences(50000, 3);
	printSequences();
	return 0;
}