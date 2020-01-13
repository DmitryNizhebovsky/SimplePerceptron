#pragma once

#include <msclr\marshal_cppstd.h>
#include <string>
#include <vector>
#include "ManagedObject.h"
#include "Pair.h"
#include "..\NeuralNetwork\ml\perceptron.h"

using namespace System;
using namespace System::Collections::Generic;

namespace MlWrapper
{
    public ref class Perceptron : public ManagedObject<ml::perceptron>
    {
    public:
        Perceptron();

        Pair<float, int>^ Forward(List<float>^ input);

        void Load(String^ pathToModel);

    private:
        static std::string ManagedStrToUnmanagedStr(String^ managedStr);

        static std::vector<float> ListToVector(List<float>^ list);
    };
}