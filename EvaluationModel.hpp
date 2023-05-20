#pragma once
#include "Xoshiro.hpp"
#include "RadixTree.hpp"
#include "IO.hpp"
#include "Defs.hpp"

class EvaluationModel {
private:
    static constexpr size_t ERROR_MAX = std::numeric_limits<size_t>::max();
    static constexpr float ERROR_MAX_FLOAT = std::numeric_limits<float>::max();
    static constexpr float WEIGHT_DEFAULT = 0.f;
    static constexpr float EVALUATION_MIN = -1000.f;
    static constexpr float EVALUATION_MAX = 1000.f;
    static constexpr float FREQUENCY_DEFAULT = 0.0107f;
    static constexpr float MAGNITUDE_DEFAULT = 0.5f;
    static constexpr size_t ROUNDS_DEFAULT = 10000;

    RadixTree m_shape_feature_tree
        [GeometricProperties::TYPE_COUNT]
        [Chess::BoardProperties::CHESS_BOARD_WIDTH]
        [Chess::BoardProperties::CHESS_BOARD_HEIGHT]
        [Chess::BoardProperties::CHESS_BOARD_WIDTH]
        [Chess::BoardProperties::CHESS_BOARD_HEIGHT];

    std::vector<std::vector<size_t>> m_containing_parents_map;
    std::vector<size_t> m_shape_feature_index_in_tree;
    std::vector<float> m_expected_scores;
    std::vector<float> m_best_weights;
    std::vector<float> m_current_weights;

    float m_mutation_frequency;
    float m_mutation_magnitude;
    float m_least_error;
    size_t m_mutation_rounds;
    size_t m_mapping_index;

public:
    EvaluationModel() :
        m_least_error(ERROR_MAX_FLOAT),
        m_mapping_index(0),
        m_mutation_frequency(FREQUENCY_DEFAULT),
        m_mutation_magnitude(MAGNITUDE_DEFAULT),
        m_mutation_rounds(ROUNDS_DEFAULT)
    {}

    void loadBestWeights(const std::string& file_name) {
        const std::pair<std::vector<float>, bool>& weights_success
            = IO::readFloatVectorFromFile(file_name);
        if (weights_success.second)
            if (weights_success.first.size() == m_best_weights.size())
                m_best_weights = weights_success.first;
    }

    void addParentShapeFeature(const ShapeFeature& parent_shape_feature) {
        m_containing_parents_map.push_back(std::vector<size_t>(0));
        const std::vector<ShapeFeature>& sub_shape_features
            = parent_shape_feature.decomposeIntoSubquadrillaterals();
        m_expected_scores.push_back(parent_shape_feature.weight());
        for (unsigned int i = 0; i < sub_shape_features.size(); i++)
            insertShapeFeature(m_expected_scores.size() - 1, sub_shape_features[i]);
    }

    RadixTree& resolveShapeFeatureTreeWithHeader(const std::string& header) {
        const size_t type_index = Utility::charToDigit(header[0]);
        const size_t width_index = Utility::charToDigit(header[1]) - 1;
        const size_t height_index = Utility::charToDigit(header[2]) - 1;
        const size_t offset_x_index = Utility::charToDigit(header[3]);
        const size_t offset_y_index = Utility::charToDigit(header[4]);
        return m_shape_feature_tree[type_index][width_index][height_index][offset_x_index][offset_y_index];
    }

    void insertShapeFeature(const size_t parent_shape_index, const ShapeFeature& shape_feature) {
        const std::string& serialized = shape_feature.serialized();
        RadixTree& shape_feature_tree = resolveShapeFeatureTreeWithHeader(serialized.substr(0, LENGTH));
        shape_feature_tree.insert(serialized.substr(LENGTH), m_mapping_index);
        m_shape_feature_index_in_tree.push_back(m_mapping_index);
        m_containing_parents_map[parent_shape_index].push_back(m_mapping_index);
        m_best_weights.push_back(0.f);
        m_current_weights.push_back(0.f);
        m_mapping_index++;
    }

