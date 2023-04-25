#pragma once
#include <cstdint>
#include <string>
#include <vector>
#include <functional>
#include <cwchar>
#include <iostream>
#include <io.h>
#include <fcntl.h>
#include <cwchar>

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

    bool operator==(const Shape& other) const {
        return m_type == other.m_type
            && m_dimensions.m_width == other.m_dimensions.m_width
            && m_dimensions.m_height == other.m_dimensions.m_height
            && m_offset.m_x == other.m_offset.m_x
            && m_offset.m_y == other.m_offset.m_y
            && m_data == other.m_data;
    }

    void print() const {
        for (unsigned int y = 0; y < m_dimensions.m_height; y++) {
            for (unsigned int x = 0; x < m_dimensions.m_width; x++) {
                wchar_t print_char;
                switch (m_data[y * m_dimensions.m_width + x]) {
                case 'k':
                    print_char = L'♔'; // Unicode value for ♔
                    break;
                case 'q':
                    print_char = L'♕'; // Unicode value for ♕
                    break;
                case 'r':
                    print_char = L'♖'; // Unicode value for ♖
                    break;
                case 'b':
                    print_char = L'♗'; // Unicode value for ♗
                    break;
                case 'n':
                    print_char = L'♘'; // Unicode value for ♘
                    break;
                case 'p':
                    print_char = L'♙'; // Unicode value for ♙
                    break;
                case 'K':
                    print_char = L'♚'; // Unicode value for ♚
                    break;
                case 'Q':
                    print_char = L'♛'; // Unicode value for ♛
                    break;
                case 'R':
                    print_char = L'♜'; // Unicode value for ♜
                    break;
                case 'B':
                    print_char = L'♝'; // Unicode value for ♝
                    break;
                case 'N':
                    print_char = L'♞'; // Unicode value for ♞
                    break;
                case 'P':
                    print_char = L'♟'; // Unicode value for ♟
                    break;
                default:
                    // Default case for unknown characters
                    print_char = m_data[y * m_dimensions.m_width + x];
                    break;
                }
                std::wcout << print_char;
            }
            std::wcout << std::endl;
        }
        std::wcout << std::endl;
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

    // function to check if a smaller DataShape is contained within this DataShape
    bool containsSubshape(const Shape& subshape) const {
        if (subshape.m_dimensions.m_width > m_dimensions.m_width || subshape.m_dimensions.m_height > m_dimensions.m_height)
            return false; // subshape is larger, cannot be contained

        const char* subshapeData = subshape.m_data.data();
        const char* data = m_data.data();
        int subshapeWidth = subshape.m_dimensions.m_width;
        int shapeWidth = m_dimensions.m_width;
        for (int i = 0; i < subshape.m_data.size(); ++i) {
            int x = i % subshapeWidth;
            int y = i / subshapeWidth;
            const char* subshapePixel = subshapeData + i;
            const char* pixel = data + (subshape.m_offset.m_y + y) * shapeWidth + (subshape.m_offset.m_x + x);
            if (*subshapePixel != *pixel)
                return false; // data does not match, subshape not contained
        }
        return true; // all data matched, subshape is contained
    }

    Type m_type;
    Dimensions m_dimensions;
    Offset m_offset;
    std::vector<char> m_data;

    static size_t precomputed_hash_offsets[8][8];
    static size_t precomputed_hash_type[Type::TYPE_COUNT];
};

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

    constexpr size_t precomputed_hash_types[2] = { 0, 1 };

    template <>
    struct hash<Shape> {
        size_t operator()(const Shape& s) const {
            // Precompute hash values for possible properties
            const size_t type_hash
                = static_cast<size_t>(precomputed_hash_types[s.m_type]);
            const size_t dimension_offset_hash = 
                precomputed_hash_dimensions_or_offsets[s.m_dimensions.m_width][s.m_dimensions.m_height];

            // Combine hash values using bitwise XOR operations
            size_t hash_value = type_hash ^ dimension_offset_hash ^ dimension_offset_hash;

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
            static constexpr size_t size_of_member_vars_without_m_data
                = sizeof(Shape) - sizeof(std::vector<char>);
            return (memcmp(&s1, &s2, size_of_member_vars_without_m_data) == 0) && (s1.m_data == s2.m_data);

            /* Alternative Implementation 
            return s1.m_type == s2.m_type
                && s1.m_dimensions.m_height == s2.m_dimensions.m_height
                && s1.m_dimensions.m_width == s2.m_dimensions.m_width
                && s1.m_offset.m_x == s2.m_offset.m_x
                && s1.m_offset.m_y == s2.m_offset.m_y
                && s1.m_data == s2.m_data;
                */
        }
    };
}