#ifndef KDTREE_H
#define KDTREE_H

#include <scene/photon.h>

enum SplitAxis
{
    UNKNOWN_AXIS = -1,
    X_AXIS = 0,
    Y_AXIS = 1,
    Z_AXIS = 2
};

struct KdNode{
    KdNode* children1;
    KdNode* children2;
    SplitAxis splitAxis;
    Point3f position;
    PhotonType photonType;
    Color3f photonColor;

    KdNode(Photon &p, PhotonType type, Color3f photonColor): children1(nullptr), children2(nullptr), splitAxis(UNKNOWN_AXIS), position(p.position),
    photonType(type), photonColor(photonColor){}

};

class KdTree{
   public:
    KdNode *root;

    KdNode* NewNode(Photon ph)
    {
        KdNode* temp = new KdNode(ph,ph.photonType,ph.power);
         temp->children1 = temp->children2 = nullptr;
         return temp;
    }

    KdNode* InsertRec(KdNode* root, Photon ph,int depth)
    {
        if(root == nullptr)
        {
            return NewNode(ph);
        }
        SplitAxis cd = chooseSplitAxis(root,depth);

        if(ph.position[cd] < root->position[cd])
        {
            root->children1 = InsertRec(root->children1,ph,depth + 1);
        }
        else
        {
            root->children2 = InsertRec(root->children2,ph,depth + 1);
        }
        return root;
    }

    KdNode* Insert(KdNode* root, Photon ph)
    {
        return InsertRec(root,ph,0);
    }

//    KdNode* Insert(Photon ph, KdNode *node)
//    {
//        if(node == nullptr)
//        {
//            node = new KdNode(ph,ph.photonType,ph.power);
//        }
//        else
//        {
//            node->splitAxis = chooseSplitAxis(node);
//            SplitAxis cd = node->splitAxis;
//            if(ph.position[cd] < node->position[cd])
//            {
//                node->children1 = Insert(ph,node->children1);
//            }
//            else
//            {
//                node->children2 = Insert(ph,node->children2);
//            }
//        }
//        return node;
//    }


    SplitAxis chooseSplitAxis(KdNode* node1, int depth)
    {
        SplitAxis splitAxis;
//        Point3f position = node1->position;
//        //find the x, y, z position that closest to the middle point
//        float dis1 = std::abs(position.x);
//        float dis2 = std::abs(position.y);
//        float dis3 = std::abs(position.z);

//        if((dis1<=dis2)&&(dis1<=dis3))
//        {
//            splitAxis = X_AXIS;
//        }
//        if((dis2<=dis1)&&(dis2<=dis3))
//        {
//            splitAxis = Y_AXIS;
//        }
//        if((dis3<=dis1)&&(dis3<=dis2))
//        {
//            splitAxis = Z_AXIS;
//        }
        splitAxis = SplitAxis(depth%Axis_Dimension);
        if(splitAxis<0)
        {
            splitAxis = X_AXIS;
        }
        if(splitAxis>2)
        {
            splitAxis = Z_AXIS;
        }
        return splitAxis;
    }

    void NearPhotons(Point3f position, KdNode* node, QList<KdNode*> nodeList)
    {
        float distance = glm::length(position-node->position);
        //does not meet the itself

        if((distance>0)&&(distance<=search_radius))
        {
            nodeList.push_back(node);
        }

        SplitAxis split = node->splitAxis;
        if(glm::length(position - node->position) > search_radius)
        {
            if(position[split] <= node->position[split])
            {
                NearPhotons(position,node->children1,nodeList);
            }
            else
            {
                NearPhotons(position,node->children2,nodeList);
            }
        }
        else
        {
            NearPhotons(position,node->children1,nodeList);
            NearPhotons(position,node->children2,nodeList);
        }

    }


};











#endif // KDTREE_H
