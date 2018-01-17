#include "progressivephotonmapping.h"

//For progressive Photon Mapping, it should return
//the color for that exact pixel
Color3f ProgressivePhotonMapping::Li(const Ray &ray, const Scene &scene, std::shared_ptr<Sampler> sampler, int depth, Color3f throughputColor, KdTree tree) const
{
    return Color3f(0.f);
}


void ProgressivePhotonMapping::TraceProgressivePhotons(const Scene& scene, ProgressiveKdNode* root,std::shared_ptr<Sampler> sampler, int depth, int numPhotons, QList<PixelHitPoint>& hitPoints)
{
    //this value functions as shrinking the radius
    //somehow needs to change based on our need
    float alpha = 0.6;
//    for(int i = 0; i< hitPoints.size();i++)
//    {
//        if((hitPoints[i].pixel.x == 193)&&(hitPoints[i].pixel.y == 12))
//        {
//            int h = 0;
//        }
//    }
    for(int count = 0;count<numPhotons;count++)
    {
        int chosenLightNum = std::floor(sampler->Get1D()*scene.lights.size());
        Ray tempRay = scene.lights[chosenLightNum]->EmitSampleLight(sampler);
        tempRay.origin = tempRay.origin + RayEpsilon * tempRay.direction;
        Intersection isec = Intersection();
        BxDFType typeBxdf;
        Color3f currentColor = scene.lights[chosenLightNum]->emittedLight;
        int traceNum = 0;

        Vector3f woW;
        Vector3f wiW;
        float currentPdf;

        //the value of depth equals to the recursion limit
        while(traceNum < depth)
        {
            traceNum++;
            woW = -tempRay.direction;
            //to make the photon tracing more realistic
            //the first tracing was done by naive direct lighting

            //if the photon does not hit anything
            //just throw it away
            if(scene.Intersect(tempRay,&isec))
            {
                //the photon meets the light source
                //throw the photon away
                //basically the photon will not hit the chosen light source itself
                if(!isec.ProduceBSDF())
                {
                    break;
                }

                Color3f fColor = isec.bsdf->Sample_f(woW,&wiW,sampler->Get2D(),&currentPdf,BSDF_ALL,&typeBxdf);
                //the photon meets specular bounce
                //if((typeBxdf&BSDF_SPECULAR)!=0)
                //{
                   // currentColor = currentColor * fColor * AbsDot(wiW,isec.bsdf->normal);
                    //tempRay = isec.SpawnRay(wiW);
                //}
                //the photon meet non-specular bounce
                //else
                //{
                    tempRay = isec.SpawnRay(wiW);
                    //if(currentPdf != 0.f)
                    //{
                        currentColor = currentColor * fColor * AbsDot(wiW,isec.bsdf->normal);
                    //}

                    if(traceNum == 1)
                    {
                        continue;
                    }

                    UpdateHitPoints(hitPoints,isec.point,currentColor);
                    //break;
                    //********************************** the kdtree method****************************
                    //find the nearest hitpoint
                    //rewrite the newColor of hitpoint
                    //add up the numNewPhotons amount
                    //
                    //int nearHitPointIndex = root->NearHitPointProg(isec.point,root);

                    //which means that there exists photon that has proper distance to a hitpoint
                    //and this is the index of the proper hitpoint
                    //add up the color of the hitpoint and then update the datas of this hitpoint
                    //int foundPhotons = 0;
                    //if(nearHitPointIndex != -1)
                    //{
                        //hitPoints[nearHitPointIndex].newColor += currentColor;
                        //hitPoints[nearHitPointIndex].numNewPhotons += 1;
                        //foundPhotons++;

                    //}
                    //if there is not proper hitpoint near the photon
                    //just throw away this photon
                    //******************************************end of kdtree******************************
                //}

                //Russian Roulette
                float uniformRandom = sampler->Get1D();
                float maxInThroughput = std::max(std::max(currentColor.x,currentColor.y),currentColor.z);
                if((depth - traceNum)<3)
                {
                    if(uniformRandom > maxInThroughput)
                    {
                        break;
                    }
                }
            }
            //if the photon does not hit anything just step out the loop
            else
            {
                break;
            }
        }
    }

    PhotonFinalGathering(hitPoints,alpha);

    return;
}

