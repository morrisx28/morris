#ifndef _PLATFORM_THREAD_H_
#define _PLATFORM_THREAD_H_
//#include "Platform.h"
/************************************************************************/
/* Fukuan Hsu                                                           */
/* Custom Windows/Linux Thread relative Mechanism                             */
/************************************************************************/

#ifdef PLATFORM_WIN
#include <windows.h>
#else
#include <unistd.h>
#include <pthread.h>
#define  CRITICAL_SECTION  pthread_mutex_t
#endif

namespace APP_UTIL{

/************************************************************************/
/* usage: use Lock() and unLock() paired in Local Code Segment          */
/************************************************************************/
class AppLock{

private:
	int lock;

public:
	AppLock(){lock = 0;}
	void Lock(){
		while(lock){
#ifdef PLATFORM_WIN			
			Sleep(5);
#else
			usleep(5*1000);
#endif
		}
		lock = 1;
	}

	void unLock(){
		lock = 0;
	}
};
#ifdef PLATFORM_WIN
class AppEvent{
private:
	HANDLE handle;
public:

	AppEvent(){
		handle = CreateEvent(0, FALSE, FALSE, 0);
	}
	~AppEvent(){
		if(handle){
			CloseHandle(handle);
			handle = 0;
		}
	}
	void signal(){
		SetEvent(handle);
	}
	void resetEvent(){
		ResetEvent(handle);
	}
	void wait(){
		WaitForSingleObject(handle,INFINITE);
	}

};
#endif
/************************************************************************/
/* Custom Critical Section                                              */
/************************************************************************/

class AppCS{
private:
	CRITICAL_SECTION cs;
public:
	friend class AppLock;
	AppCS(){
		//init a critical section
#ifdef PLATFORM_WIN
		InitializeCriticalSection(&cs);
#else
		pthread_mutex_init(&cs,NULL);
#endif	
	}
	~AppCS(){
		
#ifdef PLATFORM_WIN
		DeleteCriticalSection(&cs);
#else
		pthread_mutex_destroy(&cs);
#endif
	}
#ifdef PLATFORM_WIN
	void lock(){
		EnterCriticalSection(&cs);
	}
	void unLock(){
		LeaveCriticalSection(&cs);
	}
#else
	void lock(){
		pthread_mutex_lock(&cs);
	}
	void unLock(){
		pthread_mutex_unlock(&cs);
	}
#endif
};

/************************************************************************/
/* usage: use Lock() only in Local Code Segment                         */
/************************************************************************/
class AppMutex{

private:
	AppCS *mutex;
public:
	AppMutex(AppCS *_mutex):mutex(_mutex)
	{
		mutex->lock();
	}
	~AppMutex(){
		mutex->unLock();
	}

};


}//end namespace APP_UTIL

#endif//end _PLATFORM_THREAD_H_
