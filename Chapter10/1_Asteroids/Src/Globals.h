#pragma once

#include "Canvas.h"
#include "FileSystem.h"
#include "Log.h"
#include "SDLLibrary.h"
#include "SDLWindow.h"
#include "LGL/GLClasses.h"
#include "Game.h"
#include "Technique.h"
#include "Camera.h"
#include "Trackball.h"
#include "Audio.h"
#include "ScreenJoystick.h"

struct sMouseState
{
	vec2 FPos;
	bool FPressed;
};

extern clVirtualTrackball g_Trackball;
extern sMouseState g_MouseState;

extern clPtr<clSDLWindow> g_Window;
extern clPtr<clFileSystem> g_FS;
extern clPtr<iCanvas> g_Canvas;
extern clPtr<clGameManager> g_Game;
extern clPtr<clMaterialSystem> g_MatSys;
extern clPtr<clForwardRenderingTechnique> g_Technique;
extern clCamera g_Camera;
extern clAudioThread g_Audio;
extern clPtr<clAudioSource> g_BackgroundMusic;

extern std::unique_ptr<sLGLAPI> LGL3;

extern clPtr<clBitmap> g_JoystickImage;
extern int g_JoystickTexture;

extern clPtr<ScreenJoystick> g_ScreenJoystick;
