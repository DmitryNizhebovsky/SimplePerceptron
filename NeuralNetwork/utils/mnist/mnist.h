#pragma once

#include <chrono>
#include <vector>
#include <fstream>
#include <optional>

#include "..\logger.h"
#include "..\binary.h"

namespace ml
{
    namespace mnist
    {
        using byte = char;
        using training_set = std::vector<std::pair<byte, std::vector<byte>>>;

        std::optional<training_set> load_mnist_db(const std::string& image_file, const std::string& label_file)
        {
            std::ifstream images_in(image_file, std::ios::in | std::ios::binary);
            std::ifstream labels_in(label_file, std::ios::in | std::ios::binary);

            if (images_in.is_open())
            {
                int magic_number = read_data_swap_endian<int>(images_in);
                if (magic_number != 2051)
                {
                    ml::utils::Logger::Error("mnist", "incorrect image file magic: " + std::to_string(magic_number));
                    return {};
                }

                ml::utils::Logger::Info("mnist", "loading images file...");
            }
            else
            {
                ml::utils::Logger::Error("mnist", "could not open file: " + image_file);
                return {};
            }

            if (labels_in.is_open())
            {
                int magic_number = read_data_swap_endian<int>(labels_in);
                if (magic_number != 2049)
                {
                    ml::utils::Logger::Error("mnist", "incorrect label file magic: " + std::to_string(magic_number));
                    return {};
                }

                ml::utils::Logger::Info("mnist", "loading labels file...");
            }
            else
            {
                ml::utils::Logger::Error("mnist", "could not open file: " + label_file);
                return {};
            }

            int magic_number = 0;
            int image_number = 0;
            int label_number = 0;
            int image_width = 0;
            int image_height = 0;
            int sequence_length = 0;

            image_number = read_data_swap_endian<int>(images_in);
            label_number = read_data_swap_endian<int>(labels_in);

            if (image_number < 0)
            {
                ml::utils::Logger::Error("mnist", "number of images must be positive\n");
            }

            if (label_number < 0)
            {
                ml::utils::Logger::Error("mnist", "number of labels must be positive\n");
            }

            if (image_number != label_number)
            {
                ml::utils::Logger::Error("mnist", "number of images and labels must match\n");
            }

            image_height = read_data_swap_endian<int>(images_in);
            image_width = read_data_swap_endian<int>(images_in);
            sequence_length = image_height * image_width;

            std::vector<byte> pixels;
            pixels.reserve(sequence_length);
            pixels.resize(sequence_length);

            training_set set;
            set.reserve(label_number);

            ml::utils::Logger::Info("mnist", "loading data...");

            auto start = std::chrono::high_resolution_clock::now();

            for (size_t i = 0; i < static_cast<size_t>(label_number); ++i)
            {
                byte label = read_data<byte>(labels_in);

                images_in.read(pixels.data(), sequence_length);

                set.emplace_back(label, pixels);
            }

            auto stop = std::chrono::high_resolution_clock::now();
            auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(stop - start);

            ml::utils::Logger::Info("mnist", "data loaded successfully");
            ml::utils::Logger::Info("mnist", "loaded items: " + std::to_string(label_number));
            ml::utils::Logger::Info("mnist", "time to load mnist sets: " + std::to_string(duration.count()) + " ms");

            images_in.close();
            labels_in.close();

            return std::optional<training_set>(std::move(set));
        }
    }
}