
#include <cstdint>
#include <algorithm>
#include <array>
#include <iostream>
#include <numeric>
#include <random>
#include <unordered_set>
#include <cstdlib>

#include "Geometric Decomposition.h"
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

        Shape::Type m_type;
        Shape m_shape;
        std::vector<char> m_data;
        float m_known_evaluation_score;
        std::vector<Shape> subshapes;
        std::vector<size_t> subshape_indicies;

        OriginalBoard(const Shape shape, float eval_score) :
            m_known_evaluation_score(eval_score),
            m_shape(shape)
        {
            subshapes = GeometricDecomposition::all(m_shape);
            subshape_indicies.assign(subshapes.size(), 0);
        }
    };

    static std::vector<OriginalBoard> original_boards;

    class EvaluationModel {
    public:
        static float calculateAccuracy(float actual_result, float expected_result) {
            float error = fabs(actual_result - expected_result);
            float accuracy = 0.0; // Initialize accuracy to 0.0

            if (expected_result != 0.0) {
                // Calculate accuracy based on the relative error compared to expected_result
                accuracy = 1.0 - (error / expected_result);
            }
            else {
                // Handle case where expected_result is 0
                // Assign high accuracy when actual_result is close to 0, and low accuracy otherwise
                accuracy = (actual_result <= 0.0) ? 1.0 : 0.0;
            }

            // Ensure accuracy is within the valid range of 0.0 to 1.0
            accuracy = fmax(0.0, fmin(1.0, accuracy));

            return accuracy;
        }

        static float mutationDelta(float magnitude) {
            // Generate a random float between 0 and 1
            float randFloat = (float)rand() / RAND_MAX;

            // Scale the random float to the magnitude and adjust the range to be between -magnitude and +magnitude
            return randFloat * (2 * magnitude) - magnitude;
        }

        static const bool mutationTriggered(const float probability) {
            float random = ((float)rand()) / (float)RAND_MAX;
            return random < probability;
        }

        static const std::vector<float> resolveParentWeights(const std::vector<float>& weights, const OriginalBoard& board) {
            std::vector<float> resolved_weights;
            for (auto& subshape_index : board.subshape_indicies)
                resolved_weights.push_back(weights[subshape_index]);
            return resolved_weights;
        }

        static void testModelOnBoard(const std::vector<float>& weights, const OriginalBoard& board) {
            const std::vector<float>& these_weights = resolveParentWeights(weights, board);
            float score = 0.f;
            for (auto& subshape_index : board.subshape_indicies)
                score += weights[subshape_index];
            std::cout << "Model's accuracy against this board was: " << calculateAccuracy(score, board.m_known_evaluation_score) * 100 << "%" << std::endl;
        }

        static void testModelOnHiddenBoards(const std::vector<float>& weights, const size_t hidden_board_count) {
            for (unsigned int i = 0; i < hidden_board_count; i++)
                testModelOnBoard(weights, original_boards[original_boards.size() - hidden_board_count + i]);
        }

        static void train() {
            static constexpr float MUTATION_FREQUENCY = 0.9f;
            static constexpr float MUTATION_MAGNITUDE = 1.f;
            static constexpr size_t MUTATION_ROUNDS = 200000;
            static constexpr size_t ORIGINAL_BOARD_SAMPLE_SIZE = 7; // Standard benchmark = 100

            // Shuffle the original boards
            Utility::shuffleVector(original_boards);

            // Prepare subshapes for training
            std::unordered_map<Shape, size_t> subshape_map;
            size_t index = 0;
            for (auto& board : original_boards)
                for (unsigned int i = 0; i < board.subshape_indicies.size(); i++)
                    if (subshape_map.insert(std::make_pair(board.subshapes[i], index)).second)
                        board.subshape_indicies[i] = index++;
                    else
                        board.subshape_indicies[i] = subshape_map.find(board.subshapes[i])->second;
            const size_t subshape_count = index;

            // Initialize weights
            std::vector<float> best_weights(subshape_count, 0.f);
            std::vector<float> current_weights(subshape_count, 0.f);
            float best_accuracy = 0.f;

            float expected_score = 0.f;
            for (auto& board : original_boards)
                expected_score += board.m_known_evaluation_score;

            // Perform mutation rounds
            for (unsigned int k = 0; k < MUTATION_ROUNDS; k++) {
                const std::vector<float>& random_mutation_frequency_floats
                    = Utility::SIMDRandomBools(subshape_count, MUTATION_FREQUENCY);
                const std::vector<float>& random_mutation_delta_floats
                    = Utility::SIMDRandomFloats(subshape_count, MUTATION_MAGNITUDE);
                current_weights = best_weights;

                const std::vector<float>& multiplied_floats = Utility::SIMDMultiply(random_mutation_delta_floats, random_mutation_frequency_floats);
                const std::vector<float>& added_floats = Utility::SIMDAdd(current_weights, multiplied_floats);
                current_weights = added_floats;

                // Establish accuracy

                std::vector<float> board_accuracies;
                for (auto& board : original_boards) {
                    float score = 0.f;
                    for (auto& subshape_index : board.subshape_indicies)
                        score += current_weights[subshape_index];
                    board_accuracies.push_back(calculateAccuracy(score, board.m_known_evaluation_score));
                }

                float round_accuracy = 0.f;
                for (auto& accuracy : board_accuracies)
                    round_accuracy += accuracy;
                round_accuracy /= original_boards.size();

                if (round_accuracy > best_accuracy) {
                    best_accuracy = round_accuracy;
                    best_weights = current_weights;
                }

                if (k % 100 == 0)
                    std::cout << "Round accuracy: " << round_accuracy * 100 << "%" << std::endl;
            }

            std::cout << "Model accuracy: " << best_accuracy * 100 << "%" << std::endl;
            testModelOnBoard(best_weights, original_boards[4]);
            //testModelOnHiddenBoards(best_weights, 20);
            //for (auto& map_shape : subshape_map)
                //std::cout << "Subshape score: " << best_weights[map_shape.second] << std::endl;
        }
    };

    static void init() {
        srand(time(NULL));
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