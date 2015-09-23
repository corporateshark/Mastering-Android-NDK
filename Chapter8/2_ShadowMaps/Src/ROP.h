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

#include "Nodes.h"

struct sMatrices
{
	mat4 m_ProjectionMatrix;
	mat4 m_ViewMatrix;
	mat4 m_ModelMatrix;
	//
	mat4 m_ModelViewMatrix;
	mat4 m_ModelViewMatrixInverse;
	mat4 m_ModelViewProjectionMatrix;
	mat4 m_NormalMatrix;

	void UpdateMatrices()
	{
		m_ModelViewMatrix           = m_ModelMatrix * m_ViewMatrix;
		m_ModelViewMatrixInverse    = m_ModelViewMatrix.GetInversed();
		m_ModelViewProjectionMatrix = m_ModelViewMatrix * m_ProjectionMatrix;
		m_NormalMatrix              = mat4( m_ModelViewMatrixInverse.ToMatrix3().GetTransposed() );
	}
};

class clRenderOp
{
public:
	clRenderOp( const clPtr<clGeometryNode>& G, const clPtr<clMaterialNode>& M )
		: m_Geometry( G )
		, m_Material( M )
	{}

	void Render( sMatrices& Matrices, const clPtr<clMaterialSystem>& MatSys, ePass Pass ) const;

	clPtr<clGeometryNode> m_Geometry;
	clPtr<clMaterialNode> m_Material;
};
