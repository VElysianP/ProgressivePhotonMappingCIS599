#include "pointlight.h"

PointLight::PointLight(const Transform &t, const Color3f &Le)
    :Light(t),pLight(t.position()){lightType = POINTLIGHT;
                                  emittedLight = Le;}

Color3f PointLight::Sample_Li(const Intersection &ref, const Point2f &xi,
                                     Vector3f *wi, Float *pdf) const
{

    *wi = glm::normalize(pLight - ref.point);
    *pdf = 1.0f;
    return emittedLight/glm::length2(pLight - ref.point);
}


float PointLight::Pdf_Li(const Intersection &ref, const Vector3f &wi) const
{
    return 0.0f;
}
Color3f PointLight::L(const Intersection &isect, const Vector3f &w) const
{
    return 4*Pi*emittedLight;
}
Ray PointLight::EmitSampleLight(std::shared_ptr<Sampler> sampler) const
{
    Point2f samplePoint = sampler->Get2D();
    Point3f sampleOutside = WarpFunctions::squareToHemisphereUniform(samplePoint);
    glm::vec3 rayDirection = glm::normalize(sampleOutside);
    Point3f lightPoint = glm::vec3(transform.T()*glm::vec4(0.0,0.0,0.0,1.0));
    glm::vec3 transDir = glm::vec3(transform.T()*glm::vec4(rayDirection.x,rayDirection.y,rayDirection.z,0.0));
    return Ray(lightPoint, transDir);
}
