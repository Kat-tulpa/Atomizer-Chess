#pragma once

#include <unordered_map>
#include "Shape.h"

class ShapeMap {
public:
    static std::unordered_map<Shape, float> m_subshape_score_map;

    static std::pair<std::unordered_map<Shape, float>::iterator, bool> insert(const Shape& subshape, float score) {
        auto result = m_subshape_score_map.insert({ subshape, score });
        return result;
    }

    static void insert(const std::vector<Shape>& subshapes) {
        for (const auto& subshape : subshapes)
            m_subshape_score_map[subshape] = 0.f;
    }

    static std::vector<std::pair<Shape, float>> getAll() {
        std::vector<std::pair<Shape, float>> entries;
        for (const auto& entry : m_subshape_score_map) {
            entries.push_back(entry);
        }
        return entries;
    }
};