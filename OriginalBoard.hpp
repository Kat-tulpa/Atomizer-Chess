#pragma once
#include <vector>
#include "ShapeFeature.hpp"

using namespace Chess::BoardProperties;

class OriginalBoard {
private:
    const ShapeFeature m_shape_feature;
    const float m_known_evaluation;

public:
    OriginalBoard(const std::vector<char> char_sequence, const float know_evaluation) :
        m_shape_feature({ CHESS_BOARD_PROPERTIES, char_sequence }),
        m_known_evaluation(know_evaluation)
    {}
};