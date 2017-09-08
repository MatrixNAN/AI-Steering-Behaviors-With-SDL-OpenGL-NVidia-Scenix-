#include "CameraSystem.h"

//CameraSystem::CameraSystem( const RenderContextGLFormat &format )
CameraSystem::CameraSystem()
{
  //m_trackballHIDSync = new TrackballCameraManipulatorHIDSync( );
  //m_trackballHIDSync->setHID( hid );
  //m_trackballHIDSync->setRenderTarget( getRenderTarget() );
  //setManipulator( m_trackballHIDSync );
}

CameraSystem::~CameraSystem()
{
  // Delete SceneRenderer here to cleanup resources before the OpenGL context dies
  //setSceneRenderer( 0 );

  // Reset Manipulator
  //setManipulator( 0 );

  //delete m_trackballHIDSync;
}
