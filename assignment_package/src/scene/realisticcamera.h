#ifndef REALISTICCAMERA_H
#define REALISTICCAMERA_H


#include <globals.h>
#include <raytracing/ray.h>
#include <openGL/drawable.h>
#include "camera.h"
#include <warpfunctions.h>
#include <samplers/sampler.h>


class RealisticCamera : public Camera
{
public:
    RealisticCamera();
    RealisticCamera(const Camera &c);

    Ray Raycast(const Point2f &pt, const std::shared_ptr<Sampler> sampler) const;

protected:
        float lensRadius, focalDistance;

};

#endif // REALISTICCAMERA_H
