#ifndef DETECTOR_HPP
#define DETECTOR_HPP
#include <vector>
#include "ConfigFile.hpp"
#include "Datatype.hpp"
#include "Geometry.hpp"
#include "Result.hpp"
#include <pthread.h>
#define DETECTOR_BAG_SIZE 64000

typedef struct
{
    datatype *buf[DETECTOR_BAG_SIZE];
    int occupied;
    int nextin;
    int nextout;
    pthread_mutex_t mutex;
    pthread_cond_t more;
    pthread_cond_t less;
} buffer_t;

class Detector
{
private:
    std::vector<datatype *> *fSelfDataset;
    int fGeneration;
    std::vector<datatype *> *fDetectors;
    ConfigFile fConfigFile;
    Geometry fGeometry;
    buffer_t *fDetectorBag;

public:
    Detector(ConfigFile, std::vector<datatype *> *);
    void randomVector(datatype *);
    std::vector<datatype *> *generateDetectors();
    result applyDetectors(std::vector<datatype *> *);
    void producer(buffer_t *b, datatype *);
    void consumer(buffer_t *b, datatype *, std::vector<datatype *> *);
};

#endif