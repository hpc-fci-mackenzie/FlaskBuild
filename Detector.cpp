#include "Detector.hpp"
#include <assert.h>
#include <cstdlib>
#include <iostream>
#include <cmath>
#include <set>
#include <string>
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

void Detector::producer(buffer_t *b, datatype *detector)
{
    pthread_mutex_lock(&b->mutex);
    while (b->occupied >= DETECTOR_BAG_SIZE)
        pthread_cond_wait(&b->less, &b->mutex);
    assert(b->occupied < DETECTOR_BAG_SIZE);

    b->buf[b->nextin] = detector;
    randomVector(b->buf[b->nextin]);
    b->nextin %= DETECTOR_BAG_SIZE;
    b->occupied++;
    pthread_cond_signal(&b->more);
    pthread_mutex_unlock(&b->mutex);
}

void Detector::consumer(buffer_t *b, datatype *detector, std::vector<datatype *> *detectors)
{
    pthread_mutex_lock(&b->mutex);
    while (b->occupied <= 0)
        pthread_cond_wait(&b->more, &b->mutex);
    assert(b->occupied > 0);
    detector = b->buf[b->nextout++];
    if (!fGeometry.matches(detector, fSelfDataset, fConfigFile.getMinDist()))
    {
        if (!fGeometry.matches(detector, detectors, 0.0))
        {
            detectors->push_back(detector);
            detector = new datatype[fConfigFile.getProblemSize()];
            std::cout << detectors->size() << "/" << fConfigFile.getMaxDetectors() << std::endl;
        }
    }
    b->nextout %= DETECTOR_BAG_SIZE;
    b->occupied--;
    pthread_cond_signal(&b->less);
    pthread_mutex_unlock(&b->mutex);
}

std::vector<datatype *> *Detector::generateDetectors()
{
    std::vector<datatype *> *detectors = new std::vector<datatype *>();
    std::cout << "Generating detectors..." << std::endl;
    datatype *detector = new datatype[fConfigFile.getProblemSize()];
    do
    {
        producer(fDetectorBag, detector);
        consumer(fDetectorBag, detector, detectors);

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