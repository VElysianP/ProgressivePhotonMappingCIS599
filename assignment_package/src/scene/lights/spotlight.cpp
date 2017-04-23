#include "spotlight.h"


SpotLight::SpotLight(const Transform &t, const Color3f& Le, const float totalWidth, const float fallOffStart)
    :Light(t), pLight(t.position()),
      cosTotalWidth(std::cos(Pi*totalWidth/180.0)), cosFallOffStart(std::cos(Pi*fallOffStart/180.0)){lightType = SPOTLIGHT;
                                                                                                    emittedLight = Le;}

Color3f SpotLight::Sample_Li(const Intersection &ref, const Point2f &xi,
                                     Vector3f *wi, Float *pdf) const
{
    *wi = glm::normalize(pLight - ref.point);
    *pdf = 1.f;

    //should also test whether the wi lies inside the visible cone

    return emittedLight * FallOff(-*wi) / glm::length2(pLight-ref.point);


}
float SpotLight::Pdf_Li(const Intersection &ref, const Vector3f &wi) const
{
    return 0.0f;
}
float SpotLight::FallOff(const Vector3f &w) const
{

    Vector3f wl = glm::normalize(Vector3f(transform.invT()*glm::vec4(w.x,w.y,w.z,0.0f)));
    float cosTheta = -wl.y;
    if(cosTheta < cosTotalWidth)
    {
        return 0.f;
    }
    if(cosTheta > cosFallOffStart)
    {
        return 1.f;
    }
    //<compute the falloff inside spotlight cone>
    float delta = (cosTheta - cosTotalWidth) / (cosFallOffStart - cosTotalWidth);
    return (delta * delta) * (delta * delta);
}


Color3f SpotLight::L(const Intersection &isect, const Vector3f &w) const
{
    return emittedLight * 2.0f * Pi *(1 - 0.5f * (cosFallOffStart + cosTotalWidth));
}

Ray SpotLight::EmitSampleLight(std::shared_ptr<Sampler> sampler) const
{
    Point2f sampleRayOut = sampler->Get2D();
    Point3f pointOnLightBefore = WarpFunctions::squareToDiskConcentric(sampleRayOut);
    //to match the point light function I have written, set the disk into xoz plane

    pointOnLightBefore.z = pointOnLightBefore.y;
    pointOnLightBefore.y = -1.f;//test on this value

    Point3f pointRayOut = Point3f(transform.T()*glm::vec4(pointOnLightBefore.x,pointOnLightBefore.y,pointOnLightBefore.z,1.0f));
    Ray r =Ray(pLight, glm::normalize(pointRayOut - pLight));

    //this r has already been trasformed into world space
    return r;
}
