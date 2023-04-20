#pragma once
#include <cstdint>
#include <string>
#include <vector>

#include "Utility.h"

class Shape {
public:

    struct Dimensions {
    public:
        Dimensions(uint8_t width, uint8_t height) : m_width(width), m_height(height) {}

        auto getWidth() const { return m_width; }
        auto getHeight() const { return m_height; }
        auto getCategorizationPrefix() const { return std::string(2, static_cast<char>(getWidth()) + static_cast<char>(getHeight())); }

    private:
        uint8_t m_width;
        uint8_t m_height;
    };

    struct Offset {
    public:
        Offset(uint8_t x, uint8_t y) : m_x(x), m_y(y) {}

        auto getX() const { return m_x; }
        auto getY() const { return m_y; }
        auto getCategorizationPrefix() const { return std::string(2, static_cast<char>(getX()) + static_cast<char>(getY())); }

    private:
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

private:
    const Type m_type;
    const Dimensions m_dimensions;
    const Offset m_offset;
    const std::vector<char> m_data;

public:

    Shape(const Type type, const Dimensions dimensions, const Offset offset, std::vector<char> data)
        : m_type(type), m_dimensions(dimensions), m_offset(offset), m_data(data)
    {};

    bool operator==(const Shape& lhs, const Shape& rhs) {
        return lhs.getDimensions().getWidth() == rhs.getDimensions().getWidth()
            && lhs.getDimensions().getHeight() == rhs.getDimensions().getHeight()
            && lhs.getOffset().getX() == rhs.getOffset().getX()
            && lhs.getOffset().getY() == rhs.getOffset().getY()
            && lhs.getData() == rhs.getData()
            && lhs.getType() == rhs.getType();
    }

    bool operator!=(const Shape& lhs, const Shape& rhs) {
        return !(lhs == rhs);
    }

    auto getDimensions() const { return m_dimensions; }
    auto getWidth() const { return m_dimensions.getWidth(); }
    auto getHeight() const { return m_dimensions.getHeight(); }
    auto getOffset() const { return m_offset; }
    auto getOffsetX() const { return m_offset.getX(); }
    auto getOffsetY() const { return m_offset.getY(); }
    auto getData() const { return m_data; }

    auto getCategorizationPrefix() const { 
        return getDimensions().getCategorizationPrefix() + getOffset().getCategorizationPrefix(); 
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
};