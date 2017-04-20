#ifndef PHOTON_H
#define PHOTON_H

#include "globals.h"

enum PhotonType
{
    UNKNOWN = -1,
    DIRECT = 0,
    CAUSTIC = 1,
    GLOBALINDIRECT = 2
};

class Photon
{
public:
    Photon(const Point3f &position,const Color3f &power, const Vector3f direction) :
        position(position), power(power), direction(direction), flag(0), photonType(UNKNOWN){}

    Point3f position;
    Color3f power;

    Vector3f direction;
    PhotonType photonType;

    int flag;//flag used in kd-tree
};

#endif // PHOTON_H
