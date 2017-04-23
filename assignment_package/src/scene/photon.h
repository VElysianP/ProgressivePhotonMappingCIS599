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

struct Photon{
        Point3f position;
        Color3f power;

        Vector3f direction;
        PhotonType photonType;

        Photon(const Point3f position, const Color3f power, const Vector3f direction, const PhotonType type):
            position(position), power(power), direction(direction),photonType(type){}
    };



#endif // PHOTON_H
