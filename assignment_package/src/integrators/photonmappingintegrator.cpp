#include "photonmappingintegrator.h"


Color3f PhotonMappingIntegrator::Li(const Ray& ray, const Scene& scene, std::shared_ptr<Sampler> sampler, int depth, Color3f throughputColor,KdTree tree) const
{

    Intersection isec;
    Ray newRay = ray;
    bool specularBounce = false;
    BxDFType typebxdf;
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
    Color3f totalColor = Color3f(1.0f);

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
                Color3f testColor = isec.bsdf->Sample_f(woW,&wiW,sampler->Get2D(),&currentPdf,BSDF_ALL,&typebxdf);
                specularBounce = false;
                if((typebxdf & BSDF_SPECULAR)!=0)
                {
                    specularBounce = true;//which means that the surface is a specular surface
                }

                //if specular bounce do ray tracing
                if(specularBounce)
                {
                    //************************do ray tracing
                    depth--;
                    totalColor = totalColor * testColor;
                    newRay = Ray(isec.point,wiW);
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
                    Color3f causticColor = CausticColor(isec,newRay,tree);
                    Color3f globalIlluimationColor = GlobalIlluminationColor(isec,newRay,tree);

                    //do we need to do some weighting to the overall color????? Or just add them together????
                    totalColor = totalColor *(directTotalColor + causticColor + globalIlluimationColor);
                    return totalColor;

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


float PhotonMappingIntegrator::PowerHeuristic(int nf, float fPdf, int ng, float gPdf) const
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

Color3f PhotonMappingIntegrator::CausticColor(const Intersection &isec, const Ray &r ,KdTree tree) const
{
    //search for the kd-tree and find the caustic color of the area
    Point3f positionToSearch = isec.point;
    QList<KdNode*> nearNodes;
    Color3f totalColor = Color3f(0.0f);

    tree.NearPhotons(positionToSearch,tree.root,nearNodes);

    if(nearNodes.size()!=0)
    {
        for(int i = 0; i < nearNodes.size() ; ++i)
        {
            if(nearNodes[i]->photonType==CAUSTIC)
            {
                totalColor = totalColor + nearNodes[i]->photonColor;
            }
        }
        totalColor = Color3f(totalColor.x/nearNodes.size(),totalColor.y/nearNodes.size(),totalColor.z/nearNodes.size());
        for(auto nearNode : nearNodes){
            delete nearNode;
        }
        nearNodes.clear();
        return totalColor;
    }
    else
    {
        for(auto nearNode : nearNodes){
            delete nearNode;
        }
        nearNodes.clear();
        return totalColor;
    }
}

Color3f PhotonMappingIntegrator::GlobalIlluminationColor(const Intersection &isec, const Ray &r,KdTree tree) const
{
    Point3f positionToSearch = isec.point;
    Vector3f woW = -r.direction;
    QList<KdNode*> nearNodes;
    Color3f totalColor = Color3f(0.f);

    for(int i = 0 ; i <max_photon_search; ++i)
    {
        float currentPdf;
        Vector3f wiW;
//        if(!isec.ProduceBSDF())
//        {
//            return Color3f(0.f);
//        }
        Color3f fColor = isec.bsdf->Sample_f(woW,&wiW,sampler->Get2D(),&currentPdf);
        Intersection newIsec = Intersection();
        Ray newRay = Ray(positionToSearch,wiW);
        if(scene->Intersect(newRay,&newIsec))
        {
            tree.NearPhotons(newIsec.point,tree.root,nearNodes);
        }

    }
    if(nearNodes.size()!=0)
    {
        for(int j = 0 ; j < nearNodes.size();++j)
        {
            if(nearNodes[j]->photonType == GLOBALINDIRECT)
            {
                totalColor = totalColor + nearNodes[j]->photonColor;
            }
        }
        totalColor = Color3f(totalColor.x/nearNodes.size(),totalColor.y/nearNodes.size(),totalColor.z/nearNodes.size());

        for(auto nearNode : nearNodes){
            delete nearNode;
        }
        nearNodes.clear();
        return totalColor;
    }
    else
    {
        for(auto nearNode : nearNodes){
            delete nearNode;
        }
        nearNodes.clear();
        return Color3f(0.f);
    }

}

