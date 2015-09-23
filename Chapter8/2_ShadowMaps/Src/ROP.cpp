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

void clRenderOp::Render( sMatrices& Matrices, const clPtr<clMaterialSystem>& MatSys, ePass Pass ) const
{
	auto SP = MatSys->GetShaderProgramForPass( Pass, this->m_Material->GetMaterial() );

	Matrices.m_ModelMatrix = this->m_Geometry->GetGlobalTransformConst();
	Matrices.UpdateMatrices();

	SP->Bind();
	SP->SetUniformNameVec4Array( "u_AmbientColor", 1, this->m_Material->GetMaterial( ).m_Ambient );
	SP->SetUniformNameVec4Array( "u_DiffuseColor", 1, this->m_Material->GetMaterial( ).m_Diffuse );
	SP->SetUniformNameMat4Array( "in_ProjectionMatrix", 1, Matrices.m_ProjectionMatrix );
	SP->SetUniformNameMat4Array( "in_ViewMatrix", 1, Matrices.m_ViewMatrix );
	SP->SetUniformNameMat4Array( "in_ModelMatrix", 1, Matrices.m_ModelMatrix );
	SP->SetUniformNameMat4Array( "in_NormalMatrix", 1, Matrices.m_NormalMatrix );
	SP->SetUniformNameMat4Array( "in_ModelViewMatrix", 1, Matrices.m_ModelViewMatrix );
	SP->SetUniformNameMat4Array( "in_ModelViewProjectionMatrix", 1, Matrices.m_ModelViewProjectionMatrix );

	this->m_Geometry->GetVA()->Draw( false );
}
