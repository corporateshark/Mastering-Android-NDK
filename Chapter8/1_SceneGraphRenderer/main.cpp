/*
 * Copyright (C) 2013-2015 Sergey Kosarevsky (sk@linderdaum.com)
 * Copyright (C) 2013-2015 Viktor Latypov (vl@linderdaum.com)
 * Based on Linderdaum Engine http://www.linderdaum.com
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must display the names 'Sergey Kosarevsky' and
 *    'Viktor Latypov'in the credits of the application, if such credits exist.
 *    The authors of this work must be notified via email (sk@linderdaum.com) in
 *    this case of redistribution.
 *
 * 3. Neither the name of copyright holders nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS ``AS
 * IS'' AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL COPYRIGHT HOLDERS OR CONTRIBUTORS
 * BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

#include "Timers.h"
#include "FileSystem.h"
#include "Blob.h"
#include "SDLLibrary.h"
#include "SDLWindow.h"
#include "LGL/LGL.h"
#include "Src/Nodes.h"
#include "Src/Traversers.h"
#include "Src/Camera.h"

#if !defined( ANDROID )
#  undef main
#endif

std::unique_ptr<sLGLAPI> LGL3;

clPtr<clSDLWindow> g_Window;
clPtr<clFileSystem> g_FS;

void OnStart( const std::string& RootPath )
{
	g_FS = make_intrusive<clFileSystem>();
	g_FS->Mount( "" );
	g_FS->Mount( RootPath );
	g_FS->AddAliasMountPoint( RootPath, "assets" );
}

void OnStop()
{
}

void OnKey( int KeyCode, bool Pressed )
{
	if ( KeyCode == VK_ESCAPE )
	{
		g_Window->RequestExit();
	}
}

void OnTouch( int X, int Y, bool Touch )
{
}

void OnMove( int X, int Y )
{
}

clTransformUpdateTraverser g_TransformUpdateTraverser;
clROPsTraverser g_ROPsTraverser;
clPtr<clSceneNode> g_Scene;

clPtr<clGeometryNode> g_Box;
clCamera g_Camera;

void OnDrawFrame()
{
	static float Angle = 0.0f;

	g_Box->SetLocalTransform(
	   mat4::GetRotateMatrixAxis( Angle, vec3( 1, 1, 1 ) )
	);

	Angle += 0.01f;

	g_TransformUpdateTraverser.Traverse( g_Scene );
	g_ROPsTraverser.Traverse( g_Scene );

	const auto& RenderQueue = g_ROPsTraverser.GetRenderQueue();

	sMatrices Matrices;
	Matrices.m_ProjectionMatrix = Math::Perspective( 45.0f, g_Window->GetAspect(), 0.4f, 2000.0f );
	Matrices.m_ViewMatrix = g_Camera.GetViewMatrix();

	LGL3->glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
	LGL3->glEnable( GL_DEPTH_TEST );

	for ( const auto& ROP : RenderQueue )
	{
		ROP.Render( Matrices );
	}
}

int main( int argc, char* argv[] )
{
	clSDL SDLLibrary;

	g_Window = clSDLWindow::CreateSDLWindow( "SceneGraph Rendering", 1024, 768 );

	LGL3 = std::unique_ptr<sLGLAPI>( new sLGLAPI() );

	LGL::GetAPI( LGL3.get() );

	OnStart( "" );

	g_Camera.SetPosition( vec3( 0, 0, 5 ) );

	g_Scene = make_intrusive<clSceneNode>();

	auto MaterialNode = make_intrusive<clMaterialNode>();
	sMaterial Material;
	Material.m_Ambient = vec4( 0.1f, 0.0f, 0.0f, 1.0f );
	Material.m_Diffuse = vec4( 0.9f, 0.0f, 0.0f, 1.0f );
	MaterialNode->SetMaterial( Material );

	{
		auto VA = clGeomServ::CreateAxisAlignedBox( vec3( -1 ), vec3( +1 ) );
		g_Box = make_intrusive<clGeometryNode>();
		g_Box->SetVertexAttribs( VA );
		MaterialNode->Add( g_Box );
	}

	// assemble
	g_Scene->Add( MaterialNode );

	while ( g_Window && g_Window->HandleInput() )
	{
		OnDrawFrame();

		g_Window->Swap();
	}

	OnStop();

	g_Window = nullptr;

	return 0;
}
