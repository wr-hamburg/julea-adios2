/*
 * Distributed under the OSI-approved Apache License, Version 2.0.  See
 * accompanying file Copyright.txt for details.
 *
 * JULEA engine using the JULEA storage framework to handle lower I/O.
 *
 *  Created on: Jul 26, 2019
 *      Author: Kira Duwe duwe@informatik.uni-hamburg.de
 */

#ifndef ADIOS2_ENGINE_JULEADBREADER_TCC_
#define ADIOS2_ENGINE_JULEADBREADER_TCC_

#include "JuleaDBReader.h"

#include <iostream>
// #include <fstream>
// #include <string>

namespace adios2
{
namespace core
{
namespace engine
{

// TODO: check whether this is sufficient for strings!
// is separate data stored which needs to be read or just metadata?
template <>
void JuleaDBReader::GetSyncCommon(Variable<std::string> &variable,
                                  std::string *data)
{
    if (m_Verbosity == 5)
    {
        std::cout << "\n______________GetSync String_____________________"
                  << std::endl;
        std::cout << "Julea DB Reader " << m_ReaderRank
                  << " Namespace: " << m_Name
                  << " Variable name: " << variable.m_Name << std::endl;
    }

    for (typename Variable<std::string>::Info &blockInfo :
         variable.m_BlocksInfo)
    {
        long unsigned int dataSize = 0;
        guint32 buffer_len = 0;
        std::string nameSpace = m_Name;
        std::string stepBlockID;
        gpointer md_buffer = nullptr;

        if (m_UseKeysForBPLS)
        {
            stepBlockID = g_strdup_printf("%lu_%lu", variable.m_StepsStart,
                                          variable.m_BlockID);
            // std::cout << "variable.m... stepBlockID: " << stepBlockID <<
            // std::endl;
        }
        else
        {
            stepBlockID =
                g_strdup_printf("%lu_%lu", m_CurrentStep, m_CurrentBlockID);
            // std::cout << "m_Current... stepBlockID: " << stepBlockID <<
            // std::endl;
        }
        // std::cout << "blocksInfos.size: " << variable.m_BlocksInfo.size()
        // << std::endl;

        // GetBlockMetadataFromJulea(nameSpace, variable.m_Name, &md_buffer,
        // &buffer_len, stepBlockID);
        // DeserializeBlockMetadata(variable, md_buffer, variable.m_BlockID,
        // blockInfo);

        if (variable.m_SingleValue)
        {
            std::cout << "Single value" << std::endl;
            return;
        }
        m_CurrentBlockID++;
    }
}

template <class T>
void JuleaDBReader::GetSyncCommon(Variable<T> &variable, T *data)
{
    if (m_Verbosity == 5)
    {
        std::cout << "\n______________GetSync T_____________________"
                  << std::endl;
        std::cout << "Julea DB Reader " << m_ReaderRank
                  << " Reached Get Sync Common (T, T)" << std::endl;
        std::cout << "Julea Reader " << m_ReaderRank << " Namespace: " << m_Name
                  << std::endl;
        std::cout << "Julea Reader " << m_ReaderRank
                  << " Variable name: " << variable.m_Name << std::endl;
    }

    if (variable.m_SingleValue)
    {
        // FIXME: just read metadata from DB. value is stored in there.
        // m_BP3Deserializer.GetValueFromMetadata(variable, data);
        // return;
    }

    InitVariableBlockInfo(variable, data);
    ReadVariableBlocks(variable);
    variable.m_BlocksInfo.pop_back();
}

template <class T>
void JuleaDBReader::GetDeferredCommon(Variable<T> &variable, T *data)
{
    if (m_Verbosity == 5)
    {
        std::cout << "Julea DB Reader " << m_ReaderRank << "     GetDeferred("
                  << variable.m_Name << ")\n";
    }

    // returns immediately without populating data
    InitVariableBlockInfo(variable, data); // TODO: needed?
    m_DeferredVariables.insert(variable.m_Name);
}

template <class T>
void JuleaDBReader::ReadVariableBlocks(Variable<T> &variable)
{
    if (m_Verbosity == 5)
    {
        std::cout << "\n__________ReadVariableBlocks_____________" << std::endl;
        std::cout << "Julea DB Reader " << m_ReaderRank
                  << " Namespace: " << m_Name
                  << " Variable name: " << variable.m_Name << std::endl;
    }

    for (typename Variable<T>::Info &blockInfo : variable.m_BlocksInfo)
    {
        std::cout << "blocksInfos.size: " << variable.m_BlocksInfo.size()
        << std::endl;
        long unsigned int dataSize = 0;
        guint32 buffer_len = 0;
        std::string nameSpace = m_Name;
        std::string stepBlockID;
        gpointer md_buffer = nullptr;
        size_t block = 0;
        size_t step = 0;

        if (m_UseKeysForBPLS)
        {
            stepBlockID = g_strdup_printf("%lu_%lu", variable.m_StepsStart,
                                          variable.m_BlockID);
            step = variable.m_StepsStart;
            block = variable.m_BlockID;
            std::cout << "variable.m... stepBlockID: " << stepBlockID <<
            std::endl;
        }
        else
        {
            stepBlockID =
                g_strdup_printf("%lu_%lu", m_CurrentStep, m_CurrentBlockID);
                step = m_CurrentStep;
                block = m_CurrentBlockID;
            std::cout << "m_Current... stepBlockID: " << stepBlockID <<
            std::endl;
        }

        // GetBlockMetadataFromJulea(nameSpace, variable.m_Name, &md_buffer,
        // &buffer_len, stepBlockID);
        // DeserializeBlockMetadata(variable, md_buffer, variable.m_BlockID,
        // blockInfo);
        DBGetBlockMetadata(variable,nameSpace,step, block, blockInfo);
        variable.m_BlocksInfo[0] = blockInfo;

        if (variable.m_SingleValue)
        {
            std::cout << "Single value" << std::endl;
            return;
        }

        size_t numberElements = helper::GetTotalSize(blockInfo.Count);
        dataSize = numberElements * variable.m_ElementSize;

        T *data = blockInfo.Data;
        if (m_UseKeysForBPLS)
        {
            // GetVariableDataFromJulea(variable, data, nameSpace, dataSize,
            // variable.m_StepsStart, variable.m_BlockID);
        }
        else
        {
            // GetVariableDataFromJulea(variable, data, nameSpace, dataSize,
            // m_CurrentStep, m_CurrentBlockID);
        }
        m_CurrentBlockID++;
    }
}

template <class T>
std::map<size_t, std::vector<typename core::Variable<T>::Info>>
JuleaDBReader::AllStepsBlocksInfo(const core::Variable<T> &variable) const
{
    std::map<size_t, std::vector<typename core::Variable<T>::Info>>
        allStepsBlocksInfo;

    // Explanation for this ugly assumption is in the header file. This assumes
    // that only bpls calls AllStepsBlocksInfo. for now that should be ok now
    // = 21.04.2020
    m_UseKeysForBPLS = true;
    // SetUseKeysForBPLS(true);
    for (const auto &pair : variable.m_AvailableStepBlockIndexOffsets)
    {
        const size_t step = pair.first;
        const std::vector<size_t> &blockPositions = pair.second;

        if (m_Verbosity == 5)
        {
            std::cout << "\n__________AllStepsBlocksInfo_____________"
                      << std::endl;
            std::cout << "Julea DB Reader " << m_ReaderRank
                      << " Namespace: " << m_Name
                      << " Variable name: " << variable.m_Name << std::endl;
            std::cout << "--- step: " << step
                      << " blockPositions: " << blockPositions.data()[0]
                      << std::endl;
        }

        // bp3 index starts at 1
        allStepsBlocksInfo[step - 1] =
            BlocksInfoCommon(variable, blockPositions, step - 1);
    }
    return allStepsBlocksInfo;
}

template <class T>
std::vector<typename core::Variable<T>::Info>
JuleaDBReader::BlocksInfoCommon(const core::Variable<T> &variable,
                                const std::vector<size_t> &blocksIndexOffsets,
                                size_t step) const
{
    if (m_Verbosity == 5)
    {
        std::cout << "\n__________BlocksInfoCommon_____________" << std::endl;
        std::cout << "Julea DB Reader " << m_ReaderRank
                  << " Namespace: " << m_Name
                  << " Variable name: " << variable.m_Name << std::endl;
        std::cout << "--- step: " << step << std::endl;
    }
    std::vector<typename core::Variable<T>::Info> blocksInfo;
    blocksInfo.reserve(blocksIndexOffsets.size());
    typename core::Variable<T>::Info blockInfo;

    for (size_t i = 0; i < blocksIndexOffsets.size(); i++)
    {
        guint32 buffer_len = 0;
        gpointer md_buffer = nullptr;

        auto nameSpace = m_Name;
        long unsigned int dataSize = 0;
        auto stepBlockID = g_strdup_printf("%lu_%lu", step, i);

        // GetBlockMetadataFromJulea(nameSpace, variable.m_Name, &md_buffer,
        // &buffer_len, stepBlockID);

        typename core::Variable<T>::Info info;
        // typename core::Variable<T>::Info info =
        // *GetDeserializedMetadata(variable, md_buffer);
        //FIXME: cannot pass const variable
         // DBGetBlockMetadata(variable,nameSpace,step, i, &info);
        info.IsReverseDims = false;
        info.Step = step;

        blocksInfo.push_back(info);
    }
    return blocksInfo;
}

// FIXME: not yet tested!
template <class T>
std::vector<std::vector<typename core::Variable<T>::Info>>
JuleaDBReader::AllRelativeStepsBlocksInfo(
    const core::Variable<T> &variable) const
{
    if (m_Verbosity == 5)
    {
        std::cout << "\n__________AllRelativeStepsBlocksInfo_____________"
                  << std::endl;
        std::cout << "Julea DB Reader " << m_ReaderRank
                  << " Namespace: " << m_Name
                  << " Variable name: " << variable.m_Name << std::endl;
    }
    std::vector<std::vector<typename core::Variable<T>::Info>>
        allRelativeStepsBlocksInfo(
            variable.m_AvailableStepBlockIndexOffsets.size());

    size_t relativeStep = 0;
    for (const auto &pair : variable.m_AvailableStepBlockIndexOffsets)
    {
        const std::vector<size_t> &blockPositions = pair.second;
        allRelativeStepsBlocksInfo[relativeStep] =
            BlocksInfoCommon(variable, blockPositions, relativeStep);
        ++relativeStep;
    }
    return allRelativeStepsBlocksInfo;
}

template <class T>
std::vector<typename core::Variable<T>::Info>
JuleaDBReader::BlocksInfo(const core::Variable<T> &variable,
                          const size_t step) const
{
    if (m_Verbosity == 5)
    {
        std::cout << "\n__________BlocksInfo_____________" << std::endl;
        std::cout << "Julea DB Reader " << m_ReaderRank
                  << " Namespace: " << m_Name
                  << " Variable name: " << variable.m_Name << std::endl;
    }

    // bp4 format starts at 1
    auto itStep = variable.m_AvailableStepBlockIndexOffsets.find(step + 1);
    if (itStep == variable.m_AvailableStepBlockIndexOffsets.end())
    {
        return std::vector<typename core::Variable<T>::Info>();
        std::cout << " --- step not found in m_AvailableStepBlockIndexOffsets "
                  << std::endl;
    }
    return BlocksInfoCommon(variable, itStep->second, step);
}

template <class T>
typename core::Variable<T>::Info &
JuleaDBReader::InitVariableBlockInfo(core::Variable<T> &variable, T *data)
{
    if (m_Verbosity == 5)
    {
        std::cout << "\n__________InitVariableBlockInfo_____________"
                  << std::endl;
        std::cout << "Julea DB Reader " << m_ReaderRank
                  << " Namespace: " << m_Name
                  << " Variable name: " << variable.m_Name << std::endl;
    }
    const size_t stepsStart = variable.m_StepsStart;
    const size_t stepsCount = variable.m_StepsCount;

    // if (m_DebugMode)
    if (m_Verbosity == 5)
    {
        const auto &indices = variable.m_AvailableStepBlockIndexOffsets;
        const size_t maxStep = indices.rbegin()->first;
        if (stepsStart + 1 > maxStep)
        {
            throw std::invalid_argument(
                "ERROR: steps start " + std::to_string(stepsStart) +
                " from SetStepsSelection or BeginStep is larger than "
                "the maximum available step " +
                std::to_string(maxStep - 1) + " for variable " +
                variable.m_Name + ", in call to Get\n");
        }

        auto itStep = std::next(indices.begin(), stepsStart);

        for (size_t i = 0; i < stepsCount; ++i)
        {
            if (itStep == indices.end())
            {
                throw std::invalid_argument(
                    "ERROR: offset " + std::to_string(i) +
                    " from steps start " + std::to_string(stepsStart) +
                    " in variable " + variable.m_Name +
                    " is beyond the largest available step = " +
                    std::to_string(maxStep - 1) +
                    ", check Variable SetStepSelection argument stepsCount "
                    "(random access), or "
                    "number of BeginStep calls (streaming), in call to Get");
            }
            ++itStep;
        }
    }

    if (variable.m_SelectionType == SelectionType::WriteBlock)
    {
        const std::vector<typename core::Variable<T>::Info> blocksInfo =
            JuleaDBReader::BlocksInfo(variable, stepsStart);

        if (variable.m_BlockID >= blocksInfo.size())
        {
            throw std::invalid_argument(
                "ERROR: invalid blockID " + std::to_string(variable.m_BlockID) +
                " from steps start " + std::to_string(stepsStart) +
                " in variable " + variable.m_Name +
                ", check argument to Variable<T>::SetBlockID, in call "
                "to Get\n");
        }

        // switch to bounding box for global array
        if (variable.m_ShapeID == ShapeID::GlobalArray)
        {
            std::cout
            << "----------- DEBUG: switch to bounding box for global array "
            << std::endl;
            const Dims &start = blocksInfo[variable.m_BlockID].Start;
            const Dims &count = blocksInfo[variable.m_BlockID].Count;

            variable.SetSelection({start, count});
        }
        else if (variable.m_ShapeID == ShapeID::LocalArray)
        {
            std::cout
            << "----------- DEBUG: switch to bounding box for local array "
            << std::endl;

            // TODO from Adios people! "keep Count for block updated"
            variable.m_Count = blocksInfo[variable.m_BlockID].Count;
        }
    }

    return variable.SetBlockInfo(data, stepsStart, stepsCount);
}

} // end namespace engine
} // end namespace core
} // end namespace adios2

#endif // ADIOS2_ENGINE_JULEADBREADER_TCC_
