#include "AgentDynamic.h"
#include <math.h>


AgentDynamic::AgentDynamic()
{
    Mass            = 1.0;
    Velocity        = 0.0;
    Acceleration    = 0.0;
    Radius          = 2.0;
}

AgentDynamic::~AgentDynamic()
{
    //dtor
}

bool AgentDynamic::SphericalVolumeCollision( Vec3f &in_position, float in_radius )
{
    if ( (Radius + in_radius) >= distance(Position, in_position) )
    {
        return true;
    }
    else
    {
        return false;
    }
}

float AgentDynamic::FindAngleBetweenVectors( Vec3f &in_position )
{
    Position.normalize();
    in_position.normalize();

    return 0;//acos(dot(posA, posB));
}

void AgentDynamic::UpdateAI( nvmath::Vec3f &in_position )
{

}

void AgentDynamic::SteerPursue( Vec3f &in_position )
{
    FindAngleBetweenVectors( in_position );

    GetPosition();
    in_position.normalize();
    nvmath::Vec3f pos(Position + (in_position / 100));
    SetPosition( pos );
    //distance(Position, in_position);
}

void AgentDynamic::SteerArrive( Vec3f &in_position, float in_radius )
{
    GetPosition();

    if ( distance(Position, in_position) > 0.01 )
    {
    }
}

void AgentDynamic::SteerFlee( Vec3f &in_position )
{
    FindAngleBetweenVectors( in_position );
}

void AgentDynamic::SetPosition( Vec3f &in_position )
{
    TrafoTransform.setTranslation( in_position );

    {
        nvsg::TransformWriteLock TransformWrite( TransformPtr );
        TransformWrite->setTrafo( TrafoTransform );
    }

    Position = in_position;
}

void AgentDynamic::SetRotation( Quatf &in_rotation )
{
    TrafoTransform.setOrientation( in_rotation );

    {
        nvsg::TransformWriteLock TransformWrite( TransformPtr );
        TransformWrite->setTrafo( TrafoTransform );
    }

    Rotation = in_rotation;
}

void AgentDynamic::SetScale( Vec3f &in_scale )
{
    TrafoTransform.setScaling( in_scale );

    {
        nvsg::TransformWriteLock TransformWrite( TransformPtr );
        TransformWrite->setTrafo( TrafoTransform );
    }

    Scale = in_scale;
}

//void AgentDynamic::SetTransformPtr( nvsg::TransformSharedPtr in_transform_ptr, const nvsg::NodeSharedPtr &node )
void AgentDynamic::SetTransformPtr( nvsg::TransformSharedPtr in_transform_ptr )
{
    TransformPtr = nvsg::Transform::create();
    TransformPtr = in_transform_ptr;
    GetPosition();
    GetRotation();
    GetScale();
}

Vec3f AgentDynamic::GetPosition()
{
    nvsg::TransformReadLock TransformRead( TransformPtr );
    Position = TransformRead->getTranslation();

    return Position;
}

Quatf AgentDynamic::GetRotation()
{
    nvsg::TransformReadLock TransformRead( TransformPtr );
    Rotation = TransformRead->getOrientation();

    return Rotation;
}

Vec3f AgentDynamic::GetScale()
{
    nvsg::TransformReadLock TransformRead( TransformPtr );
    Scale = TransformRead->getScaling();

    return Scale;
}

nvsg::TransformSharedPtr AgentDynamic::GetTransformPtr()
{
   return TransformPtr;
}