void ProgressivePhotonMapping::UpdateHitPoints(QList<PixelHitPoint>& hitPoints,Point3f position,Color3f currentColor)
{
    int loopSize = hitPoints.size();
    //loop through all the hitpoints inside this scene and find all the hitpoints that
    //its radius can cover the photon
    //although this method could be extremely slow
    //but it is accurate
    for(int i = 0;i<loopSize;i++)
    {
        Point3f  hitPointPos = hitPoints[i].position;
        //for every hitPoint, if the photon has the possibility that its color will
        //influnce the final color
        //we should write the color into the hitPoint
        if(glm::length(hitPointPos - position) <= hitPoints[i].radius)
        {
            hitPoints[i].newColor += currentColor;
            hitPoints[i].numNewPhotons ++;
        }
    }
}

void ProgressivePhotonMapping::PhotonFinalGathering(QList<PixelHitPoint>& hitPoints,float alpha)
{
    //change the density and the radius
    //recalculate the hitpoint parameters
    //correct the color for this hit point
    for(int hitCount = 0;hitCount < hitPoints.size();hitCount++)
    {
//        PixelHitPoint tempHit;
//        if((hitPoints[hitCount].pixel.x == 193)&&(hitPoints[hitCount].pixel.y == 12))
//        {
//            //break;
//             tempHit = hitPoints[hitCount];
//        }
        //which means this hitPoint hits the light source
        //go to the next hitpoint
        if(hitPoints[hitCount].isec.ProduceBSDF())
        {
            //which means the first photon trace
            if(hitPoints[hitCount].numPhotons == 0)
            {
                if(hitPoints[hitCount].numNewPhotons!=0)
                {
                hitPoints[hitCount].numPhotons += hitPoints[hitCount].numNewPhotons;

                hitPoints[hitCount].indirectColor = Color3f(hitPoints[hitCount].newColor.x / hitPoints[hitCount].numNewPhotons,
                                                    hitPoints[hitCount].newColor.y / hitPoints[hitCount].numNewPhotons,
                                                    hitPoints[hitCount].newColor.z / hitPoints[hitCount].numNewPhotons);
                hitPoints[hitCount].density = hitPoints[hitCount].numPhotons/(Pi * std::pow(hitPoints[hitCount].radius,2));
                }
            }
            //more than 1 photon traces
            else
            {
                if(hitPoints[hitCount].numNewPhotons!=0)
                {
                    //radius shrinking
                    float lastRadius = hitPoints[hitCount].radius;
                    int tempPhotonAmount = hitPoints[hitCount].numNewPhotons + hitPoints[hitCount].numPhotons;
                    hitPoints[hitCount].numPhotons += std::floor(hitPoints[hitCount].numNewPhotons * alpha);

                    float tempPhotonRatio = hitPoints[hitCount].numPhotons/tempPhotonAmount;
                    hitPoints[hitCount].radius = lastRadius * std::sqrt(tempPhotonRatio);

                    //flux correction
                    Color3f averageColor = Color3f(hitPoints[hitCount].newColor.x / hitPoints[hitCount].numNewPhotons,
                                                   hitPoints[hitCount].newColor.y / hitPoints[hitCount].numNewPhotons,
                                                   hitPoints[hitCount].newColor.z / hitPoints[hitCount].numNewPhotons);
                    hitPoints[hitCount].indirectColor += averageColor * tempPhotonRatio;
                }

            }
        }

        //reinitialize
        hitPoints[hitCount].numNewPhotons = 0;
        hitPoints[hitCount].newColor = Color3f(0.f);
    }
}
