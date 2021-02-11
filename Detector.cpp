#include "Detector.hpp"
#include <cstdlib>
#include <iostream>
#include <cmath>
#include <queue>
#include <set>
#include <string>
#include "omp.h"
#include "Debug.hpp"
#include "Datatype.hpp"
#include "Geometry.hpp"

Detector::Detector(ConfigFile configFile, std::vector<datatype *> *selfDataset)
{
    fConfigFile = configFile;
    fGeometry.setProblemSize(configFile.getProblemSize());
    fSelfDataset = selfDataset;
}

void Detector::randomVector(datatype *vector)
{
    for (int i = 0; i < fConfigFile.getProblemSize(); i++)
    {
#ifdef DEBUG
        vector[i] = fConfigFile.getSearchSpaceIndex(2 * i) + ((fConfigFile.getSearchSpaceIndex(2 * i + 1) - fConfigFile.getSearchSpaceIndex(2 * i)) * Random());
#else
        vector[i] = fConfigFile.getSearchSpaceIndex(2 * i) + ((fConfigFile.getSearchSpaceIndex(2 * i + 1) - fConfigFile.getSearchSpaceIndex(2 * i)) * uniform(engine));
#endif
    }
}

std::vector<datatype *> *Detector::generateDetectors()
{
    std::vector<datatype *> *detectors = new std::vector<datatype *>();
    std::cout << "Generating detectors..." << std::endl;
    datatype *detector = new datatype[fConfigFile.getProblemSize()];
    std::queue<datatype *> detectorBag;
    int flag = 0;
    datatype *auxiliaryDetector;
    do
    {
#pragma omp parallel sections
        {
#pragma omp section
            {
                while(detectorBag.size() < DETECTOR_BAG_SIZE){
                    detectorBag.push(detector);
                    randomVector(detectorBag.front());
                    auxiliaryDetector = detectorBag.front();
                    detectorBag.pop();
                    detectorBag.push(auxiliaryDetector);
                }
                randomVector(detector);
#pragma omp flush
                flag = 1;
#pragma omp flush(flag)
            }
#pragma omp section
            {
#pragma omp flush(flag)
                while (flag != 1)
                {
#pragma omp flush(flag)
                }
#pragma omp flush
                detector = detectorBag.front();
                if (!fGeometry.matches(detector, fSelfDataset, fConfigFile.getMinDist()))
                {
                    if (!fGeometry.matches(detector, detectors, 0.0))
                    {
                        detectors->push_back(detector);
                        detector = new datatype[fConfigFile.getProblemSize()];
                        std::cout << detectors->size() << "/" << fConfigFile.getMaxDetectors() << std::endl;
                    }
                }
                detectorBag.pop();
            }
        }
    } while (detectors->size() < fConfigFile.getMaxDetectors());

    if (detector != *detectors->cend())
    {
        delete[] detector;
    }

    return detectors;
}
result Detector::applyDetectors(std::vector<datatype *> *detectors)
{
    std::set<int> *detected = new std::set<int>();
    int trial = 1;
    for (auto it = fSelfDataset->cbegin(); it != fSelfDataset->cend(); it++)
    {
        bool actual = fGeometry.matches(*it, detectors, fConfigFile.getMinDist());
        bool expected = fGeometry.matches(*it, fSelfDataset, fConfigFile.getMinDist());
        if (actual == expected)
        {
            detected->insert(trial);
        }
        trial++;
    }

    std::cout << "Expected to be detected: ";
    std::vector<int> configExpectedDetected = fConfigFile.getExpectedDetected();
    for (auto it = configExpectedDetected.cbegin(); it != configExpectedDetected.cend();)
    {
        std::cout << *it;
        if (++it != configExpectedDetected.cend())
            std::cout << ", ";
    }
    std::cout << std::endl;

    std::cout << "Found: ";
    for (auto it = detected->cbegin(); it != detected->cend();)
    {
        std::cout << *it;
        if (++it != detected->cend())
            std::cout << ", ";
    }
    std::cout << std::endl;

    std::set<int> expectedDetected(configExpectedDetected.begin(), configExpectedDetected.end());
    datatype expectedDetectedSize = expectedDetected.size();
    for (auto it = detected->cbegin(); it != detected->cend(); it++)
    {
        auto found = expectedDetected.find(*it);
        if (found != expectedDetected.end())
        {
            expectedDetected.erase(*it);
        }
    }
    datatype new_expectedDetectedSize = expectedDetected.size();

    result result;
    result.DR = (-(new_expectedDetectedSize - expectedDetectedSize) / expectedDetectedSize);

    expectedDetected.clear();
    expectedDetected.insert(configExpectedDetected.begin(), configExpectedDetected.end());
    for (auto it = expectedDetected.cbegin(); it != expectedDetected.cend(); it++)
    {
        auto found = detected->find(*it);
        if (found != detected->end())
        {
            detected->erase(*it);
        }
    }
    datatype newDetectedSize = detected->size();
    result.FAR = (newDetectedSize / expectedDetectedSize);

    return result;
}