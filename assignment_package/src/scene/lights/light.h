#pragma once
#include <globals.h>
#include <scene/transform.h>
#include <raytracing/intersection.h>
#include <samplers/sampler.h>
#include <warpfunctions.h>

class Intersection;


enum LightType
{
    UKNOWN = -1,
    DIFFUSEAREALIGHT = 0,
    POINTLIGHT = 1,
    SPOTLIGHT = 2,
    DIRECTIONALLIGHT = 3
};

class Light
{
  public:
    virtual ~Light(){}
    Light(Transform t)
        : transform(t), name(), pLight(t.position()),lightType(UKNOWN)
    {}

    // Returns the light emitted along a ray that does
    // not hit anything within the scene bounds.
    // Necessary if we want to support things like environment
    // maps, or other sources of light with infinite area.
    // The default implementation for general lights returns
    // no energy at all.
    virtual Color3f Le(const Ray &r) const;

    virtual Color3f Sample_Li(const Intersection &ref, const Point2f &xi,
                                                Vector3f *wi, Float *pdf) const = 0;

    virtual Ray EmitSampleLight(std::shared_ptr<Sampler> sampler) const = 0;

    virtual float Pdf_Li(const Intersection &ref, const Vector3f &wi) const = 0;
    QString name; // For debugging
    virtual Color3f L(const Intersection &isect, const Vector3f &w) const = 0;

    Point3f pLight;
    LightType lightType;

    Color3f emittedLight;

protected:
    const Transform transform;
};

class AreaLight : public Light
{
public:
    AreaLight(const Transform &t) : Light(t){}
    // Returns the light emitted from a point on the light's surface _isect_
    // along the direction _w_, which is leaving the surface.
    virtual Color3f L(const Intersection &isect, const Vector3f &w) const = 0;
    virtual Ray EmitSampleLight(std::shared_ptr<Sampler> sampler) const = 0;
};
