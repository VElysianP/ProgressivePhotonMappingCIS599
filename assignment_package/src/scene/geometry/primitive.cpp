#include "primitive.h"

bool Primitive::Intersect(const Ray &r, Intersection *isect) const
{
    if(!shape->Intersect(r, isect)) return false;
    isect->objectHit = this;
    // We create a BSDF for this intersection in our Integrator classes
    if(material)
    {
        material->ProduceBSDF(isect);
    }
    return true;
}

bool Primitive::ProduceBSDF(Intersection *isect) const
{
    if(material)
    {
        material->ProduceBSDF(isect);
        return true;
    }
    return false;
}

//const AreaLight* Primitive::GetAreaLight() const
//{
//    return areaLight.get();
//}

const Light* Primitive::GetLight() const
{
    return light.get();
}

const Material* Primitive::GetMaterial() const
{
    return material.get();
}
Bounds3f Primitive::WorldBound() const
{
    return shape->WorldBound();
}
