// alarm.cc
//	Routines to use a hardware timer device to provide a
//	software alarm clock.  For now, we just provide time-slicing.
//
//	Not completely implemented.
//
// Copyright (c) 1992-1996 The Regents of the University of California.
// All rights reserved.  See copyright.h for copyright notice and limitation 
// of liability and disclaimer of warranty provisions.

#include "copyright.h"
#include "alarm.h"
#include "main.h"

//----------------------------------------------------------------------
// Alarm::Alarm
//      Initialize a software alarm clock.  Start up a timer device
//
//      "doRandom" -- if true, arrange for the hardware interrupts to 
//		occur at random, instead of fixed, intervals.
//----------------------------------------------------------------------

Alarm::Alarm(bool doRandom)
{
    timer = new Timer(doRandom, this);
}

//----------------------------------------------------------------------
// Alarm::CallBack
//	Software interrupt handler for the timer device. The timer device is
//	set up to interrupt the CPU periodically (once every TimerTicks).
//	This routine is called each time there is a timer interrupt,
//	with interrupts disabled.
//
//	Note that instead of calling Yield() directly (which would
//	suspend the interrupt handler, not the interrupted thread
//	which is what we wanted to context switch), we set a flag
//	so that once the interrupt handler is done, it will appear as 
//	if the interrupted thread called Yield at the point it is 
//	was interrupted.
//
//	For now, just provide time-slicing.  Only need to time slice 
//      if we're currently running something (in other words, not idle).
//	Also, to keep from looping forever, we check if there's
//	nothing on the ready list, and there are no other pending
//	interrupts.  In this case, we can safely halt.
//----------------------------------------------------------------------

void 
Alarm::CallBack() 
{
	Interrupt *interrupt = kernel->interrupt;
	MachineStatus status = interrupt->getStatus();
	bool wake = s_hotel.PutWakeUp();

	if (status == IdleMode && wake == false && s_hotel.CheckEmpty() ) 
	{
		if (!interrupt->AnyFutureInterrupts()) 
		{
			timer->Disable();	// turn off the timer
		}
	} else 
	{
		interrupt->YieldOnReturn();
	}
}


void Alarm::WaitUntil(int sleepTime)
{
	cout << "Alarm::WaitUntil() has been called" << endl;
	IntStatus previousLevel = kernel->interrupt->SetLevel(IntOff);
	Thread* td = kernel->currentThread;
	s_hotel.PutSleep(td, sleepTime);
	kernel->interrupt->SetLevel(previousLevel);
}

bool SleepHotel::CheckEmpty()
{
	if(sleepList.size() == 0)
	{
		return true;
	}
	else
	{
		return false;
	}
}

void SleepHotel::PutSleep(Thread* td, int sleepTime)
{
	ASSERT(kernel->interrupt->getLevel() == IntOff);
	sleepList.push_back(SleepRoom(td, timeNow + sleepTime));
	td->Sleep(false);
}


bool SleepHotel::PutWakeUp()
{
	bool wake = false;
	timeNow++;
	for(std::list<SleepRoom>::iterator pos = sleepList.begin(); pos != sleepList.end();)
	{
		if(timeNow >= pos->wakeUpTime)
		{
			wake = true;
			cout << "Thread wakes up" << endl;
			kernel->scheduler->ReadyToRun(pos->sleepThread);
			pos = sleepList.erase(pos);
		}
		else
		{
			pos++;
		}
	}
	return wake;

}
