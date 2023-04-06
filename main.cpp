#include <iostream>
#include "Chess.h"

int main() {
    Chess::FEN::CSV::load("chessData.csv");
    SequenceManager::print();
    return 0;
}