#ifndef PROGRESSIVEKDTREE_H
#define PROGRESSIVEKDTREE_H

#include <globals.h>
#include <scene/progressivephoton.h>

struct ProgressiveKdNode{
    ProgressiveKdNode* chid0;
    ProgressiveKdNode* chid1;
    SplitAxis axis;
    PixelHitPoint hitPoint;

    ProgressiveKdNode(PixelHitPoint hitPoint) :
        hitPoint(hitPoint), axis(UNKNOWN_AXIS), chid0(nullptr),chid1(nullptr){}
};

class ProgressiveKdTree
{
public:
    ProgressiveKdNode rootProg;


    ProgressiveKdTree(PixelHitPoint hitPoint) : rootProg(ProgressiveKdNode(hitPoint)) {}
};

#endif // PROGRESSIVEKDTREE_H
