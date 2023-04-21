
#include <cstdint>
#include <algorithm>
#include <array>
#include <iostream>
#include <numeric>
#include <random>

#include "Geometric Decomposition.h"
#include "ShapeMap.h"
#include "Shape.h"
#include "FEN.h"
#include "CSV.h"

class Chess {
public:
    static constexpr const char* const CSV_POSITION_EVALUATION_FILE_NAME = "chessData.csv";
    static constexpr const char CSV_DELIMITERS[] { ' ', ','};
 
    static CSV csv_manager;

    class OriginalBoard {
    public:

        OriginalBoard(const Shape shape, float eval_score) :
            m_known_evaluation_score(eval_score),
            m_shape(shape)
        {
            ShapeMap::insert(GeometricDecomposition::all(m_shape));
        }

        Shape::Type m_type;
        Shape m_shape;
        std::vector<char> m_data;
        float m_known_evaluation_score;
    };

    static std::vector<OriginalBoard> original_boards;

    class EvaluationModel {
    public:
        static float calculateAccuracy(float actual_result, float expected_result) {
            float error = fabs(actual_result - expected_result);
            float accuracy = 1.0 - error / expected_result;
            return accuracy;
        }

        static float mutationIncriment(float magnitude) {
            return magnitude;
        }

        static const bool mutationTriggered(const float probability) {
            srand(time(NULL));
            float random = ((float)rand()) / (float)RAND_MAX;
            return random < probability;
        }

        static void train() {
            constexpr float MUTATION_FREQUENCY = 0.5f;
            constexpr float MUTATION_MAGNITUDE = 500.f;
            constexpr size_t MUTATION_ROUNDS = 5;
            constexpr size_t ORIGINAL_BOARD_SAMPLE_SIZE = 75;

            const std::vector<std::pair<Shape, float>>& subshapes = ShapeMap::getAll();
            std::vector<float> highest_scoring_weights;
            for (unsigned int i = 0; i < subshapes.size(); i++)
                highest_scoring_weights.push_back(subshapes[i].second);
            float highest_accuracy = 0.f;

            for (unsigned int k = 0; k < MUTATION_ROUNDS; k++) {
                // Reset weights to highest scoring and mutate
                std::vector<float> current_scoring_weights = highest_scoring_weights;
                for (unsigned int n = 0; n < current_scoring_weights.size(); n++)
                    current_scoring_weights[n] += mutationTriggered(MUTATION_FREQUENCY) * mutationIncriment(MUTATION_MAGNITUDE);

                // Sample a fresh set of original boards at random
                std::vector<OriginalBoard> original_board_samples;
                for (unsigned int m = 0; m < ORIGINAL_BOARD_SAMPLE_SIZE; m++) {
                    const size_t random_index = rand() % original_boards.size();
                    original_board_samples.push_back(original_boards[random_index]);
                }

                float average_accuracy_on_all_boards = 0.f;
                for (unsigned int i = 0; i < ORIGINAL_BOARD_SAMPLE_SIZE; i++) {
                    float score_on_current_board = 0.f;
                    for (unsigned int m = 0; m < subshapes.size(); m++) {
                        if (original_board_samples[i].m_shape.containsSubshape(subshapes[m].first)) {
                            score_on_current_board += current_scoring_weights[m] / subshapes.size();
                            //std::cout << "Subshape Found" << std::endl;
                        }
                    }
                    const float accuracy_on_current_board = calculateAccuracy(score_on_current_board, original_board_samples[i].m_known_evaluation_score);
                    average_accuracy_on_all_boards += accuracy_on_current_board;
                    std::cout << "Expected Position Score:" << original_board_samples[i].m_known_evaluation_score << std::endl;
                    std::cout << "Score on current board: " << score_on_current_board << std::endl;
                }
                average_accuracy_on_all_boards /= ORIGINAL_BOARD_SAMPLE_SIZE;

                if (average_accuracy_on_all_boards > highest_accuracy) {
                    highest_accuracy = average_accuracy_on_all_boards;
                    highest_scoring_weights = current_scoring_weights;
                }
            }
            std::cout << "Original Boards: " << original_boards.size() << std::endl;
            std::cout << "Subshapes: " << subshapes.size() << std::endl;
            std::cout << "Weights: " << highest_scoring_weights.size() << std::endl;
            std::cout << "Highest accuracy: " << highest_accuracy * 100 << "%" << std::endl;
            std::cout << "Weights: ";
            for (auto& c : highest_scoring_weights)
                std::cout << c;
            std::cout << std::endl;
        }
    };

    static void init() {
        size_t lines_processed = 0;

        const size_t csv_line_count = csv_manager.getLineCount();
        original_boards.reserve(csv_line_count);

        while (!csv_manager.getFileStream().std::ios::eof()) {
            const std::vector<std::string>& delimitedLine = csv_manager.readLineDelimited(CSV_DELIMITERS);
            const std::string FEN_string = delimitedLine[0];
            const std::string eval_string = delimitedLine[delimitedLine.size() - 1];
            OriginalBoard board{ FEN::positionStringToShape(FEN_string), FEN::evalStringToFloat(eval_string) };
            board.m_known_evaluation_score;
            original_boards.emplace_back(board);

            lines_processed++;
            if (lines_processed % csv_line_count == 0) 
                return;
        }
    }
};

CSV Chess::csv_manager = CSV(CSV_POSITION_EVALUATION_FILE_NAME);
std::vector<Chess::OriginalBoard> Chess::original_boards;