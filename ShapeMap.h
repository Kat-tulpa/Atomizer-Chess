#pragma once

#include <unordered_map>
#include <iostream>
#include "Shape.h"

class ShapeMap {
public:
    static std::unordered_map<Shape, float> m_subshape_score_map;

    static void insert(const std::vector<Shape>& subshapes) {
        for (const auto& subshape : subshapes)
            m_subshape_score_map.insert(std::make_pair(subshape, 0.0f));
    }

    static std::vector<std::pair<Shape, float>> getAll() {
        std::vector<std::pair<Shape, float>> entries;
        for (const auto& entry : m_subshape_score_map) {
            entries.push_back(entry);
        }
        return entries;
    }

    static void print() {
        for (const auto& entry : m_subshape_score_map) {
            std::cout << entry.second << std::endl;
        }
    }
};

std::unordered_map<Shape, float> ShapeMap::m_subshape_score_map;