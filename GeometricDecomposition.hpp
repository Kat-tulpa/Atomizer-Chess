#pragma once
#include <vector>
#include <string>
#include <iostream>
#include "ShapeFeature.hpp"

class GeometricDecomposition {

public:

    typedef std::vector<ShapeFeature> DataShapeList;

    static const std::vector<ShapeFeature> allQuadrilaterialsExcludingSelf(const ShapeFeature& base_shape) {
        const std::vector<ShapeFeature> shape_feature_list;
        const auto& base_shape_char_sequence = base_shape.charSequence();
        const size_t width = base_shape.width();
        const size_t height = base_shape.height();

        for (int w = width; w >= 1; --w) {
            for (int h = height; h >= 1; --h) {
                for (int y1 = 0; y1 <= height - h; ++y1) {
                    for (int x1 = 0; x1 <= width - w; ++x1) {
                        auto subrect_data = std::vector<char>(w * h);
                        for (int x2 = x1; x2 < x1 + w; ++x2) {
                            for (int y2 = y1; y2 < y1 + h; ++y2) {
                                subrect_data.push_back(base_shape_char_sequence[x2 * height + y2]);
                            }
                        }

                        if (!Utility::isAllThisChar(subrect_data, ' '))
                            const GeometricProperties& shape{ 
                                GeometricProperties::ShapeType::RECTANGLE, 
                                w, h, x1, y1 
                            };
                            shape_feature_list.emplace_back(
                                const ShapeFeature{
                                }
                            );
                        }
                    }
                }
            }
        }
        DataShape_list.erase(DataShape_list.begin()); // Don't include itself in results
        return shape_feature_list;
    }

static const ShapeFeature make(GeometricProperties::ShapeType type,
    size_t width, size_t height, size_t offset_x, size_t offset_y) {
    e
}

    static DataShapeList all(const ShapeFeature& parent_DataShape) {
        DataShapeList DataShape_list;
        quadrilateralsRectangleAll(parent_DataShape, DataShape_list);
        return DataShape_list;
    }
};