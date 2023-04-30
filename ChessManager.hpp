
#include <cstdint>
#include <algorithm>
#include <array>
#include <iostream>
#include <numeric>
#include <random>
#include <unordered_set>
#include <cstdlib>

#include "Geometric Decomposition.h"
#include "DataShape.h"
#include "FEN.h"
#include "CSV.h"
#include "RandomDevice.h"
#include "OriginalBoard.hpp"
#include "EvaluationModel.hpp"

class ChessManager {
private:
    static constexpr const char* const CSV_POSITION_EVALUATION_FILE_NAME = "chessData.csv";
    static constexpr const char CSV_DELIMITERS[]{ ' ', ',' };
    static CSV csv_manager;

public:
    static void init() {
        EvaluationModel model(12345);
        size_t lines_processed = 0;

        const size_t csv_line_count = csv_manager.getLineCount();
        model.m_original_boards.reserve(csv_line_count);

        while (!csv_manager.getFileStream().std::ios::eof()) {
            const std::vector<std::string>& delimitedLine = csv_manager.readLineDelimited(CSV_DELIMITERS);
            const std::string FEN_string = delimitedLine[0];
            const std::string eval_string = delimitedLine[delimitedLine.size() - 1];
            OriginalBoard board{ FEN::positionStringToDataShape(FEN_string), FEN::evalStringToFloat(eval_string) };
            model.m_original_boards.emplace_back(board);

            lines_processed++;
            if (lines_processed % csv_line_count == 0) 
                break;
        }
        model.train();
    }
};

CSV ChessManager::csv_manager = CSV(CSV_POSITION_EVALUATION_FILE_NAME);