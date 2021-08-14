#define USE_LIBCO_LOOP 1
//#define USE_LIBEV

/**
 * every event loop file must implement the following interface.
 * 
void *createEventLoop();
int eventLoopRun(stCoEpoll_t *ctx);
uint32_t pollEvent2Me( short events );
short myEvent2Poll( uint32_t events );
int createFileEvent(stCoEpoll_t *ctx,stTimeoutItem_t *clientData,size_t idx);
void deleteFileEvent(stCoEpoll_t *ctx, stTimeoutItem_t *clientData,size_t idx);
long long createTimeEvent(stCoEpoll_t *ctx, long long milliseconds, stTimeoutItem_t *clientData);
int deleteTimeEvent(void *eventLoop, stTimeoutItem_t *clientData);
*/

// libev,libae interface has been fully implemented and well tested(I think so).
#include "3party_loop/co_libev.cpp"
#include "3party_loop/co_libae.cpp"

// The following event loop interface has not yet been implemented!!
// but I am working on them.
#include "3party_loop/co_libuv.cpp"
#include "3party_loop/co_libevent.cpp"
