#include "Chess.h"

int main() {
    Chess::init();
    Chess::EvaluationModel eval_model;
    eval_model.train();
    return 0;
}