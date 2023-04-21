#pragma once
#include <cstdint>
#include <string>
#include <vector>
#include <functional>

#include "Utility.h"

class Shape {

public:
    struct Dimensions {
        Dimensions(uint8_t width, uint8_t height) : m_width(width), m_height(height) {}
        auto getCategorizationPrefix() const { return std::string(2, static_cast<char>(m_width) + static_cast<char>(m_height)); }

        uint8_t m_width;
        uint8_t m_height;
    };

    struct Offset {
        Offset(uint8_t x, uint8_t y) : m_x(x), m_y(y) {}
        auto getCategorizationPrefix() const { return std::string(2, static_cast<char>(m_x) + static_cast<char>(m_y)); }

        uint8_t m_x;
        uint8_t m_y;
    };

    enum Type : uint8_t {
        RECTANGLE_SQUARE,
        RECTANGLE,
        ELLIPSE_CIRCLE,
        ELLIPSE_OVAL,
        TRIANGLE_RIGHT,
        TRIANGLE_EQUILATERAL,
        TRIANGLE_ISOSCELES,
        POLYGON_HEXAGON,
        POLYGON_OCTAGON,
        DIAGONAL_LINE,
        TYPE_COUNT
    };

    Shape(const Type type, const Dimensions dimensions, const Offset offset, std::vector<char> data)
        : m_type(type), m_dimensions(dimensions), m_offset(offset), m_data(data)
    {}

    Shape& operator=(const Shape& other) {
        if (this == &other) {
            return *this;
        }
        m_type = other.m_type;
        m_dimensions.m_width = other.m_dimensions.m_width;
        m_dimensions.m_height = other.m_dimensions.m_height;
        m_offset.m_x = other.m_offset.m_x;
        m_offset.m_y = other.m_offset.m_y;
        m_data = other.m_data;
        return *this;
    }

    auto getType() const { return m_type; }
    auto getWidth() const { return m_dimensions.m_width; }
    auto getHeight() const { return m_dimensions.m_height; }
    auto getOffsetX() const { return m_offset.m_x; }
    auto getOffsetY() const { return m_offset.m_y; }
    auto getData() const { return m_data; }

    auto getCategorizationPrefix() const { 
        return m_dimensions.getCategorizationPrefix() + m_offset.getCategorizationPrefix(); 
    }

    bool containsSubshape(const Shape& subshape) {
        const size_t limit_x = subshape.getWidth() + subshape.getOffsetX();
        const size_t limit_y = subshape.getHeight() + subshape.getOffsetY();
        if (limit_x > getWidth() || limit_y > getHeight()) return false; // Subshape must fit inside shape

        const std::vector<char>& shape_data = getData();
        const std::vector<char>& subshape_data = subshape.getData();
        size_t position = subshape.getOffsetY() * getWidth() + subshape.getOffsetX();
        size_t position_y = subshape.getOffsetY();
        size_t position_x = subshape.getOffsetX();

        for (unsigned int i = 0; i < subshape_data.size(); i++) {
            if (!subshape_data[i] == shape_data[(position_y * getHeight()) + (subshape.getOffsetX() + position_x)])
                return false;
            position_x++;
            if (position_x % subshape.getWidth() == 0) {
                position_x = 0;
                position_y++;
            }
        }

        return true;
    }

    Type m_type;
    Dimensions m_dimensions;
    Offset m_offset;
    std::vector<char> m_data;
};

namespace std {
    template <>
    struct hash<Shape> {
        size_t operator()(const Shape& s) const {
            size_t hash_value = static_cast<size_t>(s.getType());
            hash_value ^= static_cast<size_t>(s.getWidth()) << 8;
            hash_value ^= static_cast<size_t>(s.getHeight()) << 16;
            hash_value ^= static_cast<size_t>(s.getOffsetX()) << 24;
            hash_value ^= static_cast<size_t>(s.getOffsetY()) << 32;
            const auto& data = s.getData();
            for (const char& c : data) {
                hash_value ^= static_cast<size_t>(c) << 40;
            }
            return hash_value;
        }
    };

    template <>
    struct equal_to<Shape> {
        bool operator()(const Shape& s1, const Shape& s2) const {
            return s1.getType() == s2.getType()
                && s1.m_dimensions.m_width == s2.m_dimensions.m_width
                && s1.m_dimensions.m_height == s2.m_dimensions.m_height
                && s1.m_offset.m_x == s2.m_offset.m_x
                && s1.m_offset.m_y == s2.m_offset.m_y
                && s1.getData() == s2.getData();
        }
    };
}