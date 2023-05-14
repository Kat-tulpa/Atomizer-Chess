#pragma once
#include "Xoshiro.hpp"
#include "RadixTree.hpp"
#include "IO.hpp"
#include "Defs.hpp"

class EvaluationModel {
private:
    static constexpr size_t ERROR_MAX = std::numeric_limits<size_t>::max();
    static constexpr float WEIGHT_DEFAULT = 0.f;
    static constexpr float EVALUATION_MIN = -500.f;
    static constexpr float EVALUATION_MAX = 500.f;
    static constexpr float FREQUENCY_DEFAULT = 0.0107f;
    static constexpr float MAGNITUDE_DEFAULT = 0.5f;
    static constexpr size_t ROUNDS_DEFAULT = 100000;

    RadixTree m_shape_feature_tree;
    std::vector<std::vector<size_t>> m_containing_parents_map;
    std::vector<ShapeFeature> m_parent_shape_features;
    std::vector<ShapeFeature> m_shape_features;
    std::vector<size_t> m_shape_feature_index_in_tree;
    std::vector<float> m_best_weights;
    std::vector<float> m_current_weights;

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

    void addParentShapeFeature(const ShapeFeature& parent_shape_feature) {
        m_parent_shape_features.push_back(parent_shape_feature);
        m_containing_parents_map.push_back(std::vector<size_t>(0));
    }

    const auto getAllShapeFeatureWeights() const {
        std::vector<float> all_shape_feature_weights;
        for (auto& shape_feature : m_shape_features)
            all_shape_feature_weights.push_back(shape_feature.weight());
        return all_shape_feature_weights;
    }

    void updateAllShapeFeatureWeightsToBestWeights() {
        for (unsigned int i = 0; i < m_best_weights.size(); i++)
            m_shape_features[i].weight(m_best_weights[i]);
    }

    void insertShapeFeature(const size_t parent_shape_index, 
        const std::string& serialized_shape_feature) {
        m_shape_feature_tree.insert(serialized_shape_feature, m_mapping_index);
        m_shape_features.emplace_back(ShapeFeature(serialized_shape_feature));
        m_shape_feature_index_in_tree.push_back(m_mapping_index);
        m_containing_parents_map[parent_shape_index].push_back(m_mapping_index);
        m_best_weights.push_back(0.f);
        m_current_weights.push_back(0.f);
    }

    void mapShapeFeatureToTreeIndex(const size_t parent_shape_index, 
        const ShapeFeature& shape_feature) {
        const std::string& serialized_shape_feature = shape_feature.serialized();
        const auto& [exists, existing_index] 
            = m_shape_feature_tree.search(serialized_shape_feature);
        if (!exists)
            insertShapeFeature(parent_shape_index, serialized_shape_feature);
        else
            m_containing_parents_map[parent_shape_index].push_back(existing_index);
    }

    static float calculateError(float actual_result, float expected_result) {
        return std::fabs(actual_result - expected_result) 
            / Chess::ORIGINAL_BOARD_SAMPLE_SIZE;
    }

    float calculateScore(const size_t parent_index) {
        float totalScore = 0.0;
        const auto& parent_indicies = m_containing_parents_map[parent_index];
        for (const auto& parentIndex : parent_indicies) {
            const auto& shape_feature = m_shape_features[parent_index];
            totalScore += shape_feature.weight();
        }
        return totalScore;
    }

    float calculateAllErrors() {
        float all_errors = 0.0;
        for (unsigned int i = 0; i < m_containing_parents_map.size(); i++) {
            float score = calculateScore(i);
            float expected_score = calculateExpectedScore(i);
            all_errors += calculateError(score, expected_score);
        }
        return all_errors;
    }

    float calculateExpectedScore(const size_t parent_index) {
        const float known_evaluation_score 
            = m_parent_shape_features[parent_index].weight();
        if (known_evaluation_score > 0.f)
            return std::min(known_evaluation_score, EVALUATION_MAX);
        return std::max(known_evaluation_score, EVALUATION_MIN);
    }

    void performWeightMutationsAndSetIfBest() {
        m_current_weights = m_best_weights;
        for (unsigned int i = 0; i < m_current_weights.size(); i++)
            m_current_weights[i] += (2 * rng() - 1) * (rng() < m_mutation_frequency);
        const float current_error = calculateAllErrors();
        if (current_error < m_least_error) {
            m_least_error = current_error;
            m_best_weights = m_current_weights;
        }
    }

    void train() {
        for (unsigned int k = 0; k < m_mutation_rounds; k++)
            performWeightMutationsAndSetIfBest();
        updateAllShapeFeatureWeightsToBestWeights();
        Chess::IO::Print::modelError(m_least_error);
    }
};