#include "transmissivematerial.h"
#include "specularbtdf.h"

void TransmissiveMaterial::ProduceBSDF(Intersection *isect) const
{
    // Important! Must initialize the intersection's BSDF!
    isect->bsdf = std::make_shared<BSDF>(*isect, indexOfRefraction);

    Color3f color = Kt;
    if(this->textureMap)
    {
        color *= Material::GetImageColor(isect->uv, this->textureMap.get());
    }
    if(this->normalMap)
    {
        isect->bsdf->normal = isect->bsdf->tangentToWorld *  Material::GetImageColor(isect->uv, this->normalMap.get());
        //Update bsdf's TBN matrices to support the new normal
        Vector3f tangent, bitangent;
        CoordinateSystem(isect->bsdf->normal, &tangent, &bitangent);
        isect->bsdf->UpdateTangentSpaceMatrices(isect->bsdf->normal, tangent, bitangent);
    }

    isect->bsdf->Add(new SpecularBTDF(color, 1.f, indexOfRefraction, new FresnelNoReflect()));
}

Color3f TransmissiveMaterial::GetMaterialKtColor() const
{
    return Kt;
}
Color3f TransmissiveMaterial::GetMaterialKdColor() const
{
    return Color3f(-1.0f);
}
Color3f TransmissiveMaterial::GetMaterialKrColor() const
{
    return Color3f(-1.0f);
}
Color3f TransmissiveMaterial::GetMaterialKsColor() const
{
    return Color3f(-1.0f);
}
