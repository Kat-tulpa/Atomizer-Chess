#pragma once
#include "Geometric Decomposition.h"

class OriginalBoard {
public:

    DataShape::Type m_type;
    DataShape m_DataShape;
    float m_known_evaluation_score;
    std::vector<DataShape> subDataShapes;
    std::vector<size_t> subDataShape_indicies;

    OriginalBoard(const DataShape DataShape, float eval_score) :
        m_known_evaluation_score(eval_score),
        m_DataShape(DataShape)
    {
        subDataShapes = GeometricDecomposition::all(m_DataShape);
        subDataShape_indicies.assign(subDataShapes.size(), 0);
    }
};