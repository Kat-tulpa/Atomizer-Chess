#pragma once
#include <vector>
#include <string>

#include "SequenceManager.h"

class GeometricDecomposition {
    static size_t sequence_manager_registration_ID;

public:
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

        enum class Type : uint8_t {
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

        Shape(Type type, Dimensions dimensions, Offset offset, const std::vector<char>& data) :
            m_type(type), m_dimensions(dimensions), m_offset(offset), m_data(data) {}

        auto getDimensions() const { return m_dimensions; }
        auto getWidth() const { return m_dimensions.getWidth(); }
        auto getHeight() const { return m_dimensions.getHeight(); }
        auto getOffset() const { return m_offset; }
        auto getOffsetX() const { return m_offset.getX(); }
        auto getOffsetY() const { return m_offset.getY(); }
        auto getData() const { return m_data; }
        auto getCategorizationPrefix() const { return getDimensions().getCategorizationPrefix() + getOffset().getCategorizationPrefix(); }

    private:
        const Type m_type;
        const Dimensions m_dimensions;
        const Offset m_offset;
        const std::vector<char> m_data;
    };

    using ShapeList = std::vector<Shape>;

    static void quadrilateralsRectangle(const Shape& parent_shape, ShapeList& shape_list) {
        const auto& parent_data = parent_shape.getData();
        const unsigned int parent_width = parent_shape.getWidth();
        const unsigned int parent_height = parent_shape.getHeight();

        for (int w = parent_width; w >= 1; --w) {
            for (int h = parent_height; h >= 1; --h) {
                for (int y1 = 0; y1 <= parent_height - h; ++y1) {
                    for (int x1 = 0; x1 <= parent_width - w; ++x1) {
                        auto subrect_data = std::vector<char>();
                        subrect_data.reserve(w * h);
                        for (int x2 = x1; x2 < x1 + w; ++x2) {
                            for (int y2 = y1; y2 < y1 + h; ++y2) {
                                subrect_data.emplace_back(parent_data[x2 * parent_height + y2]);
                            }
                        }
                        Shape shape(Shape::Type::RECTANGLE, Shape::Dimensions(w, h), Shape::Offset(x1, y1), std::move(subrect_data));
                        shape_list.push_back(std::move(shape));
                    }
                }
            }
        }
    }

    static void addListToSequenceManager(const ShapeList& shape_list) {
        std::vector<std::string> entries;
        for (const auto& shape : shape_list) {
            const std::string& prefix = shape.getCategorizationPrefix();
            const std::vector<char>& data = shape.getData();
            std::string data_string(data.begin(), data.end());
            entries.emplace_back(prefix + data_string);
        }
        SequenceManager::addDataEntries(sequence_manager_registration_ID, entries);
    }

    static void all(const Shape& parent_shape) {
        ShapeList shape_list;
        const unsigned int parent_width = parent_shape.getWidth();
        const unsigned int parent_height = parent_shape.getHeight();
        const unsigned int total_subshapes = (parent_width * (parent_width + 1) / 2) * (parent_height * (parent_height + 1) / 2);
        shape_list.reserve(shape_list.size() + total_subshapes);
        quadrilateralsRectangle(parent_shape, shape_list);
        addListToSequenceManager(shape_list);
    }
};

size_t GeometricDecomposition::sequence_manager_registration_ID = SequenceManager::makeAddDataType();