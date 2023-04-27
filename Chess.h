
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
#include "RandomDevice.h"
#include "Xoshiro256ss.h"

Xoshiro256ss rng(12345);

class Chess {
public:
    static constexpr const char* const CSV_POSITION_EVALUATION_FILE_NAME = "chessData.csv";
    static constexpr const char CSV_DELIMITERS[] { ' ', ','};
    static CSV csv_manager;

    class OriginalBoard {
    public:

        Shape::Type m_type;
        Shape m_shape;
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

            if (expected_result != 0.0)
                accuracy = 1.0 - (error / expected_result);
            else
                // Assign high accuracy when actual_result is close to 0, and low accuracy otherwise
                accuracy = (actual_result <= 0.0) ? 1.0 : 0.0; 

            // Ensure accuracy is within the valid range of 0.0 to 1.0
            accuracy = fmax(0.0, fmin(1.0, accuracy));

            return accuracy;
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
            std::cout << "Model's accuracy against this board was: " << 
                calculateAccuracy(score, board.m_known_evaluation_score) * 100 << "%" << std::endl;
        }

        static void testModelOnHiddenBoards(const std::vector<float>& weights, const size_t hidden_board_count) {
            for (unsigned int i = 0; i < hidden_board_count; i++)
                testModelOnBoard(weights, original_boards[original_boards.size() - hidden_board_count + i]);
        }

        void prepareSubshapes() {
            size_t index = 0;
            for (unsigned int i = 0; i < original_boards.size(); i++) {
                for (unsigned int j = 0; j < original_boards[i].subshapes.size(); j++) {
                    auto subshape_inserted = m_subshape_map.insert(std::make_pair(original_boards[i].subshapes[j], index));
                    if (subshape_inserted.second)
                        original_boards[i].subshape_indicies[j] = index++;
                    else
                        original_boards[i].subshape_indicies[j] = subshape_inserted.first->second;
                    m_subshapes.push_back(std::make_pair(original_boards[i].subshapes[j], index));
                }
                original_boards[i].subshapes.clear();
            }
            m_subshape_count = index;
        }

        void prepareWeights() {
            m_best_weights.assign(m_subshape_count, 0.f);
            m_current_weights.assign(m_subshape_count, 0.f);
            m_best_accuracy = 0.f;
        }

        static void shuffleOriginalBoards() {
            Utility::shuffleVector(original_boards);
        }

        void performWeightMutations() {
            m_current_weights = m_best_weights;

            std::random_device rd;
            std::mt19937 gen(rd());
            std::uniform_real_distribution<float> dis(-1.f, 1.f);

            // Apply mutations to current weights
            for (unsigned int i = 0; i < m_subshape_count; i++) {
                m_current_weights[i] += dis(gen) * MUTATION_MAGNITUDE * ((dis(gen) + 1) / 2 < MUTATION_FREQUENCY);
            }
        }

        void updateBestWeights(const std::vector<float>& board_accuracies) {
            m_round_accuracy = 0.f;
            for (unsigned int i = 0; i < ORIGINAL_BOARD_SAMPLE_SIZE; i++)
                m_round_accuracy += board_accuracies[i];
            m_round_accuracy /= ORIGINAL_BOARD_SAMPLE_SIZE;
            
            if (m_round_accuracy > m_best_accuracy) {
                m_best_accuracy = m_round_accuracy;
                m_best_weights = m_current_weights;
            }
        }

        const std::vector<float> establishWeightAccuraciesOnBoards() {
            std::vector<float> board_accuracies;
            for (unsigned int i = 0; i < original_boards.size(); i++) {
                float score = 0.f;
                for (unsigned int j = 0; j < original_boards[i].subshape_indicies.size(); j++) {
                    score += m_current_weights[original_boards[i].subshape_indicies[j]];
                }
                const float& accuracy = calculateAccuracy(score, original_boards[i].m_known_evaluation_score);
                board_accuracies.push_back(accuracy);
            }
            return board_accuracies;
        }
        
        void train() {
            //shuffleOriginalBoards();
            prepareSubshapes();
            prepareWeights();

            for (unsigned int k = 0; k < MUTATION_ROUNDS; k++) {
                performWeightMutations();
                const std::vector<float>& board_accuracies 
                    = establishWeightAccuraciesOnBoards();
                updateBestWeights(board_accuracies);

                if (k % 100 == 0) // Don't spam the console
                    std::cout << "Round accuracy: " << m_round_accuracy * 100 << "%" << std::endl;
            }

            std::cout << "Model accuracy: " << m_best_accuracy * 100 << "%" << std::endl;
            testModelOnBoard(m_best_weights, original_boards[4]);

            std::vector<std::pair<Shape, float>> subshape_scores;
            for (auto& subshape_pair : m_subshapes)
                subshape_scores.push_back(std::make_pair(subshape_pair.first, m_best_weights[subshape_pair.second]));
            std::sort(subshape_scores.begin(), subshape_scores.end(),
                [](const auto& a, const auto& b) { return a.second > b.second; });

            for (auto& subshape_score_pair : subshape_scores) {
                //if (subshape_score_pair.first.m_dimensions.m_height == 1 && subshape_score_pair.first.m_dimensions.m_width == 1) {
                    std::cout << "Subshape Score: " << subshape_score_pair.second << std::endl;
                    subshape_score_pair.first.print();
                //}
            }
        }

        private:
            static constexpr float MUTATION_FREQUENCY = 0.1f;
            static constexpr float MUTATION_MAGNITUDE = 1.f;
            static constexpr size_t MUTATION_ROUNDS = 20000;
            static constexpr size_t ORIGINAL_BOARD_SAMPLE_SIZE = 1000; // Standard benchmark = 100

            std::vector<float> m_best_weights;
            std::vector<float> m_current_weights;
            float m_best_accuracy;
            float m_round_accuracy;

            std::unordered_map<Shape, size_t> m_subshape_map;
            std::vector<std::pair<Shape, size_t>> m_subshapes;
            size_t m_subshape_count;
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
            original_boards.emplace_back(board);

            lines_processed++;
            if (lines_processed % csv_line_count == 0) 
                return;
        }
    }
};

CSV Chess::csv_manager = CSV(CSV_POSITION_EVALUATION_FILE_NAME);
std::vector<Chess::OriginalBoard> Chess::original_boards;