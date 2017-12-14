#pragma once
#include <QRunnable>
#include <raytracing/film.h>
#include <scene/camera.h>
#include <scene/scene.h>
#include <globals.h>
#include <samplers/sampler.h>
#include <scene/lights/light.h>
#include <scene/photon.h>
#include <warpfunctions.h>
#include <scene/kdtree.h>
#include <scene/progressivekdtree.h>
#include <scene/progressivephoton.h>


// An interface for rendering scenes by evaluating the Light Transport Equation
class Integrator : public QRunnable
{
public:
    Integrator(Bounds2i bounds, Scene* s, std::shared_ptr<Sampler> sampler, int recursionLimit)
        : scene(s), camera(&(s->camera)), film(&(s->film)), sampler(sampler), realCamera(&(s->realCamera)),
          bounds(bounds), recursionLimit(recursionLimit)
    {
        ClampBounds();
    }

    virtual ~Integrator(){}

    virtual void run();     // A function declared in the QRunnable class that we must implement
                            // in order for our class to be compatible with a QThreadPool.
                            // This function should iterate over all the pixels this Integrator
                            // is supposed to handle and trace a ray through each of them, then
                            // set the pixel of the film to the correct color.

    virtual void Render(); // Called from run() or from MyGL depending on whether we're multithreading our render.


    // Evaluate the energy transmitted along the ray back to
    // its origin, e.g. the camera or an intersection in the scene
    virtual Color3f Li(const Ray& ray, const Scene& scene, std::shared_ptr<Sampler> sampler, int depth, Color3f throughputColor,KdTree tree) const = 0;

    void PhotonTracing(const Scene& scene, std::shared_ptr<Sampler> sampler, const int depth);
    void cachePhotonColor(const Ray &r, const Scene &scene, int depth, const Color3f lightColor);

    // Clamp the upper end of our bounds to not go past the edge of the film.
    void ClampBounds();


    //specially for Progressive Photon Mapping
    void ProgressiveRayTracing(Ray cameraRay, const Scene& scene, const Point2i pixel, std::shared_ptr<Sampler> sampler, const int depth, QList<PixelHitPoint> &progHitPoint);
    void DirectLightingTraceForProPhotonMapping(const Scene& scene, std::shared_ptr<Sampler> sampler, PixelHitPoint& hitPoint);
    void ProgressiveKdTree(ProgressiveKdNode *root, PixelHitPoint hitPoint, int linearIndex);
    virtual void TraceProgressivePhotons(const Scene& scene, ProgressiveKdNode* root,std::shared_ptr<Sampler> sampler, int depth, int numPhotons, QList<PixelHitPoint>& hitPoints);

protected:
    Scene * scene;
    Camera const * const camera;			// A pointer to the Camera instance stored in MyGL.
    RealisticCamera const * const realCamera;
    Film * const film;						// A pointer to the Film instance stored in MyGL.
    std::shared_ptr<Sampler> sampler;       // A pointer to the Sampler that we will use to generate pixel samples for this thread.

//****************************************Used in Photon Mapping********************************
    QList<Photon> photonMap;
    KdTree tree;
//*******************************End of used in Photon Mapping***********************************

//*************************************Used in Progressive Photon Mapping***************************************
    //QList<PixelHitPoint> progHitPoint;//the size is equal to the total size of the pixels
    //ProgressiveKdNode* rootProg;
//*****************************************End of Use in Progressive Photon Mapping*******************

    Bounds2i bounds;                  // The min and max bounds of the film to which this thread renders
    int recursionLimit;

    const int totalNumPhoton = 200000;
    const int traceTimes = 4;

};
