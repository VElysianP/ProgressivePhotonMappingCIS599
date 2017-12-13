#include <globals.h>
#include <scene/progressivephoton.h>

struct ProgressiveKdNode{
    ProgressiveKdNode* chid0;
    ProgressiveKdNode* chid1;
    SplitAxis axis;
    PixelHitPoint hitPoint;
    int hitPointLinearIndex;
    int depth; //shows the depth of the kdtree, starts from 0
    //Point3f hitPosition;//the hit position in world space inside the scene

    ProgressiveKdNode(PixelHitPoint hitPoint) :
        hitPoint(hitPoint), axis(UNKNOWN_AXIS), chid0(nullptr),chid1(nullptr),depth(0){}

    ProgressiveKdNode* NewNodeProg(PixelHitPoint hitPoint, int depth, int linearIndex)
    {
        ProgressiveKdNode* temp = new ProgressiveKdNode(hitPoint);
        int axisNum = depth - 3 * std::floor(depth/3);
        if(axisNum == 0)
        {
            temp->axis = X_AXIS;
        }
        if(axisNum == 1)
        {
            temp->axis = Y_AXIS;
        }
        if(axisNum == 2)
        {
            temp->axis = Z_AXIS;
        }
        temp->depth = depth;
        temp->hitPointLinearIndex = linearIndex;
        return temp;
    }

    //the most important part!
    //? currently not implemented
    SplitAxis ChooseSplitAxisProg(ProgressiveKdNode* node2, int depth)
    {
        if(depth == 0)
        {

        }
    }

    //the insertRec will start from the root and then do resursion
    ProgressiveKdNode* InsertRecProg(ProgressiveKdNode* root, PixelHitPoint hitPoint, int depth, int linearIndex)
    {
        //if the node is empty
        if(root == nullptr)
        {
            return NewNodeProg(hitPoint,depth, linearIndex);
        }
        //SplitAxis cd = ChooseSplitAxis(root,depth);
        SplitAxis cd = root->axis;

        if(hitPoint.position[cd] <= root->hitPoint.position[cd])
        {
            root->chid0 = InsertRecProg(root->chid0, hitPoint, depth+1, linearIndex);
        }
        else
        {
            root->chid1 = InsertRecProg(root->chid1, hitPoint, depth+1, linearIndex);
        }
        return root;
    }

//For each hitpoint, start lnserting from the root and then search for the tree and find
    //the proper place to insert the hitPoint
    ProgressiveKdNode* InsertProgressiveKdTree(ProgressiveKdNode* root, PixelHitPoint hitPoint,int linearIndex)
    {
        return InsertRecProg(root,hitPoint,0,linearIndex);
    }

    //search for the nearest hitpoint for each photons
    //only returns the linear index of the nearest hitpoint
    //or returns -1 means not proper hitPoint found
    int NearHitPointProg(Point3f position, ProgressiveKdNode* root)
    {
        if(root == nullptr)
        {
            return -1;
        }
        else
        {
            float dis = glm::length(position - root->hitPoint.position);
            if(dis <= root->hitPoint.radius)
            {
                return root->hitPointLinearIndex;
            }
            else
            {
                SplitAxis axis = root->axis;
                if(position[axis] <= root->hitPoint.position[axis])
                {
                    return NearHitPointProg(position, root->chid0);
                }
                else
                {
                    return NearHitPointProg(position, root->chid1);
                }
            }

        }
    }

    void TreeDeleteProg(ProgressiveKdNode* root)
    {
        if((root->chid0 == nullptr)&&(root->chid1 == nullptr))
        {
            free(root);
            return;
        }
        else
        {
            if(root->chid0!= nullptr)
            {
                TreeDeleteProg(root->chid0);
            }
            else
            {
                TreeDeleteProg(root->chid1);
            }
        }
    }

};






