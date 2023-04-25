
#include <cstdint>
#include <algorithm>
#include <array>
#include <iostream>
#include <numeric>
#include <random>
#include <unordered_set>

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
            subshape_indicies.reserve(subshapes.size());
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

        static std::vector<Shape>& mergeVectorsWithoutDuplicates(const std::vector<Shape>& vec1, const std::vector<Shape>& vec2) {
            std::vector<Shape> merged_shapes;
            std::vector<Shape> other_vec;

            if (vec1.size() > vec2.size()) {
                merged_shapes = vec1;
                other_vec = vec2;
            }
            else {
                merged_shapes = vec2;
                other_vec = vec1;
            }

            for (unsigned int i = 0; i < other_vec.size(); i++) {
                bool already_exists = false;
                for (unsigned int j = 0; j < merged_shapes.size(); j++)
                    if (merged_shapes[j] == other_vec[i]) {
                        already_exists = false;
                        break;
                    }
                if (!already_exists)
                    merged_shapes.push_back(other_vec[i]);
            }

            return merged_shapes;
        }

        static void train() {
            constexpr float MUTATION_FREQUENCY = 0.005f;
            constexpr float MUTATION_MAGNITUDE = 4.f;
            constexpr size_t MUTATION_ROUNDS = 200;
            constexpr size_t ORIGINAL_BOARD_SAMPLE_SIZE = 5;

            // Prepare indexes for original board samples
            std::vector<size_t> original_board_sample_indexes;
            original_board_sample_indexes.reserve(ORIGINAL_BOARD_SAMPLE_SIZE);
            for (unsigned int i = 0; i < MUTATION_ROUNDS; i++)
                for (unsigned int j = 0; j < ORIGINAL_BOARD_SAMPLE_SIZE; j++)
                    original_board_sample_indexes.emplace_back(rand() % original_boards.size());

            // Prepare subshapes for training
            std::unordered_map<Shape, size_t> subshape_map;
            std::vector<Shape> subshapes;
            for (unsigned int i = 0; i < ORIGINAL_BOARD_SAMPLE_SIZE; i++) {
                const std::vector<Shape> tmp_subshapes = original_boards[i].subshapes;
                for (unsigned int j = 0; j < tmp_subshapes.size(); j++) {
                    const size_t index = subshape_map.size();
                    original_boards[i].subshape_indicies.emplace_back(index);
                    subshape_map[original_boards[i].subshapes[j]] = index;
                }
            }

            // Calculate subshape indices for original board samples
            for (unsigned int i = 0; i < ORIGINAL_BOARD_SAMPLE_SIZE; i++) {
                OriginalBoard& board = original_boards[original_board_sample_indexes[i]];
                board.subshape_indicies.reserve(board.subshapes.size());
                for (unsigned int j = 0; j < board.subshapes.size(); j++) {
                    auto it = subshape_map.find(board.subshapes[j]);
                    if (it != subshape_map.end()) {
                        const size_t shape_index = it->second;
                        board.subshape_indicies.emplace_back(shape_index);
                    }
                    // handle case where subshape is not found in the map
                    // you can choose to ignore, skip, or raise an error based on your requirement
                }
            }

            // Initialize highest scoring weights
            std::vector<float> highest_scoring_weights(subshape_map.size(), 0.f);
            float highest_accuracy = 0.f;

            // Perform mutation rounds
            for (unsigned int k = 0; k < MUTATION_ROUNDS; k++) {
                // Reset weights to highest scoring and mutate
                std::vector<float> current_scoring_weights = highest_scoring_weights;
                for (auto& current_weight : current_scoring_weights)
                    current_weight += mutationTriggered(MUTATION_FREQUENCY) * mutationDelta(MUTATION_MAGNITUDE);

                // Calculate average accuracy on all boards
                float average_accuracy_on_all_boards = 0.f;
                for (unsigned int i = 0; i < ORIGINAL_BOARD_SAMPLE_SIZE; i++) {
                    float score_on_current_board = 0.f;
                    const OriginalBoard& current_board = original_boards[original_board_sample_indexes[i]];
                    const size_t subshape_count = current_board.subshapes.size();
                    for (size_t index : current_board.subshape_indicies) {
                        score_on_current_board += current_scoring_weights[index];
                    }
                    const float accuracy_on_current_board = calculateAccuracy(score_on_current_board, current_board.m_known_evaluation_score);
                    average_accuracy_on_all_boards += accuracy_on_current_board;
                }
                average_accuracy_on_all_boards /= ORIGINAL_BOARD_SAMPLE_SIZE;

                // Update highest accuracy and scoring weights if necessary
                if (average_accuracy_on_all_boards > highest_accuracy) {
                    highest_accuracy = average_accuracy_on_all_boards;
                        highest_scoring_weights = current_scoring_weights;
                }

                //std::cout << "Current Generation Accuracy Was: " << average_accuracy_on_all_boards * 100 << "%" << std::endl;
            }

            std::cout << "Highest accuracy was: " << highest_accuracy * 100 << "%" << std::endl;

            for (auto& map_shape : subshape_map) {
                std::cout << "Subshape score: " << highest_scoring_weights[map_shape.second] << std::endl;
                map_shape.first.print();
            }
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