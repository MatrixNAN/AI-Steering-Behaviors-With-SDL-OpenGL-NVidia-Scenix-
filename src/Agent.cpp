#include "Agent.h"

Agent::Agent()
{
    Position = Vec3f( 0.0f, 0.0f, 0.0f );
    Rotation = Quatf( Vec3f(  0.0f, 0.0f, 0.0f ), 0.0f );
    Scale = Vec3f( 1.0f, 1.0f, 1.0f );
    Life = 100;
    ID = 1;
}

Agent::~Agent()
{
    //dtor
}
