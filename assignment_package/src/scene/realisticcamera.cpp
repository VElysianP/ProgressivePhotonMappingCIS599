#include "realisticcamera.h"

RealisticCamera::RealisticCamera()
    : Camera(),lensRadius(1.f),focalDistance(1.0f)
{

}
RealisticCamera::RealisticCamera(const Camera &c)
    : Camera(c),lensRadius(1.f),focalDistance(1.f)
{

}



Ray RealisticCamera::Raycast(const Point2f &pt, const std::shared_ptr<Sampler> sampler) const
{
    //<compute raster and camera sample positions>

    Ray pinholeRay = Camera::Raycast(pt);
    Ray ray = Ray(glm::vec3(0.0f),glm::vec3(0.0f,0.0f,1.0f));
    if(lensRadius > 0)
    {
        Point3f pLens = lensRadius * WarpFunctions::squareToDiskConcentric(sampler->Get2D());

        float ft = (focalDistance - pinholeRay.origin.z) / pinholeRay.direction.z;
        Point3f pFocus = pinholeRay.origin + pinholeRay.direction * ft;


        ray.origin = pinholeRay.origin + pLens;
        ray.direction = glm::normalize(pFocus - ray.origin);
    }


    return ray;
}
