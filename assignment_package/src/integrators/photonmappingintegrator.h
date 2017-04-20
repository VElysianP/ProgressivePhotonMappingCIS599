#ifndef PHOTONMAPPINGINTEGRATOR_H
#define PHOTONMAPPINGINTEGRATOR_H

#include "globals.h"
#include "integrator.h"
#include <scene/photon.h>

class PhotonMappingIntegrator : public Integrator
{
public:
    PhotonMappingIntegrator();

    PhotonMappingIntegrator(Bounds2i bounds, Scene* s, std::shared_ptr<Sampler> sampler, int recursionLimit)
        :Integrator(bounds, s, sampler, recursionLimit){}

    virtual Color3f Li(const Ray& ray, const Scene& scene, std::shared_ptr<Sampler> sampler, int depth, Color3f throughputColor) const;

    Color3f CausticColor(const Intersection &isec, const Ray &r);
    Color3f GlobalIlluminationColor(const Intersection &isec, const Ray &r);

    SearchAdjcent(const Point3f &intersectionPoint, const Intersection &isec);


};

#endif // PHOTONMAPPINGINTEGRATOR_H
