
#include <cstdint>
#include <algorithm>
#include <array>

#include "Geometric Decomposition.h"
#include "ShapeMap.h"
#include "FEN.h"
#include "CSV.h"

class Chess {

    static constexpr const char CSV_POSITION_EVALUATION_FILE_NAME[]{ "chessData.csv" };
    static constexpr const char CSV_DELIMITERS[] { ' ', ','};
 
    static CSV csv_manager;

    class OriginalBoard {
    public:
       
        OriginalBoard(std::vector<char> data) {
            Shape m_shape{
                Shape::Type::RECTANGLE,
                Shape::Dimensions{ 8, 8 },
                Shape::Offset{ 0, 0 },
                data
            };

            ShapeMap::insert(GeometricDecomposition::all(m_shape));
        }

        Shape::Type m_type;
        Shape m_shape;
        std::vector<char> m_data;
        float m_known_evaluation_score;
    };

    static std::vector<OriginalBoard> original_boards;

    class EvaluationModel {

        static void train() {
            constexpr float MUTATION_FREQUENCY = 0.1f;
            constexpr float MUTATION_MAGNITUDE = 0.1f;
            constexpr size_t MUTATION_ROUNDS = 75;
            constexpr size_t ORIGINAL_BOARD_SAMPLE_SIZE = 75;
            float highest_accuracy = 0.f;

            const std::vector<std::pair<Shape, float>>& subshape_weights_pair = ShapeMap::getAll();
            std::vector<float> highest_scoring_weights;
            for (const auto& pair : subshape_weights_pair)
                highest_scoring_weights.push_back(pair.second);



            for (unsigned int k = 0; k < MUTATION_ROUNDS; k++) {
                std::vector<float> current_scoring_weights = highest_scoring_weights;
                for (unsigned int l = 0; l < current_scoring_weights.size(); l++)


                float current_score = 0.f;
                float average_accuracy = 0.f;
                for (unsigned int i = 0; i < ORIGINAL_BOARD_SAMPLE_SIZE; i++) {
                    for (unsigned int j = 0; j < subshape_weights_pair.size(); j++)
                        if (original_boards[i].m_shape.containsSubshape(subshape_weights_pair[j].first))
                            current_score += subshape_weights_pair[j].second;
                    float current_accuracy = current_score / original_boards[i].m_known_evaluation_score;
                    average_accuracy += current_accuracy;
                }

                average_accuracy /= highest_scoring_weights.size();
                if (average_accuracy > highest_accuracy) {
                    highest_accuracy = average_accuracy;

                }
            }
                for (unsigned int j = 0; j < )

           

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
        }
    };

    static void init() {
        csv_manager = CSV(CSV_POSITION_EVALUATION_FILE_NAME);
        const size_t csv_line_count = csv_manager.getLineCount();
        original_boards.reserve(csv_line_count);

        while (!csv_manager.getFileStream().std::ios::eof()) {
            const std::vector<std::string>& delimitedLine = csv_manager.readLineDelimited(CSV_DELIMITERS, 6);
            const std::vector<char>& FEN_part = Utility::stringToVector(delimitedLine[0]);
            const std::vector<char>& evaluation_score_string = FEN::evaluationScoreStringToFloat(delimitedLine[6]);
            OriginalBoard board{ FEN_part, ev };
            board.m_known_evaluation_score;
            original_boards.emplace_back(board);
        }
    }
};