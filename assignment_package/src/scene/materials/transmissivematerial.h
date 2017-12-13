#pragma once
#include "material.h"

class TransmissiveMaterial : public Material
{
public:
    TransmissiveMaterial(const Color3f &Kt, float indexOfRefraction,
                  const std::shared_ptr<QImage> &textureMap,
                  const std::shared_ptr<QImage> &normalMap)
        : Kt(Kt), indexOfRefraction(indexOfRefraction),
          textureMap(textureMap), normalMap(normalMap)
    {}

    void ProduceBSDF(Intersection *isect) const;

    Color3f GetMaterialKtColor() const;
    Color3f GetMaterialKdColor() const;
    Color3f GetMaterialKrColor() const;
    Color3f GetMaterialKsColor() const;


private:
    Color3f Kt;                    // The spectral transmission of this material.
                                   // This is just the base color of the material

    float indexOfRefraction;       // The IoR of this glass. We assume the IoR of
                                   // any external medium is that of a vacuum: 1.0

    std::shared_ptr<QImage> textureMap; // The color obtained from this (assuming it is non-null) is multiplied with the base material color.
    std::shared_ptr<QImage> normalMap;
};
