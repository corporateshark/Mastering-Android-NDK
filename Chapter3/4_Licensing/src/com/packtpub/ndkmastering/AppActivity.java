/*
 * Copyright (C) 2015 Sergey Kosarevsky (sk@linderdaum.com)
 * Copyright (C) 2015 Viktor Latypov (vl@linderdaum.com)
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

package com.packtpub.ndkmastering;

import android.app.Activity;
import android.os.Bundle;
import android.provider.Settings.Secure;

import com.google.android.vending.licensing.LicenseChecker;
import com.google.android.vending.licensing.LicenseCheckerCallback;
import com.google.android.vending.licensing.ServerManagedPolicy;
import com.google.android.vending.licensing.AESObfuscator;

public class AppActivity extends Activity
{
	static
	{
		System.loadLibrary( "NativeLib" );
	}

	public static AppActivity m_Activity;

	private LicenseCheckerCallback m_LicenseCheckerCallback;
	private LicenseChecker m_Checker;

	public static final String BASE64_PUBLIC_KEY = "MIIBIjANBgkqhkiG9w0BAQEFAAOCAQ8AMIIBCgKCAQEAhJ9qRUZv6a36avDjoJ98D2l8X5iou6YoYEZeewo5BS4IT0Ow8cJYPpeVz8Jwm8ZVlKXy5EgpRWGBalFAbTRtD7fy2xFYgSLm6L7adTnIOPX0Q/uxlUqmHg9o09zHXixVGnGKTQ7M2aFSe0VxP8Y4drNppg6Dkcl9EcUgWhQGNfmVdEOXbH9/Zgbw1fIgFyeQuqAO//B7MSI9xB38hmDy2O1G3BD5darSaaf9L6BrNZp+dWjH/oHAxs9VoBjcsUJvzRB9N3z8siS9y+DLTvdmpzgR8GyHTU0qj82PCG7m4SbDt4EKY6IH0TnVYI60bDlMJesbH1I98jxyD84VYK/RFwIDAQAB";
	public static final byte[] SALT = new byte[] { 49, 114, 15, 4, 86, 27, 100, 46, 13, 68, 76, 36,38, 112, 5, 105, 7, 20, 30, 73 };

	@Override protected void onCreate( Bundle icicle )
	{
		super.onCreate( icicle );

		m_Activity = this;

		onCreateNative();

		CheckLicense( BASE64_PUBLIC_KEY, SALT );
	}

	public void CheckLicense( String BASE64_PUBLIC_KEY, byte[] SALT )
	{
		String deviceId = Secure.getString( getContentResolver(), Secure.ANDROID_ID );

		// Construct the LicenseCheckerCallback. The library calls this when done.
		m_LicenseCheckerCallback = new AppLicenseChecker();

		// Construct the LicenseChecker with a Policy.
		m_Checker = new LicenseChecker(
							 this, new ServerManagedPolicy(this,
							 new AESObfuscator( SALT, getPackageName(), deviceId) ),
							 BASE64_PUBLIC_KEY
						);

		m_Checker.checkAccess( m_LicenseCheckerCallback );
	}

	public static native void onCreateNative();

	// license checking
	public static native void Allow( int reason );
	public static native void DontAllow( int reason );
	public static native void ApplicationError( int errorCode );
};
