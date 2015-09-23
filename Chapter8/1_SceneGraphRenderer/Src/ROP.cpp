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

#include "ROP.h"

#include "LGL/GLClasses.h"

static const char g_vShaderStr[] =
   R"(
   uniform mat4 in_ModelViewProjectionMatrix;
	uniform mat4 in_NormalMatrix;
	uniform mat4 in_ModelMatrix;
   in vec4 in_Vertex;
   in vec2 in_TexCoord;
	in vec3 in_Normal;
   out vec2 v_Coords;
	out vec3 v_Normal;
	out vec3 v_WorldNormal;
   void main()
   {
      v_Coords = in_TexCoord.xy;
		v_Normal = mat3(in_NormalMatrix) * in_Normal;
		v_WorldNormal = ( in_ModelMatrix * vec4( in_Normal, 0.0 ) ).xyz;
      gl_Position = in_ModelViewProjectionMatrix * in_Vertex;
   }
	)";

static const char g_fShaderStr[] =
   R"(
   in vec2 v_Coords;
	in vec3 v_Normal;
	in vec3 v_WorldNormal;
   out vec4 out_FragColor;
	uniform vec4 u_AmbientColor;
	uniform vec4 u_DiffuseColor;
   void main()
   {
		vec4 Ka = u_AmbientColor;
		vec4 Kd = u_DiffuseColor;

		// fixed light from the camera
		vec3 L = normalize( vec3( 0.0, 0.0, 1.0 ) );

		float d = clamp( dot( L, normalize(v_WorldNormal) ), 0.0, 1.0 );

		vec4 Color = Ka + Kd * d;
		
      out_FragColor = Color;
   }
	)";

clPtr<clGLSLShaderProgram> g_ShaderProgram;

void clRenderOp::Render( sMatrices& Matrices ) const
{
	if ( !g_ShaderProgram )
	{
		g_ShaderProgram = make_intrusive<clGLSLShaderProgram>( g_vShaderStr, g_fShaderStr );
	}

	Matrices.m_ModelMatrix = this->m_Geometry->GetGlobalTransformConst();
	Matrices.UpdateMatrices();

	g_ShaderProgram->Bind();
	g_ShaderProgram->SetUniformNameVec4Array( "u_AmbientColor", 1, this->m_Material->GetMaterial().m_Ambient );
	g_ShaderProgram->SetUniformNameVec4Array( "u_DiffuseColor", 1, this->m_Material->GetMaterial().m_Diffuse );
	g_ShaderProgram->SetUniformNameMat4Array( "in_ProjectionMatrix", 1, Matrices.m_ProjectionMatrix );
	g_ShaderProgram->SetUniformNameMat4Array( "in_ViewMatrix", 1, Matrices.m_ViewMatrix );
	g_ShaderProgram->SetUniformNameMat4Array( "in_ModelMatrix", 1, Matrices.m_ModelMatrix );
	g_ShaderProgram->SetUniformNameMat4Array( "in_NormalMatrix", 1, Matrices.m_NormalMatrix );
	g_ShaderProgram->SetUniformNameMat4Array( "in_ModelViewMatrix", 1, Matrices.m_ModelViewMatrix );
	g_ShaderProgram->SetUniformNameMat4Array( "in_ModelViewProjectionMatrix", 1, Matrices.m_ModelViewProjectionMatrix );

	this->m_Geometry->GetVA()->Draw( false );
}
