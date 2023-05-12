#pragma once
#include <vector>
#include <string>

#include "CSV.hpp"
#include "FEN.hpp"
#include "EvaluationModel.hpp"
#include "OriginalBoard.hpp"

namespace ChessManager {
    using namespace Chess::IO;
    static void init() {
        CSV csv_manager(CSV_POSITION_EVALUATION_FILE_NAME, ORIGINAL
        );
        EvaluationModel model;
        size_t lines_processed = 0;

        const size_t csv_line_count = csv_manager.getLineCount();
        model.m_original_boards.reserve(csv_line_count);

        while (!csv_manager.eof()) {
            const std::vector<std::string>& delimitedLine 
                = csv_manager.readLineDelimited(Chess::IO::CSV_DELIMITERS);
            const std::string FEN_string 
                = delimitedLine[0];
            const std::string eval_string 
                = delimitedLine[delimitedLine.size() - 1];

            const float known_evaluation_score 
                = FEN::evalStringToFloat(eval_string);
            const std::vector<char>& char_sequence 
                = FEN::positionStringToCharSequence(FEN_string);

            const OriginalBoard& board{ char_sequence, known_evaluation_score };
            model.m_original_boards.emplace_back(board);

            lines_processed++;
            if (lines_processed % csv_line_count == 0) 
                break;
        }
        csv_manager.close();
        model.train();
    }
};