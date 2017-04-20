#include "integrator.h"


void Integrator::run()
{
    Render();
}

void Integrator::Render()
{
    // Compute the bounds of our sample, clamping to screen's max bounds if necessary
    // Instantiate a FilmTile to store this thread's pixel colors
    std::vector<Point2i> tilePixels = bounds.GetPoints();


    //*****************specially used in photon mapping************************
    PhotonTracing(scene,sampler,recursionLimit);


    // For every pixel in the FilmTile:
    for(Point2i pixel : tilePixels)
    {
        //Uncomment this to debug a particular pixel within this tile
//        if(pixel.x != 247 || pixel.y != 438)
//        {
//            continue;
//        }
        Color3f pixelColor(0.f);
        // Ask our sampler for a collection of stratified samples, then raycast through each sample
        std::vector<Point2f> pixelSamples = sampler->GenerateStratifiedSamples();
        for(Point2f sample : pixelSamples)
        {
            sample = sample + Point2f(pixel); // _sample_ is [0, 1), but it needs to be translated to the pixel's origin.
            // Generate a ray from this pixel sample



//************************For pinhole camera**************************//
            Ray ray = camera->Raycast(sample);

//************************For realistic camera*****************************//
//            Ray ray = realCamera->Raycast(sample,sampler);


            // Get the L (energy) for the ray by calling Li(ray, scene, tileSampler, arena)
            // Li is implemented by Integrator subclasses, like DirectLightingIntegrator
            Color3f L = Li(ray, *scene, sampler, recursionLimit,Color3f(1.0f));
            // Accumulate color in the pixel
            pixelColor += L;
        }
        // Average all samples' energies
        pixelColor /= pixelSamples.size();
        film->SetPixelColor(pixel, glm::clamp(pixelColor, 0.f, 1.f));
    }
    //We're done here! All pixels have been given an averaged color.
}


void Integrator::ClampBounds()
{
    Point2i max = bounds.Max();
    max = Point2i(std::min(max.x, film->bounds.Max().x), std::min(max.y, film->bounds.Max().y));
    bounds = Bounds2i(bounds.Min(), max);
}

void Integrator::PhotonTracing(const Scene &scene, std::shared_ptr<Sampler> sampler, const int depth)
{
    int lightsTotal = scene.lights.size();// the number of lights of the scene
    float totalLightColor;//the total R+G+B float color of all the lights in the scene
    float photonWeight[lightsTotal];//the R+G+B float color of light sources
    int numberPhoton[lightTotal];//the number of the photons of every lights

    //preprocess of the weight of the photon
    for(int i= 0 ; i < lightsTotal ; i++)
    {
        totalLightColor = totalLightColor + scene.lights[i]->emittedLight.x + scene.lights[i]->emittedLight.y + scene.lights[i]->emittedLight.z;
        photonWeight[i] = scene.lights[i]->emittedLight.x + scene.lights[i]->emittedLight.y + scene.lights[i]->emittedLight.z;
    }


    //calculate the number of photons shot from every ligths
    for(int j = 0 ; j < lightsTotal ; j++)
    {
        numberPhoton[j] = std::floor(totalPhoton * photonWeight[j]/totalLightColor);

        shared_ptr<Light> chosenLight = scene.lights[j];

        for(int tempLightCount = 0 ; tempLightCount < numberPhoton[j] ; tempLightCount++)
        {
            if(chosenLight->lightType == DIFFUSEAREALIGHT)
            {
                Point2f sampleOnLight = sampler->Get2D();
                Point3f pointOnLight = Point3f(chosenLight->transform.T()*glm::vec4(sampleOnLight.x - 0.5,sampleOnLight.y - 0.5,0.0f,1.0f));

                Point2f sampleRayOut = sampler->Get2D();
                Point3f pointOnLightBefore = WarpFunctions::squareToHemisphereCosine(sampleRayOut);
                Point3f pointRayOut = Point3f(chosenLight->transform.T()*glm::vec4(pointOnLightBefore.x,pointOnLightBefore.y,pointOnLightBefore.z,1.0f));

                Ray r = Ray(pointOnLight,glm::normalize(pointRayOut - PointOnLight));


                cachePhotonColor(r,scene,depth,chosenLight->emittedLight);
           }
            if(chosenLight->lightType == SPOTLIGHT)
            {
                Point2f sampleRayOut = sampler->Get2D();
                Point3f pointOnLightBefore = WarpFunctions::squareToDiskConcentric(sampleRayOut);
                //to match the point light function I have written, set the disk into xoz plane

                pointOnLightBefore.z = pointOnLightBefore.y;
                pointOnLightBefore.y = -1.f;//test on this value

                Point3f pointRayOut = Point3f(chosenLight->transform.T()*glm::vec4(pointOnLightBefore.x,pointOnLightBefore.y,pointOnLightBefore.z,1.0f));
                Ray r =Ray(chosenLight->pLight, glm::normalize(pointRayOut - chosenLight->pLight));

                cachePhotonColor(r,scene,depth,chosenLight->emittedLight);

            }
            //add up other light types
        }
    }
}


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
        float *currentPdf;

        if(scene.Intersect(newRay,&isec))
        {
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
                    shared_ptr<CausticPhoton> photonCache(new CausticPhoton(isec.point, cacheColor, wiW));
                    scene.scenePhoton.push_back(photonCache);
                    newRay = Ray(isec.point,wiW);
                }
                //if last bounce is not specular bounce, we should cache the global illumination photon
                else
                {
                    Color3f cacheColor = fColor * currentColor *AbsDot(wiW, isec.bsdf->normal);
                    currentColor = cacheColor;
                    shared_ptr<GlobalIlluminationPhoton> photonCache(new GlobalIlluminationPhoton(isec.point,cacheColor,wiW));
                    scene.scenePhoton.push_back(photonCache);
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
