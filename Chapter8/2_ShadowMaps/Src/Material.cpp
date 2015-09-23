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

#include "Material.h"
#include "LGL/GLClasses.h"

static const char g_vShaderStr[] =
   R"(
   uniform mat4 in_ModelViewProjectionMatrix;
	uniform mat4 in_NormalMatrix;
	uniform mat4 in_ModelMatrix;
	uniform mat4 in_ShadowMatrix;
   in vec4 in_Vertex;
   in vec2 in_TexCoord;
	in vec3 in_Normal;
   out vec2 v_Coords;
	out vec3 v_Normal;
	out vec3 v_WorldNormal;
	out vec4 v_ProjectedVertex;
	out vec4 v_ShadowMapCoord;

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
      v_Coords = in_TexCoord.xy;
		v_Normal = mat3(in_NormalMatrix) * in_Normal;
		v_WorldNormal = ( in_ModelMatrix * vec4( in_Normal, 0.0 ) ).xyz;
		v_ProjectedVertex = GetProjScaleBiasMat() * in_ModelViewProjectionMatrix * in_Vertex;
		v_ShadowMapCoord = in_ShadowMatrix * in_ModelMatrix * in_Vertex;
      gl_Position = in_ModelViewProjectionMatrix * in_Vertex;
   }
	)";

static const char g_fShaderAmbientStr[] =
   R"(
   in vec2 v_Coords;
	in vec3 v_Normal;
	in vec3 v_WorldNormal;
   out vec4 out_FragColor;
	uniform vec4 u_AmbientColor;
	uniform vec4 u_DiffuseColor;
   void main()
   {
      out_FragColor = u_AmbientColor;
   }
	)";

static const char g_fShaderLightStr[] =
   R"(
   in vec2 v_Coords;
	in vec3 v_Normal;
	in vec3 v_WorldNormal;
	in vec4 v_ProjectedVertex;
	in vec4 v_ShadowMapCoord;
   out vec4 out_FragColor;
	uniform vec4 u_AmbientColor;
	uniform vec4 u_DiffuseColor;
	uniform vec3 u_LightPos;
	uniform vec3 u_LightDir;
	uniform vec4 u_LightDiffuse;
	uniform sampler2D Texture0;

	float PCF5x5( const vec2 ShadowCoord, float Depth )
	{
		float Size = 1.0 / float( textureSize( Texture0, 0 ).x );
		float Shadow = ( Depth >= texture( Texture0, ShadowCoord ).r ) ? 1.0 : 0.0;
		for ( int v=-2; v<=2; v++ ) for ( int u=-2; u<=2; u++ )
		{
	      Shadow += ( Depth >= texture( Texture0, ShadowCoord + Size * vec2(u, v) ).r ) ? 1.0 : 0.0;
	   }

	   return Shadow / 26.0;
	}

	float ComputeSpotLightShadow()
	{
		vec4 ShadowCoords4 = v_ShadowMapCoord / v_ShadowMapCoord.w;

		if ( ShadowCoords4.w > 0.0 )
		{
			vec2 ShadowCoord = vec2( ShadowCoords4 );
			float DepthBias = -0.0002;
			float ShadowSample = 1.0 - PCF5x5( ShadowCoord, ShadowCoords4.z + DepthBias );
			float ShadowCoef = 0.3;
			return ShadowSample * ShadowCoef;
		}

		return 1.0;
	}

   void main()
   {
		vec4 Kd = u_DiffuseColor * u_LightDiffuse;

		vec3 L = normalize( u_LightDir );
		vec3 N = normalize( v_WorldNormal );

		float d = clamp( dot( -L, N ), 0.0, 1.0 );

		vec4 Color = Kd * d * ComputeSpotLightShadow();

		Color.w = 1.0;

		out_FragColor = Color;
   }
	)";

static const char g_vShaderShadowStr[] =
   R"(
   uniform mat4 in_ModelViewProjectionMatrix;
   in vec4 in_Vertex;
   void main()
   {
      gl_Position = in_ModelViewProjectionMatrix * in_Vertex;
   }
	)";

static const char g_fShaderShadowStr[] =
   R"(
   out vec4 out_FragColor;
   void main()
   {
      out_FragColor = vec4( 1, 1, 1, 1 );
   }
	)";

clMaterialSystem::clMaterialSystem()
{
	m_ShaderPrograms[ ePass_Ambient ] = make_intrusive<clGLSLShaderProgram>( g_vShaderStr, g_fShaderAmbientStr );
	m_ShaderPrograms[ ePass_Light   ] = make_intrusive<clGLSLShaderProgram>( g_vShaderStr, g_fShaderLightStr );
	m_ShaderPrograms[ ePass_Shadow  ] = make_intrusive<clGLSLShaderProgram>( g_vShaderShadowStr, g_fShaderShadowStr );
}

clPtr<clGLSLShaderProgram> clMaterialSystem::GetShaderProgramForPass( ePass Pass, const sMaterial& Mtl )
{
	return m_ShaderPrograms[ Pass ];
}
