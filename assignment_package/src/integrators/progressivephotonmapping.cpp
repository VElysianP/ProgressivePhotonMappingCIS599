#include "progressivephotonmapping.h"

//For progressive Photon Mapping, it should return
//the color for that exact pixel
Color3f ProgressivePhotonMapping::Li(const Ray &ray, const Scene &scene, std::shared_ptr<Sampler> sampler, int depth, Color3f throughputColor, KdTree tree) const
{
    return Color3f(0.f);
}


//void ProgressivePhotonMapping::TraceProgressivePhotons(const Scene& scene, ProgressiveKdNode root,std::shared_ptr<Sampler> sampler, int depth, int numPhotons, QList<PixelHitPoint>& hitPoints)
//{

    //return;
//}
