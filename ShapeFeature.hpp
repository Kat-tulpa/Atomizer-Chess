#pragma once
#include <cstdint>
#include <string>
#include <vector>
#include <functional>
#include <iostream>

#include "Utility.hpp"
#include "GeometricProperties.hpp"

class ShapeFeature {
private:
    const GeometricProperties m_shape_properties;
    const std::vector<size_t> m_contained_by_index;
    const std::vector<char> m_char_sequence;
    float m_weight;

public:
    ShapeFeature(const GeometricProperties shape_properties, 
        const std::vector<char> char_sequence) :
        m_shape_properties(shape_properties),
        m_char_sequence(char_sequence)
    {}

    ShapeFeature(const std::string& as_string) :
        m_shape_properties(as_string),
        m_char_sequence(Utility::stringToCharVector(
            GeometricProperties::TrailingCharSequence(as_string)))
    {}

    const auto height() const {
        return m_shape_properties.height();
    }

    const auto width() const {
        return m_shape_properties.width();
    }

    const auto offset_x() const {
        return m_shape_properties.offset_x();
    }

    const auto offset_y() const {
        return m_shape_properties.offset_y();
    }

    const auto charSequence() const {
        return m_char_sequence;
    }

    const auto shapeType() const {
        return m_shape_properties.m_type;
    }

    const std::vector<std::vector<char>> charSequenceTo2D() const {
        std::vector<std::vector<char>> result(height(), 
            std::vector<char>(width()));

        for (int i = 0; i < height(); ++i) {
            for (int j = 0; j < width(); ++j) {
                result[i][j] = m_char_sequence[i * width() + j];
            }
        }
        return result;
    }

    const bool charDataMatches(const ShapeFeature& other_feature) const {
        const std::vector<std::vector<char>>& my_char_sequence_2d
            = charSequenceTo2D();
        const std::vector<std::vector<char>>& other_char_sequence_2d
            = other_feature.charSequenceTo2D();

        // Disqualify if char data doesn't match
        for (int i = 0; i < other_feature.height(); i++)
            for (int j = 0; j < other_feature.width(); j++)
                if (my_char_sequence_2d[offset_y() + i][offset_x() + j] 
                    != other_char_sequence_2d[i][j])
                    return false;
    }

    const bool canFitInto(const ShapeFeature& other_feature) const {
        return m_shape_properties.canFitInto(other_feature.m_shape_properties);
    }

    const bool contains(const ShapeFeature& other_feature) const {
        if (!canFitInto(other_feature))
            return false;
        if (!charDataMatches(other_feature))
            return false;
        return true; // It's contained if no disqualifiers occured
    }

    void print() const {
        for (unsigned int y = 0; y < height(); y++) {
            for (unsigned int x = 0; x < width(); x++)
                std::cout << m_char_sequence[(width() * y) + x];
            std::cout << std::endl;
        }
        std::cout << std::endl;
    }

    const std::string serialized() const {
        using namespace Utility;
        std::string serialized;
        serialized.reserve(LENGTH + m_char_sequence.size());

        serialized[TYPE_POS] = toChar(shapeType());
        serialized[WIDTH_POS] = toChar(width());
        serialized[HEIGHT_POS] = toChar(height());
        serialized[OFFSET_X_POS] = toChar(offset_x());
        serialized[OFFSET_Y_POS] = toChar(offset_y());
        serialized += charVectorToString(m_char_sequence);

        return serialized;
    }
};
