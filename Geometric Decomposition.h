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

    private:
        const Type m_type;
        const Dimensions m_dimensions;
        const Offset m_offset;
        const std::vector<char> m_data;

    public:

        // Construct Shape With String From Sequence Manager
        Shape(const std::string& sequence_manager_string)
            : m_type(static_cast<Type>(static_cast<uint8_t>(sequence_manager_string[0])))
            , m_dimensions({ static_cast<uint8_t>(sequence_manager_string[1]), static_cast<uint8_t>(sequence_manager_string[2]) })
            , m_offset({ static_cast<uint8_t>(sequence_manager_string[3]), static_cast<uint8_t>(sequence_manager_string[4]) })
            , m_data(Utility::stringToVector(sequence_manager_string.substr(5, sequence_manager_string.size() - 5)))
        {}

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

    using ShapeList = std::vector<Shape>;

    static void quadrilateralsRectangleAll(const Shape& parent_shape, ShapeList& shape_list) {
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
                        shape_list.emplace_back(std::move(shape));
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
        quadrilateralsRectangleAll(parent_shape, shape_list);
        addListToSequenceManager(shape_list);
    }

    static const size_t reigsterNewSequenceManagerID() {
        const size_t current_sequence_manager_registration_ID = sequence_manager_registration_ID;
        sequence_manager_registration_ID = SequenceManager::makeAddDataType();
        return current_sequence_manager_registration_ID;
    }
};

size_t GeometricDecomposition::sequence_manager_registration_ID = SequenceManager::makeAddDataType();