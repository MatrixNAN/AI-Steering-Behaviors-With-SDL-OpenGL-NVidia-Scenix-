# Copyright NVIDIA Corporation 2009-2010
# TO THE MAXIMUM EXTENT PERMITTED BY APPLICABLE LAW, THIS SOFTWARE IS PROVIDED
# *AS IS* AND NVIDIA AND ITS SUPPLIERS DISCLAIM ALL WARRANTIES, EITHER EXPRESS
# OR IMPLIED, INCLUDING, BUT NOT LIMITED TO, IMPLIED WARRANTIES OF MERCHANTABILITY
# AND FITNESS FOR A PARTICULAR PURPOSE.  IN NO EVENT SHALL NVIDIA OR ITS SUPPLIERS
# BE LIABLE FOR ANY SPECIAL, INCIDENTAL, INDIRECT, OR CONSEQUENTIAL DAMAGES
# WHATSOEVER (INCLUDING, WITHOUT LIMITATION, DAMAGES FOR LOSS OF BUSINESS PROFITS,
# BUSINESS INTERRUPTION, LOSS OF BUSINESS INFORMATION, OR ANY OTHER PECUNIARY LOSS)
# ARISING OUT OF THE USE OF OR INABILITY TO USE THIS SOFTWARE, EVEN IF NVIDIA HAS
# BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGES

cmake_minimum_required(VERSION 2.6)

project( SDLNV )

#file(TO_CMAKE_PATH $ENV{SCENIXHOME} SCENIX_HOME)
set(CMAKE_MODULE_PATH "${SCENIX_HOME}/CMake")

find_package(OpenGL REQUIRED )
find_package(SceniX REQUIRED )
find_package(SDL REQUIRED )
find_package(SDL_sound REQUIRED )
find_package(SDL_mixer REQUIRED )
find_package(SDL_net REQUIRED )
find_package(SDL_image REQUIRED )

set(CMAKE_BUILD_TYPE "Release")

if (WIN32)
    find_library ( GLU_LIBRARY
	       NAMES OpenGL32.dll GLU32.dll
 	       PATHS "C:/Windows/System32"
	      )

    find_path ( GLU_INCLUDES glu.h gl.h
	    "E:/Programming/CodeBlocks/MinGW/include/GL"
	  )

    find_library ( SDL_LIBRARY
	       NAMES SDL.dll SDLmain.dll
 	       PATHS "src/SDL/lib"
	      )

    find_path ( SDL_INCLUDES SDL.h
	    "src/SDL/include/SDL"
	  )
endif()

if (UNIX)
    find_library ( GLU_LIBRARY
	       NAMES libGL.so libGLU.so
 	       PATHS /usr/lib
        )

    find_path ( GLU_INCLUDES glu.h glx.h gl.h
	    /usr/include/GL
	  )

    find_library ( SDL_LIBRARY
	       NAMES libSDL.so libSDLmain.so
 	       PATHS /usr/lib
	      )

    find_path ( SDL_INCLUDES SDL.h
	    /usr/include/SDL
	  )
endif()

SCENIX_SET_BINARY_FOLDERS()

set( SCENIX_COMMON_BASE "${SCENIX_HOME}/samples/common" )

file( GLOB uisources
      "${SCENIX_COMMON_BASE}/src/ui/*.cpp" )

set( UI_SOURCES "${uisources}" )

set( SDLNV_SOURCES
  src/main.cpp
  src/CameraSystem.cpp
  src/Agent.cpp
  src/AgentDynamic.cpp
  ${SCENIX_COMMON_BASE}/src/AIScene.cpp
  ${SCENIX_COMMON_BASE}/src/MeshGenerator.cpp
  ${SCENIX_COMMON_BASE}/src/SceneFunctions.cpp
  ${SCENIX_COMMON_BASE}/src/SceniXWidget.cpp
  ${UI_SOURCES}
)

set (SDLNV_HEADERS
  src/CameraSystem.h
  src/Agent.h
  src/AgentDynamic.h
  ${SCENIX_COMMON_BASE}/inc/AIScene.h
  ${SCENIX_COMMON_BASE}/inc/MeshGenerator.h
  ${SCENIX_COMMON_BASE}/inc/SceneFunctions.h
  ${SCENIX_COMMON_BASE}/inc/SceniXWidget.h
)

include_directories( ${SCENIX_INCLUDES} )
include_directories( ${SCENIXRT_INCLUDES} )
include_directories( "." )
include_directories( "inc" )
include_directories( "${SCENIX_COMMON_BASE}/inc" )
include_directories( ${SDL_INCLUDES} )
include_directories( ${GLU_INCLUDES} )

add_executable( SDLNV
  ${SDLNV_SOURCES}
  ${SDLNV_HEADERS}
)

target_link_libraries( SDLNV
  ${SCENIX_LIBS}
  ${SCENIXRT_LIBS}
  #${OPENGL_gl_LIBRARY}
  ${OPENGL_LIBRARY}
  ${SDL_LIBRARY}
  ${GLU_LIBRARY}
  SDLmain
)

if(TARGET SceniX)
  add_dependencies( SDLNV SceniX SceniXRT )
  add_perforce_to_target( SDLNV )
endif()

set_target_properties( SDLNV PROPERTIES FOLDER "Samples")

