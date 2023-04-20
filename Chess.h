
#include <cstdint>
#include <algorithm>

#include "Geometric Decomposition.h"

namespace Chess {

    class OriginalBoard {
    public:
        static constexpr size_t number_of_decomposable_shapes[GeometricDecomposition::Shape::Type::TYPE_COUNT] = { 0, 1962 };
        
    private:
        OriginalBoard(const std::vector<char> data) {
            GeometricDecomposition::Shape shape{
                GeometricDecomposition::Shape::Type::RECTANGLE,
                GeometricDecomposition::Shape::Dimensions{ 8, 8 },
                GeometricDecomposition::Shape::Offset{ 0, 0 },
                data
            };

            subshapes.reserve(number_of_decomposable_shapes[type]);
            subshapes = GeometricDecomposition::all(shape);
        }

        GeometricDecomposition::Shape::Type type;
        GeometricDecomposition::ShapeList subshapes;
        std::vector<char> data;
        float known_evaluation_score;
    };

    class EvaluationModel {
    public:
        static std::vector<OriginalBoard> original_boards;
        static std::vector<std::vector<float>> subshape_scores;
        static float highest_accuracy;
         
        static void initOriginalBoards() {

        }

        void init() {
            for (unsigned int i = 0; i < original_boards.size(); i++)
                m_subshape_scores.push_back(0.f);
            m_mutation_rate = 0.f;
            m_accuracy = 0.f;
        }

        EvaluationModel(const float mutation_rate, const float mutation_magnitude)
            : m_mutation_rate(mutation_rate), m_mutation_magnitude(mutation_magnitude) {}


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

    private:
        float m_mutation_rate;
        float m_mutation_magnitude;
        float m_accuracy;
    };

    std::vector<OriginalBoard> original_boards = OriginalBoard::initOriginalBoards();
    std::vector<std::vector<float>> subshape_scores = OriginalBoard::initSubshapes();
    float highest_accuracy = 0.f;
};