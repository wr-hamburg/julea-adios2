/*
 * Distributed under the OSI-approved Apache License, Version 2.0.  See
 * accompanying file Copyright.txt for details.
 */

#include "TestSscCommon.h"
#include <adios2.h>
#include <gtest/gtest.h>
#include <mpi.h>
#include <numeric>
#include <thread>

using namespace adios2;
int mpiRank = 0;
int mpiSize = 1;
MPI_Comm mpiComm;

class SscEngineTest : public ::testing::Test
{
public:
    SscEngineTest() = default;
};

void Writer(const Dims &shape, const Dims &start, const Dims &count,
            const size_t steps, const adios2::Params &engineParams,
            const std::string &name)
{
    size_t datasize = std::accumulate(count.begin(), count.end(), 1,
                                      std::multiplies<size_t>());
    adios2::ADIOS adios(mpiComm);
    adios2::IO dataManIO = adios.DeclareIO("WAN");
    dataManIO.SetEngine("ssc");
    dataManIO.SetParameters(engineParams);
    std::vector<char> myChars(datasize);
    std::vector<unsigned char> myUChars(datasize);
    std::vector<short> myShorts(datasize);
    std::vector<unsigned short> myUShorts(datasize);
    std::vector<int> myInts(datasize);
    std::vector<unsigned int> myUInts(datasize);
    std::vector<float> myFloats(datasize);
    std::vector<double> myDoubles(datasize);
    std::vector<std::complex<float>> myComplexes(datasize);
    std::vector<std::complex<double>> myDComplexes(datasize);
    auto bpChars =
        dataManIO.DefineVariable<char>("bpChars", shape, start, count);
    auto bpUChars = dataManIO.DefineVariable<unsigned char>("bpUChars", shape,
                                                            start, count);
    auto bpShorts =
        dataManIO.DefineVariable<short>("bpShorts", shape, start, count);
    auto bpUShorts = dataManIO.DefineVariable<unsigned short>(
        "bpUShorts", shape, start, count);
    auto bpInts = dataManIO.DefineVariable<int>("bpInts", shape, start, count);
    auto bpUInts =
        dataManIO.DefineVariable<unsigned int>("bpUInts", shape, start, count);
    auto bpFloats =
        dataManIO.DefineVariable<float>("bpFloats", shape, start, count);
    auto bpDoubles =
        dataManIO.DefineVariable<double>("bpDoubles", shape, start, count);
    auto bpComplexes = dataManIO.DefineVariable<std::complex<float>>(
        "bpComplexes", shape, start, count);
    auto bpDComplexes = dataManIO.DefineVariable<std::complex<double>>(
        "bpDComplexes", shape, start, count);
    auto scalarInt = dataManIO.DefineVariable<int>("scalarInt");
    dataManIO.DefineAttribute<int>("AttInt", 110);
    adios2::Engine dataManWriter = dataManIO.Open(name, adios2::Mode::Write);
    for (int i = 0; i < steps; ++i)
    {
        dataManWriter.BeginStep();

        Dims startTmp = {(size_t)mpiRank * 2, 0};

        GenData(myChars, i, startTmp, count, shape);
        GenData(myUChars, i, startTmp, count, shape);
        GenData(myShorts, i, startTmp, count, shape);
        GenData(myUShorts, i, startTmp, count, shape);
        GenData(myInts, i, startTmp, count, shape);
        GenData(myUInts, i, startTmp, count, shape);
        GenData(myFloats, i, startTmp, count, shape);
        GenData(myDoubles, i, startTmp, count, shape);
        GenData(myComplexes, i, startTmp, count, shape);
        GenData(myDComplexes, i, startTmp, count, shape);

        bpChars.SetSelection({startTmp, count});
        bpUChars.SetSelection({startTmp, count});
        bpShorts.SetSelection({startTmp, count});
        bpUShorts.SetSelection({startTmp, count});
        bpInts.SetSelection({startTmp, count});
        bpUInts.SetSelection({startTmp, count});
        bpFloats.SetSelection({startTmp, count});
        bpDoubles.SetSelection({startTmp, count});
        bpComplexes.SetSelection({startTmp, count});
        bpDComplexes.SetSelection({startTmp, count});

        dataManWriter.Put(bpChars, myChars.data(), adios2::Mode::Sync);
        dataManWriter.Put(bpUChars, myUChars.data(), adios2::Mode::Sync);
        dataManWriter.Put(bpShorts, myShorts.data(), adios2::Mode::Sync);
        dataManWriter.Put(bpUShorts, myUShorts.data(), adios2::Mode::Sync);
        dataManWriter.Put(bpInts, myInts.data(), adios2::Mode::Sync);
        dataManWriter.Put(bpUInts, myUInts.data(), adios2::Mode::Sync);
        dataManWriter.Put(bpFloats, myFloats.data(), adios2::Mode::Sync);
        dataManWriter.Put(bpDoubles, myDoubles.data(), adios2::Mode::Sync);
        dataManWriter.Put(bpComplexes, myComplexes.data(), adios2::Mode::Sync);
        dataManWriter.Put(bpDComplexes, myDComplexes.data(),
                          adios2::Mode::Sync);
        dataManWriter.Put(scalarInt, i);

        startTmp = {(size_t)mpiRank * 2 + 1, 0};

        GenData(myChars, i, startTmp, count, shape);
        GenData(myUChars, i, startTmp, count, shape);
        GenData(myShorts, i, startTmp, count, shape);
        GenData(myUShorts, i, startTmp, count, shape);
        GenData(myInts, i, startTmp, count, shape);
        GenData(myUInts, i, startTmp, count, shape);
        GenData(myFloats, i, startTmp, count, shape);
        GenData(myDoubles, i, startTmp, count, shape);
        GenData(myComplexes, i, startTmp, count, shape);
        GenData(myDComplexes, i, startTmp, count, shape);

        bpChars.SetSelection({startTmp, count});
        bpUChars.SetSelection({startTmp, count});
        bpShorts.SetSelection({startTmp, count});
        bpUShorts.SetSelection({startTmp, count});
        bpInts.SetSelection({startTmp, count});
        bpUInts.SetSelection({startTmp, count});
        bpFloats.SetSelection({startTmp, count});
        bpDoubles.SetSelection({startTmp, count});
        bpComplexes.SetSelection({startTmp, count});
        bpDComplexes.SetSelection({startTmp, count});

        dataManWriter.Put(bpChars, myChars.data(), adios2::Mode::Sync);
        dataManWriter.Put(bpUChars, myUChars.data(), adios2::Mode::Sync);
        dataManWriter.Put(bpShorts, myShorts.data(), adios2::Mode::Sync);
        dataManWriter.Put(bpUShorts, myUShorts.data(), adios2::Mode::Sync);
        dataManWriter.Put(bpInts, myInts.data(), adios2::Mode::Sync);
        dataManWriter.Put(bpUInts, myUInts.data(), adios2::Mode::Sync);
        dataManWriter.Put(bpFloats, myFloats.data(), adios2::Mode::Sync);
        dataManWriter.Put(bpDoubles, myDoubles.data(), adios2::Mode::Sync);
        dataManWriter.Put(bpComplexes, myComplexes.data(), adios2::Mode::Sync);
        dataManWriter.Put(bpDComplexes, myDComplexes.data(),
                          adios2::Mode::Sync);
        dataManWriter.Put(scalarInt, i);

        dataManWriter.EndStep();
    }
    dataManWriter.Close();
}

