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

#include <algorithm>
#include <vector>

#include "IntrusivePtr.h"
#include "VecMath.h"
#include "Material.h"
#include "Geometry.h"
#include "Light.h"
#include "LGL/GLClasses.h"

class clSceneNode;
class clMaterialNode;
class clGeometryNode;
class clLightNode;
class iLight;

class iSceneTraverser: public iIntrusiveCounter
{
public:
	virtual void Traverse( clPtr<clSceneNode> Node );
	virtual void Reset() = 0;

protected:
	friend class clSceneNode;
	friend class clMaterialNode;
	friend class clGeometryNode;
	friend class clLightNode;

	virtual void PreAcceptSceneNode( clSceneNode* Node ) {};
	virtual void PostAcceptSceneNode( clSceneNode* Node ) {};

	virtual void PreAcceptMaterialNode( clMaterialNode* Node ) {};
	virtual void PostAcceptMaterialNode( clMaterialNode* Node ) {};

	virtual void PreAcceptGeometryNode( clGeometryNode* Node ) {};
	virtual void PostAcceptGeometryNode( clGeometryNode* Node ) {};

	virtual void PreAcceptLightNode( clLightNode* Node ) {};
	virtual void PostAcceptLightNode( clLightNode* Node ) {};
};

class clSceneNode: public iIntrusiveCounter
{
public:
	clSceneNode()
		: m_LocalTransform( mat4::Identity() )
		, m_GlobalTransform( mat4::Identity() )
	{
	}

	virtual void Add( const clPtr<clSceneNode>& Node )
	{
		m_ChildNodes.push_back( Node );
	}
	virtual void Remove( const clPtr<clSceneNode>& Node )
	{
		m_ChildNodes.erase(
		   std::remove( m_ChildNodes.begin(), m_ChildNodes.end(), Node ),
		   m_ChildNodes.end()
		);
	}

	void SetLocalTransform( const mat4& Mtx ) { m_LocalTransform = Mtx; };
	const mat4& GetLocalTransformConst() const { return m_LocalTransform; };
	mat4& GetLocalTransform() { return m_LocalTransform; };
	const mat4& GetGlobalTransformConst() const { return m_GlobalTransform; };
	mat4& GetGlobalTransform() { return m_GlobalTransform; };
	virtual void SetGlobalTransform( const mat4& Mtx ) { m_GlobalTransform = Mtx; };

	void RecalculateLocalFromGlobal();

	virtual void AcceptTraverser( iSceneTraverser* Traverser );

protected:
	void AcceptChildren( iSceneTraverser* Traverser );

protected:
	mat4  m_LocalTransform;
	mat4  m_GlobalTransform;
	std::vector< clPtr<clSceneNode> > m_ChildNodes;
};

class clMaterialNode: public clSceneNode
{
public:
	clMaterialNode()
	{}

	sMaterial& GetMaterial() { return m_Material; }
	const sMaterial& GetMaterial() const { return m_Material; }
	void SetMaterial( const sMaterial& Mtl ) { m_Material = Mtl; }

	virtual void AcceptTraverser( iSceneTraverser* Traverser ) override;

private:
	sMaterial m_Material;
};

class clGeometryNode: public clSceneNode
{
public:
	clGeometryNode()
	{}

	clPtr<clVertexAttribs> GetVertexAttribs() const { return m_VertexAttribs; }
	void SetVertexAttribs( const clPtr<clVertexAttribs>& VA ) { m_VertexAttribs = VA; }
	clPtr<clGLVertexArray> GetVA() const;

	virtual void AcceptTraverser( iSceneTraverser* Traverser ) override;

private:
	clPtr<clVertexAttribs> m_VertexAttribs;
	mutable clPtr<clGLVertexArray> m_VA;
};

class clLightNode: public clSceneNode
{
public:
	clLightNode()
	{}

	clPtr<iLight> GetLight() const { return m_Light; }
	void SetLight( const clPtr<iLight>& Light ) { m_Light = Light; }

	virtual void AcceptTraverser( iSceneTraverser* Traverser ) override;

	clPtr<iLight> m_Light;
};
