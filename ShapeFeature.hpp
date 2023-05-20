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
    GeometricProperties m_geometric_properties;
    //std::vector<size_t> m_contained_by_index;
    std::vector<char> m_char_sequence;
    float m_weight;

public:
    ShapeFeature(const GeometricProperties shape_properties, 
        const std::vector<char> char_sequence) :
        m_geometric_properties(shape_properties),
        m_char_sequence(char_sequence),
        m_weight(0.f)
    {}

    ShapeFeature(const std::string& as_string) :
        m_geometric_properties(as_string),
        m_char_sequence(Utility::stringToCharVector(
            GeometricProperties::TrailingCharSequence(as_string))),
        m_weight(0.f)
    {}

    const GeometricProperties geometricProperties() const {
        return m_geometric_properties;
    }

    const auto height() const {
        return m_geometric_properties.height();
    }

    const auto width() const {
        return m_geometric_properties.width();
    }

    const auto offset_x() const {
        return m_geometric_properties.offset_x();
    }

    const auto offset_y() const {
        return m_geometric_properties.offset_y();
    }

    const auto charSequence() const {
        return m_char_sequence;
    }

    const auto shapeType() const {
        return m_geometric_properties.m_type;
    }

    const auto weight() const {
        return m_weight;
    }

    void weight(const float weight) {
        m_weight = weight;
    }

    ShapeFeature& operator=(const ShapeFeature& other) {
        if (this == &other) {
            return *this;  // Self-assignment check
        }
        m_geometric_properties = other.geometricProperties();
        m_char_sequence = other.charSequence();
        m_weight = other.weight();
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
        return m_geometric_properties.canFitInto(other_feature.m_geometric_properties);
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
        std::string serialized(LENGTH, ' ');

        serialized[TYPE_POS] = '0';
        serialized[WIDTH_POS] = toChar(width());
        serialized[HEIGHT_POS] = toChar(height());
        serialized[OFFSET_X_POS] = toChar(offset_x());
        serialized[OFFSET_Y_POS] = toChar(offset_y());
        serialized += charVectorToString(m_char_sequence);

        return serialized;
    }

    // Todo : Refactor this, it's fucking trash
    const std::vector<ShapeFeature> decomposeIntoSubquadrillaterals() const {
        std::vector<ShapeFeature> shape_feature_list;
        for (size_t w = width(); w >= 1; --w) {
            for (size_t h = height(); h >= 1; --h) {
                if (w < 2 && h < 2) {
                for (size_t y1 = 0; y1 <= height() - h; ++y1) {
                    for (size_t x1 = 0; x1 <= width() - w; ++x1) {
                        std::vector<char> subrect_data;
                        for (size_t x2 = x1; x2 < x1 + w; ++x2) {
                            for (size_t y2 = y1; y2 < y1 + h; ++y2) {
                                subrect_data.push_back(m_char_sequence[x2 * height() + y2]);
                            }
                        }


                        if (!Utility::isAllThisChar(subrect_data, ' '))
                            shape_feature_list.push_back(
                                { { GeometricProperties::ShapeType::RECTANGLE,
                                { w, h }, { x1, y1 } }, subrect_data });
                    }
                    }
                }
            }
        }

        shape_feature_list.erase(shape_feature_list.begin()); // Exclude self
        return shape_feature_list;
    }
};