void Reader(const Dims &shape, const Dims &start, const Dims &count,
            const size_t steps, const adios2::Params &engineParams,
            const std::string &name)
{
    adios2::ADIOS adios(mpiComm);
    adios2::IO dataManIO = adios.DeclareIO("Test");
    dataManIO.SetEngine("ssc");
    dataManIO.SetParameters(engineParams);
    adios2::Engine dataManReader = dataManIO.Open(name, adios2::Mode::Read);

    size_t datasize = std::accumulate(shape.begin(), shape.end(), 1,
                                      std::multiplies<size_t>());
    std::vector<char> myChars(datasize);
    std::vector<unsigned char> myUChars(datasize);
    std::vector<short> myShorts(datasize);
    std::vector<unsigned short> myUShorts(datasize);
    std::vector<int> myInts(datasize);
    std::vector<unsigned int> myUInts(datasize);
    std::vector<float> myFloats(datasize);
    std::vector<double> myDoubles(datasize);
    std::vector<std::complex<float>> myComplexes(datasize);
    std::vector<std::complex<double>> myDComplexes(datasize);

    while (true)
    {
        adios2::StepStatus status = dataManReader.BeginStep(StepMode::Read, 5);
        if (status == adios2::StepStatus::OK)
        {
            const auto &vars = dataManIO.AvailableVariables();
            ASSERT_EQ(vars.size(), 11);
            size_t currentStep = dataManReader.CurrentStep();
            adios2::Variable<char> bpChars =
                dataManIO.InquireVariable<char>("bpChars");
            adios2::Variable<unsigned char> bpUChars =
                dataManIO.InquireVariable<unsigned char>("bpUChars");
            adios2::Variable<short> bpShorts =
                dataManIO.InquireVariable<short>("bpShorts");
            adios2::Variable<unsigned short> bpUShorts =
                dataManIO.InquireVariable<unsigned short>("bpUShorts");
            adios2::Variable<int> bpInts =
                dataManIO.InquireVariable<int>("bpInts");
            adios2::Variable<unsigned int> bpUInts =
                dataManIO.InquireVariable<unsigned int>("bpUInts");
            adios2::Variable<float> bpFloats =
                dataManIO.InquireVariable<float>("bpFloats");
            adios2::Variable<double> bpDoubles =
                dataManIO.InquireVariable<double>("bpDoubles");
            adios2::Variable<std::complex<float>> bpComplexes =
                dataManIO.InquireVariable<std::complex<float>>("bpComplexes");
            adios2::Variable<std::complex<double>> bpDComplexes =
                dataManIO.InquireVariable<std::complex<double>>("bpDComplexes");
            auto scalarInt = dataManIO.InquireVariable<int>("scalarInt");

            int i;
            dataManReader.Get(scalarInt, &i);
            ASSERT_EQ(i, currentStep);

            adios2::Dims startTmp = start;
            adios2::Dims countTmp = count;
            startTmp[1] = mpiRank * 2;
            countTmp[1] = 1;
            bpChars.SetSelection({startTmp, countTmp});
            bpUChars.SetSelection({startTmp, countTmp});
            bpShorts.SetSelection({startTmp, countTmp});
            bpUShorts.SetSelection({startTmp, countTmp});
            bpInts.SetSelection({startTmp, countTmp});
            bpUInts.SetSelection({startTmp, countTmp});
            bpFloats.SetSelection({startTmp, countTmp});
            bpDoubles.SetSelection({startTmp, countTmp});
            bpComplexes.SetSelection({startTmp, countTmp});
            bpDComplexes.SetSelection({startTmp, countTmp});
            dataManReader.Get(bpChars, myChars.data(), adios2::Mode::Sync);
            dataManReader.Get(bpUChars, myUChars.data(), adios2::Mode::Sync);
            dataManReader.Get(bpShorts, myShorts.data(), adios2::Mode::Sync);
            dataManReader.Get(bpUShorts, myUShorts.data(), adios2::Mode::Sync);
            dataManReader.Get(bpInts, myInts.data(), adios2::Mode::Sync);
            dataManReader.Get(bpUInts, myUInts.data(), adios2::Mode::Sync);
            dataManReader.Get(bpFloats, myFloats.data(), adios2::Mode::Sync);
            dataManReader.Get(bpDoubles, myDoubles.data(), adios2::Mode::Sync);
            dataManReader.Get(bpComplexes, myComplexes.data(),
                              adios2::Mode::Sync);
            dataManReader.Get(bpDComplexes, myDComplexes.data(),
                              adios2::Mode::Sync);
            VerifyData(myChars.data(), currentStep, startTmp, countTmp, shape,
                       mpiRank);
            VerifyData(myUChars.data(), currentStep, startTmp, countTmp, shape,
                       mpiRank);
            VerifyData(myShorts.data(), currentStep, startTmp, countTmp, shape,
                       mpiRank);
            VerifyData(myUShorts.data(), currentStep, startTmp, countTmp, shape,
                       mpiRank);
            VerifyData(myInts.data(), currentStep, startTmp, countTmp, shape,
                       mpiRank);
            VerifyData(myUInts.data(), currentStep, startTmp, countTmp, shape,
                       mpiRank);
            VerifyData(myFloats.data(), currentStep, startTmp, countTmp, shape,
                       mpiRank);
            VerifyData(myDoubles.data(), currentStep, startTmp, countTmp, shape,
                       mpiRank);
            VerifyData(myComplexes.data(), currentStep, startTmp, countTmp,
                       shape, mpiRank);
            VerifyData(myDComplexes.data(), currentStep, startTmp, countTmp,
                       shape, mpiRank);

            startTmp[1] = mpiRank * 2 + 1;
            countTmp[1] = 1;
            bpChars.SetSelection({startTmp, countTmp});
            bpUChars.SetSelection({startTmp, countTmp});
            bpShorts.SetSelection({startTmp, countTmp});
            bpUShorts.SetSelection({startTmp, countTmp});
            bpInts.SetSelection({startTmp, countTmp});
            bpUInts.SetSelection({startTmp, countTmp});
            bpFloats.SetSelection({startTmp, countTmp});
            bpDoubles.SetSelection({startTmp, countTmp});
            bpComplexes.SetSelection({startTmp, countTmp});
            bpDComplexes.SetSelection({startTmp, countTmp});
            dataManReader.Get(bpChars, myChars.data(), adios2::Mode::Sync);
            dataManReader.Get(bpUChars, myUChars.data(), adios2::Mode::Sync);
            dataManReader.Get(bpShorts, myShorts.data(), adios2::Mode::Sync);
            dataManReader.Get(bpUShorts, myUShorts.data(), adios2::Mode::Sync);
            dataManReader.Get(bpInts, myInts.data(), adios2::Mode::Sync);
            dataManReader.Get(bpUInts, myUInts.data(), adios2::Mode::Sync);
            dataManReader.Get(bpFloats, myFloats.data(), adios2::Mode::Sync);
            dataManReader.Get(bpDoubles, myDoubles.data(), adios2::Mode::Sync);
            dataManReader.Get(bpComplexes, myComplexes.data(),
                              adios2::Mode::Sync);
            dataManReader.Get(bpDComplexes, myDComplexes.data(),
                              adios2::Mode::Sync);
            VerifyData(myChars.data(), currentStep, startTmp, countTmp, shape,
                       mpiRank);
            VerifyData(myUChars.data(), currentStep, startTmp, countTmp, shape,
                       mpiRank);
            VerifyData(myShorts.data(), currentStep, startTmp, countTmp, shape,
                       mpiRank);
            VerifyData(myUShorts.data(), currentStep, startTmp, countTmp, shape,
                       mpiRank);
            VerifyData(myInts.data(), currentStep, startTmp, countTmp, shape,
                       mpiRank);
            VerifyData(myUInts.data(), currentStep, startTmp, countTmp, shape,
                       mpiRank);
            VerifyData(myFloats.data(), currentStep, startTmp, countTmp, shape,
                       mpiRank);
            VerifyData(myDoubles.data(), currentStep, startTmp, countTmp, shape,
                       mpiRank);
            VerifyData(myComplexes.data(), currentStep, startTmp, countTmp,
                       shape, mpiRank);
            VerifyData(myDComplexes.data(), currentStep, startTmp, countTmp,
                       shape, mpiRank);

            dataManReader.EndStep();
        }
        else if (status == adios2::StepStatus::EndOfStream)
        {
            std::cout << "[Rank " + std::to_string(mpiRank) +
                             "] SscTest reader end of stream!"
                      << std::endl;
            break;
        }
    }
    auto attInt = dataManIO.InquireAttribute<int>("AttInt");
    std::cout << "[Rank " + std::to_string(mpiRank) + "] Attribute received "
              << attInt.Data()[0] << ", expected 110" << std::endl;
    ASSERT_EQ(110, attInt.Data()[0]);
    ASSERT_NE(111, attInt.Data()[0]);
    dataManReader.Close();
}

