#pragma once
#include "OriginalBoard.hpp"
#include "Xoshiro.hpp"
#include "RadixTree.hpp"
#include "IO.hpp"
#include "Defs.hpp"

class EvaluationModel {
private:
    RadixTree m_shape_feature_tree;
    std::vector<OriginalBoard> m_original_boards;
    std::vector<ShapeFeature> m_shape_features;
    float m_mutation_frequency;
    float m_mutation_magnitude;
    float m_least_error;
    size_t m_mutation_rounds;
    size_t m_mapping_index;

public:


    EvaluationModel() :
        m_least_error(0.f),
        m_mapping_index(0),
        m_mutation_frequency(FREQUENCY_DEFAULT),
        m_mutation_magnitude(MAGNITUDE_DEFAULT),
        m_mutation_rounds(ROUNDS_DEFAULT)
    {}

    void insertShapeFeature(const std::string& serialized_shape_feature) {
        m_shape_feature_tree.insert(serialized_shape_feature, m_mapping_index);
        m_shape_features.emplace_back(serialized_shape_feature, m_mapping_index);
        board.subDataShape_indicies.push_back(m_mapping_index);
    }

    void mapShapeFeatureToTreeIndex(OriginalBoard& board, const DataShape& shape) {
        size_t mapping_index = 0;
        const std::string& sub_data_shape_as_string = shape.asString();
        const auto& [exists, existing_index] = m_subDataShape_tree.search(sub_data_shape_as_string);
        if (exists) {
            board.subDataShape_indicies.push_back(existing_index);
        }
        else {
            insertSubDataShapeTreeIndexPair(sub_data_shape_as_string);
        }
    }

    void mapAllSubDataShapesToTreeIndex() {
        for (auto& board : m_original_boards)
            for (auto& shape : board.subDataShapes)
                mapSubDataShapeToTreeIndex(board, shape);
        board.subDataShapes.clear();
        m_subDataShape_count = m_mapping_index;
    }

    static float calculateError(float actual_result, float expected_result) {
        return std::fabs(actual_result - expected_result) 
            / Chess::ORIGINAL_BOARD_SAMPLE_SIZE;
    }

    float calculateScore(const OriginalBoard& board, 
        const std::vector<float>& weights) {

        float score = 0.0;
        for (unsigned int j = 0; j < board.subDataShape_indicies.size(); j++)
            score += weights[board.subDataShape_indicies[j]];
        return score;
    }

    float calculateAllErrors(const std::vector<OriginalBoard>& original_boards, 
        const std::vector<float>& current_weights) {

        float all_errors = 0.0;
        for (unsigned int i = 0; i < original_boards.size(); i++) {
            const OriginalBoard& board = original_boards[i];
            float score = calculateScore(board, current_weights);
            float expected_score = calculateExpectedScore(board.m_known_evaluation_score);
            all_errors += calculateError(score, expected_score);
        }
        return all_errors;
    }

    float calculateExpectedScore(float known_evaluation_score) {
        if (known_evaluation_score > 0.f)
            return std::min(known_evaluation_score, EVALUATION_MAX);
        return std::max(known_evaluation_score, EVALUATION_MIN);
    }

    float calculateError(float score, float expected_score) {
        return std::abs(score - expected_score);
    }

    void performWeightMutationsAndSetIfBest() {
        m_current_weights = m_least_error_weights;
        for (unsigned int i = 0; i < m_current_weights.size(); i++)
            m_current_weights[i] += (2 * rng() - 1) * (rng() < m_mutation_frequency);
        if (testError() < m_least_error) {
            m_least_error = testError();
            m_least_error_weights = m_current_weights;
        }
    }

    std::vector<std::string> reverseMap() {
        std::vector<std::string> reverse_map(m_subDataShape_count);
        for (unsigned int i = 0; i < m_subDataShape_count; i++)
            reverse_map[m_subDataShapes[i].second] = m_subDataShapes[i].first;
        return reverse_map;
    }

    void train() {
        for (unsigned int k = 0; k < m_mutation_rounds; k++)
            performWeightMutationsAndSetIfBest();
        Chess::IO::Print::model();
    }
};