#ifdef __cplusplus
    #include <cstdlib>
#else
    #include <stdlib.h>
#endif

#ifdef WIN32
	#define NOMINMAX
	#include <windows.h>
#endif

#include <SDL.h>
#include <SDL_image.h>
#include <SDL_mixer.h>
#include <SDL_sound.h>
#include <SDL_net.h>

#include <vector>
#include <string>
#include <math.h>

#include <iostream>

#include <nvsg/nvsg.h>
#include <nvrt/RTInit.h>

#include "SceneFunctions.h"
#include "AIScene.h"
//#include "CameraSystem.h"
//#include "AgentDynamic.h"

#include <nvsg/Scene.h>
#include <nvsg/ViewState.h>
#include "nvmath/nvmath.h"
#include "nvmath/Vecnt.h"
#include "nvsg/Transform.h"

#include <nvgl/SceneRendererGL2.h>
#include <nvgl/RenderTargetGLFB.h>
#include <nvrt/SceneRendererRTBuiltinGL.h>

#include "ui/TrackballCameraManipulatorHIDSync.h"

#include "nvtraverser/CombineTraverser.h"
#include "nvtraverser/EliminateTraverser.h"
#include "nvtraverser/UnifyTraverser.h"

#include <nvutil/DbgNew.h>  // enable leak detection

using namespace nvsg;
using namespace nvgl;
using namespace nvui;
using namespace nvrt;
using namespace nvutil;
using namespace nvtraverser;
using namespace nvmath;

nvgl::SmartRenderTargetGL   rtgl;   // RenderTarget for SceneRenderer
nvgl::SmartSceneRendererGL2 srgl2;  // OpenGL2 based renderer

const GLsizei windowWidth = 1000;
const GLsizei windowHeight = 950;

bool leftMouseButtonPressed = false;
bool middleMouseButtonPressed = false;
bool rightMouseButtonPressed = false;

const Uint8 leftMouseButton = 1;
const Uint8 middleMouseButton = 2;
const Uint8 rightMouseButton = 4;

Uint8 mouseAbsState;// = NULL;
Uint8 mouseRelState;// = NULL;

int mouseAbsX = 0;
int mouseAbsY = 0;
int mouseRelX = 0;
int mouseRelY = 0;

const float rotateX_speed = 0.000f;
const float rotateY_speed = 0.001f;
const float rotateZ_speed = 0.002f;

const float transX_speed = 0.001f;
const float transY_speed = 0.001f;
const float transZ_speed = 0.001f;
const float zoomSpeed    = 0.1f;

GLfloat rotateX = 0.0f;
GLfloat rotateY = 0.0f;
GLfloat rotateZ = 0.0f;
GLfloat zoom = -5.0f;
GLfloat PCtransX = 0.0;
GLfloat PCtransY = 0.0;
GLfloat PCtransZ = 0.0;

Uint8 *keyState = NULL;
SDLMod keyMod;
int keyDelay = 1;
int keyRepeat = 10;
std::vector<SDL_Joystick*> Joystick;
int numJoysticks = 0;

struct JoyBall
{
    int x;
    int y;
};

struct JoyControl
{
    std::string Name;
    int NumAxes;
    int NumButtons;
    int NumHats;
    int NumBalls;

    std::vector<Sint16> Axis;
    std::vector<Uint8> Button;
    std::vector<Uint8> Hat;
    std::vector<JoyBall> Ball;
};

std::vector<JoyControl> Joysticks;

// create a SimpleScene object, containing a SceniX scene
AIScene scene;
//std::vector<AgentDynamic*> NPCs;

// the sample's reshape function, called when the window is reshaped
void reshape( GLint width, GLint height )
{
  // pass the new window size to the render target
  rtgl->setSize( width, height );
}

