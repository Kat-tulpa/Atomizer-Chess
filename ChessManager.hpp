#pragma once
#include <vector>
#include <string>
#include "CSVReader.hpp"
#include "FEN.hpp"
#include "EvaluationModel.hpp"
#include "Defs.hpp"

namespace ChessManager {
    static void init() {
        using namespace Chess::IO;
        using namespace Chess;
        CSVReader csv_reader(CSV_POSITION_EVALUATION_FILE_NAME);
        EvaluationModel model;
        size_t lines_processed = 0;

        for (unsigned int i = 0; i < ORIGINAL_BOARD_SAMPLE_SIZE; i++) {
            const std::vector<std::string>& delimitedLine
                = csv_reader.delimit(CSV_DELIMITERS);
            const std::string FEN_string = delimitedLine[0];
            const std::string eval_string 
                = delimitedLine[delimitedLine.size() - 1];

            const float known_evaluation_score 
                = FEN::evalStringToFloat(eval_string);
            const std::vector<char>& char_sequence 
                = FEN::positionStringToCharSequence(FEN_string);

            ShapeFeature board(BoardProperties::CHESS_BOARD_PROPERTIES, char_sequence);
            board.weight(known_evaluation_score);
            model.addParentShapeFeature(board);

            lines_processed++;
            if (lines_processed % (ORIGINAL_BOARD_SAMPLE_SIZE / 100) == 0)
                std::cout << lines_processed << std::endl;
        }
        model.train();
    }
};