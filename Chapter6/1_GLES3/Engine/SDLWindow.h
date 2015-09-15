/*
 * Copyright (C) 2013-2015 Sergey Kosarevsky (sk@linderdaum.com)
 * Copyright (C) 2013-2015 Viktor Latypov (vl@linderdaum.com)
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

#include <atomic>
#include <string>
#include <memory>
#include <SDL2/SDL.h>

#include "Core/VecMath.h"
#include "IntrusivePtr.h"

class clSDLWindow: public iIntrusiveCounter
{
public:
	clSDLWindow( const std::string& Title, int Width, int Height );
	virtual ~clSDLWindow();

	void RequestExit();
	void Swap();

	bool HandleInput();

	vec2 GetNormalizedPoint( int x, int y ) const;

	float GetAspect() const { return m_Width / m_Height; }
	int GetWidth() const { return static_cast<int>( m_Width ); }
	int GetHeight( ) const { return static_cast<int>( m_Height ); }

public:
	static clPtr<clSDLWindow> CreateSDLWindow( const std::string& Title, int Width, int Height );

private:
	bool HandleEvent( const SDL_Event& Event );

private:
	SDL_Window* m_Window;
	SDL_GLContext m_Context;
	float m_Width;
	float m_Height;
	std::atomic<bool> m_PendingExit;
};
