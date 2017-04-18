#include "pointlight.h"

PointLight::PointLight(const Transform &t, const Color3f &Le)
    :Light(t),emittedLight(Le),pLight(t.position()){lightType = POINTLIGHT;}

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
