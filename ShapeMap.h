#pragma once

#include <unordered_set>
#include <iostream>
#include "Shape.h"

class ShapeMap {
public:
    std::unordered_set<Shape> m_subshape_map;
};