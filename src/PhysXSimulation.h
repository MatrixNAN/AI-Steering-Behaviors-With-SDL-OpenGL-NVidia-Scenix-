#include "NxVec3.h"

// Physics SDK globals
NxPhysicsSDK* gPhysicsSDK = NULL;
NxScene*  gScene = NULL;
NxVec3  gDefaultGravity(0,-9.8,0);

void InitPhysX()
{
    // Create the physics SDK
    gPhysicsSDK = NxCreatePhysicsSDK(NX_PHYSICS_SDK_VERSION);
    if (!gPhysicsSDK)  return;

    // Create the scene
    NxSceneDesc sceneDesc;
    sceneDesc.gravity                      = gDefaultGravity;
    gScene = gPhysicsSDK->createScene(sceneDesc);
}



void main(int argc, char** argv)
{
    InitPhysX();
}