    void mapShapeFeatureToTreeIndex(const size_t parent_shape_index, 
        const ShapeFeature& shape_feature) {
        const std::string& serialized = shape_feature.serialized();
        const auto& [exists, existing_index] 
            = m_shape_feature_tree
            [Utility::charToDigit(serialized[0])]
            [Utility::charToDigit(serialized[1]) - 1]
            [Utility::charToDigit(serialized[2]) - 1]
            [Utility::charToDigit(serialized[3])]
            [Utility::charToDigit(serialized[4])]
            .search(serialized.substr(LENGTH));

        if (!exists)
            insertShapeFeature(parent_shape_index, serialized);
        else
            m_containing_parents_map[parent_shape_index].push_back(existing_index);
    }

    static float calculateError(float actual_result, float expected_result) {
        return std::fabs(actual_result - expected_result);
    }

    float calculateScore(const size_t parent_index) {
        float total_score = 0.0;
            for (unsigned int i = 0; i < m_containing_parents_map[parent_index].size(); i++)
                total_score += m_current_weights[m_containing_parents_map[parent_index][i]];
        return total_score;
    }

    float calculateAllErrors() {
        float all_errors = 0.0;
        for (unsigned int i = 0; i < m_containing_parents_map.size(); i++)
            all_errors += calculateError(calculateScore(i), 
                calculateExpectedScore(i));
        return all_errors;
    }

    float calculateExpectedScore(const size_t parent_index) {
        const float known_evaluation_score 
            = m_expected_scores[parent_index];
        if (known_evaluation_score > 0.f)
            return std::min(known_evaluation_score, EVALUATION_MAX);
        return std::max(known_evaluation_score, EVALUATION_MIN);
    }

    float performWeightMutationsAndSetIfBest() {
        m_current_weights = m_best_weights;
        for (unsigned int i = 0; i < m_current_weights.size(); i++) {
            float mutation = (2 * rng() - 1) * (rng() < m_mutation_frequency) * m_mutation_magnitude;
            m_current_weights[i] += mutation;
        }
        const float current_error = calculateAllErrors();
        if (current_error < m_least_error) {
            m_least_error = current_error;
            m_best_weights = m_current_weights;
        }
        return current_error;
    }

    float scoreHiddenParentShapeFeature(const ShapeFeature& hidden_parent_shape_feature) {
        const std::vector<ShapeFeature>& hidden_shape_features
            = hidden_parent_shape_feature.decomposeIntoSubquadrillaterals();
        float score = 0.f;
        for (auto& hidden_shape_feature : hidden_shape_features) {
            const std::string& serialized = hidden_shape_feature.serialized();
            const auto& [exists, existing_index]
                = m_shape_feature_tree
                [Utility::charToDigit(serialized[0])]
            [Utility::charToDigit(serialized[1]) - 1]
            [Utility::charToDigit(serialized[2]) - 1]
            [Utility::charToDigit(serialized[3])]
            [Utility::charToDigit(serialized[4])]
            .search(serialized.substr(LENGTH));

            if (exists)
                score += m_best_weights[existing_index];
        }
        return score;
    }

    void train() {
        loadBestWeights("BestWeights.txt");
        for (unsigned int k = 0; k < m_mutation_rounds; k++) {
            performWeightMutationsAndSetIfBest();
            if (k % static_cast<size_t>((m_mutation_rounds / 100)) == 0) {
                const ShapeFeature hidden_parent_shape_feature{
                    Chess::BoardProperties::CHESS_BOARD_PROPERTIES,
                    FEN::positionStringToCharSequence("r1b1kbnr/n1q1pppp/pp1p4/2pP4/2P1PP2/2NBBN2/PP4PP/R2QK2R")
                };

                std::cout << "Hidden Board Score: " 
                    << scoreHiddenParentShapeFeature(hidden_parent_shape_feature) << std::endl;
                std::cout << "Least Error: " 
                    << (m_least_error) << std::endl;
            }
        }
        IO::writeFloatVectorToFile(m_best_weights, "BestWeights.txt");

        
    }
};