#ifndef PROGRESSIVEPHOTON_H
#define PROGRESSIVEPHOTON_H


#include <globals.h>
#include "scene.h"
#include <raytracing/ray.h>
#include <raytracing/intersection.h>

//Specially for Progressive Photon Mapping

struct PixelHitPoint{

    Intersection isec;//intersection in the scene contains all of the information
    Ray ray; //the coming firection of ray
    Point2i pixel; //the corresponding pixel of the hitpoint
    Point3f position; //the hitpoint position in world coordinate in the scene
    int numPhotons = 0; //the number of photons
    int numNewPhotons = 0; //the number of new Photons
    Color3f color;//the color to color the exact pixel finally
    Color3f newColor = Color3f(0.0f);//the tempColor storage for flux correction
    Color3f indirectColor = Color3f(0.0f);
    float radius = 0.375f;// the radius that we will check for the final color result
    float density = 1.0f; //for shrinking the radius for better results
//    Spectrum spec;
//    float intensity;

    PixelHitPoint() : isec(Intersection()),ray(Ray(Point3f(0.0f),Vector3f(0.0f))){}
};

//Specially for Progressive Photon Mapping
struct ProgressivePhoton{
    Ray ray;//the coming ray
    Point3f hitPoint;//the photon intersection point at last
    Color3f color;//photon Color;
    //    Spectrum spec;
    //    float intensity;
};

#endif // PROGRESSIVEPHOTON_H


