#ifndef DIRECTIONALLIGHT_H
#define DIRECTIONALLIGHT_H

#pragma once

#include "light.h"
#include "globals.h"
#include "scene/geometry/shape.h"

class DirectionalLight : public Light
{
public:
    DirectionalLight(const Transform &t, const Color3f& Le, const Vector3f &wLight, std::shared_ptr<Shape> &shape)
        : Light(t),  wLight(wLight), worldCenter(t.position()), worldRadius(2.0),shape(shape){lightType = DIRECTIONALLIGHT;
                                                                                             emittedLight = Le;}

    Color3f Sample_Li(const Intersection &ref, const Point2f &xi,
                                         Vector3f *wi, Float *pdf) const;


    virtual float Pdf_Li(const Intersection &ref, const Vector3f &wi) const;

    virtual Ray EmitSampleLight(std::shared_ptr<Sampler> sampler) const;

    virtual Color3f L(const Intersection &isect, const Vector3f &w) const;

    const Vector3f wLight;
    const Point3f worldCenter;
    const Float worldRadius;
    std::shared_ptr<Shape> shape;
};

#endif // DIRECTIONALLIGHT_H
