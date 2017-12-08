#include "integrator.h"


void Integrator::run()
{
    Render();
}

//void Integrator::TraceProgressivePhotons(const Scene &scene, ProgressiveKdNode *root, std::shared_ptr<Sampler> sampler, int depth, int numPhotons, QList<PixelHitPoint> &hitPoints)
//{
//    return;
//}

void Integrator::Render()
{
    //******************************Defined for Progressive Photon Mapping*********
    QList<PixelHitPoint> progHitPoint;//the size is equal to the total size of the pixels
    ProgressiveKdNode* rootProg = nullptr;
    //*****************************End of Progressive Photon Mapping design******

    // Compute the bounds of our sample, clamping to screen's max bounds if necessary
    // Instantiate a FilmTile to store this thread's pixel colors
    std::vector<Point2i> tilePixels = bounds.GetPoints();

    //***************************specially used in progressive photon mapping*************
    int indexCount = 0;
    for(Point2i pix :tilePixels)
    {
        Ray ray = camera->Raycast(pix);
        //Ray ray = realCamera->Raycast(pix,sampler);
        ProgressiveRayTracing(ray, *scene, pix, sampler,recursionLimit, progHitPoint);
        rootProg = rootProg->InsertProgressiveKdTree(rootProg,progHitPoint[indexCount],indexCount);
        indexCount++;
    }
    //**************************************end of progressive photon mapping *******************

    //*****************specially used in photon mapping************************
    //PhotonTracing(*scene,sampler,recursionLimit);
    //std::cout<<"finish1"<<std::endl;
    //if(photonMap.size()!=0)
    //{
        //tree.root = new KdNode(photonMap[0],photonMap[0].photonType,photonMap[0].power);
//        tree.root->splitAxis = tree.chooseSplitAxis(tree.root,0);
        //for(int i = 1 ; i < photonMap.size();++i)
        //{
//            KdNode* tempNode;
//            tempNode = tree.Insert(photonMap[i],tree.root);
            //KdNode* tempNode = tree.Insert(tree.root,photonMap[i]);
        //}
    //}
    //std::cout<<"finish2"<<std::endl;
    //********************************************end of photon mapping **********************************


    //******************************************For Path Tracing and Photon Mapping***********************************
    // For every pixel in the FilmTile:
    //for(Point2i pixel : tilePixels)
    //{
        //Uncomment this to debug a particular pixel within this tile
//        if(pixel.x != 404 || pixel.y != 418)
//        {
//            continue;
//        }
        //Color3f pixelColor(0.f);
        // Ask our sampler for a collection of stratified samples, then raycast through each sample
        //std::vector<Point2f> pixelSamples = sampler->GenerateStratifiedSamples();
        //for(Point2f sample : pixelSamples)
        //{
            //sample = sample + Point2f(pixel); // _sample_ is [0, 1), but it needs to be translated to the pixel's origin.
            //Generate a ray from this pixel sample

      //************************For pinhole camera**************************//
            //Ray ray = camera->Raycast(sample);

     //************************For realistic camera*****************************//
     //            Ray ray = realCamera->Raycast(sample,sampler);
            // Get the L (energy) for the ray by calling Li(ray, scene, tileSampler, arena)
            // Li is implemented by Integrator subclasses, like DirectLightingIntegrator
            //Color3f L = Li(ray, *scene, sampler, recursionLimit, Color3f(1.0f), tree);
            // Accumulate color in the pixel
            //pixelColor += L;
        //}
        // Average all samples' energies
        //pixelColor /= pixelSamples.size();
        //int pixelIndex = pixel.x + pixel.y * camera->height;
        //pixelColor = progHitPoint[pixelIndex].color;
        //film->SetPixelColor(pixel, glm::clamp(pixelColor, 0.f, 1.f));
    //}

    //We're done here! All pixels have been given an averaged color.
    //do deletion
//    for(auto mapPhs : photonMap){
//        free(mapPhs);
//    }
//    photonMap.clear();
//    tree.TreeDelete(tree.root);
    //************************************************End of Path tracing and Photon Mapping********************************



    //*****************************For Progressive Photon Mapping******************************************

    //Kd tree construction part here //////////
    //*************************************************//
    for(int trace = 0;trace<traceTimes;trace++)
    {
        int photonsToTrace = ceil(totalNumPhoton / traceTimes);
        TraceProgressivePhotons(*scene, rootProg, sampler, recursionLimit, photonsToTrace, progHitPoint);

        for(int index = 0;index <tilePixels.size();index++)
        {
            //int pixelIndex = px.x + camera->height * px.y;
            Color3f pixelColor = progHitPoint[index].color;
            Point2i pixelNum = progHitPoint[index].pixel;
            film->SetPixelColor(pixelNum, glm::clamp(pixelColor, 0.f, 1.f));
        }
    }
    rootProg->TreeDeleteProg(rootProg);
    //*************************************End of Progressive Photon Mapping***********************************
}


