// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com

#include <iostream>
#include <random>
#include <chrono>
#include <vector>
#include <algorithm>
#include <string>
#include <fstream>
#include <vector>
#include <optional>

#include "..\utils\mnist\mnist.h"
#include "..\utils\progress_bar.h"
#include "..\ml\perceptron.h"

constexpr char train_images[] = "D:\\train_images.idx";
constexpr char train_labels[] = "D:\\train_labels.idx";

constexpr char test_images[] = "D:\\test_images.idx";
constexpr char test_labels[] = "D:\\test_labels.idx";

constexpr char save_path[] = "D:\\model.bin";

int main(int argc, char* argv[])
{
    ml::perceptron neural_network({ 784, 150, 10 }, 0.2f);
    ml::perceptron load_network;

    auto mnist_training_set = ml::mnist::load_mnist_db(train_images, train_labels);
    auto mnist_test_set = ml::mnist::load_mnist_db(test_images, test_labels);

    if (mnist_training_set)
    {
        size_t training_set_size = mnist_training_set->size();
        progress_bar p_bar(training_set_size, 70);

        std::cout << "network training:" << std::endl;

        for (size_t step = 0; step < training_set_size; ++step)
        {
            std::vector<float> target_values(10, 0.01f);

            size_t index = static_cast<size_t>((*mnist_training_set)[step].first);
            target_values[index] = 0.99f;

            size_t train_size = (*mnist_training_set)[step].second.size();
            std::vector<float> train_image(train_size);

            for (size_t pix = 0; pix < train_size; ++pix)
            {
                auto color = static_cast<unsigned char>((*mnist_training_set)[step].second[pix]);
                train_image[pix] = (color / 255.f) * 0.99f + 0.01f;
            }

            neural_network.train(train_image, target_values);

            ++p_bar;
            p_bar.display();
        }

        neural_network.save(save_path);
        p_bar.done();
    }
    else
    {
        std::cout << "failed to load data\n" << std::flush;
    }

    if (mnist_test_set)
    {
        load_network.load(save_path);

        size_t right_answers = 0;
        size_t total_images = mnist_test_set->size();

        progress_bar p_bar(total_images, 70);

        std::cout << "network testing:" << std::endl;

        for (size_t step = 0; step < total_images; ++step)
        {
            std::vector<float> target_values(10, 0.01f);

            size_t label = (*mnist_test_set)[step].first;

            size_t test_size = (*mnist_test_set)[step].second.size();
            std::vector<float> test_image(test_size);

            for (size_t pix = 0; pix < test_size; ++pix)
            {
                auto color = static_cast<unsigned char>((*mnist_test_set)[step].second[pix]);
                test_image[pix] = (color / 255.f) * 0.99f + 0.01f;
            }

            auto answer = load_network.forward(test_image);

            auto result = std::max_element(answer.begin(), answer.end());
            auto predict_label = static_cast<size_t>(std::distance(answer.begin(), result));

            if (predict_label == label) ++right_answers;

            ++p_bar;
            p_bar.display();
        }

        p_bar.done();

        float quality = (static_cast<float>(right_answers) / total_images) * 100.f;
        std::cout << "recognition quality: " << quality << "%" << std::endl;
    }
    else
    {
        std::cout << "failed to load data\n" << std::flush;
    }

    system("pause");
    return 0;
}