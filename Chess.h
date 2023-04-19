/*
About Chess.h
    Intention is to provide various utility functions and data structures related to chess, such as
    - Parsing FEN std::strings
    - Representing chess pieces
    - Loading FEN std::strings and evaluations from a CSV file into a SequenceManager
 */

 /*
     Here is a summary of the main components of the code:
 */

#include <cstdint>
#include <algorithm>

#include "Geometric Decomposition.h"

namespace Chess {

    class EvaluationModel {
        static GeometricDecomposition::ShapeList& parent_board_shapes;
        
        std::vector<std::pair<const Shape, float>> m_subshape_score;
        float m_mutation_rate;
        float m_highest_accuracy;

        // Creates while initalizing all weights to 0
        EvaluationModel() {
            for (unsigned int i = 0; i < parent_board_shapes.size(); i++)
                m_subshape_score.push_back(std::make_pair(shape_list[i], 0.f));
        }



        static const ShapeList& fromSequenceManagerData(std::vector<std::pair<std::string, uint8_t>> sequence_manager_datas) {
            const size_t sequence_manager_data_count = sequence_manager_datas.size();
            ShapeList shape_list = ShapeList(sequence_manager_data_count);
            for (unsigned int i = 0; i < sequence_manager_data_count; i++)
                shape_list.emplace_back(Shape(sequence_manager_datas[i].first));
            return shape_list;
        }

        void train() {
            const size_t original_boards_sequence_manager_registration_ID = reigsterNewSequenceManagerID();
            const ShapeList& original_board_shapes = fromSequenceManagerData(SequenceManager::getAll(original_boards_sequence_manager_registration_ID));
            const size_t original_boards_count = original_board_shapes.size();
            const size_t subshape_count = m_subshape_score.size();

            for (unsigned int i = 0; i < subshape_count; i++)
                for (unsigned int j = 0; j < original_boards_count; j++)
                    if (original_board_shapes[j].containsSubshape(subshapes[i]))
                        subshapes_scores[j] += evaluation_scores[i];

            for (unsigned int i = 0; i < subshape_count; i++)
                subshape_scores[i] /= subshapes.size();

            // Test and print accuracy
            static constexpr size_t RANDOM_TEST_POSITION_SAMPLE_COUNT = 75;
            static constexpr size_t EVOLUTION_ROUNDS = 75;
            static constexpr float MUTATION_MAGNITUDE = 0.1f;
            static constexpr float MUTATION_RATE = 0.1f;
            float best_accuracy = 0.f;

            for (unsigned int i = 0; i < RANDOM_TEST_POSITION_SAMPLE_COUNT; i++) {
                const size_t random_board_index = rand() % original_boards_count;
                float predicted_evaluation_score = 0.f;

                for (unsigned int j = 0; j < subshape_count; j++)
                    if (original_board_shapes[random_board_index].containsSubshape(subshapes[j]))
                        predicted_evaluation_score += subshape_scores[j];

            }

            // for all subshapes in the model
                // for all 8x8 original boards in the sequence manager
                    // check if subshape exists inside 8x8 original board
                        // add score if the eval score for the original 8x8 board was positive
                        // subtract score if the eval score for the original 8x8 board was negative    
        }
    };
};