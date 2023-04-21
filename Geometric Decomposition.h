#pragma once
#include <vector>
#include <string>
#include <iostream>

#include "Shape.h"

class GeometricDecomposition {

public:

    typedef std::vector<Shape> ShapeList;

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
                                subrect_data.push_back(parent_data[x2 * parent_height + y2]);
                                //std::cout << parent_data[x2 * parent_height + y2] << std::endl;
                            }
                        }
                        Shape shape(Shape::Type::RECTANGLE, Shape::Dimensions(w, h), Shape::Offset(x1, y1), subrect_data);
                        shape_list.emplace_back(std::move(shape));
                    }
                }
            }
        }
        shape_list.erase(shape_list.begin());
    }

    static ShapeList all(const Shape& parent_shape) {
        ShapeList shape_list;
        quadrilateralsRectangleAll(parent_shape, shape_list);
        return shape_list;
    }
};