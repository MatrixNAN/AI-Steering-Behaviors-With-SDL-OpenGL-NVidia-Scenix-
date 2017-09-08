#ifndef AGENTDYNAMIC_H
#define AGENTDYNAMIC_H

#include "Agent.h"
#include "nvsg/Transform.h"
#include <nvsg/Node.h>

enum DynamicAgentStates
{
    Pursue,
    Arrive,
    Flee
};

class AgentDynamic : Agent
{
    public:
        AgentDynamic();
        virtual ~AgentDynamic();

        float Mass;
        float Velocity;
        float Acceleration;
        float Radius;
        DynamicAgentStates State;

        // the sample's reshape function, called when the window is reshaped
        bool SphericalVolumeCollision( nvmath::Vec3f &in_position, float in_radius );
        float FindAngleBetweenVectors( nvmath::Vec3f &in_position );

        void UpdateAI( nvmath::Vec3f &in_position );

        void SteerPursue( nvmath::Vec3f &in_position );
        void SteerArrive( nvmath::Vec3f &in_position, float in_radius );
        void SteerFlee( nvmath::Vec3f &in_pos );

        void SetPosition( nvmath::Vec3f &in_position );
        void SetRotation( nvmath::Quatf &in_rotation );
        void SetScale( nvmath::Vec3f &in_scale );
        //void SetTransformPtr( nvsg::TransformSharedPtr in_transform_ptr, const nvsg::NodeSharedPtr &node );
        void SetTransformPtr( nvsg::TransformSharedPtr in_transform_ptr );

        Vec3f GetPosition();
        Quatf GetRotation();
        Vec3f GetScale();
        nvsg::TransformSharedPtr GetTransformPtr();

    protected:
        Trafo TrafoTransform;
        nvsg::TransformSharedPtr TransformPtr;

    private:
};

#endif // AGENTDYNAMIC_H
