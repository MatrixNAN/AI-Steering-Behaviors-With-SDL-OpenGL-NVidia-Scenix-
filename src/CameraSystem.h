#ifndef CAMERASYSTEM_H
#define CAMERASYSTEM_H

#include "ui/TrackballCameraManipulatorHIDSync.h"

class CameraSystem
{
    public:
        CameraSystem();
        virtual ~CameraSystem();

        TrackballCameraManipulatorHIDSync *m_trackballHIDSync;
        HumanInterfaceDevice *hid;

    protected:
    private:
};

#endif // CAMERASYSTEM_H
