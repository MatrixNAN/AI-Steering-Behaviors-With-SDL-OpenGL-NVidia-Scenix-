
NxActor* CreateBox()
{
    // Set the box starting height to 3.5m so box starts off falling onto the ground
    NxReal boxStartHeight = 3.5;



    // Add a single-shape actor to the scene
    NxActorDesc actorDesc;
    NxBodyDesc bodyDesc;



    // The actor has one shape, a box, 1m on a side
    NxBoxShapeDesc boxDesc;
    boxDesc.dimensions.set(0.5,0.5,0.5);
    actorDesc.shapes.pushBack(&boxDesc);

    actorDesc.body = &bodyDesc;
    actorDesc.density = 10;
    actorDesc.globalPose.t = NxVec3(0,boxStartHeight,0);
    return gScene->createActor(actorDesc);
}

NxActor* CreateGroundPlane()
{
    // Create a plane with default descriptor
    NxPlaneShapeDesc planeDesc;
    NxActorDesc actorDesc;
    actorDesc.shapes.pushBack(&planeDesc);
    return gScene->createActor(actorDesc);
}
