#include "Globals.h"

clVirtualTrackball g_Trackball;
sMouseState g_MouseState;

clPtr<clSDLWindow> g_Window;
clPtr<clFileSystem> g_FS;
clPtr<iCanvas> g_Canvas;
clPtr<clGameManager> g_Game;
clPtr<clMaterialSystem> g_MatSys;
clPtr<clForwardRenderingTechnique> g_Technique;
clCamera g_Camera;
clAudioThread g_Audio;
clPtr<clAudioSource> g_BackgroundMusic;

std::unique_ptr<sLGLAPI> LGL3;

clPtr<clBitmap> g_JoystickImage;
int g_JoystickTexture;

clPtr<ScreenJoystick> g_ScreenJoystick;
