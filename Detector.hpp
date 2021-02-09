#ifndef DETECTOR_HPP
#define DETECTOR_HPP
#include <vector>
#include "ConfigFile.hpp"
#include "Datatype.hpp"
#include "Geometry.hpp"
#include "Result.hpp"

class Detector
{
private:
    std::vector<datatype *> *fSelfDataset;
    int fGeneration;
    std::vector<datatype *> *fDetectors;
    ConfigFile fConfigFile;
    Geometry geometry;

public:
    Detector(ConfigFile, std::vector<datatype *> *);
    void randomVector(datatype *);
    std::vector<datatype *> *generateDetectors();
    result applyDetectors(std::vector<datatype *> *);
    void *producer();
    void *consumer(std::vector<datatype *> *);
};

#endif