void initGL()
{
  // create a SimpleScene object, containing a SceniX scene
  //AIScene scene;

  // create a ViewState object, containing view specific state like a camera and a scene
  nvsg::ViewStateSharedPtr viewState = nvsg::ViewState::create();

  // pass the scene contained in the SimpleScene object to the ViewState object
  nvsg::ViewStateWriteLock(viewState)->setScene( scene.m_sceneHandle );

  // setup the view state with some default values, e.g. create a camera looking at the scene
  setupDefaultViewState(viewState);

  // create a render context
  nvgl::SmartRenderContextGL renderContextGL = nvgl::RenderContextGL::create( nvgl::RenderContextGL::Attach() );

  // create a render target in the render context
  rtgl = nvgl::RenderTargetGLFB::create ( renderContextGL );

  // configure the render target to clear the color and depth buffer
  rtgl->setClearMask( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

  // create a SceneRenderer for OpenGL
  srgl2 = nvgl::SceneRendererGL2::create();

  // pass it the render target and the view state
  srgl2->setRenderTarget( rtgl );
  srgl2->setViewState( viewState );
}

void drawScene(void)
{
    // render the scene
    srgl2->render();

	// this line is modified depending on which window manager is used...
	SDL_GL_SwapBuffers();
}

bool sphericalVolumeCollision( Vec3f &Position, float Radius, Vec3f &in_position, float in_radius )
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

float findAngleBetweenVectors( Vec3f &Position, Vec3f &in_position )
{
    Position.normalize();
    in_position.normalize();

    return 0;//acos(dot(posA, posB));
}

void steerPursue()
{
    //FindAngleBetweenVectors( in_position );

    nvmath::Vec3f transPC( 0.0f, 0.0f, 0.0f );
    nvmath::Vec3f pos( 0.0f, 0.0f, 0.0f );
    TransformReadLock trPC( scene.m_transformHandle[0] );
    transPC = trPC->getTranslation();

    nvmath::Vec3f transAgent01( 0.0f, 0.0f, 0.0f );
    TransformReadLock trAgent01( scene.m_transformHandle[1] );
    transAgent01 = trAgent01->getTranslation();

    nvmath::Vec3f diffStep( 0.0f, 0.0f, 0.0f );
    diffStep = (transPC - transAgent01);
    diffStep.normalize();
    diffStep /= 500;
    float dist = distance(transAgent01, transPC);

    if ( dist > 0 )
    {
        pos = nvmath::Vec3f(transAgent01 + diffStep);// - nvmath::Vec3f(3, 3, 3));
        TransformWriteLock twAgent01( scene.m_transformHandle[1] );
        twAgent01->setTranslation(pos);
    }
    fprintf(stdout, "Dist: %f \n", dist);
    fprintf(stdout, "Diff: %f %f %f \n", diffStep[0], diffStep[1], diffStep[2]);
}

void steerArrive( Vec3f &Position, Vec3f &in_position )
{
    //GetPosition();

    if ( distance(Position, in_position) > 0.01 )
    {
    }
}

void steerFlee( Vec3f &in_position )
{
    //findAngleBetweenVectors( in_position );
}


void initializeAI()
{
    //NPCs.resize(3);
// Causes Segfault!!!!!
//    NPCs[0]->SetTransformPtr(scene.m_transformHandle[1]);
//    NPCs[1]->SetTransformPtr(scene.m_transformHandle[2]);
//    NPCs[2]->SetTransformPtr(scene.m_transformHandle[3]);

}

void updateAI()
{
    steerPursue();
/*
    Vec3f transPC( 0.0f, 0.0f, 0.0f );
    TransformReadLock trPC( scene.m_transformHandle[0] );
    transPC = trPC->getTranslation();

    for ( int num = 0; num >= NPCs.size(); num++ )
    {
        //NPCs[num]->SteerPursue( transPC );
    }
*/
}

void Keyboard(void)
{
    static float mag = 0;
    static Quatf quatRot;
    static Quatf quatRotOffset;
    static Vec3f UnitVec( 1.0f, 1.0f, 1.0f );
    static Vec3f transPC( 0.0f, 0.0f, 0.0f );

    quatRot = Quatf( nvmath::Vec3f(  0.0f, 0.0f, 1.0f ) , rotateZ);

    keyState = SDL_GetKeyState(NULL);
    SDL_EnableKeyRepeat(keyDelay, keyRepeat);

    //int resultUnicode = SDL_EnableUNICODE(1); // 1 Enables Unicode
    //int resultUnicode = SDL_EnableUNICODE(0); // 0 Disables Unicode
    //int resultUnicode = SDL_EnableUNICODE(-1); // -1 Does Not Enable of Disable Unicode

    if( keyState[SDLK_LEFT] )
    {
        rotateZ += rotateZ_speed;
        quatRot = Quatf( nvmath::Vec3f(  0.0f, 0.0f, 1.0f ) , rotateZ);
        scene.setTransform( scene.m_transformHandle[0], Vec3f( PCtransX, PCtransY, 0.0f ), quatRot, Vec3f(   1.0f, 1.0f, 1.0f ) );
        fprintf(stdout, "Rotate Z: %f \n", radToDeg(rotateZ));
    }

    if( keyState[SDLK_RIGHT] )
    {
        rotateZ -= rotateZ_speed;
        quatRot = Quatf( nvmath::Vec3f(  0.0f, 0.0f, 1.0f ) , rotateZ);
        scene.setTransform( scene.m_transformHandle[0], Vec3f( PCtransX, PCtransY, 0.0f ), quatRot, Vec3f(   1.0f, 1.0f, 1.0f ) );
        fprintf(stdout, "Rotate Z: %f \n", radToDeg(rotateZ));
    }

    if( keyState[SDLK_UP] )
    {
        mag += transY_speed;
        TransformReadLock tr( scene.m_transformHandle[0] );
        transPC = tr->getTranslation();
        PCtransX = transPC[0];
        PCtransY = transPC[1];
        quatRotOffset = Quatf( nvmath::Vec3f(  0.0f, 0.0f, 1.0f ) , (rotateZ - 45) );
        transPC = quatRotOffset * UnitVec;
        transPC.normalize();
        PCtransX = ((mag * transPC[0]) / -1000) + PCtransX;
        PCtransY = ((mag * transPC[1]) / 1000) + PCtransY;
        scene.setTransform( scene.m_transformHandle[0], Vec3f( PCtransX, PCtransY, 0.0f ), quatRot, Vec3f(   1.0f, 1.0f, 1.0f ) );
        fprintf(stdout, "Rotate Z: %f \n", radToDeg(rotateZ));
        fprintf(stdout, "Translate: %f %f \n", PCtransX, PCtransY);
        fprintf(stdout, "Translate Unit Vector: %f %f %f \n", transPC[0], transPC[1], transPC[2]);
    }

    if( keyState[SDLK_DOWN] )
    {
        mag += transY_speed;
        TransformReadLock tr( scene.m_transformHandle[0] );
        transPC = tr->getTranslation();
        PCtransX = transPC[0];
        PCtransY = transPC[1];
        quatRotOffset = Quatf( nvmath::Vec3f(  0.0f, 0.0f, 1.0f ) , (rotateZ - 45) );
        transPC = quatRotOffset * UnitVec;
        transPC.normalize();
        PCtransX = ((mag * transPC[0]) / 1000) + PCtransX;
        PCtransY = ((mag * transPC[1]) / -1000) + PCtransY;
        scene.setTransform( scene.m_transformHandle[0], Vec3f( PCtransX, PCtransY, 0.0f ), quatRot, Vec3f(   1.0f, 1.0f, 1.0f ) );
        fprintf(stdout, "Rotate Z: %f \n", radToDeg(rotateZ));
        fprintf(stdout, "Translate: %f %f \n", PCtransX, PCtransY);
        fprintf(stdout, "Translate Unit Vector: %f %f %f \n", transPC[0], transPC[1], transPC[2]);
    }

    if(keyState[SDLK_a])
    {
        fprintf(stdout,"Key a \n");
    }

    if(keyState[SDLK_b])
        fprintf(stdout,"Key b \n");

    if(keyState[SDLK_c])
        fprintf(stdout,"Key c \n");

    if(keyState[SDLK_d])
        fprintf(stdout,"Key d \n");

    if(keyState[SDLK_e])
        fprintf(stdout,"Key e \n");

    if(keyState[SDLK_f])
        fprintf(stdout,"Key f \n");

    if(keyState[SDLK_g])
        fprintf(stdout,"Key g \n");

    if(keyState[SDLK_h])
        fprintf(stdout,"Key h \n");

    if(keyState[SDLK_i])
        fprintf(stdout,"Key i \n");

    if(keyState[SDLK_j])
        fprintf(stdout,"Key j \n");

    if(keyState[SDLK_k])
        fprintf(stdout,"Key k \n");

    if(keyState[SDLK_l])
        fprintf(stdout,"Key l \n");

    if(keyState[SDLK_m])
        fprintf(stdout,"Key m \n");

    if(keyState[SDLK_n])
        fprintf(stdout,"Key n \n");

    if(keyState[SDLK_o])
        fprintf(stdout,"Key o \n");

    if(keyState[SDLK_p])
        fprintf(stdout,"Key p \n");

    if(keyState[SDLK_q])
        fprintf(stdout,"Key q \n");

    if(keyState[SDLK_r])
        fprintf(stdout,"Key r \n");

    if(keyState[SDLK_s])
        fprintf(stdout,"Key s \n");

    if(keyState[SDLK_t])
        fprintf(stdout,"Key t \n");

    if(keyState[SDLK_u])
        fprintf(stdout,"Key u \n");

    if(keyState[SDLK_v])
        fprintf(stdout,"Key v \n");

    if(keyState[SDLK_w])
        fprintf(stdout,"Key w \n");

    if(keyState[SDLK_x])
        fprintf(stdout,"Key x \n");

    if(keyState[SDLK_y])
        fprintf(stdout,"Key y \n");

    if(keyState[SDLK_z])
        fprintf(stdout,"Key z \n");

    if(keyState[SDLK_F1])
        fprintf(stdout,"Key F1 \n");

    if(keyState[SDLK_F2])
        fprintf(stdout,"Key F2 \n");

    if(keyState[SDLK_F3])
        fprintf(stdout,"Key F3 \n");

    if(keyState[SDLK_F4])
        fprintf(stdout,"Key F4 \n");

    if(keyState[SDLK_F5])
        fprintf(stdout,"Key F5 \n");

    if(keyState[SDLK_F6])
        fprintf(stdout,"Key F6 \n");

    if(keyState[SDLK_F7])
        fprintf(stdout,"Key F7 \n");

    if(keyState[SDLK_F8])
        fprintf(stdout,"Key F8 \n");

    if(keyState[SDLK_F9])
        fprintf(stdout,"Key F9 \n");

    if(keyState[SDLK_F10])
        fprintf(stdout,"Key F10 \n");

    if(keyState[SDLK_F11])
        fprintf(stdout,"Key F11 \n");

    if(keyState[SDLK_F12])
        fprintf(stdout,"Key F12 \n");

    if(keyState[SDLK_F13])
        fprintf(stdout,"Key F13 \n");

    if(keyState[SDLK_F14])
        fprintf(stdout,"Key F14 \n");

    if(keyState[SDLK_F15])
        fprintf(stdout,"Key F15 \n");

    if(keyState[SDLK_KP0])
        fprintf(stdout,"Key Pad 0 \n");

    if(keyState[SDLK_KP1])
        fprintf(stdout,"Key Pad 1 \n");

    if(keyState[SDLK_KP2])
        fprintf(stdout,"Key Pad 2 \n");

    if(keyState[SDLK_KP3])
        fprintf(stdout,"Key Pad 3 \n");

    if(keyState[SDLK_KP4])
        fprintf(stdout,"Key Pad 4 \n");

    if(keyState[SDLK_KP5])
        fprintf(stdout,"Key Pad 5 \n");

    if(keyState[SDLK_KP6])
        fprintf(stdout,"Key Pad 6 \n");

    if(keyState[SDLK_KP7])
        fprintf(stdout,"Key Pad 7 \n");

    if(keyState[SDLK_KP8])
        fprintf(stdout,"Key Pad 8 \n");

    if(keyState[SDLK_KP9])
        fprintf(stdout,"Key Pad 9 \n");

    if(keyState[SDLK_BACKSPACE])
        fprintf(stdout,"Key Backspace \n");

    if(keyState[SDLK_TAB])
        fprintf(stdout,"Key Tab \n");

    if(keyState[SDLK_CLEAR])
        fprintf(stdout,"Key Clear \n");

    if(keyState[SDLK_RETURN])
        fprintf(stdout,"Key Return \n");

    if(keyState[SDLK_PAUSE])
        fprintf(stdout,"Key Pause \n");

    if(keyState[SDLK_ESCAPE])
        fprintf(stdout,"Key Escape \n");

    if(keyState[SDLK_SPACE])
        fprintf(stdout,"Key Space \n");

    if(keyState[SDLK_EXCLAIM])
        fprintf(stdout,"Key Exclaimation Mark \n");

    if(keyState[SDLK_QUOTEDBL])
        fprintf(stdout,"Key Double Quote \n");

    if(keyState[SDLK_HASH])
        fprintf(stdout,"Key Hash \n");

    if(keyState[SDLK_DOLLAR])
        fprintf(stdout,"Key Dollar \n");

    if(keyState[SDLK_AMPERSAND])
        fprintf(stdout,"Key Ampersand \n");

    if(keyState[SDLK_QUOTE])
        fprintf(stdout,"Key Quote \n");

    if(keyState[SDLK_LEFTPAREN])
        fprintf(stdout,"Key Left Parenthesis \n");

    if(keyState[SDLK_RIGHTPAREN])
        fprintf(stdout,"Key Right Parenthesis \n");

    if(keyState[SDLK_ASTERISK])
        fprintf(stdout,"Key Asterisk \n");

    if(keyState[SDLK_PLUS])
        fprintf(stdout,"Key Plus Sign \n");

    if(keyState[SDLK_COMMA])
        fprintf(stdout,"Key Comma \n");

    if(keyState[SDLK_MINUS])
        fprintf(stdout,"Key Minus Sign \n");

    if(keyState[SDLK_PERIOD])
        fprintf(stdout,"Key Period \n");

    if(keyState[SDLK_SLASH])
        fprintf(stdout,"Key Slash \n");

    if(keyState[SDLK_COLON])
        fprintf(stdout,"Key Colon \n");

    if(keyState[SDLK_SEMICOLON])
        fprintf(stdout,"Key Semicolon \n");

    if(keyState[SDLK_LESS])
        fprintf(stdout,"Key Less-Than Symbol \n");

    if(keyState[SDLK_EQUALS])
        fprintf(stdout,"Key Equals Sign \n");

    if(keyState[SDLK_GREATER])
        fprintf(stdout,"Key Greater-Than Symbol \n");

    if(keyState[SDLK_QUESTION])
        fprintf(stdout,"Key Question Mark \n");

    if(keyState[SDLK_AT])
        fprintf(stdout,"Key At Sign \n");

    if(keyState[SDLK_LEFTBRACKET])
        fprintf(stdout,"Key Left Bracket \n");

    if(keyState[SDLK_BACKSLASH])
        fprintf(stdout,"Key Backslash \n");

    if(keyState[SDLK_RIGHTBRACKET])
        fprintf(stdout,"Key Right Bracket \n");

    if(keyState[SDLK_CARET])
        fprintf(stdout,"Key Caret \n");

    if(keyState[SDLK_UNDERSCORE])
        fprintf(stdout,"Key Underscore \n");

    if(keyState[SDLK_BACKQUOTE])
        fprintf(stdout,"Key Grave \n");

    if(keyState[SDLK_DELETE])
        fprintf(stdout,"Key Delete \n");

    if(keyState[SDLK_KP_PERIOD])
        fprintf(stdout,"Key Pad Period \n");

    if(keyState[SDLK_KP_DIVIDE])
        fprintf(stdout,"Key Pad Division \n");

    if(keyState[SDLK_KP_MULTIPLY])
        fprintf(stdout,"Key Pad Multiply \n");

    if(keyState[SDLK_KP_MINUS])
        fprintf(stdout,"Key Pad Minus \n");

    if(keyState[SDLK_KP_PLUS])
        fprintf(stdout,"Key Pad Plus Sign \n");

    if(keyState[SDLK_KP_ENTER])
        fprintf(stdout,"Key Pad Enter \n");

    if(keyState[SDLK_KP_EQUALS])
        fprintf(stdout,"Key Pad Equals Sign \n");

    if(keyState[SDLK_INSERT])
        fprintf(stdout,"Key Insert \n");

    if(keyState[SDLK_HOME])
        fprintf(stdout,"Key Home \n");

    if(keyState[SDLK_END])
        fprintf(stdout,"Key End \n");

    if(keyState[SDLK_PAGEUP])
        fprintf(stdout,"Key Page Up \n");

    if(keyState[SDLK_PAGEDOWN])
        fprintf(stdout,"Key Page Down \n");

    if(keyState[SDLK_NUMLOCK])
        //fprintf(stdout,"Key Num Lock \n");

    if(keyState[SDLK_CAPSLOCK])
        fprintf(stdout,"Key Caps Lock \n");

    if(keyState[SDLK_SCROLLOCK])
        fprintf(stdout,"Key Scroll Lock \n");

    if(keyState[SDLK_RSHIFT])
        fprintf(stdout,"Key Right Shift \n");

    if(keyState[SDLK_LSHIFT])
        fprintf(stdout,"Key Left Shift \n");

    if(keyState[SDLK_RCTRL])
        fprintf(stdout,"Key Right Ctrl \n");

    if(keyState[SDLK_LCTRL])
        fprintf(stdout,"Key Left Ctrl \n");

    if(keyState[SDLK_RALT])
        fprintf(stdout,"Key Right Alt \n");

    if(keyState[SDLK_LALT])
        fprintf(stdout,"Key Left Alt \n");

    if(keyState[SDLK_RMETA])
        fprintf(stdout,"Key Right Meta \n");

    if(keyState[SDLK_LMETA])
        fprintf(stdout,"Key Left Meta \n");

    if(keyState[SDLK_LSUPER])
        fprintf(stdout,"Key Left Super \n");

    if(keyState[SDLK_RSUPER])
        fprintf(stdout,"Key Right Super \n");

    if(keyState[SDLK_MODE])
        fprintf(stdout,"Key Mode Shift \n");

    if(keyState[SDLK_HELP])
        fprintf(stdout,"Key Help \n");

    if(keyState[SDLK_PRINT])
        fprintf(stdout,"Key Print Screen \n");

    if(keyState[SDLK_SYSREQ])
        fprintf(stdout,"Key SysRq \n");

    if(keyState[SDLK_BREAK])
        fprintf(stdout,"Key Break \n");

    if(keyState[SDLK_MENU])
        fprintf(stdout,"Key Menu \n");

    if(keyState[SDLK_POWER])
        fprintf(stdout,"Key Power \n");

    if(keyState[SDLK_EURO])
        fprintf(stdout,"Key Euro \n");

    keyMod = SDL_GetModState();

    if(keyMod == KMOD_NONE)
        //fprintf(stdout,"Key Mod None \n");

    if(keyMod == KMOD_NUM)
        fprintf(stdout,"Key Mod Num Lock \n");

    if(keyMod == KMOD_CAPS)
        fprintf(stdout,"Key Mod Caps Lock \n");

    if(keyMod == KMOD_LCTRL)
        fprintf(stdout,"Key Mod Left Ctrl \n");

    if(keyMod == KMOD_RCTRL)
        fprintf(stdout,"Key Mod Right Ctrl \n");

    if(keyMod == KMOD_RSHIFT)
        fprintf(stdout,"Key Mod Right Shift \n");

    if(keyMod == KMOD_LSHIFT)
        fprintf(stdout,"Key Mod Left Shift \n");

    if(keyMod == KMOD_RALT)
        fprintf(stdout,"Key Mod Right Alt \n");

    if(keyMod == KMOD_LALT)
        fprintf(stdout,"Key Mod Left Alt \n");

    if(keyMod == KMOD_CTRL)
        fprintf(stdout,"Key Mod Ctrl \n");

    if(keyMod == KMOD_SHIFT)
        fprintf(stdout,"Key Mod Shift \n");

    if(keyMod == KMOD_ALT)
        fprintf(stdout,"Key Mod Alt \n");
}

void Mouse(void)
{
    mouseAbsState = SDL_GetMouseState(&mouseAbsX, &mouseAbsY);
    mouseRelState = SDL_GetRelativeMouseState(&mouseRelX, &mouseRelY);

    if(mouseRelState == leftMouseButton)
    {
        rotateX += mouseRelY * rotateY_speed;
        rotateY += mouseRelX * rotateX_speed;
    }

    if(mouseRelState == rightMouseButton)
    {
        zoom -= mouseRelY * zoomSpeed;
        //fprintf(stdout,"Mouse Absolute Button %d pressed at (%d,%d)\n", mouseAbsState, mouseAbsX, mouseAbsY);
    }
}

void initJoystick(void)
{
    numJoysticks = SDL_NumJoysticks();

    if (numJoysticks == 0)
        fprintf(stdout,"No Joysticks Found!!!\n");
    else
    {
        Joysticks.resize(numJoysticks);
        Joystick.resize(numJoysticks);

        for(int index = 0; index < numJoysticks; index++)
        {
            Joysticks[index].Name = SDL_JoystickName(index);
            fprintf(stdout,"Name Of Joystick %s \n", SDL_JoystickName(index));

            Joystick[index] = (SDL_JoystickOpen(index));
                SDL_JoystickEventState(SDL_ENABLE);
                Joysticks[index].NumAxes = SDL_JoystickNumAxes(Joystick[index]);
                Joysticks[index].Axis.resize(Joysticks[index].NumAxes);
                fprintf(stdout,"Num Of Axes %d \n", Joysticks[index].NumAxes);
                Joysticks[index].NumButtons = SDL_JoystickNumButtons(Joystick[index]);
                Joysticks[index].Button.resize(Joysticks[index].NumButtons);
                fprintf(stdout,"Num Of Buttons %d \n", Joysticks[index].NumButtons);
                Joysticks[index].NumHats = SDL_JoystickNumHats(Joystick[index]);
                Joysticks[index].Hat.resize(Joysticks[index].NumHats);
                fprintf(stdout,"Num Of Hats %d \n", Joysticks[index].NumHats);
                Joysticks[index].NumBalls = SDL_JoystickNumBalls(Joystick[index]);
                Joysticks[index].Ball.resize(Joysticks[index].NumBalls);
                fprintf(stdout,"Num Of Balls %d \n", Joysticks[index].NumBalls);

            fprintf(stdout,"\n\n");
        }
    }
}

void getJoysticks(void)
{
    SDL_JoystickUpdate();
    for(int index = 0; index < numJoysticks; index++)
    {

        if(Joystick[index] == NULL)
            fprintf(stderr, "Can Not Open Joystick!\n");

            SDL_JoystickEventState(SDL_ENABLE);
            for (int indexAxis = 0; indexAxis < Joysticks[index].NumAxes; indexAxis++)
            {
                Joysticks[index].Axis[indexAxis] = SDL_JoystickGetAxis(Joystick[index], indexAxis);
                //fprintf(stdout,"Axis %d: Value %d \n", indexAxis, Joysticks[index].Axis[indexAxis]);
            }
            for (int indexButton = 0; indexButton < Joysticks[index].NumButtons; indexButton++)
            {
                Joysticks[index].Button[indexButton] = SDL_JoystickGetButton(Joystick[index], indexButton);
                //fprintf(stdout,"Button %d: Value %d \n", indexButton, Joysticks[index].Button[indexButton]);
            }
            for (int indexHat = 0; indexHat < Joysticks[index].NumHats; indexHat++)
            {
                Joysticks[index].Hat[indexHat] = SDL_JoystickGetHat(Joystick[index], indexHat);
                //fprintf(stdout,"Hat %d: Value %d \n", indexHat, Joysticks[index].Hat[indexHat]);
            }
            if (Joysticks[index].NumBalls > 0)
            {
                for (int indexBall = 0; indexBall < Joysticks[index].NumBalls; indexBall++)
                {
                    int JoyBallSuccess = SDL_JoystickGetBall(Joystick[index], indexBall, &Joysticks[index].Ball[indexBall].x, &Joysticks[index].Ball[indexBall].y);
                    if (JoyBallSuccess == 0)
                        fprintf(stdout,"Ball %d: X %d   Y %d \n", indexBall, Joysticks[index].Ball[indexBall].x, Joysticks[index].Ball[indexBall].y);
                    else
                        fprintf(stdout,"Unable to get Ball values!!!\n");
                }
            }

        #if WIN32
            if(abs(Joysticks[index].Axis[1]) > 3000)
                rotateX += Joysticks[0].Axis[1] * 0.001 * 0.05;
            if(abs(Joysticks[index].Axis[0]) > 3000)
                rotateY += Joysticks[0].Axis[0] * 0.001 * -0.05;
        #elif (linux)
            if(abs(Joysticks[index].Axis[1]) > 3000)
                rotateX += Joysticks[0].Axis[1] * 0.001 * 0.001 * 1.0;
            if(abs(Joysticks[index].Axis[0]) > 3000)
                rotateY += Joysticks[0].Axis[0] * 0.001 * 0.001 * -1.0;
        #endif

        //fprintf(stdout,"\n\n");
    }
}

int main(int argc,  char **argv)
{
	// Make sure SDL quits after exit
    atexit(SDL_Quit);

	if(SDL_Init(SDL_INIT_VIDEO | SDL_INIT_NOPARACHUTE) < 0)
	{
		// error
		fprintf(stderr, "Unable to initalize SDL Video %s", SDL_GetError());
		exit(1);
	}

	if(SDL_InitSubSystem(SDL_INIT_JOYSTICK) < 0)
	{
		// error
		fprintf(stderr, "Unable to initalize SDL Joystick %s", SDL_GetError());
		exit(1);
	}

    //  window width, window height, bpp, flag
    //  bpp is 0 when in windowed mode can be something else when in full screen mode 16, 24, 32 etc...
    //  flag is one of many options listed below.
    //  SDL_FULLSCREEN runs your application in full screen mode.
    //  SDL_RESIZABLE if you want to be able to resize the window.
    //  SDL_DOUBLEBUF allows for double buffering.
    //  SDL_OPENGL if you are using OpenGL with SDL.
    //  SDL_OPENGLBLIT if you are using OpenGL with SDL but want SDL to do the actual rendering.
    //  SDL_NOFRAME this gets rid of the window around the edges when in window mode.  Fullscreen runs
    //  in this mode for instance.
    //  SDL_ANYFORMAT sets you up to use the pixel format from physics display device aka your screen.
    //  SDL_ASYNCBLIT does asynchronous blitting
    //  SDL_HWPALETTE 8 bit graphics from like the old mario brothers.
    //	SDL_SWSURFACE renders the display in software mode in main memory.
    //  SDL_HWSURFACE renders the display in hardware mode in video card's memory
	if(SDL_SetVideoMode(windowWidth, windowHeight, 0, SDL_OPENGL | SDL_RESIZABLE) == NULL)
	{
		// error
		fprintf(stderr, "Unable to Create OpenGL Scene %s", SDL_GetError());
		exit(2);
	}

	initGL();
    initJoystick();

    // initialize SceniX
    nvsg::nvsgInitialize();
    initializeAI();

	bool done = false;

	while(!done)
	{
		SDL_Event event;
		while (SDL_PollEvent (&event))
		{
            switch (event.type)
            {
				case SDL_VIDEORESIZE:
					reshape( event.resize.w, event.resize.h );
					break;
                case SDL_KEYDOWN:
                    if(event.key.keysym.sym == SDLK_ESCAPE)
                        done = true;
                    if(event.key.keysym.mod == KMOD_LCTRL)
                        fprintf(stdout,"Left Ctrl Pressed \n");
                    break;
                case SDL_MOUSEBUTTONUP://button released
                    if(event.button.button == 1)
                        leftMouseButtonPressed = false;
                    //report event
                    /*fprintf(stdout,"Mouse Button %d Released\n",
                            event.button.button);*/
                    break;
                case SDL_MOUSEMOTION:
                    if(leftMouseButtonPressed == true)
                    {
                        //rotateX += event.motion.yrel * 0.5;
                        //rotateY += event.motion.xrel * 0.5;
                    }
                    /*
                    fprintf(stdout,"Mouse moved by %d,%d to (%d,%d)\n",
                        event.motion.xrel, event.motion.yrel,
                        event.motion.x, event.motion.y);
                    */
                    break;
                case SDL_MOUSEBUTTONDOWN:
                    if(event.button.button == 1)
                        leftMouseButtonPressed = true;

                    /*fprintf(stdout,"Mouse button %d pressed at (%d,%d)\n",
                        event.button.button, event.button.x, event.button.y);*/
                    break;
                case SDL_QUIT:
                    done = true;
                    break;
                /*
                case SDL_JOYAXISMOTION:
                    if(abs(event.jaxis.value) > 3000)
                    {
                        for (int indexAxis = 0; indexAxis < Joysticks[index].NumAxes; indexAxis++)
                        {
                            if(event.jaxis.axis == indexAxis)
                            {
                                Joysticks[index].Axis[indexAxis] = event.jaxis.value;
                            }
                        }
                    }
                    break;
                case SDL_JOYBUTTONDOWN:
                    for (int indexButton = 0; indexButton < Joysticks[index].NumButtons; indexButton++)
                    {
                        if(event.jbutton.button == indexButton)
                        {
                            Joysticks[index].Button[indexButton] = 1; //SDL_JoystickGetButton(Joystick, indexButton);
                            fprintf(stdout,"Button %d: Value %d \n", indexButton, Joysticks[index].Button[indexButton]);
                        }
                    }
                    break;
                case SDL_JOYHATMOTION:
                    if(event.jhat.hat | SDL_HAT_CENTERED)
                    {
                        fprintf(stdout,"Hat Centered \n");
                    }

                    else if(event.jhat.hat | SDL_HAT_UP)
                    {
                        fprintf(stdout,"Hat UP \n");
                    }

                    else if(event.jhat.hat | SDL_HAT_DOWN)
                    {
                        fprintf(stdout,"Hat Down \n");
                    }

                    else if(event.jhat.hat | SDL_HAT_LEFT)
                    {
                        fprintf(stdout,"Hat Left \n");
                    }

                    else if(event.jhat.hat | SDL_HAT_RIGHT)
                    {
                        fprintf(stdout,"Hat Right \n");
                    }

                    else if(event.jhat.hat | SDL_HAT_LEFTDOWN)
                    {
                        fprintf(stdout,"Hat Left Down \n");
                    }

                    else if(event.jhat.hat | SDL_HAT_LEFTUP)
                    {
                        fprintf(stdout,"Hat Left Up \n");
                    }

                    else if(event.jhat.hat | SDL_HAT_RIGHTDOWN)
                    {
                        fprintf(stdout,"Hat Right Down \n");
                    }

                    else if(event.jhat.hat | SDL_HAT_RIGHTUP)
                    {
                        fprintf(stdout,"Hat Right Up \n");
                    }
                    break;
                    */
            }
		}
        Keyboard();
        Mouse();
        /*
        if(abs(Joysticks[index].Axis[1]) > 3000)
            rotateX += Joysticks[0].Axis[1] * 0.001 * 0.05;
        if(abs(Joysticks[index].Axis[0]) > 3000)
            rotateY += Joysticks[0].Axis[0] * 0.001 * 0.05;
        */
        getJoysticks();
        updateAI();
		drawScene();

          // reset the scene renderer and the target
	}

    rtgl.reset();
    srgl2.reset();

    for(int index = 0; index < numJoysticks; index++)
    {
        SDL_JoystickClose(Joystick[index]);
    }

    // terminate SceniX
    nvsg::nvsgTerminate();

	SDL_Quit();

	return 0;
}

