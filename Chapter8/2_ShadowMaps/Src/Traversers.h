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
#include "ROP.h"

class clTransformUpdateTraverser: public iSceneTraverser
{
public:
	clTransformUpdateTraverser()
		: m_ModelView()
	{}

	virtual void Reset() override
	{
		m_ModelView.clear();
		m_ModelView.push_back( mat4::Identity() );
	}

protected:
	virtual void PreAcceptSceneNode( clSceneNode* Node ) override
	{
		m_ModelView.push_back( Node->GetLocalTransform() * m_ModelView.back() );
		Node->SetGlobalTransform( m_ModelView.back() );
	}

	virtual void PostAcceptSceneNode( clSceneNode* Node ) override
	{
		m_ModelView.pop_back();
	}

private:
	std::vector<LMatrix4> m_ModelView;
};

class clROPsTraverser: public iSceneTraverser
{
public:
	clROPsTraverser()
		: m_RenderQueue()
		, m_Materials()
	{}

	virtual void Reset() override
	{
		m_RenderQueue.clear();
		m_Materials.clear();
		m_LightNodes.clear();
	}

	virtual const std::vector<clRenderOp>& GetRenderQueue() const
	{
		return m_RenderQueue;
	}

	virtual const std::vector<clLightNode*>& GetLights() const
	{
		return m_LightNodes;
	}

protected:
	virtual void PreAcceptSceneNode( clSceneNode* Node ) override
	{
	}
	virtual void PostAcceptSceneNode( clSceneNode* Node ) override
	{
	}
	virtual void PreAcceptGeometryNode( clGeometryNode* Node ) override
	{
		if ( !m_Materials.size() ) { return; }

		m_RenderQueue.emplace_back( Node, m_Materials.front() );
	}
	virtual void PostAcceptGeometryNode( clGeometryNode* Node ) override
	{
	}
	virtual void PreAcceptMaterialNode( clMaterialNode* Node ) override
	{
		m_Materials.push_back( Node );
	}
	virtual void PostAcceptMaterialNode( clMaterialNode* Node ) override
	{
		m_Materials.pop_back();
	}
	virtual void PreAcceptLightNode( clLightNode* Node ) override
	{
		m_LightNodes.push_back( Node );
	}
	virtual void PostAcceptLightNode( clLightNode* Node ) override
	{
	}

private:
	//
	std::vector<clRenderOp> m_RenderQueue;
	std::vector<clMaterialNode*> m_Materials;
	std::vector<clLightNode*> m_LightNodes;
};
