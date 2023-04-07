#include <iostream>
#include "Chess.h"

int main() {
    Chess::CSV::loadCsvFile("chessData.csv");
    SequenceManager::print();
    return 0;
}