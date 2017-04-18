#ifndef POINTLIGHT_H
#define POINTLIGHT_H

#pragma once

#include "light.h"

class PointLight : public Light
{
public:
    PointLight(const Transform &t, const Color3f& Le);

    Color3f Sample_Li(const Intersection &ref, const Point2f &xi,
                                         Vector3f *wi, Float *pdf) const;


    virtual float Pdf_Li(const Intersection &ref, const Vector3f &wi) const;
    virtual Color3f L(const Intersection &isect, const Vector3f &w) const;

    const Color3f emittedLight;
    const Point3f pLight;
};

#endif // POINTLIGHT_H
