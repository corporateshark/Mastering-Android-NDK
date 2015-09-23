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

#include "ParticleMaterial.h"

static const char g_vShaderParticleStr[] =
   R"(
   uniform mat4 in_ModelViewProjectionMatrix;
	uniform mat4 in_NormalMatrix;
	uniform mat4 in_ModelMatrix;
	uniform mat4 in_ModelViewMatrix;
	uniform mat4 in_ShadowMatrix;
   in vec4 in_Vertex;
   in vec2 in_TexCoord;
	in vec3 in_Normal;
	in vec4 in_Color;
   out vec2 v_Coords;
	out vec3 v_Normal;
	out vec3 v_WorldNormal;
	out vec4 v_ProjectedVertex;
	out vec4 v_ShadowMapCoord;
	out vec3 v_Params;
	out vec4 v_Color;

	mat4 GetProjScaleBiasMat()
	{ 
	   // transform from -1..1 to 0..1
	   return mat4( 0.5, 0.0, 0.0, 0.0,
	                0.0, 0.5, 0.0, 0.0,
	                0.0, 0.0, 0.5, 0.0,
	                0.5, 0.5, 0.5, 1.0 );
	}

   void main()
   {
	   vec3 X = vec3( in_ModelViewMatrix[0][0], in_ModelViewMatrix[1][0], in_ModelViewMatrix[2][0] );
	   vec3 Y = vec3( in_ModelViewMatrix[0][1], in_ModelViewMatrix[1][1], in_ModelViewMatrix[2][1] );

	   float SizeX = in_Normal.z;
	   float SizeY = in_Normal.z;

		vec3 XOfs = ( 2.0 * (in_TexCoord.x-0.5) * SizeX ) * X;
		vec3 YOfs = ( 2.0 * (in_TexCoord.y-0.5) * SizeY ) * Y;

		vec3 Position = in_Vertex.xyz + XOfs + YOfs;

	   vec4 TransformedPos = in_ModelViewProjectionMatrix * vec4(Position, 1.0);

	   gl_Position = TransformedPos;

      v_Coords = in_TexCoord.xy;
		v_Normal = mat3(in_NormalMatrix) * in_Normal;
		v_WorldNormal = ( in_ModelMatrix * vec4( in_Normal, 0.0 ) ).xyz;
		v_ProjectedVertex = GetProjScaleBiasMat() * in_ModelViewProjectionMatrix * vec4(Position, 1.0);
		v_ShadowMapCoord = in_ShadowMatrix * in_ModelMatrix * vec4(Position, 1.0);

	   v_Params   = in_Normal;
	   v_Color    = in_Color;
   }
	)";

static const char g_fShaderAmbientParticleStr[] =
   R"(
   in vec2 v_Coords;
	in vec3 v_Normal;
	in vec3 v_WorldNormal;
	in vec3 v_Params; /* TTL, LifeTime, Size */
	in vec4 v_Color; /* RGBA overlay */
   out vec4 out_FragColor;
	uniform vec4 u_AmbientColor;
	uniform vec4 u_DiffuseColor;
   void main()
   {
		vec4 Color = v_Color * u_AmbientColor;

		float NormalizedTime = (v_Params.y-v_Params.x) / v_Params.y;

		float Falloff = 1.0 - 2.0 * length(v_Coords-vec2(0.5, 0.5));

		if ( NormalizedTime < 0.1 )
		{
			Falloff *= NormalizedTime / 0.1;
		}
		else if ( NormalizedTime > 0.5 )
		{
			Falloff *= 1.0 - (NormalizedTime-0.5) / 0.5;
		}

		Color.w = Falloff;

      out_FragColor = Color;
   }
	)";

static const char g_fShaderLightParticleStr[] =
   R"(
   in vec2 v_Coords;
	in vec3 v_Normal;
	in vec3 v_WorldNormal;
	in vec4 v_ProjectedVertex;
	in vec4 v_ShadowMapCoord;
	in vec3 v_Params; /* TTL, LifeTime, Size */
	in vec4 v_Color; /* RGBA overlay */
   out vec4 out_FragColor;
	uniform vec4 u_AmbientColor;
	uniform vec4 u_DiffuseColor;
	uniform vec3 u_LightPos;
	uniform vec3 u_LightDir;
	uniform vec4 u_LightDiffuse;
	uniform sampler2D Texture0;

   void main()
   {
		discard;
   }
	)";

static const char g_vShaderShadowParticleStr[] =
   R"(
   uniform mat4 in_ModelViewProjectionMatrix;
	uniform mat4 in_ModelViewMatrix;
   in vec4 in_Vertex;
   in vec2 in_TexCoord;
	in vec3 in_Normal;
	out vec2 v_Coords;
	out vec3 v_Params;
   void main()
   {
	   vec3 X = vec3( in_ModelViewMatrix[0][0], in_ModelViewMatrix[1][0], in_ModelViewMatrix[2][0] );
	   vec3 Y = vec3( in_ModelViewMatrix[0][1], in_ModelViewMatrix[1][1], in_ModelViewMatrix[2][1] );

	   float SizeX = in_Normal.z;
	   float SizeY = in_Normal.z;

		vec3 XOfs = ( 2.0 * (in_TexCoord.x-0.5) * SizeX ) * X;
		vec3 YOfs = ( 2.0 * (in_TexCoord.y-0.5) * SizeY ) * Y;

		vec3 Position = in_Vertex.xyz + XOfs + YOfs;

	   vec4 TransformedPos = in_ModelViewProjectionMatrix * vec4(Position, 1.0);

	   gl_Position = TransformedPos;

		v_Coords = in_TexCoord.xy;
	   v_Params = in_Normal;
   }
	)";

static const char g_fShaderShadowParticleStr[] =
   R"(
	in vec2 v_Coords;
	in vec3 v_Params; /* TTL, LifeTime, Size */
   out vec4 out_FragColor;
   void main()
   {
		float NormalizedTime = (v_Params.y-v_Params.x) / v_Params.y;

		float Falloff = ( NormalizedTime < 0.5 ) ? NormalizedTime : 1.0-NormalizedTime;

		if ( length(v_Coords-vec2(0.5, 0.5)) > 0.5 * Falloff ) discard;

      out_FragColor = vec4( 1.0 );
   }
	)";

clParticleMaterialSystem::clParticleMaterialSystem()
{
	m_ParticleShaderPrograms[ ePass_Ambient ] = make_intrusive<clGLSLShaderProgram>( g_vShaderParticleStr, g_fShaderAmbientParticleStr );
	m_ParticleShaderPrograms[ ePass_Light   ] = make_intrusive<clGLSLShaderProgram>( g_vShaderParticleStr, g_fShaderLightParticleStr );
	m_ParticleShaderPrograms[ ePass_Shadow  ] = make_intrusive<clGLSLShaderProgram>( g_vShaderShadowParticleStr, g_fShaderShadowParticleStr );
}

clPtr<clGLSLShaderProgram> clParticleMaterialSystem::GetShaderProgramForPass( ePass Pass, const sMaterial& Mtl )
{
	if ( Mtl.m_MaterialClass == "Particle" )
	{
		return m_ParticleShaderPrograms[ Pass ];
	}

	return clMaterialSystem::GetShaderProgramForPass( Pass, Mtl );
}
