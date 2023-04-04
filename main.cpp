#include <iostream>
#include "Chess.h"

int main() {
    Chess::load_FEN_CSV("chessData.csv");
    SequenceManager::print();
    return 0;
}