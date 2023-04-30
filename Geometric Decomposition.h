#pragma once
#include <vector>
#include <string>
#include <iostream>

#include "DataShape.h"

class GeometricDecomposition {

public:

    typedef std::vector<DataShape> DataShapeList;

    static void quadrilateralsRectangleAll(const DataShape& parent_DataShape, DataShapeList& DataShape_list) {
        const auto& parent_data = parent_DataShape.m_data;
        const unsigned int parent_width = parent_DataShape.m_dimensions.m_width;
        const unsigned int parent_height = parent_DataShape.m_dimensions.m_height;

        for (int w = parent_width; w >= 1; --w) {
            for (int h = parent_height; h >= 1; --h) {
                for (int y1 = 0; y1 <= parent_height - h; ++y1) {
                    for (int x1 = 0; x1 <= parent_width - w; ++x1) {
                        auto subrect_data = std::vector<char>();
                        subrect_data.reserve(w * h);
                        for (int x2 = x1; x2 < x1 + w; ++x2) {
                            for (int y2 = y1; y2 < y1 + h; ++y2) {
                                subrect_data.push_back(parent_data[x2 * parent_height + y2]);
                            }
                        }

                        if (!Utility::isAllThisChar(subrect_data, ' ')) {
                            DataShape_list.emplace_back(
                                DataShape::Type::RECTANGLE, 
                                DataShape::Dimensions(w, h), 
                                DataShape::Offset(x1, y1), 
                                subrect_data
                            );
                        }
                    }
                }
            }
        }
        DataShape_list.erase(DataShape_list.begin());
    }

    static DataShapeList all(const DataShape& parent_DataShape) {
        DataShapeList DataShape_list;
        quadrilateralsRectangleAll(parent_DataShape, DataShape_list);
        return DataShape_list;
    }
};