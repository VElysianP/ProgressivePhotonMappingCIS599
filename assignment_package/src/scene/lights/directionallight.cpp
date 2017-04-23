#include "directionallight.h"


Color3f DirectionalLight::Sample_Li(const Intersection &ref, const Point2f &xi,
                                     Vector3f *wi, Float *pdf) const
{
    *wi = wLight;
    *pdf = 1.f;

    if((ref.point.x >= (worldCenter.x - worldRadius))&&(ref.point.x <= (worldCenter.x + worldRadius)))
    {
        if(glm::dot((worldCenter - ref.point),wLight)<0)
        {
            return emittedLight;
        }
        else
        {
            return Color3f(0.0f);
        }
    }
    else
    {
        return Color3f(0.0f);
    }
}
float DirectionalLight::Pdf_Li(const Intersection &ref, const Vector3f &wi) const
{
    return 0.f;
}
Color3f DirectionalLight::L(const Intersection &isect, const Vector3f &w) const
{

    return emittedLight*Pi*worldRadius*worldRadius;
}
Ray DirectionalLight::EmitSampleLight(std::shared_ptr<Sampler> sampler) const
{
    //this is not correct at present
    return Ray(pLight, wLight);
}
