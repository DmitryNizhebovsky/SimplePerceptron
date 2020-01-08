#pragma once

#include <vector>
#include <stack>
#include <random>
#include <math.h>
#include <initializer_list>

#include "..\math\matrix.h"
#include "..\math\functions.h"
#include "..\utils\binary.h"

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

        void save(const std::string& fileName)
        {
            std::ofstream outFile(fileName, std::ios::binary | std::ios::out);

            if (!outFile.is_open())
            {
                std::cout << "file " << fileName << " can't to open\n";
                return;
            }

            write_data(learning_rate, outFile);
            write_data(layers.size(), outFile);

            for (const auto& layer : layers)
            {
                write_data(layer.size_m(), outFile);
                write_data(layer.size_n(), outFile);

                for (auto it = layer.cbegin(); it != layer.cend(); ++it)
                {
                    write_data(*it, outFile);
                }
            }

            outFile.close();
        }

        void load(const std::string& fileName)
        {
            std::ifstream inFile(fileName, std::ios::binary | std::ios::in);

            if (!inFile.is_open())
            {
                std::cout << "file " << fileName << " can't to open\n";
                return;
            }

            layers.clear();

            learning_rate = read_data<float>(inFile);
            const size_t layersNum = read_data<size_t>(inFile);
            size_t size_m = 0;
            size_t size_n = 0;
            std::vector<float> data;

            for (size_t layerNum = 0; layerNum < layersNum; ++layerNum)
            {
                size_m = read_data<size_t>(inFile);
                size_n = read_data<size_t>(inFile);
                const size_t data_length = size_m * size_n;

                data.clear();
                data.shrink_to_fit();
                data.reserve(data_length);
                data.resize(data_length);

                inFile.read(reinterpret_cast<char*>(data.data()), data_length);
            }

            layers.emplace_back(size_m, size_n, data);
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