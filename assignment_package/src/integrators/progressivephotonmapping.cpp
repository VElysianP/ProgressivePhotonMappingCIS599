#include "progressivephotonmapping.h"

//For progressive Photon Mapping, it should return
//the color for that exact pixel
Color3f ProgressivePhotonMapping::Li(const Ray &ray, const Scene &scene, std::shared_ptr<Sampler> sampler, int depth, Color3f throughputColor, KdTree tree) const
{
    return Color3f(0.f);
}


void ProgressivePhotonMapping::TraceProgressivePhotons(const Scene& scene, ProgressiveKdNode* root,std::shared_ptr<Sampler> sampler, int depth, int numPhotons, QList<PixelHitPoint>& hitPoints)
{

    for(int count = 0;count<numPhotons;count++)
    {
        int chosenLightNum = std::floor(sampler->Get1D()*scene.lights.size());
        Ray tempRay = scene.lights[chosenLightNum]->EmitSampleLight(sampler);
        Intersection isec = Intersection();
        BxDFType typeBxdf;
        Color3f currentColor = scene.lights[chosenLightNum]->emittedLight;

        while(depth>0)
        {
            Vector3f woW = -tempRay.direction;
            Vector3f wiW;
            float currentPdf;

            //if the photon does not hit anything
            //just throw it away
            if(scene.Intersect(tempRay,&isec))
            {
                //the photon meets the light source
                if(!isec.ProduceBSDF())
                {
                    return;
                }
                Color3f fColor = isec.bsdf->Sample_f(woW,&wiW,sampler->Get2D(),&currentPdf,BSDF_ALL,&typeBxdf);
                if((typeBxdf&BSDF_SPECULAR)!=0)
                {
                    currentColor = currentColor * fColor * AbsDot(wiW,isec.bsdf->normal);
                    tempRay = Ray(isec.point,wiW);
                }
                else
                {
                    //find the nearest hitpoint
                    //rewrite the color of hitpoint
                }
            }

            //Russian Roulette
            float uniformRandom = sampler->Get1D();
            float maxInThroughput = std::max(std::max(currentColor.x,currentColor.y),currentColor.z);
            if((recursionLimit - depth)>3)
            {
                if(uniformRandom > maxInThroughput)
                {
                    break;
                }
            }
            depth--;
        }
    }

    //change the density and the radius
    //recalculate the hitpoint parameters
    //correct the color for this hit point
    return;
}
