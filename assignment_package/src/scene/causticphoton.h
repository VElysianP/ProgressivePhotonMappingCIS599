#ifndef CAUSTICPHOTON_H
#define CAUSTICPHOTON_H

#include "globals.h"
#include "photon.h"

class CausticPhoton : public Photon
{
public:
    CausticPhoton(const Point3f &position,const Color3f &power, const Vector3f direction) :
        Photon(position,power,direction){photonType(CAUSTIC);}


};

#endif // CAUSTICPHOTON_H
