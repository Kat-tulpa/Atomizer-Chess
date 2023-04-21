#include "Chess.h"
#include "ShapeMap.h"

int main() {
    Chess::init();
    //ShapeMap::print();
    Chess::EvaluationModel::train();
    return 0;
}