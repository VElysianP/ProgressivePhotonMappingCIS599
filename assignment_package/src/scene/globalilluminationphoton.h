#ifndef GLOBALILLUMINATIONPHOTON_H
#define GLOBALILLUMINATIONPHOTON_H

#include "globals.h"
#include "photon.h"

class GlobalIlluminationPhoton : public Photon
{
public:
    GlobalIlluminationPhoton(const Point3f &position,const Color3f &power, const Vector3f direction)
        :Photon(position,power,direction){photonType(GLOBALINDIRECT);}

};

#endif // GLOBALILLUMINATIONPHOTON_H
