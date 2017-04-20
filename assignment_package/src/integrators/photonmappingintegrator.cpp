#include "photonmappingintegrator.h"


Color3f PhotonMappingIntegrator::Li(const Ray& ray, const Scene& scene, std::shared_ptr<Sampler> sampler, int depth, Color3f throughputColor) const
{

    Intersection isec;
    Ray newRay = ray;
    bool specularBounce = false;
    BxDFType typeBxdf;
    int chosenLightNum = std::floor(sampler->Get1D()*scene.lights.size());

    float directLightPdf1 = 0.f;
    float directLightPdf2 = 0.f;
    float directBSDFPdf1 = 0.f;
    float directBSDFPdf2 = 0.f;

    Vector3f woW;
    Vector3f wiW;
    Vector3f directLightWiW;
    Vector3f directBSDFWiW;

    Color3f directLightTotalColor = Color3f(0.0f);
    Color3f directNaiveTotalColor = Color3f(0.0f);
    Color3f directTotalColor = Color3f(0.0f);

    while(depth>0)
    {
        isec = Intersection();
        woW =  -newRay.direction;

        if(scene.Intersect(newRay,&isec))
        {
            //if we hits the light
            if(isec.objectHit->light!=nullptr)
            {
                return isec.objectHit->light->emittedLight;
            }
            //if we hit some other primitives
            else
            {
                float currentPdf;
                Color3f directBSDFFColor = isec.bsdf->Sample_f(woW,&wiW,sampler->Get2D(),&currentPdf,BSDF_ALL,&typebxdf);
                specularBounce = false;
                if((typebxdf & BSDF_SPECULAR)!=0)
                {
                    specularBounce = true;
                }

                //if specular bounce do ray tracing
                if(specularBounce)
                {
                    //************************do ray tracing
                    depth--;
                    newRay = Ray(isec.point,&wiW);
                }
                //first do MIS and then add up caustic and global photons
                else
                {

                    //MIS direct lighting
                    //*******************************************************//
                    Color3f directLightLiColor = scene.lights[chosenLightNum]->Sample_Li(isec,sampler->Get2D(),&directLightWiW,&directLightPdf1);
                    Color3f directLightFColor = isec.bsdf->f(woW,directLightWiW);
                    Ray shadowTestRay = isec.SpawnRay(directLightWiW);
                    Intersection shadowIntersection = Intersection();
                    //specially for pointlight and spotlight
                    std::shared_ptr<Light> chosenLight = scene.lights[chosenLightNum];
                    if((chosenLight->lightType == POINTLIGHT)||(chosenLight->lightType == SPOTLIGHT))
                    {
                        //with Intersection in the scene
                        if(scene.Intersect(shadowTestRay,&shadowIntersection))
                        {
                            if(shadowIntersection.t < (1 - FLT_EPSILON))
                            {
                                directLightTotalColor = Color3f(0.0f);
                            }
                            else
                            {
                                directLightTotalColor = directLightFColor * directLightLiColor * AbsDot(directLightWiW,isec.normalGeometric)/directLightPdf1;
                            }

                        }
                        else
                        {
                            directLightTotalColor = directLightFColor * directLightLiColor * AbsDot(directLightWiW,isec.normalGeometric)/directLightPdf1;
                        }
                    }
                    else
                    {
                        if(scene.Intersect(shadowTestRay,&shadowIntersection))
                        {
                            //the situation without shadow
                            //***********************************light and arealight
                            if(shadowIntersection.objectHit->light==scene.lights[chosenLightNum])
                            {
                                if(directLightPdf1==0)
                                {
                                    directLightTotalColor = Color3f(0.f);
                                }
                                else
                                {
                                    directLightTotalColor = directLightFColor * directLightLiColor * AbsDot(directLightWiW,isec.normalGeometric)/directLightPdf1;

                                }
                            }
                            else
                            {
                                directLightTotalColor = Color3f(0.f);
                            }
                        }
                        else
                        {
                            directLightTotalColor = Color3f(0.f);
                        }

                    }

                    //MIS direct BSDF
                    //**********************************************//
                    Color3f directBSDFFColor = isec.bsdf->Sample_f(woW,&directBSDFWiW,sampler->Get2D(),&directBSDFPdf1,BSDF_ALL,&typebxdf);
                    Ray rayOut = isec.SpawnRay(directBSDFWiW);
                    Color3f directBSDFLiColor;
                    Intersection newIntersection = Intersection();
                    if((chosenLight->lightType == POINTLIGHT)||(chosenLight->lightType == SPOTLIGHT))
                    {

                        directNaiveTotalColor = Color3f(0.0f);
                    }
                    else
                    {
                        //the new ray has any intersection
                        if(scene.Intersect(rayOut,&newIntersection))
                        {
                            //hits the light we want
                            //***********************************light and arealight
                            if(newIntersection.objectHit->light==scene.lights[chosenLightNum])
                            {

                                Vector3f wiwInverse = directBSDFWiW;
                                directBSDFLiColor = newIntersection.Le(-wiwInverse);
                                if(directBSDFPdf1==0)
                                {
                                    directNaiveTotalColor = Color3f(0.0f);
                                }
                                else
                                {
                                    directNaiveTotalColor = directBSDFFColor * directBSDFLiColor * AbsDot(directBSDFWiW,isec.normalGeometric)/directBSDFPdf1;
                                }

                            }
                            else
                            {
                                directNaiveTotalColor = Color3f(0.0f);
                            }

                        }
                        else
                        {
                            directNaiveTotalColor = Color3f(0.0f);
                        }
                    }

                    //MIS sum up
                    //******************************MIS summing up part****************************//

                    directLightPdf2 = scene.lights[chosenLightNum]->Pdf_Li(isec,directBSDFWiW);
                    directBSDFPdf2 = isec.bsdf->Pdf(woW, directLightWiW);

                    float weightDirectLight2 = PowerHeuristic(1,directLightPdf1,1,directBSDFPdf2);
                    float weightDirectBSDF2 = PowerHeuristic(1,directBSDFPdf1,1,directLightPdf2);

                    directTotalColor = directLightTotalColor*weightDirectLight2+directNaiveTotalColor*weightDirectBSDF2;
                    directTotalColor = 1.0f * scene.lights.size() * directTotalColor;



                    //add up caustic and global Color
                    Color3f causticColor = CausticColor(isec,newRay);
                    Color3f globalIlluimationColor = GlobalIlluminationColor(isec,newRay);

                    //do we need to do some weighting to the overall color????? Or just add them together????
                    return directTotalColor + causticColor + globalIlluimationColor;

                }
            }
        }
        //no intersection with the scene so that the color would be black
        else
        {
            return Color3f(0.0f);
        }
    }
}


float PowerHeuristic(int nf, Float fPdf, int ng, Float gPdf)
{
    //TODO
    float f = nf*fPdf, g=ng*gPdf;

    if((f * f + g * g)==0)
    {
        return 0.f;
    }
    else
    {
         return (f * f)/(f * f + g * g);
    }
}

Color3f PhotonMappingIntegrator::CausticColor(const Intersection &isec, const Ray &r)
{
    //search for the kd-tree and find the caustic color of the area
    return Color3f(0.0f);
}

Color3f PhotonMappingIntegrator::GlobalIlluminationColor(const Intersection &isec, const Ray &r)
{
    //search for the kd-tree and find the global illumination of the area
    // how many wis do we need????
    return Color3f(0.0f);
}

