#include <pthread.h>


//event lock
static pthread_mutex_t _lock;


//installs event lock
void _install_event_lock()
{
   pthread_mutex_init(&_lock, NULL);
}


//locks events
void _lock_events()
{
    pthread_mutex_lock(&_lock);
}


//unlocks events
void _unlock_events()
{
    pthread_mutex_unlock(&_lock);
}
