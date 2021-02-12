#ifndef DETECTOR_HPP
#define DETECTOR_HPP
#include <vector>
#include "ConfigFile.hpp"
#include "Datatype.hpp"
#include "Geometry.hpp"
#include "Result.hpp"
#define DETECTOR_BAG_SIZE 64

class Detector
{
private:
    std::vector<datatype *> *fSelfDataset;
    int fGeneration;
    std::vector<datatype *> *fDetectors;
    ConfigFile fConfigFile;
    Geometry fGeometry;


public:
    Detector(ConfigFile, std::vector<datatype *> *);
    void randomVector(datatype *);
    std::vector<datatype *> *generateDetectors();
    result applyDetectors(std::vector<datatype *> *);
};

#endif