void Integrator::ClampBounds()
{
    Point2i max = bounds.Max();
    max = Point2i(std::min(max.x, film->bounds.Max().x), std::min(max.y, film->bounds.Max().y));
    bounds = Bounds2i(bounds.Min(), max);
}

//For Photon mapping
void Integrator::PhotonTracing(const Scene &scene, std::shared_ptr<Sampler> sampler, const int depth)
{
    int lightsTotal = scene.lights.size();// the number of lights of the scene
    int numberPhoton[lightsTotal];//the number of the photons of every lights
    int totalIntensity = 0;

    //preprocess of the weight of the photon
    for(int i= 0 ; i < lightsTotal ; i++)
    {
        totalIntensity = totalIntensity + scene.lights[i]->emittedLight[0] + scene.lights[i]->emittedLight[1]+ scene.lights[i]->emittedLight[2];
    }

    for(int j =0 ; j < lightsTotal ; j++)
    {
        numberPhoton[j] = totalPhoton * (scene.lights[j]->emittedLight.x + scene.lights[j]->emittedLight.y + scene.lights[j]->emittedLight.z)/totalIntensity;
    }

    //calculate the number of photons shot from every ligths
    for(int k = 0 ; k < lightsTotal ; k++)
    {
        std::shared_ptr<Light> chosenLight = scene.lights[k];

        for(int tempLightCount = 0 ; tempLightCount < numberPhoton[k] ; tempLightCount++)
        {
            Ray lightRay = chosenLight->EmitSampleLight(sampler);
            cachePhotonColor(lightRay,scene,depth,chosenLight->emittedLight);
        }
    }
}

//For Photon mapping
void Integrator::cachePhotonColor(const Ray &r, const Scene &scene, int depth, const Color3f lightColor)
{
    Intersection isec;
    bool specularBounce = false;

    bool lastSpecularBounce = false;
    BxDFType typeBxdf;
    Ray newRay = r;
    Color3f currentColor = lightColor;

    while(depth>0)
    {
        isec = Intersection();//initialize the intersection
        Vector3f woW = -newRay.direction;
        Vector3f wiW;
        float currentPdf;

        if(scene.Intersect(newRay,&isec))
        {
            if(!isec.ProduceBSDF())
            {
                return;
            }
            Color3f fColor = isec.bsdf->Sample_f(woW,&wiW,sampler->Get2D(),&currentPdf,BSDF_ALL,&typeBxdf);
            specularBounce = false;
            if((typeBxdf & BSDF_SPECULAR)!=0)
            {
                specularBounce = true;
            }
            //we do not do any direct mapping, so avoid first bounce and specular surfaces
            if((depth == recursionLimit)||specularBounce)
            {

                if(specularBounce)
                {
                    currentColor = fColor * currentColor * AbsDot(wiW,isec.bsdf->normal);
                    newRay = Ray(isec.point,wiW);
                }
                else
                {
                    currentColor = fColor * currentColor * AbsDot(wiW,isec.bsdf->normal);
                    newRay = Ray(isec.point,wiW);
                }
                //spawnray
            }
            else
            {
                //last bounce is the specular bounce, we should cache the caustic photon
                if(lastSpecularBounce)
                {
                    Color3f cacheColor = fColor * currentColor * AbsDot(wiW,isec.bsdf->normal);
                    currentColor = cacheColor;
                    Photon ph = Photon(isec.point,cacheColor,wiW,CAUSTIC);
                    photonMap.push_back(ph);
                    newRay = Ray(isec.point,wiW);
                }
                //if last bounce is not specular bounce, we should cache the global illumination photon
                else
                {
                    Color3f cacheColor = fColor * currentColor *AbsDot(wiW, isec.bsdf->normal);
                    currentColor = cacheColor;
                    Photon ph = Photon(isec.point,cacheColor,wiW,GLOBALINDIRECT);
                    photonMap.push_back(ph);
                    newRay = Ray(isec.point,wiW);
                }

            }

            //Russian Roulette
            float uniformRandom = sampler->Get1D();
            float maxInThroughput = std::max(std::max(currentColor.x,currentColor.y),currentColor.z);
            if((recursionLimit - depth)>3)
            {
                //what should we do to absorb the color?
                if(uniformRandom > maxInThroughput)
                {
                    break;
                }
            }
            lastSpecularBounce = specularBounce;
            depth--;

        }
        else
        {
            break;
        }
    }

}


