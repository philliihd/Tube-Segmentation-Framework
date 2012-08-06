#ifndef TUBE_SEGMENTATION
#define TUBE_SEGMENTATION

#include "OpenCLUtilities/openCLUtilities.hpp"
#include "SIPL/Core.hpp"
#include <iostream>
#include <string>
#include <unordered_map>

typedef struct OpenCL {
    cl::Context context;
    cl::CommandQueue queue;
    cl::Program program;
} OpenCL;

typedef struct TubeSegmentation {
    float *Fx, *Fy, *Fz; // The GVF vector field
    float *TDF; // The TDF response
    float *radius;
    char *centerline;
    char *segmentation;
} TubeSegmentation;

typedef std::unordered_map<std::string, std::string> paramList;

cl::Image3D readDatasetAndTransfer(OpenCL, std::string, paramList, SIPL::int3 *);

paramList getParameters(int argc, char ** argv);

TubeSegmentation runCircleFittingAndRidgeTraversal(OpenCL, cl::Image3D dataset, SIPL::int3 size, paramList);

#endif
