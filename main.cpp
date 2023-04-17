#include <iostream>
#include "Chess.h"
#include "CSV.h"
#include "FEN.h"

int main() {
    CSV csv_manager("chessData.csv");

    const std::vector<char>& delimiters{ ' ', ',' };
    while (!csv_manager.getFile().std::ios::eof()) {
        const std::vector<std::string>& delimitedLine = csv_manager.readLineDelimited(delimiters, 6);
        const std::string& FEN_part = delimitedLine.at(0);
        GeometricDecomposition::all(FEN::formatToShape(FEN_part));
    }

    SequenceManager::print();
    return 0;
}