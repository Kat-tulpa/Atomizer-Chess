#pragma once
#include <cstdint>
#include <string>
#include <vector>
#include <functional>
#include <iostream>

#include "Utility.h"

class DataShape {
public:
    struct Dimensions {
        Dimensions(size_t width, size_t height) : m_width(width), m_height(height) {}
        auto getCategorizationPrefix() const { return std::to_string(m_width) + std::to_string(m_height); }

        size_t m_width;
        size_t m_height;
    };

    struct Offset {
        Offset(size_t x, size_t y) : m_x(x), m_y(y) {}
        auto getCategorizationPrefix() const { return std::to_string(m_x) + std::to_string(m_y); }

        size_t m_x;
        size_t m_y;
    };

    enum Type : size_t {
        RECTANGLE_SQUARE,
        RECTANGLE,
        TYPE_COUNT
    };

    DataShape(const Type type, const Dimensions dimensions, const Offset offset, std::vector<char> data)
        : m_type(type), m_dimensions(dimensions), m_offset(offset), m_data(data)
    {}

    DataShape(const std::string& as_string)
        : m_type(RECTANGLE_SQUARE),
        m_dimensions(Dimensions{ static_cast<size_t>(as_string[0]), static_cast<size_t>(as_string[1]) }),
        m_offset(Offset{ static_cast<size_t>(as_string[2]), static_cast<size_t>(as_string[3]) }),
        m_data(Utility::stringToCharVector(as_string.substr(4)))
    {}

    bool operator==(const DataShape& other) const {
        return m_type == other.m_type
            && m_dimensions.m_width == other.m_dimensions.m_width
            && m_dimensions.m_height == other.m_dimensions.m_height
            && m_offset.m_x == other.m_offset.m_x
            && m_offset.m_y == other.m_offset.m_y
            && m_data == other.m_data;
    }

    auto getCategorizationPrefix() const { 
        return m_dimensions.getCategorizationPrefix() + m_offset.getCategorizationPrefix(); 
    }

    bool containsSubDataShape(const DataShape& subDataShape) const {
        if (subDataShape.m_dimensions.m_width > m_dimensions.m_width ||
            subDataShape.m_dimensions.m_height > m_dimensions.m_height) {
            return false;
        }
        const char* subDataShapeData = subDataShape.m_data.data();
        const char* data = m_data.data();
        int subDataShapeWidth = subDataShape.m_dimensions.m_width;
        int dataShapeWidth = m_dimensions.m_width;
        for (int i = 0; i < subDataShape.m_data.size(); ++i) {
            int x = i % subDataShapeWidth;
            int y = i / subDataShapeWidth;
            const char* subDataShapePixel = subDataShapeData + i;
            const char* pixel = data + (subDataShape.m_offset.m_y + y) * dataShapeWidth +
                (subDataShape.m_offset.m_x + x);
            if (*subDataShapePixel != *pixel) {
                return false;
            }
        }
        return true;
    }
    
    void print() const {
        for (unsigned int y = 0; y < m_dimensions.m_height; y++) {
            for (unsigned int x = 0; x < m_dimensions.m_width; x++)
                std::cout << m_data[(m_dimensions.m_width * y) + x];
            std::cout << std::endl;
        }
        std::cout << std::endl;
    }

    const std::string asString() const {
        return getCategorizationPrefix() + std::string(m_data.begin(), m_data.end());
    }

    Type m_type;
    Dimensions m_dimensions;
    Offset m_offset;
    std::vector<char> m_data;
    static size_t precomputed_hash_offsets[8][8];
};

static constexpr size_t precomputed_hash_types[2] = { 0, 1 };

namespace std {
    constexpr size_t precomputed_hash_dimensions_or_offsets[8][8]{
        { 0,  1,  2,  3,  4,  5,  6,  7  },
        { 8,  9,  10, 11, 12, 13, 14, 15 },
        { 16, 17, 18, 19, 20, 21, 22, 23 },
        { 24, 25, 26, 27, 28, 29, 30, 31 },
        { 32, 33, 34, 35, 36, 37, 38, 39 },
        { 40, 41, 42, 43, 44, 45, 46, 47 },
        { 48, 49, 50, 51, 52, 53, 54, 55 },
        { 56, 57, 58, 59, 60, 61, 62, 63 }
    };

    template <>
    struct hash<DataShape> {
        size_t operator()(const DataShape& s) const {
            const size_t type_hash = static_cast<size_t>(precomputed_hash_types[s.m_type]);
            const size_t dimension_offset_hash = 
                precomputed_hash_dimensions_or_offsets[s.m_dimensions.m_width][s.m_dimensions.m_height];

            // Combine hash values using bitwise XOR operations
            size_t hash_value = type_hash ^ dimension_offset_hash ^ dimension_offset_hash;
            for (const char& c : s.m_data)
                hash_value ^= static_cast<size_t>(c) << 40;
            return hash_value;
        }
    };
}