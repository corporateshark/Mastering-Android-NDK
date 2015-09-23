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

#pragma once

#include <vector>

#include "IntrusivePtr.h"
#include "VecMath.h"
#include "GLFrameBuffer.h"

class clRenderOp;
class clMaterialSystem;
class clLightNode;
class clGLFrameBuffer;

class iLight: public iIntrusiveCounter
{
public:
	iLight()
		: m_Ambient( 0.2f )
		, m_Diffuse( 0.8f )
		, m_Position( 0 )
		, m_Direction( 0.0f, 0.0f, 1.0f )
		, m_SpotOuterAngle( 45.0f )
	{}

	void UpdateROPUniforms( const std::vector<clRenderOp>& ROPs, const clPtr<clMaterialSystem>& MatSys, const clPtr<clLightNode>& LightNode ) const;

	mat4 GetViewForShadowMap() const;
	mat4 GetProjectionForShadowMap() const;

	clPtr<clGLFrameBuffer> GetShadowMap() const;

public:
	vec4 m_Ambient;
	vec4 m_Diffuse;
	vec3 m_Position;
	vec3 m_Direction;
	float m_SpotOuterAngle;

	mutable clPtr<clGLFrameBuffer> m_ShadowMap;
};