TEST_F(SscEngineTest, TestSscReaderMultiblock)
{
    std::string filename = "TestSscReaderMultiblock";
    adios2::Params engineParams = {{"Verbose", "0"}};

    int worldRank, worldSize;
    MPI_Comm_rank(MPI_COMM_WORLD, &worldRank);
    MPI_Comm_size(MPI_COMM_WORLD, &worldSize);
    int mpiGroup = worldRank / (worldSize / 2);
    MPI_Comm_split(MPI_COMM_WORLD, mpiGroup, worldRank, &mpiComm);

    MPI_Comm_rank(mpiComm, &mpiRank);
    MPI_Comm_size(mpiComm, &mpiSize);

    size_t steps = 100;

    if (mpiGroup == 0)
    {
        Dims shape = {(size_t)mpiSize * 2, 10};
        Dims start = {(size_t)mpiRank * 2, 0};
        Dims count = {1, 10};
        Writer(shape, start, count, steps, engineParams, filename);
    }

    std::this_thread::sleep_for(std::chrono::milliseconds(1));

    if (mpiGroup == 1)
    {
        Dims shape = {(size_t)mpiSize * 2, 10};
        Dims start = {0, 0};
        Dims count = {(size_t)mpiSize * 2, 10};
        Reader(shape, start, count, steps, engineParams, filename);
    }

    MPI_Barrier(MPI_COMM_WORLD);
}

int main(int argc, char **argv)
{
    MPI_Init(&argc, &argv);
    int worldRank, worldSize;
    MPI_Comm_rank(MPI_COMM_WORLD, &worldRank);
    MPI_Comm_size(MPI_COMM_WORLD, &worldSize);
    ::testing::InitGoogleTest(&argc, argv);
    int result = RUN_ALL_TESTS();

    MPI_Finalize();
    return result;
}