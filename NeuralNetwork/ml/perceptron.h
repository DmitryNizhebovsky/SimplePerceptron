#pragma once

#include <vector>
#include <stack>
#include <random>
#include <math.h>
#include <initializer_list>

#include "..\math\matrix.h"
#include "..\math\functions.h"

namespace ml
{
    class perceptron
    {
    public:
        perceptron() {}

        explicit perceptron(std::initializer_list<size_t> list, float learning_rate = 0.3)
        {
            this->learning_rate = learning_rate;

            for (auto it = list.begin(); it != list.end() - 1; ++it)
            {
                layers.emplace_back(math::matrix<float>(*(it + 1), *it));
            }

            weight_initialization();
        }

        void train(const std::vector<float>& input_values, const std::vector<float>& target_values)
        {
            math::matrix<float> output(input_values.size(), 1, input_values);
            math::matrix<float> target(target_values.size(), 1, target_values);

            std::vector<math::matrix<float>> outputs;

            outputs.push_back(output);

            for (auto& layer : layers)
            {
                auto layer_outputs = layer * output;

                std::for_each(layer_outputs.begin(), layer_outputs.end(),
                    [](float& item) { item = function::sigmoid_function(item); });

                outputs.push_back(layer_outputs);
                output = layer_outputs;
            }

            auto errors = target - output;

            for (size_t iter = layers.size(); iter >= 1; --iter)
            {
                layers[iter - 1] += learning_rate * math::elem_mult(math::elem_mult(errors, outputs[iter]), 1.0 - outputs[iter]) * outputs[iter - 1].transposed();
                errors = layers[iter - 1].transposed() * errors;
            }
        }

        math::matrix<float> forward(const std::vector<float>& input_values)
        {
            math::matrix<float> input(input_values.size(), 1, input_values);

            for (auto& layer : layers)
            {
                auto layer_outputs = layer * input;

                std::for_each(layer_outputs.begin(), layer_outputs.end(), 
                    [](float& item) { item = function::sigmoid_function(item); });

                input = std::move(layer_outputs);
            }

            return input;
        }

    private:
        void weight_initialization()
        {
            std::random_device rd{};
            std::mt19937 gen{ rd() };

            for (auto& layer : layers)
            {
                float std_dev = powf(static_cast<float>(layer.size()), -0.5f);

                std::normal_distribution<float> normal_distribution{ 0.f, std_dev };

                std::for_each(layer.begin(), layer.end(), 
                    [&normal_distribution, &gen](float& item) { item = normal_distribution(gen); });
            }
        }

    private:
        std::vector<math::matrix<float>> layers;
        float learning_rate;
    };
}