#ifndef PHOTON_H
#define PHOTON_H

#include "globals.h"

class Photon
{
public:
    Photon();
    Photon(const Point3f &position) : position(position),
        phi(255*(atan2(position.y,position.x)+Pi)/(2*Pi)),
        theta(255*(acos(position.x))/Pi){}

    Point3f position;
    Color3f power;
    float phi, theta;//compressed incident direction
    int flag;//flag used in kd-tree
};

#endif // PHOTON_H
