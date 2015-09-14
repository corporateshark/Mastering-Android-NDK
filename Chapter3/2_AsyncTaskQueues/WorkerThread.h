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

#include <list>
#include <string>
#include <atomic>

#include "tinythread.h"
#include "Thread.h"
#include "IntrusivePtr.h"

class iTask: public iIntrusiveCounter
{
public:
	iTask()
		: FIsPendingExit( false )
		, FTaskID( 0 )
		, FPriority( 0 )
	{};

	virtual void Run() = 0;
	virtual void Exit() { FIsPendingExit = true; }
	virtual bool IsPendingExit() const volatile { return FIsPendingExit; }

	virtual void   SetTaskID( size_t ID ) { FTaskID = ID; };
	virtual size_t GetTaskID() const { return FTaskID; };

	virtual void   SetPriority( int Priority ) { FPriority = Priority; };
	virtual int    GetPriority() const { return FPriority; };

private:
	std::atomic<bool> FIsPendingExit;
	size_t            FTaskID;
	int               FPriority;
};

class clWorkerThread: public iIntrusiveCounter, public iThread
{
private:
	std::list< clPtr<iTask> >   FPendingTasks;
	clPtr<iTask>                FCurrentTask;
	mutable tthread::mutex      FTasksMutex;
	tthread::condition_variable FCondition;

public:
	virtual void AddTask( const clPtr<iTask>& Task )
	{
		tthread::lock_guard<tthread::mutex> Lock( FTasksMutex );
		FPendingTasks.push_back( Task );
		FCondition.notify_all();
	}

	virtual bool   CancelTask( size_t ID )
	{
		if ( !ID ) { return false; }

		tthread::lock_guard<tthread::mutex> Lock( FTasksMutex );

		if ( FCurrentTask && FCurrentTask->GetTaskID() == ID )
		{
			FCurrentTask->Exit();
		}

		FPendingTasks.remove_if(
			[ID]( const clPtr<iTask> T )
			{
				if ( T->GetTaskID() == ID )
				{
					T->Exit();
					return true;
				}
				return false;
			}
		);

		FCondition.notify_all();

		return true;
	}

	virtual void   CancelAll()
	{
		tthread::lock_guard<tthread::mutex> Lock( FTasksMutex );

		if ( FCurrentTask ) { FCurrentTask->Exit(); }

		for ( auto& Task : FPendingTasks )
		{
			Task->Exit();
		}

		FPendingTasks.clear();

		FCondition.notify_all();
	}

	virtual size_t GetQueueSize() const
	{
		tthread::lock_guard<tthread::mutex> Lock( FTasksMutex );

		return FPendingTasks.size() + ( FCurrentTask ? 1 : 0 );
	}

protected:
	virtual void Run() override
	{
		while ( !IsPendingExit() )
		{
			FCurrentTask = ExtractTask();

			if ( FCurrentTask && !FCurrentTask->IsPendingExit() )
			{
				FCurrentTask->Run();
			}

			// we need to reset current task since ExtractTask() is blocking operation and could take some time
			FCurrentTask = nullptr;
		}
	}

	virtual void NotifyExit()
	{
		FCondition.notify_all();
	}

private:
	clPtr<iTask> ExtractTask()
	{
		tthread::lock_guard<tthread::mutex> Lock( FTasksMutex );

		while ( FPendingTasks.empty() && !IsPendingExit() )
		{
			FCondition.wait( FTasksMutex );
		}

		if ( FPendingTasks.empty() )
		{
			return clPtr<iTask>();
		}

		auto Best = FPendingTasks.begin();

		for ( auto& Task : FPendingTasks )
		{
			if ( Task->GetPriority() > ( *Best )->GetPriority() )
			{
				*Best = Task;
			}
		}

		clPtr<iTask> Result = *Best;

		FPendingTasks.erase( Best );

		return Result;
	}
};
