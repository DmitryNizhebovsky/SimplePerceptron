#include "Perceptron.h"
#include <algorithm>

namespace MlWrapper
{
    Perceptron::Perceptron() : ManagedObject(new ml::perceptron())
    {}

    void Perceptron::Load(String^ pathToModel)
    {
        auto filePath = ManagedStrToUnmanagedStr(pathToModel);
        m_Instance->load(filePath);
    }

    Pair<float, int>^ Perceptron::Forward(List<float>^ input)
    {
        auto vector = ListToVector(input);
        auto out = m_Instance->forward(vector);
        auto result = std::max_element(out.begin(), out.end());
        auto predict_label = static_cast<int>(std::distance(out.begin(), result));
        
        return gcnew Pair<float, int>(*result, predict_label);
    }

    std::string Perceptron::ManagedStrToUnmanagedStr(String^ managedStr)
    {
        return msclr::interop::marshal_as<std::string>(managedStr);
    }

    std::vector<float> Perceptron::ListToVector(List<float>^ list)
    {
        std::vector<float> temp;

        for each(float value in list)
        {
            temp.push_back(value);
        }

        return temp;
    }
}