//for progressive photon mapping
void Integrator::ProgressiveRayTracing(Ray cameraRay, const Scene& scene, const Point2i pixel, std::shared_ptr<Sampler> sampler, const int depth, QList<PixelHitPoint> &progHitPoint)
{
    Intersection isec = Intersection();
    //cameraRay = scene.camera.Raycast((float)pixel.x,(float)pixel.y);
    Ray currentRay = cameraRay;
    int dep = depth;

    //shoot ray into the scene
    while(dep>0)
    {
        //the ray hits something
        if(scene.Intersect(currentRay,&isec))
        {
            //hits the light sources
            if(!isec.ProduceBSDF())
            {
                PixelHitPoint tempHitPoint;
//                tempHitPoint.isec = isec;
                tempHitPoint.ray = currentRay;
                tempHitPoint.color = isec.Le(-currentRay.direction);
                tempHitPoint.pixel = pixel;
                tempHitPoint.position = isec.point;
                progHitPoint.push_back(tempHitPoint);
                return;
            }
            else
            {
                Vector3f woW = -currentRay.direction;
                Vector3f wiW;
                BxDFType typeBxdf;
                float currentPdf;
                Color3f fColor = isec.bsdf->Sample_f(woW,&wiW,sampler->Get2D(),&currentPdf,BSDF_ALL,&typeBxdf);

                //specular bounce
                if((typeBxdf & BSDF_SPECULAR)!=0)
                {
                    dep--;
                    currentRay = Ray(isec.point,wiW);
                }
                //nonspecular bounce
                else
                {
                    PixelHitPoint tempHitPoint;
                    tempHitPoint.isec = isec;
                    tempHitPoint.ray = currentRay;
                    //tempHitPoint.color = isec.Le(-currentRay.direction);
                    tempHitPoint.color = Color3f(1.0,0.0,0.0);
                    tempHitPoint.pixel = pixel;
                    tempHitPoint.position = isec.point;
                    progHitPoint.push_back(tempHitPoint);
                    //go out of the loop
                    return;
                }
            }
        }
        //the ray does not hit anything
        else
        {
            PixelHitPoint tempHitPoint;
            //tempHitPoint.isec = isec;
            //tempHitPoint.ray = currentRay;
            tempHitPoint.color = Color3f(0.0f);
            tempHitPoint.pixel = pixel;
            tempHitPoint.position = Point3f((float)-INFINITY);
            progHitPoint.push_back(tempHitPoint);
            return;
        }
    }
}

void Integrator::ProgressiveKdTree(ProgressiveKdNode* root, PixelHitPoint hitPoint, int linearIndex)
{
    root = root->InsertProgressiveKdTree(root,hitPoint,linearIndex);
}

//implemented inside ProgressivePhotonMapping
//not necessary to do anything here
void Integrator::TraceProgressivePhotons(const Scene& scene, ProgressiveKdNode* root,std::shared_ptr<Sampler> sampler, int depth, int numPhotons, QList<PixelHitPoint>& hitPoints)
{
    return;
}
