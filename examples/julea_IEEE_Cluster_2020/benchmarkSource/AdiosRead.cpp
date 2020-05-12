/*
 * Distributed under the OSI-approved Apache License, Version 2.0.  See
 * accompanying file Copyright.txt for details.
 *
 * An application demonstrating some of the query possibilities enabled by the
 * JULEA database engine.
 *
 *  Created on: May 06, 2020
 *      Author: Kira Duwe duwe@informatik.uni-hamburg.de
 */
#include <adios2.h>
#include <iomanip>
#include <iostream>
#include <vector>

// #include "AdiosQuery.h"

void AdiosReadMinMax(std::string fileName, std::string variableName)
{
    std::cout << "AdiosReadMinMax" << std::endl;
}

void AdiosRead(std::string engineName, std::string directory, size_t fileCount,
               uint32_t percentageVarsToRead)
{
    std::cout << "AdiosRead" << std::endl;

    // is directory? is file?
    // get all files
    // read first fileCount
    // read variables but only percentage
    // std::string fileName;
    std::string fileName = "TESTsresa1b_ncar_ccsm3-example.bp";
    size_t steps = 0;
    size_t stepsStart = 0;
    size_t varCount = 0;
    std::string varName;

    adios2::ADIOS adios(adios2::DebugON);
    adios2::IO io = adios.DeclareIO("Output");
    io.SetEngine(engineName);

    adios2::Engine reader = io.Open(fileName, adios2::Mode::Read);
    auto varMap = io.AvailableVariables();
            reader.BeginStep(adios2::StepMode::Read);
    for (const auto &var : varMap)
    {
        varName = var.first;
        adios2::Params params = var.second;
        std::cout << "\nvarName: " << varName << std::endl;
        auto type = io.VariableType(varName);

        std::vector<float> test(128);
        if (type == "compound")
        {
        }
#define declare_type(T)                                                        \
    else if (type == adios2::GetType<T>())                                     \
    {                                                                          \
        std::cout << "type: " << type << std::endl;                            \
        auto variable = io.InquireVariable<T>(varName);                        \
        adios2::Dims shape = variable.Shape();                                 \
        adios2::Dims start = variable.Start();                                 \
        adios2::Dims count = variable.Count();                                 \
        std::cout << "shape size: " << shape.size() << std::endl;              \
        std::cout << "start size: " << start.size() << std::endl;              \
        std::cout << "count size: " << count.size() << std::endl;              \
        steps = variable.Steps();                                              \
        std::cout << "steps: " << steps << std::endl;                          \
        for (size_t step = 0; step < steps; step++)                            \
        {                                                                      \
            stepsStart = variable.StepsStart();                                \
            auto blocksInfo = reader.BlocksInfo(variable, step);               \
            std::cout << "number of blocks = " << blocksInfo.size()            \
                      << std::endl;                                            \
        std::cout << "shape size: " << shape.size() << std::endl;              \
        std::cout << "start size: " << start.size() << std::endl;              \
        std::cout << "count size: " << count.size() << std::endl;              \
            std::vector<std::vector<T>> dataSet;                               \
            dataSet.resize(blocksInfo.size());                                 \
            std::vector<T> dataEntry;                                          \
            std::vector<T> dataEntry2;                                         \
            std::cout << "sizeof(dataSet): " << sizeof(dataSet) << std::endl;  \
            size_t i = 0;                                                      \
            for (auto &info : blocksInfo)                                      \
            {                                                                  \
                std::cout << "\ni: " << i << std::endl;                        \
                variable.SetBlockSelection(info.BlockID);                      \
                std::cout << "blockID: " << variable.BlockID() << std::endl;   \
                if (shape.size() == 0)                                         \
                {                                                              \
                    std::cout << "-- IF --" << std::endl;                      \
                    std::cout << "start front" << start.front() << std::endl;\
                    std::cout << "start " << start[0] << std::endl;\
                    std::cout << "start " << start[1] << std::endl;\
                    std::cout << "count front" << count.front() << std::endl;\
                    std::cout << "count " << count[0] << std::endl;\
                    std::cout << "count " << count[1] << std::endl;\
                    dataEntry.resize(count.size());                            \
                    std::cout << "resize worked" << std::endl;                 \
                    reader.Get<T>(variable, dataSet[i], adios2::Mode::Sync);   \
                    std::cout << "get worked" << std::endl;                    \
                    reader.Get<T>(variable, dataEntry.data(),                  \
                                  adios2::Mode::Sync);                         \
                }                                                              \
                else if (shape.size() == 1)                                    \
                {                                                              \
                    std::cout << "-- ELSE IF --" << std::endl;                 \
                    dataEntry.resize(shape[step]);                             \
                    std::cout <<"shape: " << shape[0] << std::endl;\
                    std::cout << "size dataEntry: " << dataEntry.size()        \
                              << std::endl;                                    \
                    reader.Get<T>(variable, dataEntry.data(),                  \
                                  adios2::Mode::Sync);                         \
                }                                                              \
                else                                                           \
                {                                                              \
                    std::cout <<"shape: " << shape[0] << std::endl;\
                    std::cout <<"shape: " << shape[1] << std::endl;\
                    std::cout << "shape front: " << shape.front()              \
                              << std::endl;                                    \
                    std::cout << "start front: " << start.front()              \
                              << std::endl;                                    \
                    std::cout << "count front: " << count.front()              \
                              << std::endl;                                    \
                    reader.Get<T>(variable, dataSet[i], adios2::Mode::Sync);   \
                    std::cout << "size: " << dataSet.size() << std::endl;      \
                    std::cout << "size: " << dataSet[i].size() << std::endl;   \
                }                                                              \
                std::cout << "reached END" << std::endl;                       \
                ++i;                                                           \
            }                                                                  \
        }                                                                      \
    }
        ADIOS2_FOREACH_STDTYPE_1ARG(declare_type)
#undef declare_type
        varCount++;
        std::cout << "------------------------- \n" << std::endl;\
    }
            reader.PerformGets();
            reader.EndStep();
}
// std::cout << "front: " << dataSet[i].front() << std::endl;     \