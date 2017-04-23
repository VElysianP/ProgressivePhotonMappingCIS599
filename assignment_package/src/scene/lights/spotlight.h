#ifndef SPOTLIGHT_H
#define SPOTLIGHT_H

#pragma once
#include "light.h"
#include "globals.h"

class SpotLight : public Light
{
public:
    SpotLight(const Transform &t, const Color3f& Le, const float totalWidth, const float fallOffStart);


    Color3f Sample_Li(const Intersection &ref, const Point2f &xi,
                                         Vector3f *wi, Float *pdf) const;

    virtual Color3f L(const Intersection &isect, const Vector3f &w) const;

    virtual Ray EmitSampleLight(std::shared_ptr<Sampler> sampler) const;

    virtual float Pdf_Li(const Intersection &ref, const Vector3f &wi) const;

    float FallOff(const Vector3f &w) const;

    const Point3f pLight;
    const float cosTotalWidth,cosFallOffStart;//radius value of angle
};

#endif // SPOTLIGHT_H
