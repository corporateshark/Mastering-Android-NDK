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

#include "Platform.h"
#include "VecMath.h"

/// Virtual trackball for user interaction with rotations
class clVirtualTrackball
{
public:
	clVirtualTrackball()
		: FCurrentPoint( 0.0f )
		, FPrevPoint( 0.0f )
		, FStarted( false )
	{
		FRotation.IdentityMatrix();
		FRotationDelta.IdentityMatrix();
	};

	/**
	   Get rotation matrix for new mouse point
	**/
	virtual LMatrix4 DragTo( LVector2 ScreenPoint, float Speed, bool KeyPressed )
	{
		if ( KeyPressed && !FStarted )
		{
			StartDragging( ScreenPoint );

			FStarted = KeyPressed;

			return mat4::Identity();
		}

		FStarted = KeyPressed;

		if ( !KeyPressed ) { return mat4::Identity(); }

		FCurrentPoint = ProjectOnSphere( ScreenPoint );

		LVector3 Direction = FCurrentPoint - FPrevPoint;

		LMatrix4 RotMatrix;

		RotMatrix.IdentityMatrix();

		float Shift = Direction.Length();

		if ( Shift > Math::EPSILON )
		{
			LVector3 Axis = FPrevPoint.Cross( FCurrentPoint );

			RotMatrix.RotateMatrixAxis( Shift * Speed, Axis );
		}

		FRotationDelta = RotMatrix;

		return RotMatrix;
	}

	LMatrix4& GetRotationDelta()
	{
		return FRotationDelta;
	};

	/**
	   Get current rotation matrix
	**/
	virtual LMatrix4 GetRotationMatrix() const
	{
		return FRotation * FRotationDelta;
	}

	static clVirtualTrackball* Create()
	{
		return new clVirtualTrackball();
	}

private:
	virtual void StartDragging( LVector2 ScreenPoint )
	{
		FRotation = FRotation * FRotationDelta;
		FCurrentPoint = ProjectOnSphere( ScreenPoint );
		FPrevPoint = FCurrentPoint;
		FRotationDelta.IdentityMatrix();
	}

	LVector3 ProjectOnSphere( LVector2 ScreenPoint )
	{
		LVector3 Proj;

		// convert to -1.0...1.0 range
		Proj.x = 2.0f * ScreenPoint.x - 1.0f;
		Proj.y = -( 2.0f * ScreenPoint.y - 1.0f );
		Proj.z = 0.0f;

		float Length = Proj.Length();

		Length = ( Length < 1.0f ) ? Length : 1.0f;

		Proj.z = sqrtf( 1.001f - Length * Length );
		Proj.Normalize();

		return Proj;
	}
	LVector3   FCurrentPoint;
	LVector3   FPrevPoint;
	LMatrix4   FRotation;
	LMatrix4   FRotationDelta;
	bool FStarted;
};
