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

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include <vector>

class clGString
{
public:
	void GenerateSound()
	{
		FSoundLen = 44100 * 8; // 4sec, 1 channel, 16 bit

		float Frc = 0.5f;

		InitString( Frc );

		FSamples.resize( FSoundLen );
		FSound.resize( FSoundLen );

		float MaxS = 0;

		int i;

		for ( i = 0 ; i < FSoundLen ; i++ )
		{
			FSamples[i] = Step();

			if ( MaxS < fabs( FSamples[i] ) ) { MaxS = fabs( FSamples[i] ); }
		}

		float k = 32767.0 / MaxS;

		for ( i = 0 ; i < FSoundLen ; i++ )
		{
			FSound[i] = FSamples[i] * k;
		}
	}

	std::vector<short int> FSound;

private:
	int FSoundLen;
	int FPickPos;

	std::vector<float> FSamples;

	std::vector<float> FForce;
	std::vector<float> FVel;
	std::vector<float> FPos;

	float k1, k2;
	int FStringLen;

	void InitString( float Freq )
	{
		int i;

		FStringLen = 200;

		FPos.resize( FStringLen );
		FVel.resize( FStringLen );
		FForce.resize( FStringLen );

		const float Damping = 1.0f / 512.0f;
		k1 = 1 - Damping;
		k2 = Damping / 2.0f;

		FPickPos = FStringLen * 5 / 100;

		for ( i = 0 ; i < FStringLen ; i++ )
		{
			FVel[i] = FPos[i] = 0;
		}

		for ( i = 1 ; i < FStringLen - 1 ; i++ )
		{
			float m = 1.0f + 0.5f * ( frand() - 0.5f );
			FForce[i] = Freq / m;
		}

		for ( i = FStringLen / 2 ; i < FStringLen - 1 ; i++ )
		{
			FVel[i] = 1;
		}
	}

	inline float frand() { return ( ( ( float )rand() ) / ( float )RAND_MAX ); }

	float Step()
	{
		FPos[0] = FPos[FStringLen - 1] = 0;
		FVel[0] = FVel[FStringLen - 1] = 0;

		for ( int i = 1 ; i < FStringLen - 1 ; i++ )
		{
			float d = ( FPos[i - 1] + FPos[i + 1] ) * 0.5f - FPos[i];
			FVel[i] += d * FForce[i];
		}

		for ( int i = 1 ; i < FStringLen - 1 ; i++ )
		{
			FVel[i] = FVel[i] * k1 + ( FVel[i - 1] + FVel[i + 1] ) * k2;
		}

		for ( int i = 1 ; i < FStringLen ; i++ )
		{
			FPos[i] += FVel[i];
		}

		return FPos[FPickPos];
	}
};
