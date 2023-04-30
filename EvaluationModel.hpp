#pragma once
#include "OriginalBoard.hpp"
#include "Xoshiro.hpp"

class EvaluationModel {
private:
    static constexpr float MUTATION_FREQUENCY = 0.1f;
    static constexpr float MUTATION_MAGNITUDE = 1.f;
    static constexpr size_t MUTATION_ROUNDS = 200000;
    static constexpr size_t ORIGINAL_BOARD_SAMPLE_SIZE = 500; // Standard benchmark = 100

    std::vector<float> m_best_weights;
    std::vector<float> m_current_weights;
    float m_best_accuracy;
    float m_round_accuracy;
    Xoshiro256ss m_rng;

    std::unordered_map<DataShape, size_t> m_subDataShape_map;
    std::vector<std::pair<DataShape, size_t>> m_subDataShapes;
    size_t m_subDataShape_count;

public:
    std::vector<OriginalBoard> m_original_boards;

    EvaluationModel(uint32_t rng_seed) :
        m_rng(rng_seed),
        m_best_accuracy(0.f),
        m_round_accuracy(0.f),
        m_subDataShape_count(0)
    {}

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

    static const std::vector<float> mapOfParentWeights(const std::vector<float>& weights, const OriginalBoard& board) {
        std::vector<float> resolved_weights;
        for (auto& subDataShape_index : board.subDataShape_indicies)
            resolved_weights.push_back(weights[subDataShape_index]);
        return resolved_weights;
    }

    static void testModelOnBoard(const std::vector<float>& weights, const OriginalBoard& board) {
        const std::vector<float>& these_weights = mapOfParentWeights(weights, board);
        float score = 0.f;
        for (auto& subDataShape_index : board.subDataShape_indicies)
            score += weights[subDataShape_index];
        std::cout << "Model's accuracy against this board was: " <<
            calculateAccuracy(score, board.m_known_evaluation_score) * 100 << "%" << std::endl;
    }

    void testModelOnHiddenBoards(const std::vector<float>& weights, const size_t hidden_board_count) {
        for (unsigned int i = 0; i < hidden_board_count; i++)
            testModelOnBoard(weights, m_original_boards[m_original_boards.size() - hidden_board_count + i]);
    }

    void prepareSubDataShapes() {
        size_t index = 0;
        for (unsigned int i = 0; i < m_original_boards.size(); i++) {
            for (unsigned int j = 0; j < m_original_boards[i].subDataShapes.size(); j++) {
                auto subDataShape_inserted = m_subDataShape_map.insert(std::make_pair(m_original_boards[i].subDataShapes[j], index));
                if (subDataShape_inserted.second)
                    m_original_boards[i].subDataShape_indicies[j] = index++;
                else
                    m_original_boards[i].subDataShape_indicies[j] = subDataShape_inserted.first->second;
                m_subDataShapes.push_back(std::make_pair(m_original_boards[i].subDataShapes[j], index));
            }
            m_original_boards[i].subDataShapes.clear();
        }
        m_subDataShape_count = index;
    }

    void prepareWeights() {
        m_best_weights.assign(m_subDataShape_count, 0.f);
        m_current_weights.assign(m_subDataShape_count, 0.f);
        m_best_accuracy = 0.f;
    }

    void shuffleOriginalBoards() {
        Utility::shuffleVector(m_original_boards);
    }

    void performWeightMutationsAndSetIfBest() {
        // Copy best weights to current weights
        m_current_weights = m_best_weights;

        // Mutate current weights
        for (unsigned int i = 0; i < m_subDataShape_count; i++) {
            float mutation = (2 * m_rng() - 1) * MUTATION_MAGNITUDE;
            if (m_rng() < MUTATION_FREQUENCY) {
                m_current_weights[i] += mutation;
            }
        }

        // Calculate board accuracies
        std::vector<float> board_accuracies;
        for (const auto& board : m_original_boards) {
            float score = 0.f;
            for (const auto& index : board.subDataShape_indicies) {
                score += m_current_weights[index];
            }
            board_accuracies.push_back(calculateAccuracy(score, board.m_known_evaluation_score));
        }

        // Calculate round accuracy
        m_round_accuracy = std::accumulate(board_accuracies.begin(),
            board_accuracies.begin() + ORIGINAL_BOARD_SAMPLE_SIZE,
            0.f) / ORIGINAL_BOARD_SAMPLE_SIZE;

        // Update best accuracy and weights if current round is better
        if (m_round_accuracy > m_best_accuracy) {
            m_best_accuracy = m_round_accuracy;
            m_best_weights = m_current_weights;
        }
    }

    void print() {
        std::cout << "Model accuracy: " << m_best_accuracy * 100 << "%" << std::endl;

        std::vector<std::pair<DataShape, float>> subDataShape_scores;
        for (const auto& subDataShape_pair : m_subDataShapes) {
            subDataShape_scores.emplace_back(subDataShape_pair.first, m_best_weights[subDataShape_pair.second]);
        }

        std::sort(subDataShape_scores.begin(), subDataShape_scores.end(),
            [](const auto& a, const auto& b) { return a.second > b.second; });

        for (const auto& subDataShape_score_pair : subDataShape_scores) {
            std::cout << "SubDataShape Score: " << subDataShape_score_pair.second << std::endl;
            subDataShape_score_pair.first.print();
        }
    }

    void train() {
        //shuffleOriginalBoards();
        prepareSubDataShapes();
        prepareWeights();

        for (unsigned int k = 0; k < MUTATION_ROUNDS; k++) {
            performWeightMutationsAndSetIfBest();

            if (k % 1000 == 0) // Don't spam the console
                std::cout << "Round accuracy: " << m_round_accuracy * 100 << "%" << std::endl;
        }
        print();
    }
};