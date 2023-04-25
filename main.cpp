#include "Chess.h"
#include "ShapeMap.h"

#include <locale>
#include <iostream>

int main() {
    // Set the locale to UTF-8 encoding
    std::locale::global(std::locale("en_US.UTF-8"));
    std::cout.imbue(std::locale());
    Chess::init();
    //ShapeMap::print();
    Chess::EvaluationModel::train();
    return 0;
}