#pragma once
#include <stdexcept>

const enum SERIALIZED_FORMAT {
    TYPE_POS,
    WIDTH_POS,
    HEIGHT_POS,
    OFFSET_X_POS,
    OFFSET_Y_POS,
    LENGTH
};

struct Dimensions {
    Dimensions(size_t width, size_t height) :
        m_width(width),
        m_height(height)
    {}

    Dimensions(const char& width, const char& height) :
        m_width(Utility::charToDigit(width)),
        m_height(Utility::charToDigit(height))
    {}

    size_t m_width;
    size_t m_height;
};

struct Offsets {
    Offsets(size_t x, size_t y) :
        m_x(x),
        m_y(y)
    {}

    Offsets(const char& x, const char& y) :
        m_x(Utility::charToDigit(x)),
        m_y(Utility::charToDigit(y))
    {}

    size_t m_x;
    size_t m_y;
};

class GeometricProperties {
public:

    enum ShapeType : size_t {
        RECTANGLE,
        TYPE_COUNT
    };

    GeometricProperties(const std::string& serialized_data) :
        m_type(static_cast<ShapeType>
            (serialized_data[SERIALIZED_FORMAT::TYPE_POS])),
        m_dimensions(Dimensions{
            serialized_data[SERIALIZED_FORMAT::WIDTH_POS],
            serialized_data[SERIALIZED_FORMAT::HEIGHT_POS]}),

        m_offset(Offsets{
            serialized_data[SERIALIZED_FORMAT::OFFSET_X_POS],
            serialized_data[SERIALIZED_FORMAT::OFFSET_Y_POS]})
    {};

    GeometricProperties(const ShapeType type, 
        const Dimensions& dimensions, const Offsets& offset) :
        m_type(type),
        m_dimensions(dimensions),
        m_offset(offset)
    {};

    GeometricProperties& operator=(const GeometricProperties& other) {
        if (this == &other) {
            return *this;  // Self-assignment check
        }
        m_type = other.m_type;
        m_dimensions = other.m_dimensions;
        m_offset = other.m_offset;
    }

    bool canFitInto(const GeometricProperties& other) const {
        return  (m_offset.m_x + m_dimensions.m_width 
            <= other.m_dimensions.m_width)
            && (m_offset.m_y + m_dimensions.m_height 
                <= other.m_dimensions.m_height);
    }

    static const std::string TrailingCharSequence(const std::string& str) {
        return str.substr(SERIALIZED_FORMAT::LENGTH);
    }

    const size_t height() const {
        return m_dimensions.m_height;
    }

    const size_t width() const {
        return m_dimensions.m_width;
    }

    const size_t offset_x() const {
        return m_offset.m_x;
    }

    const size_t offset_y() const {
        return m_offset.m_y;
    }

    ShapeType m_type;
    Dimensions m_dimensions;
    Offsets m_offset;
};