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

#include "Light.h"
#include "ROP.h"

mat4 GetProjScaleBiasMat()
{
	return mat4( 0.5f, 0.0f, 0.0f, 0.0f,
	             0.0f, 0.5f, 0.0f, 0.0f,
	             0.0f, 0.0f, 0.5f, 0.0f,
	             0.5f, 0.5f, 0.5f, 1.0 );
}

void iLight::UpdateROPUniforms( const std::vector<clRenderOp>& ROPs, const clPtr<clMaterialSystem>& MatSys, const clPtr<clLightNode>& LightNode ) const
{
	mat4 LightMV   = this->GetViewForShadowMap();
	mat4 LightProj = GetProjectionForShadowMap();

	mat4 Mtx = LightNode->GetGlobalTransformConst();

	vec3 Pos =  ( Mtx * vec4( this->m_Position,  1.0f ) ).XYZ();
	vec3 Dir =  ( Mtx * vec4( this->m_Direction, 0.0f ) ).XYZ();

	for ( const auto& ROP : ROPs )
	{
		auto AmbientSP = MatSys->GetShaderProgramForPass( ePass_Ambient, ROP.m_Material->GetMaterial() );
		AmbientSP->Bind();
		AmbientSP->SetUniformNameVec3Array( "u_LightPos", 1, Pos );
		AmbientSP->SetUniformNameVec3Array( "u_LightDir", 1, Dir );

		auto LightSP = MatSys->GetShaderProgramForPass( ePass_Light, ROP.m_Material->GetMaterial() );
		LightSP->Bind();
		LightSP->SetUniformNameVec3Array( "u_LightPos", 1, Pos );
		LightSP->SetUniformNameVec3Array( "u_LightDir", 1, Dir );
		LightSP->SetUniformNameVec4Array( "u_LightDiffuse", 1, this->m_Diffuse );

		mat4 ScaleBias = GetProjScaleBiasMat();
		mat4 ShadowMatrix = ( Mtx * LightMV * LightProj ) * ScaleBias;

		LightSP->SetUniformNameMat4Array( "in_ShadowMatrix", 1, ShadowMatrix );
	}

	this->GetShadowMap()->GetDepthTexture()->Bind( 0 );
}

mat4 iLight::GetViewForShadowMap() const
{
	return Math::LookAt( this->m_Position, this->m_Position + this->m_Direction, vec3( 0, 0, 1 ) );
}

mat4 iLight::GetProjectionForShadowMap() const
{
	float NearCP = 0.1f;
	float FarCP  = 1000.0f;

	return Math::Perspective( 2.0f * this->m_SpotOuterAngle, 1.0f, NearCP, FarCP );
}

clPtr<clGLFrameBuffer> iLight::GetShadowMap() const
{
	if ( !m_ShadowMap )
	{
		m_ShadowMap = make_intrusive<clGLFrameBuffer>();
		m_ShadowMap->InitRenderTargetV(
		{ ivec4( 1024, 1024, 8, 0 ) },
		true
		);
	}

	return m_ShadowMap;
}
