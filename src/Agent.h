#ifndef AGENT_H
#define AGENT_H

#include "nvmath/Vecnt.h"
#include "nvmath/nvmath.h"
#include "nvsg/Transform.h"

using namespace nvmath;

class Agent
{
    public:
        Agent();
        virtual ~Agent();

        Vec3f Position;
        Quatf Rotation;
        Vec3f Scale;
        float Life;
        int ID;


    protected:
    private:
};

#endif // AGENT_H
