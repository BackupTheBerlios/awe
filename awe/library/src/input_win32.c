#include "windows.h"


//event lock
static CRITICAL_SECTION _lock;


//installs event lock
void _install_event_lock()
{
    InitializeCriticalSection(&_lock);
}


//locks events
void _lock_events()
{
    EnterCriticalSection(&_lock);
}


//unlocks events
void _unlock_events()
{
    LeaveCriticalSection(&_lock);
}
