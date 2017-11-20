#ifndef PROGRESSIVEPHOTONMAPPING_H
#define PROGRESSIVEPHOTONMAPPING_H

#include "globals.h"
#include "integrator.h"

class ProgressivePhotonMapping : public Integrator
{
public:
    ProgressivePhotonMapping();

    ProgressivePhotonMapping(Bounds2i bounds, Scene* s, std::shared_ptr<Sampler> sampler, int recursionLimit)
    :Integrator(bounds, s, sampler, recursionLimit){}

    virtual Color3f Li(const Ray& ray, const Scene& scene, std::shared_ptr<Sampler> sampler, int depth, Color3f throughputColor,KdTree tree) const;

    virtual void TraceProgressivePhotons(const Scene& scene, ProgressiveKdNode* root,std::shared_ptr<Sampler> sampler, int depth, int numPhotons, QList<PixelHitPoint>& hitPoints);
};

#endif // PROGRESSIVEPHOTONMAPPING_H
