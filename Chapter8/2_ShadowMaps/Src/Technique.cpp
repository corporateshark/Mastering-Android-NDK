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

#include <iterator>
#include <functional>
#include <algorithm>

#include "Technique.h"
#include "LGL/LGL.h"

extern std::unique_ptr<sLGLAPI> LGL3;

void RenderROPs( sMatrices& Matrices, const std::vector<clRenderOp>& RenderQueue, ePass Pass, const clPtr<clMaterialSystem>& MatSys )
{
	for ( const auto& ROP : RenderQueue )
	{
		ROP.Render( Matrices, MatSys, Pass );
	}
}

void UpdateShadowMaps( const std::vector<clLightNode*>& Lights, const std::vector<clRenderOp>& ROPs, const clPtr<clMaterialSystem>& MatSys )
{
	LGL3->glDisable( GL_BLEND );

	for ( size_t i = 0; i != Lights.size(); i++ )
	{
		sMatrices ShadowMatrices;

		clPtr<iLight> L = Lights[ i ]->GetLight();

		clPtr<clGLFrameBuffer> ShadowBuffer = L->GetShadowMap();

		ShadowBuffer->Bind( 0 );
		LGL3->glClearColor( 0, 0, 0, 1 );
		LGL3->glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

		LMatrix4 Proj = L->GetProjectionForShadowMap();
		LMatrix4 MV = L->GetViewForShadowMap();

		ShadowMatrices.m_ViewMatrix = MV;
		ShadowMatrices.m_ProjectionMatrix = Proj;
		ShadowMatrices.UpdateMatrices();
		L->UpdateROPUniforms( ROPs, MatSys, Lights[i] );

		RenderROPs( ShadowMatrices, ROPs, ePass_Shadow, MatSys );

		ShadowBuffer->UnBind();
	}
}

clForwardRenderingTechnique::clForwardRenderingTechnique()
{
}

std::vector<clRenderOp> Select( const std::vector<clRenderOp>& ROPs, const std::function<bool( const clRenderOp& )>& Func )
{
	std::vector<clRenderOp> Result;

	std::copy_if( ROPs.begin(), ROPs.end(),  std::back_inserter( Result ), Func );

	return Result;
}

void clForwardRenderingTechnique::Render( const clPtr<clSceneNode> Root, const mat4& Proj, const mat4& View, const clPtr<clMaterialSystem>& MatSys )
{
	m_TransformUpdateTraverser.Traverse( Root );
	m_ROPsTraverser.Traverse( Root );

	const auto& RenderQueue = m_ROPsTraverser.GetRenderQueue();

	auto RenderQueue_Opaque = Select( RenderQueue, []( const clRenderOp & ROP ) { return ROP.m_Material->GetMaterial().m_MaterialClass != "Particle"; } );
	auto RenderQueue_Transparent = Select( RenderQueue, []( const clRenderOp & ROP ) { return ROP.m_Material->GetMaterial().m_MaterialClass == "Particle"; } );

	sMatrices Matrices;
	Matrices.m_ProjectionMatrix = Proj;
	Matrices.m_ViewMatrix = View;
	Matrices.UpdateMatrices();

	LGL3->glClearColor( 0.0f, 0.0f, 0.0f, 0.0f );
	LGL3->glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
	LGL3->glEnable( GL_DEPTH_TEST );

	// 1. Render ambient lighting and fill the Z-buffer for opaque objects
	LGL3->glDepthFunc( GL_LEQUAL );
	LGL3->glDisablei( GL_BLEND, 0 );

	RenderROPs( Matrices, RenderQueue_Opaque, ePass_Ambient, MatSys );

	// 2. Update shadow maps
	auto Lights = m_ROPsTraverser.GetLights();

	UpdateShadowMaps( Lights, RenderQueue, MatSys );

	// 3. Add per-light lighting
	LGL3->glDepthFunc( GL_EQUAL );
	LGL3->glBlendFunc( GL_ONE, GL_ONE );
	LGL3->glEnablei( GL_BLEND, 0 );

	for ( const auto& L : Lights )
	{
		L->GetLight()->UpdateROPUniforms( RenderQueue, MatSys, L );

		RenderROPs( Matrices, RenderQueue, ePass_Light, MatSys );
	}

	// 4. Render ambient lighting and fill the Z-buffer for transparent objects
	LGL3->glBlendFunc( GL_SRC_ALPHA, GL_ONE );
	LGL3->glDepthFunc( GL_LESS );
	LGL3->glEnablei( GL_BLEND, 0 );
	LGL3->glDepthMask( GL_FALSE );

	RenderROPs( Matrices, RenderQueue_Transparent, ePass_Ambient, MatSys );

	LGL3->glDepthMask( GL_TRUE );
}
